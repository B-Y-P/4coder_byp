
CUSTOM_COMMAND_MC_GLOBAL_SIG(vim_toggle_relative_line_num)
CUSTOM_DOC("Toggles relative line numbers")
{ def_toggle_config_b32(vars_save_string_lit("vim_line_numbers_relative")); }

CUSTOM_COMMAND_MC_CURSOR_SIG(right_adjust_view)
CUSTOM_DOC("Sets the right size of the view near the x position of the cursor.")
{
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  i64 pos = view_get_cursor_pos(app, view);
  if(!line_is_valid_and_blank(app, buffer, get_line_number_from_pos(app, buffer, pos))){
    i64 new_pos = get_line_side_pos_from_pos(app, buffer, pos, Side_Min);
    if(character_is_whitespace(buffer_get_char(app, buffer, new_pos))){
      new_pos = buffer_seek_character_class_change_1_0(app, buffer, &character_predicate_whitespace, Scan_Forward, new_pos);
    }
    view_set_cursor_and_preferred_x(app, view, seek_pos(new_pos));
  }
  Buffer_Scroll scroll = view_get_buffer_scroll(app, view);
  scroll.target.pixel_shift.x = 0.f;
  view_set_buffer_scroll(app, view, scroll, SetBufferScroll_NoCursorChange);
}

VIM_COMMAND_SIG(vim_normal_mode){
  if(mc_context.active){ // && vim_state.mode == VIM_Normal && vim_state.sub_mode == SUB_None && vim_state.number == 0){
    MC_end(app);
  }else if(vim_state.mode == VIM_Insert){
    vim_set_insert_register(app);
    View_ID view = get_active_view(app, Access_ReadVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
    const i32 N = Max(0, vim_state.params.number-1);
    History_Group h = history_group_begin(app, buffer);
    foreach(i, N){
      vim_paste(app, view, buffer, &vim_registers.insert);
    }
    if(N){ h.first--; history_group_end(h); }
    move_horizontal_lines(app, -1);
  }
  if(vim_state.mode == VIM_Visual){
    vim_set_prev_visual(app, get_active_view(app, Access_ReadVisible));
  }
  vim_reset_state();
}


VIM_COMMAND_SIG(vim_insert_mode_after){
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  i64 line = get_line_number_from_pos(app, buffer, view_get_cursor_pos(app, view));
  if(!line_is_valid_and_blank(app, buffer, line)){
    move_right(app);
  }
  vim_enter_insert_mode(app);
}
VIM_COMMAND_SIG(vim_insert_mode){ vim_enter_insert_mode(app); }
VIM_COMMAND_SIG(vim_insert_begin){ vim_begin_line(app); vim_enter_insert_mode(app); }
VIM_COMMAND_SIG(vim_insert_end){ vim_end_line(app); vim_insert_mode_after(app); }

VIM_COMMAND_SIG(vim_modal_i){
  if(vim_state.mode == VIM_Visual || vim_state.params.request != REQUEST_None){
    vim_state.params.clusivity = VIM_Exclusive;
    u8 key = vim_query_user_key(app, string_u8_litexpr("-- TEXT OBJECT --"));
    if(key){
      vim_state.params.seek.character = key;
      vim_state.active_command = vim_text_object;
      vim_text_object(app);
    }
  }
  else{ vim_insert_mode(app); }
}

VIM_COMMAND_SIG(vim_modal_a){
  if(vim_state.mode == VIM_Visual || vim_state.params.request != REQUEST_None){
    vim_state.params.clusivity = VIM_Inclusive;
    u8 key = vim_query_user_key(app, string_u8_litexpr("-- TEXT OBJECT --"));
    if(key){
      vim_state.params.seek.character = key;
      vim_state.active_command = vim_text_object;
      vim_text_object(app);
    }
  }
  else{ vim_insert_mode_after(app); }
}

VIM_COMMAND_SIG(vim_newline_below){
  vim_insert_end(app);
  vim_state.insert_index++;
  write_text(app, string_u8_litexpr("\n"));
  auto_indent_line_at_cursor(app);
}

VIM_COMMAND_SIG(vim_newline_above){
  seek_beginning_of_textual_line(app);
  vim_enter_insert_mode(app);
  vim_state.insert_index++;
  write_string(app, string_u8_litexpr("\n"));
  move_vertical_lines(app, -1);
  auto_indent_line_at_cursor(app);
}

VIM_COMMAND_SIG(vim_visual_mode){
  if(vim_state.mode != VIM_Visual){
    set_mark(app);
    vim_state.mode = VIM_Visual;
  }
  vim_state.params.edit_type = EDIT_CharWise;
  Input_Event event = get_current_input(app).event;
  if(event.kind == InputEventKind_KeyStroke){
    if(0){}
    else if(has_modifier(&event, KeyCode_Shift)){   vim_state.params.edit_type = EDIT_LineWise; }
    else if(has_modifier(&event, KeyCode_Control)){ vim_state.params.edit_type = EDIT_Block; }
  }
}

VIM_COMMAND_SIG(vim_prev_visual){
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  Managed_Scope scope = buffer_get_managed_scope(app, buffer);

  Vim_Prev_Visual *prev_visual = scope_attachment(app, scope, vim_buffer_prev_visual, Vim_Prev_Visual);
  if(prev_visual && prev_visual->cursor_pos != 0 && prev_visual->mark_pos != 0){
    view_set_cursor_and_preferred_x(app, view, seek_pos(prev_visual->cursor_pos));
    view_set_mark(app, view, seek_pos(prev_visual->mark_pos));
    vim_state.params.edit_type = prev_visual->edit_type;
    vim_state.mode = VIM_Visual;
  }
}

VIM_COMMAND_SIG(vim_block_swap){
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  i64 c = view_get_cursor_pos(app, view);
  i64 m = view_get_mark_pos(app, view);
  i64 line = get_line_number_from_pos(app, buffer, c);
  Vec2_f32 c_p = view_relative_xy_of_pos(app, view, line, c);
  Vec2_f32 m_p = view_relative_xy_of_pos(app, view, line, m);
  Swap(f32, c_p.x, m_p.x);
  c = view_pos_at_relative_xy(app, view, line, c_p);
  m = view_pos_at_relative_xy(app, view, line, m_p);
  view_set_cursor(app, view, seek_pos(c));
  view_set_mark(app, view, seek_pos(m));
}

VIM_COMMAND_SIG(vim_replace_mode){
  vim_state.mode = VIM_Replace;
  set_mark(app);
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  vim_history_group = history_group_begin(app, buffer);
}

VIM_COMMAND_SIG(vim_visual_insert){
  if(vim_state.params.edit_type == EDIT_Block){
    vim_visual_insert_inner(app, get_active_view(app, Access_ReadVisible));
  }
}

VIM_COMMAND_SIG(vim_visual_append){
  if(vim_state.params.edit_type == EDIT_Block){
    View_ID view = get_active_view(app, Access_ReadVisible);
    vim_visual_insert_inner(app, view);
    MC_apply(app, cursor_mark_swap, MC_Command_Cursor);
    MC_apply(app, move_right,       MC_Command_Cursor);
  }
}

VIM_COMMAND_SIG(vim_submode_g)     { vim_state.sub_mode = SUB_G;      vim_state.chord_state = VIM_CHORD_Pending; }
VIM_COMMAND_SIG(vim_submode_z)     { vim_state.sub_mode = SUB_Z;      vim_state.chord_state = VIM_CHORD_Pending; }
VIM_COMMAND_SIG(vim_submode_leader){ vim_state.sub_mode = SUB_Leader; vim_state.chord_state = VIM_CHORD_Pending; }
VIM_COMMAND_SIG(vim_submode_window){ vim_state.sub_mode = SUB_Window; vim_state.chord_state = VIM_CHORD_Pending; }

VIM_COMMAND_SIG(vim_replace_next_char){
  u8 key = vim_query_user_key(app, string_u8_litexpr("-- REPLACE NEXT --"));
  if(key){
    View_ID view = get_active_view(app, Access_ReadVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
    i64 pos = view_get_cursor_pos(app, view);
    buffer_replace_range(app, buffer, Ii64(pos, pos+1), SCu8(&key, 1));
  }
}


VIM_COMMAND_SIG(vim_replace_range_next){
  u8 key = vim_query_user_key(app, string_u8_litexpr("-- RANGE REPLACE NEXT --"));
  if(key){
    vim_state.params.seek.character = key;
    vim_make_request(app, REQUEST_Replace);
  }
}

VIM_COMMAND_SIG(vim_request_yank){    vim_make_request(app, REQUEST_Yank); }
VIM_COMMAND_SIG(vim_request_delete){  vim_make_request(app, REQUEST_Delete); }
VIM_COMMAND_SIG(vim_request_change){  vim_make_request(app, REQUEST_Change); }
VIM_COMMAND_SIG(vim_uppercase){       vim_make_request(app, REQUEST_Upper); }
VIM_COMMAND_SIG(vim_lowercase){       vim_make_request(app, REQUEST_Lower); }
VIM_COMMAND_SIG(vim_toggle_case){     vim_make_request(app, REQUEST_ToggleCase); }
VIM_COMMAND_SIG(vim_request_indent){  vim_make_request(app, REQUEST_Indent); }
VIM_COMMAND_SIG(vim_request_outdent){ vim_make_request(app, REQUEST_Outdent); }
VIM_COMMAND_SIG(vim_request_fold){    vim_make_request(app, REQUEST_Fold); }
VIM_COMMAND_SIG(vim_request_auto_indent){ vim_make_request(app, REQUEST_AutoIndent); }

VIM_COMMAND_SIG(vim_toggle_char){
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  i64 pos = view_get_cursor_pos(app, view);
  u8 c = buffer_get_char(app, buffer, pos);
  c = character_toggle_case(c);
  buffer_replace_range(app, buffer, Ii64_size(pos, 1), SCu8(&c, 1));
}

VIM_COMMAND_SIG(vim_delete_end){
  View_ID view = get_active_view(app, Access_ReadWriteVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
  i64 pos = view_get_cursor_pos(app, view);
  i64 line_num = get_line_number_from_pos(app, buffer, pos);
  if(line_is_valid_and_blank(app, buffer, line_num)){ vim_reset_state(); return; }
  Vim_Motion_Block vim_motion_block(app);
  vim_state.params.request = REQUEST_Delete;
  vim_state.params.clusivity = VIM_Exclusive;
  seek_end_of_line(app);
}

VIM_COMMAND_SIG(vim_delete_to_begin){
  View_ID view = get_active_view(app, Access_ReadWriteVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
  i64 pos = view_get_cursor_pos(app, view);
  i64 line_num = get_line_number_from_pos(app, buffer, pos);
  if(line_is_valid_and_blank(app, buffer, line_num)){ vim_reset_state(); return; }
  Vim_Motion_Block vim_motion_block(app);
  vim_state.params.request = REQUEST_Delete;
  vim_state.params.clusivity = VIM_Exclusive;
  seek_beginning_of_line(app);
}

VIM_COMMAND_SIG(vim_change_end){
  Vim_Motion_Block vim_motion_block(app);
  vim_state.params.request = REQUEST_Change;
  vim_state.params.clusivity = VIM_Exclusive;
  seek_end_of_line(app);
}

VIM_COMMAND_SIG(vim_yank_end){
  Vim_Motion_Block vim_motion_block(app);
  vim_state.params.request = REQUEST_Yank;
  vim_state.params.clusivity = VIM_Exclusive;
  seek_end_of_line(app);
}


VIM_COMMAND_SIG(vim_leader_d){ vim_state.params.selected_reg=0; vim_request_delete(app); }
VIM_COMMAND_SIG(vim_leader_D){ vim_state.params.selected_reg=0; vim_delete_end(app); }
VIM_COMMAND_SIG(vim_leader_c){ vim_state.params.selected_reg=0; vim_request_change(app); }
VIM_COMMAND_SIG(vim_leader_C){ vim_state.params.selected_reg=0; vim_change_end(app); }

VIM_COMMAND_SIG(vim_digit){
  User_Input input = get_current_input(app);
  if(input.event.kind == InputEventKind_KeyStroke){
    int digit = input.event.key.code - KeyCode_0;
    if(in_range(0, digit, 10)){
      vim_state.number *= 10;
      vim_state.number += digit;
    }
    vim_state.chord_state = VIM_CHORD_Pending;
  }
}

VIM_COMMAND_SIG(vim_digit_del){
  if(vim_state.number != 0){
    vim_state.number /= 10;
    vim_keystroke_text.size = vim_pre_keystroke_size-1;
    vim_state.chord_state = VIM_CHORD_Pending;
  }else{
    u64 size = vim_keystroke_text.size;
    vim_reset_state();
    vim_keystroke_text.size = size;
  }
}

VIM_COMMAND_SIG(vim_modal_0){
  if(vim_state.number){ vim_digit(app); }
  else{ vim_begin_line(app); }
  //else{ vim_line_start(app); }
}

VIM_COMMAND_SIG(vim_paragraph_up){
  vim_push_jump();
  Vim_Motion_Block vim_motion_block(app);
  vim_state.params.edit_type = EDIT_LineWise;
  vim_state.params.clusivity = VIM_Exclusive;
  const i32 N = vim_consume_number();
  foreach(i,N)
    move_up_to_blank_line_end(app);
}

VIM_COMMAND_SIG(vim_paragraph_down){
  vim_push_jump();
  Vim_Motion_Block vim_motion_block(app);
  vim_state.params.edit_type = EDIT_LineWise;
  vim_state.params.clusivity = VIM_Exclusive;
  const i32 N = vim_consume_number();
  foreach(i,N)
    move_down_to_blank_line_end(app);
}

VIM_COMMAND_SIG(vim_whole_page_up){   vim_page_scroll_inner(app, -1.0f); }
VIM_COMMAND_SIG(vim_whole_page_down){ vim_page_scroll_inner(app,  1.0f); }
VIM_COMMAND_SIG(vim_half_page_up){    vim_page_scroll_inner(app, -0.5f); }
VIM_COMMAND_SIG(vim_half_page_down){  vim_page_scroll_inner(app,  0.5f); }

VIM_COMMAND_SIG(vim_scroll_screen_top){ vim_scroll_inner(app,  0.0f); }
VIM_COMMAND_SIG(vim_scroll_screen_mid){ vim_scroll_inner(app, -0.5f); }
VIM_COMMAND_SIG(vim_scroll_screen_bot){ vim_scroll_inner(app, -1.0f); }

VIM_COMMAND_SIG(vim_screen_top){ vim_screen_inner(app, 0.0f, +1*vim_consume_number()); }
VIM_COMMAND_SIG(vim_screen_mid){ vim_screen_inner(app, 0.5f, +0*vim_consume_number()); }
VIM_COMMAND_SIG(vim_screen_bot){ vim_screen_inner(app, 1.0f, -1*vim_consume_number()); }

VIM_COMMAND_SIG(vim_line_up)  { vim_line_delta(app, +1.f*vim_consume_number()); }
VIM_COMMAND_SIG(vim_line_down){ vim_line_delta(app, -1.f*vim_consume_number()); }

// TODO(BYP): Decide how I want to clamp word deletion in newline cases
VIM_COMMAND_SIG(vim_forward_word){
  Vim_Motion_Block vim_motion_block(app);
  vim_state.params.clusivity = VIM_Exclusive;
  View_ID view = get_active_view(app, Access_ReadVisible);
  i64 prev_pos = -1;
  i64 pos = vim_scan_word(app, view, Scan_Forward, &prev_pos, vim_consume_number());
  view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
  if(prev_pos != pos){
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
    i64 line0 = get_line_number_from_pos(app, buffer, prev_pos);
    i64 line1 = get_line_number_from_pos(app, buffer, pos);
    if(line0 != line1){
      vim_motion_block.clamp_end = get_line_side_pos(app, buffer, line0, Side_Max);
    }
  }
}

VIM_COMMAND_SIG(vim_backward_word){
  Vim_Motion_Block vim_motion_block(app);
  View_ID view = get_active_view(app, Access_ReadVisible);
  i64 pos = vim_scan_word(app, view, Scan_Backward, 0, vim_consume_number());
  view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
}

VIM_COMMAND_SIG(vim_forward_WORD){
  Vim_Motion_Block vim_motion_block(app);
  vim_state.params.clusivity = VIM_Exclusive;
  View_ID view = get_active_view(app, Access_ReadVisible);
  i64 prev_pos = -1;
  i64 pos = vim_scan_WORD(app, view, Scan_Forward, &prev_pos, vim_consume_number());
  view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
  if(prev_pos != pos){
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
    i64 line0 = get_line_number_from_pos(app, buffer, prev_pos);
    i64 line1 = get_line_number_from_pos(app, buffer, pos);
    if(line0 != line1){
      vim_motion_block.clamp_end = get_line_side_pos(app, buffer, line0, Side_Max);
    }
  }
}

VIM_COMMAND_SIG(vim_backward_WORD){
  View_ID view = get_active_view(app, Access_ReadVisible);
  i64 cursor_pos = view_get_cursor_pos(app, view);
  Vim_Motion_Block vim_motion_block(app, cursor_pos-1);
  i64 pos = vim_scan_WORD(app, view, Scan_Backward, 0, vim_consume_number());
  view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
}

VIM_COMMAND_SIG(vim_forward_end){
  Vim_Motion_Block vim_motion_block(app);
  View_ID view = get_active_view(app, Access_ReadVisible);
  i64 pos = vim_scan_end(app, view, Scan_Forward, vim_consume_number());
  view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
}

VIM_COMMAND_SIG(vim_backward_end){
  Vim_Motion_Block vim_motion_block(app);
  View_ID view = get_active_view(app, Access_ReadVisible);
  i64 pos = vim_scan_end(app, view, Scan_Backward, vim_consume_number());
  view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
}

VIM_COMMAND_SIG(vim_forward_END){
  Vim_Motion_Block vim_motion_block(app);
  View_ID view = get_active_view(app, Access_ReadVisible);
  i64 pos = vim_scan_END(app, view, Scan_Forward, vim_consume_number());
  view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
}

VIM_COMMAND_SIG(vim_backward_END){
  Vim_Motion_Block vim_motion_block(app);
  View_ID view = get_active_view(app, Access_ReadVisible);
  i64 pos = vim_scan_END(app, view, Scan_Backward, vim_consume_number());
  view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
}

VIM_COMMAND_SIG(vim_bounce){
  vim_push_jump();
  Vim_Motion_Block vim_motion_block(app);
  Scan_Direction direction = Scan_Forward;
  Input_Event event = get_current_input(app).event;
  if(event.kind == InputEventKind_KeyStroke && has_modifier(&event, KeyCode_Control)){ direction=Scan_Backward; }
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  i64 pos = view_get_cursor_pos(app, view);
  pos = vim_scan_bounce(app, buffer, pos, direction);
  view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
}

VIM_COMMAND_SIG(vim_modal_percent){
  if(vim_state.number){ vim_percent_file(app); }
  else{ vim_bounce(app); }
}

VIM_COMMAND_SIG(vim_paste_after){
  if(vim_state.params.selected_reg){
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    if(vim_state.params.selected_reg->edit_type == EDIT_LineWise){
      seek_end_of_line(app);
      move_right(app);
      vim_paste(app, view, buffer, vim_state.params.selected_reg);
      move_up(app);
    }else{
      move_right(app);
      vim_paste(app, view, buffer, vim_state.params.selected_reg);
    }

    Vim_Register *reg = vim_state.prev_params.selected_reg;
    vim_state.params.command = vim_paste_after;
    vim_state.prev_params = vim_state.params;
    vim_state.prev_params.selected_reg = reg;
  }
}

VIM_COMMAND_SIG(vim_paste_before){
  if(vim_state.params.selected_reg){
    if(vim_state.params.selected_reg->edit_type == EDIT_LineWise){
      seek_beginning_of_line(app);
    }
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    vim_paste(app, view, buffer, vim_state.params.selected_reg);

    Vim_Register *reg = vim_state.prev_params.selected_reg;
    vim_state.params.command = vim_paste_before;
    vim_state.prev_params = vim_state.params;
    vim_state.prev_params.selected_reg = reg;
  }
}

VIM_COMMAND_SIG(vim_paste_visual){
  if (vim_state.mode != VIM_Visual){ return; }
  View_ID view = get_active_view(app, Access_ReadWriteVisible);
  Vim_Register *reg = vim_state.params.selected_reg;
  vim_state.params.selected_reg = 0;
  vim_make_request(app, REQUEST_Change);
  vim_state.params.selected_reg = reg;
  view_enqueue_command_function(app, view, vim_paste_before);
  view_enqueue_command_function(app, view, vim_normal_mode);
}

// NOTE: This intentionally doesn't update selected register
function void vim_backspace_char_inner(Application_Links *app, i32 offset){
  View_ID view = get_active_view(app, Access_ReadWriteVisible);
  if(!if_view_has_highlighted_range_delete_range(app, view)){
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i64 pos = view_get_cursor_pos(app, view);
    i64 buffer_size = buffer_get_size(app, buffer);
    if(in_range(0, pos, buffer_size)){
      Buffer_Cursor cursor = view_compute_cursor(app, view, seek_pos(pos));
      i64 character = view_relative_character_from_pos(app, view, cursor.line, cursor.pos);
      i64 start = view_pos_from_relative_character(app, view, cursor.line, character + offset);
      u8 c = buffer_get_char(app, buffer, start);
      vim_register_copy(&vim_registers.small_delete, SCu8(&c, 1));
      vim_registers.small_delete.edit_type = EDIT_CharWise;
      vim_update_registers(app);
      buffer_replace_range(app, buffer, Ii64(start, pos), string_u8_empty);
    }
  }
}

VIM_COMMAND_SIG(vim_backspace_char){ vim_backspace_char_inner(app, -1); }
VIM_COMMAND_SIG(vim_delete_char){    vim_backspace_char_inner(app, 1); }

VIM_COMMAND_SIG(vim_last_command){
  const i32 N = vim_consume_number();
  Custom_Command_Function *command = vim_state.prev_params.command;
  if(command){
    View_ID view = get_active_view(app, Access_ReadVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
    History_Group history_group = history_group_begin(app, buffer);
    foreach(i,N){
      vim_state.params = vim_state.prev_params;
      vim_state.active_command = command;
      vim_state.number = vim_state.params.number;
      b32 do_insert = vim_state.params.do_insert;
      vim_skip_insert = false;

      if((command == vim_paste_before || command == vim_paste_after) &&
         in_range(0, (vim_state.params.selected_reg - vim_registers.cycle), 8))
      {
        vim_state.params.selected_reg++;
      }

      command(app);

      if(do_insert && !vim_skip_insert){
        vim_paste(app, view, buffer, &vim_registers.insert);
        vim_state.mode = VIM_Normal;
      }
    }
    history_group_end(history_group);
  }
}

function b32
vim_combine_line_inner(Application_Links *app, View_ID view, Buffer_ID buffer, i64 line_num){
  if(!is_valid_line(app, buffer, line_num+1)){ return true; }
  i64 pos = get_line_end_pos(app, buffer, line_num);
  Range_i64 range = {};
  range.min = pos;

  i64 new_pos = pos + 1;
  String_Const_u8 delimiter = (vim_state.sub_mode == SUB_G ? string_u8_empty : string_u8_litexpr(" "));
  if(!line_is_valid_and_blank(app, buffer, line_num+1)){
    if(character_is_whitespace(buffer_get_char(app, buffer, new_pos))){
      new_pos = buffer_seek_character_class_change_1_0(app, buffer, &character_predicate_whitespace, Scan_Forward, new_pos);
    }
  }else{
    new_pos = get_line_end_pos(app, buffer, line_num+1);
    delimiter.size = 0;
  }
  i64 end_pos = get_line_side_pos_from_pos(app, buffer, pos, Side_Max);
  view_set_cursor_and_preferred_x(app, view, seek_pos(end_pos));
  move_right(app);

  range.max = new_pos;

  buffer_replace_range(app, buffer, range, delimiter);

  return false;
}

VIM_COMMAND_SIG(vim_combine_line){
  View_ID view = get_active_view(app, Access_ReadWriteVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
  if(buffer == 0){ return; }
  i64 pos = view_get_cursor_pos(app, view);
  i64 line = buffer_compute_cursor(app, buffer, seek_pos(pos)).line;

  i32 N = vim_consume_number();
  if(vim_state.mode == VIM_Visual){
    Range_i64 range = Ii64(view_get_cursor_pos(app, view), view_get_mark_pos(app, view));
    i64 line_min = get_line_number_from_pos(app, buffer, range.min);
    i64 line_max = get_line_number_from_pos(app, buffer, range.max);
    N = Max(1, i32(line_max-line_min));
    view_set_cursor_and_preferred_x(app, view, seek_pos(range.min));
    view_set_mark(app, view, seek_pos(range.max));
    line = line_min;
  }

  History_Group history_group = history_group_begin(app, buffer);
  foreach(i,N){
    if(vim_combine_line_inner(app, view, buffer, line)){
      break;
    }
  }
  if(N > 1){ history_group_end(history_group); }
}

VIM_COMMAND_SIG(vim_set_mark){
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  i64 pos = view_get_cursor_pos(app, view);
  Scratch_Block scratch(app);
  u8 character = vim_query_user_key(app, string_u8_litexpr("-- SET MARK NEXT --"));
  if(in_range('a', character, 'z'+1)){
    Managed_Scope scope = buffer_get_managed_scope(app, buffer);
    i64 *marks = (i64 *)managed_scope_get_attachment(app, scope, vim_buffer_marks, 26*sizeof(i64));
    if(marks){
      marks[character-'a'] = pos;
      vim_set_bottom_text(push_stringf(scratch, "Mark %c set", character));
    }
  }
  else if(in_range('A', character, 'Z'+1)){
    vim_global_marks[character-'A'] = {buffer_identifier(buffer), pos};
    vim_set_bottom_text(push_stringf(scratch, "Global mark %c set", character));
  }
}

VIM_COMMAND_SIG(vim_goto_mark){
  User_Input input = get_current_input(app);
  if(input.event.kind == InputEventKind_KeyStroke){
    if(input.event.key.code == KeyCode_Tick){
      vim_state.params.edit_type = EDIT_LineWise;
    }
  }
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  u8 c = vim_query_user_key(app, string_u8_litexpr("-- GOTO MARK NEXT --"));
  if(in_range('a', c, 'z'+1)){
    Managed_Scope scope = buffer_get_managed_scope(app, buffer);
    i64 *marks = (i64 *)managed_scope_get_attachment(app, scope, vim_buffer_marks, 26*sizeof(i64));
    if(marks){
      i64 pos = marks[c-'a'];
      if(pos > 0){
        vim_push_jump();
        Vim_Motion_Block vim_motion_block(app);
        view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
      }else{
        Scratch_Block scratch(app);
        vim_set_bottom_text(push_stringf(scratch, "Mark %c not set", c));
      }
    }
  }
  else if(in_range('A', c, 'Z'+1)){
    vim_push_jump();
    Vim_Global_Mark mark = vim_global_marks[c-'A'];
    if(mark.buffer_id.id){
      vim_push_jump();
      view_set_buffer(app, view, mark.buffer_id.id, 0);
      view_set_cursor_and_preferred_x(app, view, seek_pos(mark.pos));
    }else{
      Scratch_Block scratch(app);
      vim_set_bottom_text(push_stringf(scratch, "Mark %c not set", c));
    }
  }
  else{
    // TODO(BYP): Special marks?
    i64 pos = -1;
    switch(c){
      case '\'':{} break;  // last jump pos
      case '`': {} break;  // last jump pos
      case '^': {} break;  // last insert pos
      case '.': {} break;  // last change pos
      case '[': {} break;  // last edit begin
      case ']': {} break;  // last edit end
      case '<': {} break;  // last Visual begin
      case '>': {} break;  // last Visual end
      //case ' ': { cursor_mark_swap(app); } break;
    }
    if(pos > 0){
      ;
    }
  }
}

VIM_COMMAND_SIG(vim_open_file_in_quotes){
  vim_push_jump();
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  if(buffer_exists(app, buffer)){
    Scratch_Block scratch(app);

    i64 pos = view_get_cursor_pos(app, view);

    vim_state.params.consume_char = '"';
    vim_state.params.clusivity = VIM_Inclusive;
    Range_i64 range = vim_scan_object_quotes(app, buffer, pos);
    range.min++;
    String_Const_u8 quoted_name = push_buffer_range(app, scratch, buffer, range);

    String_Const_u8 file_name = push_buffer_file_name(app, scratch, buffer);
    String_Const_u8 path = string_remove_last_folder(file_name);

    if(character_is_slash(string_get_character(path, path.size-1))){
      path = string_chop(path, 1);
    }

    String_Const_u8 new_file_name = push_u8_stringf(scratch, "%.*s/%.*s", string_expand(path), string_expand(quoted_name));

    vim_push_jump();
    view = get_next_view_looped_primary_panels(app, view, Access_Always);
    if(view && view_open_file(app, view, new_file_name, true)){
      view_set_active(app, view);
    }
  }
}

function void vim_move_selection(Application_Links *app, Scan_Direction direction){
  View_ID view = get_active_view(app, Access_ReadWriteVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
  i64 max_line = buffer_get_line_count(app, buffer);
  i64 max_pos = buffer_get_size(app, buffer);
  i64 N = vim_consume_number();
  b32 forward = direction == Scan_Forward;

  i64 cursor_pos = view_get_cursor_pos(app, view);
  i64 mark_pos = view_get_mark_pos(app, view);
  Range_i64 range = Ii64(cursor_pos, mark_pos);
  i64 line0 = get_line_number_from_pos(app, buffer, range.min);
  i64 line1 = get_line_number_from_pos(app, buffer, range.max);
  if(!forward ? line0 <= 1 : max_line <= line1){ return; }

  Range_i64 copy_range = range_union(get_line_pos_range(app, buffer, line0),
                                     get_line_pos_range(app, buffer, line1));
  copy_range.max += buffer_get_char(app, buffer, copy_range.max) == '\r';
  copy_range.max += 1;

  i64 paste_pos = (forward ?
                   get_line_pos_range(app, buffer, line1 + N).max + 1 :
                   get_line_pos_range(app, buffer, line0 - N).min);
  paste_pos = Min(paste_pos, max_pos);

  Scratch_Block scratch(app);
  String_Const_u8 copy_string = push_buffer_range(app, scratch, buffer, copy_range);
  History_Group group = history_group_begin(app, buffer);
  if(forward){
    buffer_replace_range(app, buffer, Ii64(paste_pos), copy_string);
    buffer_replace_range(app, buffer, copy_range, string_u8_empty);
  }else{
    buffer_replace_range(app, buffer, copy_range, string_u8_empty);
    buffer_replace_range(app, buffer, Ii64(paste_pos), copy_string);
  }
  i64 offset = paste_pos - (forward ? copy_range.max : copy_range.min);
  view_set_cursor(app, view, seek_pos(cursor_pos + offset));
  view_set_mark(app, view, seek_pos(mark_pos + offset));
  history_group_end(group);
}

VIM_COMMAND_SIG(vim_move_selection_up){   vim_move_selection(app, Scan_Backward); }
VIM_COMMAND_SIG(vim_move_selection_down){ vim_move_selection(app, Scan_Forward); }


function i32 vim_macro_index(u8 c){
  return((character_to_lower(c) - 'a') + 26*in_range('A', c, 'Z'+1));
}

VIM_COMMAND_SIG(vim_toggle_macro){
  if(vim_state.macro_char){
    Buffer_ID buffer = get_keyboard_log_buffer(app);
    i64 end = buffer_get_size(app, buffer);
    Buffer_Cursor cursor = buffer_compute_cursor(app, buffer, seek_pos(end));
    Buffer_Cursor back_cursor = buffer_compute_cursor(app, buffer, seek_line_col(cursor.line - 1, 1));
    vim_macros[vim_macro_index(vim_state.macro_char)].max = back_cursor.pos;
    vim_state.macro_char = 0;
  }else{
    if(vim_state.macro_char){ return; }
    u8 c = vim_query_user_key(app, string_u8_litexpr("-- SELECT MACRO TO RECORD --"));
    if(in_range('a', c, 'z'+1) || in_range('A', c, 'Z'+1)){
      vim_state.macro_char = c;
      i32 index = vim_macro_index(c);
      vim_macros[index].min = buffer_get_size(app, get_keyboard_log_buffer(app));
    }
  }
}

VIM_COMMAND_SIG(vim_play_macro){
  u8 c = vim_query_user_key(app, string_u8_litexpr("-- SELECT MACRO TO PLAY --"));
  if(c == '@'){ c = vim_state.prev_macro; }
  if(in_range('a', c, 'z'+1) || in_range('A', c, 'Z'+1)){
    i32 index = vim_macro_index(c);
    Range_i64 range = vim_macros[index];
    if(range.min == 0 || range.max == 0){ return; }
    vim_state.prev_macro = c;

    Buffer_ID key_buffer = get_keyboard_log_buffer(app);
    Scratch_Block scratch(app);
    String_Const_u8 macro = push_buffer_range(app, scratch, key_buffer, range);

    View_ID view = get_active_view(app, Access_Always);
    Buffer_ID buffer = view_get_buffer(app, view, Access_Always);
    History_Group group = history_group_begin(app, buffer);
    keyboard_macro_play(app, macro);
    history_group_end(group);  // assumes macro only modifies current buffer
  }
}

/// Vim command aliases
CUSTOM_COMMAND_SIG(qa)  CUSTOM_DOC("Vim: Attempt to exit") { exit_4coder(app); }
CUSTOM_COMMAND_SIG(e)   CUSTOM_DOC("Vim: Open file") { interactive_open_or_new(app); }
CUSTOM_COMMAND_SIG(b)   CUSTOM_DOC("Vim: Open file") { interactive_open_or_new(app); }
CUSTOM_COMMAND_SIG(vs)  CUSTOM_DOC("Vim: Vertical split") { open_panel_vsplit(app); }
CUSTOM_COMMAND_SIG(sp)  CUSTOM_DOC("Vim: Horizontal split") { open_panel_hsplit(app); }
CUSTOM_COMMAND_SIG(s)   CUSTOM_DOC("Vim: Substitute"){ replace_in_buffer(app); }
CUSTOM_COMMAND_SIG(w)   CUSTOM_DOC("Vim: Saves the buffer") { save(app); }
CUSTOM_COMMAND_SIG(wq)  CUSTOM_DOC("Vim: Saves and quits the buffer") { save(app); q(app); }
CUSTOM_COMMAND_SIG(wqa) CUSTOM_DOC("Vim: Saves and quits all buffers") { save_all_dirty_buffers(app); exit_4coder(app); }
CUSTOM_COMMAND_SIG(qk)  CUSTOM_DOC("Vim: Attempt to kill buffer and close panel") { vim_try_buffer_kill(app); close_panel(app); }
CUSTOM_COMMAND_SIG(q)   CUSTOM_DOC("Vim: Close panel, exits on closing last panel")
{
  View_ID view = get_active_view(app, Access_Always);
  View_ID view_next = get_next_view_looped_primary_panels(app, view, Access_Always);
  if (view != view_next){ close_panel(app); return; }
  if (g_tab_ctx.count > 0){ TAB_close(app); return; }
  exit_4coder(app);
}
CUSTOM_COMMAND_SIG(tabnew)   CUSTOM_DOC("Vim: New Tab")   { TAB_new(app); }
CUSTOM_COMMAND_SIG(tabclose) CUSTOM_DOC("Vim: Close Tab") { TAB_close(app); }

//CUSTOM_COMMAND_SIG(jumps) CUSTOM_DOC("Vim: jump stack lister"){ vim_jump_lister(app); }