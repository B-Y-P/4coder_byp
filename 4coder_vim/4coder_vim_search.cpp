
function void qol_isearch(Application_Links *app, Scan_Direction scan, i64 first_pos, String_Const_u8 query_prompt[2], String_Const_u8 query_init, b32 use_bot_string);

function i64 vim_pattern_inner_v(Application_Links *app, Buffer_Seek_String_Flags seek_flags){
  String_u8 *pattern = &vim_registers.search.data;
  if(pattern->size == 0){ return -1; }
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  i64 pos = view_get_cursor_pos(app, view);
  i64 new_pos = -1;
  seek_string(app, buffer, pos, 0, 0, pattern->string, &new_pos, seek_flags);
  return new_pos;
}

function void vim_in_pattern_inner(Application_Links *app, Buffer_Seek_String_Flags seek_flags){
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  i64 old_pos = view_get_cursor_pos(app, view);
  i64 new_pos = vim_pattern_inner_v(app, seek_flags);
  i64 p1 = new_pos + vim_registers.search.data.size-1;
  if(old_pos != new_pos && 0 <= new_pos && new_pos < buffer_get_size(app, buffer)){
    vim_push_jump();
    Vim_Motion_Block vim_motion_block(app, p1);
    view_set_cursor_and_preferred_x(app, view, seek_pos(new_pos));
  }else{
    vim_skip_insert = true;
    vim_state.params.request = REQUEST_None;
    qol_range_fade(app, view, buffer, Ii64(old_pos, buffer_get_size(app, buffer)));
  }
}

function void vim_to_pattern_inner(Application_Links *app, Buffer_Seek_String_Flags seek_flags){
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  i64 old_pos = view_get_cursor_pos(app, view);
  i64 new_pos = vim_pattern_inner_v(app, seek_flags);
  if(old_pos != new_pos && 0 <= new_pos && new_pos < buffer_get_size(app, buffer)){
    vim_push_jump();
    Vim_Motion_Block vim_motion_block(app);
    view_set_cursor_and_preferred_x(app, view, seek_pos(new_pos));
  }
}

function void vim_start_search_inner(Application_Links *app, Scan_Direction scan){
  View_ID view = get_active_view(app, Access_ReadVisible);
  i64 pos = view_get_cursor_pos(app, view);
  String_Const_u8 prompts[] = { string_u8_litexpr("/"), string_u8_litexpr("?") };
  qol_isearch(app, scan, pos, prompts, string_u8_empty, true);
}

VIM_COMMAND_SIG(vim_clear_search){
  vim_registers.search.data.size = 0;
  vim_registers.search.flags &= (~REGISTER_Set);
  vim_update_registers(app);
  qol_bot_text_set(string_u8_empty);
}

VIM_COMMAND_SIG(vim_start_search_forward){  vim_start_search_inner(app, Scan_Forward); }
VIM_COMMAND_SIG(vim_start_search_backward){ vim_start_search_inner(app, Scan_Backward); }

VIM_COMMAND_SIG(vim_to_next_pattern){ vim_to_pattern_inner(app, 0); }
VIM_COMMAND_SIG(vim_to_prev_pattern){ vim_to_pattern_inner(app, BufferSeekString_Backward); }

VIM_COMMAND_SIG(vim_in_next_pattern){ vim_in_pattern_inner(app, 0); }
VIM_COMMAND_SIG(vim_in_prev_pattern){ vim_in_pattern_inner(app, BufferSeekString_Backward); }

VIM_COMMAND_SIG(vim_search_identifier){
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  i64 pos = view_get_cursor_pos(app, view);
  Range_i64 range = enclose_pos_alpha_numeric_underscore(app, buffer, pos);
  vim_state.params.selected_reg = &vim_registers.search;
  vim_request_vtable[REQUEST_Yank](app, view, buffer, range);
  vim_default_register();
}