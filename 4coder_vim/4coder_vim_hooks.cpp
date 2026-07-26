
function void
vim_file_save(Application_Links *app, Buffer_ID buffer_id){
  Scratch_Block scratch(app);
  String_Const_u8 unique_name = push_buffer_unique_name(app, scratch, buffer_id);
  i64 line_count = buffer_get_line_count(app, buffer_id);
  i64 bytes = buffer_get_size(app, buffer_id);
  String_Const_u8 msg = push_stringf(scratch, "\"%.*s\"  %dL, %dC written", string_expand(unique_name), line_count, bytes);
  vim_set_bottom_text(msg);
}

BUFFER_HOOK_SIG(vim_file_save_hook){
  default_file_save(app, buffer_id);
  vim_file_save(app, buffer_id);
  return 0;
}

function void vim_set_file_register(Application_Links *app, Buffer_ID buffer){
  Scratch_Block scratch(app);
  String_Const_u8 unique_name = push_buffer_unique_name(app, scratch, buffer);
  Vim_Register *reg = &vim_registers.file;
  b32 valid = vim_register_copy(reg, unique_name);
  if(!valid){ return; }
  vim_update_registers(app);
}

function void
vim_view_change_buffer(Application_Links *app, View_ID view_id, Buffer_ID old_buffer_id, Buffer_ID new_buffer_id){
  vim_set_file_register(app, new_buffer_id);
}

function void
vim_begin_buffer_inner(Application_Links *app, Buffer_ID buffer_id){
  Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
  Vim_Prev_Visual *prev_visual = scope_attachment(app, scope, vim_buffer_prev_visual, Vim_Prev_Visual);
  prev_visual->cursor_pos = prev_visual->mark_pos = 0;

  i64 *marks = (i64 *)managed_scope_get_attachment(app, scope, vim_buffer_marks, 26*sizeof(i64));
  block_fill_u64(marks, 26*sizeof(i64), max_u64);

  b32 *wrap_lines_ptr = scope_attachment(app, scope, buffer_wrap_lines, b32);
  *wrap_lines_ptr = false;
}

BUFFER_HOOK_SIG(vim_begin_buffer){
  default_begin_buffer(app, buffer_id);
  //fold_begin_buffer_hook(app, buffer_id);
  vim_begin_buffer_inner(app, buffer_id);
  return 0;
}

#define exp_interp(cur, nxt, dt, rate) (cur += (((nxt) - (cur))*(1.f - powf(rate, dt))))

function void vim_buffer_edit_range_inner(Application_Links *app, Buffer_ID buffer_id, Range_i64 new_range, Range_Cursor old_cursor_range){
  Range_i64 old_range = Ii64(old_cursor_range.min.pos, old_cursor_range.max.pos);
  i64 insert_size = range_size(new_range);

  Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
  i64 *marks = (i64*)managed_scope_get_attachment(app, scope, vim_buffer_marks, 26*sizeof(i64));
  if(marks){
    foreach(i, 26){
      index_shift(&marks[i], old_range, insert_size);
    }
  }

  foreach(i, 26){
    if(vim_global_marks[i].buffer_id.id == buffer_id){
      index_shift(&vim_global_marks[i].pos, old_range, insert_size);
    }
  }
}

BUFFER_EDIT_RANGE_SIG(vim_buffer_edit_range){
  default_buffer_edit_range(app, buffer_id, new_range, old_cursor_range);
  //fold_buffer_edit_range_inner(app, buffer_id, new_range, old_cursor_range);
  vim_buffer_edit_range_inner(app, buffer_id, new_range, old_cursor_range);
  return 0;
}

function void
vim_tick(Application_Links *app, Frame_Info frame_info){
  //fold_tick(app, frame_info);
  vim_cursor_blink += frame_info.animation_dt;

  View_ID view = get_active_view(app, Access_Always);
  Buffer_ID buffer = view_get_buffer(app, view, Access_Always);
  if (vim_state.mode == VIM_Insert && vim_history_group.buffer != buffer){
    vim_history_group.app = app;
    history_group_end(vim_history_group);
    vim_history_group = history_group_begin(app, buffer);
    vim_state.insert_index = buffer_history_get_current_state_index(app, buffer);
    vim_state.insert_cursor = buffer_compute_cursor(app, buffer, seek_pos(view_get_cursor_pos(app, view)));
  }
}

CUSTOM_COMMAND_MC_GLOBAL_SIG(vim_try_exit)
CUSTOM_DOC("Vim command for responding to a try-exit event")
{
  User_Input input = get_current_input(app);
  if(match_core_code(&input, CoreCode_TryExit)){
    b32 do_exit = true;
    if(!allow_immediate_close_without_checking_for_changes){
      b32 has_unsaved_changes = false;
      for(Buffer_ID buffer = get_buffer_next(app, 0, Access_Always);
          buffer;
          buffer = get_buffer_next(app, buffer, Access_Always))
      {
        Dirty_State dirty = buffer_get_dirty_state(app, buffer);
        if(HasFlag(dirty, DirtyState_UnsavedChanges)){
          has_unsaved_changes = true;
          break;
        }
      }
      if(has_unsaved_changes){
        View_ID view = get_active_view(app, Access_Always);
        do_exit = vim_do_4coder_close_user_check(app, view);
      }
    }
    if(do_exit){
      hard_exit(app);
    }
  }
}
