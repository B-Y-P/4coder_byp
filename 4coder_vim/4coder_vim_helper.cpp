
CUSTOM_COMMAND_MC_GLOBAL_SIG(no_op)
CUSTOM_DOC("no op for binding keybinds to resolve without side effect")
{}

function b32 vim_show_cursor(Application_Links *app){
  //return true;
  if(vim_cursor_blink < 10.f){ animate_in_n_milliseconds(app, 0); }
  return ACTIVE_BLINK(vim_cursor_blink);
}

// Blocking call
function u8 vim_query_user_key(Application_Links *app, String_Const_u8 message){
  u8 result = 0;
  Scratch_Block scratch(app);
  String_Const_u8 before = push_string_copy(scratch, g_qol_bot_string.string);
  vim_set_bottom_text(message);
  vim_is_querying_user_key = true;
  vim_state.chord_state = VIM_CHORD_Pending;

  for(;;){
    User_Input in = get_next_input(app, EventPropertyGroup_Any, EventProperty_Escape);
    if(in.abort){ vim_state.params.request = REQUEST_None; break; }
    if(in.event.kind == InputEventKind_TextInsert){
      result = in.event.text.string.str[0];
      string_append_character(&vim_keystroke_text, result);
      break;
    }
    else if(in.event.kind == InputEventKind_KeyStroke){
      in.event.kind = InputEventKind_None;
      leave_current_input_unhandled(app);
    }
    else{
      leave_current_input_unhandled(app);
    }
  }

  vim_set_bottom_text(before);
  vim_is_querying_user_key = false;
  vim_state.chord_state = VIM_CHORD_Resolved;
  return result;
}


function void vim_enter_insert_mode(Application_Links *app){
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  vim_state.params.number = vim_consume_number();
  vim_history_group = history_group_begin(app, buffer);
  vim_state.mode = VIM_Insert;
  vim_state.insert_index = buffer_history_get_current_state_index(app, buffer);
  vim_state.insert_cursor = buffer_compute_cursor(app, buffer, seek_pos(view_get_cursor_pos(app, view)));
}

function void vim_clamp_newline(Application_Links *app, View_ID view, Buffer_ID buffer, i64 cursor_pos){
  u8 c = buffer_get_char(app, buffer, cursor_pos);
  i64 line = get_line_number_from_pos(app, buffer, cursor_pos);
  if(!line_is_valid_and_blank(app, buffer, line) && (c == '\r' || c == '\n')){ move_left(app); }
}

function u8 character_toggle_case(u8 c){
  i32 shift = ((2*character_is_upper(c)-1)*('a'-'A'));
  return (c + u8((character_is_alpha(c) && c != '_')*shift));
}

function Range_i64 get_view_range(Application_Links *app, View_ID view){
  Range_i64 range = Ii64(view_get_cursor_pos(app, view), view_get_mark_pos(app, view));
  if(vim_state.mode == VIM_Visual){
    if(vim_state.params.edit_type == EDIT_LineWise){
      Buffer_ID buffer = view_get_buffer(app, view, Access_Always);
      range = range_union(get_line_range_from_pos(app, buffer, range.min),
                          get_line_range_from_pos(app, buffer, range.max));
    }
    if(vim_state.params.edit_type == EDIT_CharWise){
      range.max += 1;
    }
  }
  return(range);
}


function void move_horizontal_lines(Application_Links *app, i32 count){
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  i64 pos = view_get_cursor_pos(app, view);
  Range_i64 line_range = get_line_range_from_pos(app, buffer, pos);
  i64 new_pos = view_set_pos_by_character_delta(app, view, pos, count);
  new_pos = clamp(line_range.min, new_pos, line_range.max);
  view_set_cursor_and_preferred_x(app, view, seek_pos(new_pos));
}

function void seek_one_past_end(Application_Links *app){
  View_ID view = get_active_view(app, Access_ReadVisible);
  i64 pos = view_get_cursor_pos(app, view);
  i64 line = view_compute_cursor(app, view, seek_pos(pos)).line;
  Vec2_f32 p = view_relative_xy_of_pos(app, view, line, pos);
  p.x = max_f32;
  i64 new_pos = view_pos_at_relative_xy(app, view, line, p);
  view_set_cursor(app, view, seek_pos(new_pos+1));
}

function void vim_set_prev_visual(Application_Links *app, View_ID view){
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  Managed_Scope scope = buffer_get_managed_scope(app, buffer);
  Vim_Prev_Visual *prev_visual = scope_attachment(app, scope, vim_buffer_prev_visual, Vim_Prev_Visual);

  if(prev_visual){
    prev_visual->cursor_pos = view_get_cursor_pos(app, view);
    prev_visual->mark_pos = view_get_mark_pos(app, view);
    prev_visual->edit_type = vim_state.params.edit_type;
  }
}

function void vim_push_jump(){
  g_qol_will_jump = true;
}

struct Vim_Motion_Block{
  Application_Links *app;
  i64 begin_pos, end_pos;
  i64 clamp_end = -1;
  Vim_Edit_Type prev_edit;

  Vim_Motion_Block(Application_Links *a, i64 b) : app(a), begin_pos(b), prev_edit(vim_state.params.edit_type) {}
  Vim_Motion_Block(Application_Links *a) : app(a), prev_edit(vim_state.params.edit_type) {
    View_ID view = get_active_view(app, Access_ReadVisible);
    begin_pos = view_get_cursor_pos(app, view);
  }
  ~Vim_Motion_Block();
};

// TODO(BYP): clamp_end is arguably a hack, but the case it approximates is even more of a hack
Vim_Motion_Block::~Vim_Motion_Block(){
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);

  if(vim_state.params.edit_type == EDIT_Block){
    Buffer_ID b = view_get_buffer(app, view, vim_request_access[vim_state.params.request]);
    Range_i64 range = Ii64(view_get_cursor_pos(app, view), view_get_mark_pos(app, view));
    if(b == 0){
      b = view_get_buffer(app, view, Access_ReadVisible);
      qol_block_apply(app, view, b, range, qol_range_fade);
    }else{
      vim_block_edit(app, view, b, range);
    }
  }else{
    end_pos = view_get_cursor_pos(app, view);
    i64 buffer_size = buffer_get_size(app, buffer);

    i64 range_begin=begin_pos, range_end=end_pos;
    if(clamp_end > 0){ range_end = Min(range_end, clamp_end); }
    if(vim_state.params.clusivity == VIM_Exclusive){
      if(begin_pos <= end_pos){ range_end--; }
      else{ range_begin++; }
    }
    Range_i64 range = Ii64(range_begin, range_end);
    range.max = Min(range.max+1, buffer_size);

    if(vim_state.params.edit_type == EDIT_LineWise){
      range = range_union(get_line_range_from_pos(app, buffer, begin_pos),
                          get_line_range_from_pos(app, buffer, end_pos));
      if(++range.max >= buffer_size){
        range.max = buffer_size;
        range.min = Max(0, range.min-1);
      }
      range.max -= (vim_state.params.request == REQUEST_Change);
    }

    Buffer_ID b = view_get_buffer(app, view, vim_request_access[vim_state.params.request]);
    if(b == 0){
      qol_range_fade(app, view, buffer, range);
      view_set_cursor_and_preferred_x(app, view, seek_pos(end_pos = begin_pos));
    }else{
      vim_request_vtable[vim_state.params.request](app, view, b, range);
    }
  }

  if(vim_state.params.request == REQUEST_Yank || (vim_state.params.request != REQUEST_None && clamp_end > 0)){
    Vec2_f32 v0 = view_relative_xy_of_pos(app, view, 0, begin_pos);
    Vec2_f32 v1 = view_relative_xy_of_pos(app, view, 0, end_pos);
    qol_cur_cursor_pos += (v1 - v0);
    view_set_cursor_and_preferred_x(app, view, seek_pos(end_pos = begin_pos));
  }

  if(vim_state.params.request != REQUEST_None && vim_state.mode != VIM_Visual){
    vim_state.params.command = vim_state.active_command;
    vim_state.prev_params = vim_state.params;
  }

  Vim_Seek_Params seek = vim_state.params.seek;
  vim_state.params = {};
  vim_state.params.seek = seek;
  if(vim_state.params.selected_reg){
    vim_state.params.selected_reg->flags &= (~REGISTER_Append);
  }
  vim_default_register();

  vim_state.sub_mode = SUB_None;
  if(vim_state.mode != VIM_Insert && vim_state.mode != VIM_Visual){
    vim_clamp_newline(app, view, buffer, end_pos);
  }
  if(vim_state.mode == VIM_Visual){ vim_state.params.edit_type = prev_edit; }
}

function void
vim_visual_insert_inner(Application_Links *app, View_ID view){
  vim_state.mode = VIM_Insert;
  vim_history_group.buffer = view_get_buffer(app, view, Access_Always);
  vim_set_prev_visual(app, view);
  MC_begin_multi_block(app);
}

VIM_COMMAND_SIG(vim_up){
  if(vim_state.number >= 10){ vim_push_jump(); }
  Vim_Motion_Block vim_motion_block(app);
  vim_state.params.edit_type = EDIT_LineWise;
  move_vertical_lines(app, -vim_consume_number());
}

VIM_COMMAND_SIG(vim_down){
  if(vim_state.number >= 10){ vim_push_jump(); }
  Vim_Motion_Block vim_motion_block(app);
  vim_state.params.edit_type = EDIT_LineWise;
  move_vertical_lines(app, vim_consume_number());
}

VIM_COMMAND_SIG(vim_left){
  Vim_Motion_Block vim_motion_block(app);
  move_horizontal_lines(app, -vim_consume_number());
}

VIM_COMMAND_SIG(vim_right){
  Vim_Motion_Block vim_motion_block(app);
  move_horizontal_lines(app, vim_consume_number());
}

function void vim_cut(Application_Links *app){
  View_ID view = get_active_view(app, Access_ReadWriteVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
  Range_i64 range = Ii64(view_get_cursor_pos(app, view), view_get_mark_pos(app, view));
  range.max++;
  if(clipboard_post_buffer_range(app, 0, buffer, range)){
    buffer_replace_range(app, buffer, range, string_u8_empty);
  }
}

function void vim_make_request(Application_Links *app, Vim_Request_Type request){
  if(vim_state.sub_mode == SUB_Leader && request != REQUEST_Yank){ vim_state.params.selected_reg = 0; }
  if(vim_state.params.request == request){
    Vim_Motion_Block vim_motion_block(app);
    vim_state.params.edit_type = EDIT_LineWise;
    move_vertical_lines(app, vim_consume_number()-1);
  }else{
    vim_state.params.count = vim_consume_number();
    vim_state.params.request = request;
    if(vim_state.mode == VIM_Visual){
      View_ID view = get_active_view(app, Access_ReadVisible);
      vim_set_prev_visual(app, view);

      if(vim_state.params.edit_type == EDIT_Block && request == REQUEST_Change){
        vim_state.mode = VIM_Insert;
        MC_begin_multi_block(app);
        MC_apply(app, vim_cut, MC_Command_CursorCopy);
      }else{
        vim_state.mode = VIM_Normal;
        Vim_Motion_Block vim_motion_block(app, view_get_mark_pos(app, view));
      }
    }
    else{ vim_state.chord_state = VIM_CHORD_Pending; }
  }
}

function void vim_page_scroll_inner(Application_Links *app, f32 ratio){
  View_ID view = get_active_view(app, Access_ReadVisible);
  vim_push_jump();

  f32 scroll_pixels = ratio*get_page_jump(app, view);
  move_vertical_pixels(app, scroll_pixels);

  Buffer_Scroll scroll = view_get_buffer_scroll(app, view);
  scroll.target = view_move_buffer_point(app, view, scroll.target, V2f32(0.f, scroll_pixels));
  view_set_buffer_scroll(app, view, scroll, SetBufferScroll_NoCursorChange);
}
