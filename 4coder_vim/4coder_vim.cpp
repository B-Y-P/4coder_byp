
VIM_REQUEST_SIG(vim_apply_none){}
VIM_REQUEST_SIG(vim_apply_yank){
  if(vim_state.params.selected_reg){
    vim_state.params.selected_reg->edit_type = vim_state.params.edit_type;
    vim_copy(app, view, buffer, range, vim_state.params.selected_reg);
  }
}

VIM_REQUEST_SIG(vim_apply_delete){
  vim_apply_yank(app, view, buffer, range);
  if(vim_state.params.edit_type != EDIT_Block && vim_state.params.selected_reg){
    Vim_Register *dst = &vim_registers.small_delete;
    if(vim_state.params.edit_type != EDIT_CharWise){
      dst = vim_registers.cycle;
      vim_push_reg_cycle(app);
    }
    vim_register_copy(dst, vim_state.params.selected_reg);
    vim_update_registers(app);
  }
  buffer_replace_range(app, buffer, range, string_u8_empty);
  if(vim_state.params.edit_type == EDIT_LineWise){
    i64 pos = get_line_side_pos_from_pos(app, buffer, view_get_cursor_pos(app, view), Side_Min);
    view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
  }
}

VIM_REQUEST_SIG(vim_apply_change){
  vim_enter_insert_mode(app);
  vim_apply_delete(app, view, buffer, range);
}

VIM_REQUEST_SIG(vim_apply_upper){
  Scratch_Block scratch(app);
  String_Const_u8 string = push_buffer_range(app, scratch, buffer, range);
  string = string_mod_upper(string);
  buffer_replace_range(app, buffer, range, string);
  buffer_post_fade(app, buffer, 0.667f, range, fcolor_resolve(fcolor_id(defcolor_paste)));
}

VIM_REQUEST_SIG(vim_apply_lower){
  Scratch_Block scratch(app);
  String_Const_u8 string = push_buffer_range(app, scratch, buffer, range);
  string = string_mod_lower(string);
  buffer_replace_range(app, buffer, range, string);
  buffer_post_fade(app, buffer, 0.667f, range, fcolor_resolve(fcolor_id(defcolor_paste)));
}

VIM_REQUEST_SIG(vim_apply_replace){
  Scratch_Block scratch(app);
  String_Const_u8 string = push_buffer_range(app, scratch, buffer, range);
  foreach(i, range_size(range)){
    if(string.str[i] != '\n'){ string.str[i] = vim_state.params.seek.character; }
  }
  buffer_replace_range(app, buffer, range, string);
  buffer_post_fade(app, buffer, 0.667f, range, fcolor_resolve(fcolor_id(defcolor_paste)));
}

VIM_REQUEST_SIG(vim_apply_indent){
  i64 line0 = get_line_number_from_pos(app, buffer, range.min);
  i64 line1 = get_line_number_from_pos(app, buffer, range.max);
  line1 += (line0 == line1);
  History_Group history_group = history_group_begin(app, buffer);
  for(i64 l=line0; l<line1; l++){
    i64 pos = get_line_start_pos(app, buffer, l);
    buffer_replace_range(app, buffer, Ii64(pos), string_u8_litexpr("\t"));
  }
  history_group_end(history_group);
}

VIM_REQUEST_SIG(vim_apply_outdent){
  i64 line0 = get_line_number_from_pos(app, buffer, range.min);
  i64 line1 = get_line_number_from_pos(app, buffer, range.max);
  line1 += (line0 == line1);
  History_Group history_group = history_group_begin(app, buffer);
  for(i64 l=line0; l<line1; l++){
    i64 pos = get_line_start_pos(app, buffer, l);
    Range_i64 tab_range = Ii64(pos, pos + (buffer_get_char(app, buffer, pos) == '\t'));
    buffer_replace_range(app, buffer, tab_range, string_u8_empty);
  }
  history_group_end(history_group);
}

VIM_REQUEST_SIG(vim_apply_auto_indent){
  auto_indent_buffer(app, buffer, range);
  buffer_post_fade(app, buffer, 0.667f, range, fcolor_resolve(fcolor_id(defcolor_paste)));
}


VIM_REQUEST_SIG(vim_apply_toggle_case){
  Scratch_Block scratch(app);
  String_Const_u8 string = push_buffer_range(app, scratch, buffer, range);
  foreach(i, i64(string.size)){
    string.str[i] = character_toggle_case(string.str[i]);
  }
  buffer_replace_range(app, buffer, range, string);
  buffer_post_fade(app, buffer, 0.667f, range, fcolor_resolve(fcolor_id(defcolor_paste)));
}

VIM_REQUEST_SIG(vim_apply_fold){
  //fold_push(app, buffer, range);
}

function void vim_replace_begin(Application_Links *app){
  View_ID view = get_active_view(app, Access_ReadWriteVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
  vim_history_group = history_group_begin(app, buffer);
  vim_state.mode = VIM_Replace;
  set_mark(app);
}

function void vim_replace_end(Application_Links *app){
  // TODO: vim_consume_number()
  vim_normal_mode(app);
  history_group_end(vim_history_group);
}

function void vim_replace_key(Application_Links *app){
  User_Input input = get_current_input(app);
  if(input.event.kind == InputEventKind_TextInsert){
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i64 p0 = view_get_cursor_pos(app, view);
    i64 p1 = p0+1;
    if(buffer_get_char(app, buffer, p0) == '\n'){ p1 = p0; }
    if(buffer_get_char(app, buffer, p0) == '\r'){ p1 = p0; }
    if(buffer_get_size(app, buffer)     ==  p0 ){ p1 = p0; }
    if(string_match(input.event.text.string, string_u8_litexpr("\n"))){ p1 = p0; }
    buffer_replace_range(app, buffer, Ii64(p0, p1), input.event.text.string);
    move_right(app);
  }
}

function void vim_replace_undo_char(Application_Links *app){
  View_ID view = get_active_view(app, Access_ReadWriteVisible);
  if(view_get_cursor_pos(app, view) != view_get_mark_pos(app, view)){
    undo(app);
  }
}

function void vim_replace_undo_word(Application_Links *app){
  View_ID view = get_active_view(app, Access_ReadWriteVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
  i64 cursor_pos = view_get_cursor_pos(app, view);
  i64 mark_pos = view_get_mark_pos(app, view);
  for(;;){
    if(cursor_pos == mark_pos){ break; }
    if(character_is_whitespace(buffer_get_char(app, buffer, cursor_pos-1))){ undo(app); break; }
    undo(app);
    cursor_pos = view_get_cursor_pos(app, view);
  }
}

function void vim_replace_undo_all(Application_Links *app){
  buffer_history_set_current_state_index(app, vim_history_group.buffer, vim_history_group.first);
}

function void vim_table_builtin(Table* t){
  table_clear(t);
  foreach(i, VIM_MODE_COUNT){
    if(i == VIM_Insert){ continue; }
    foreach(j, VIM_SUBMODE_COUNT){
      vim_map_bind_text(t, u32(i), u32(j), no_op);
    }
  }

  vim_map_bind_key(t, VIM_Normal,   0, vim_replace_begin,     Sft,     KeyCode_R);
  vim_map_bind_key(t, VIM_Replace,  0, vim_replace_end,       0,       KeyCode_Escape);
  vim_map_bind_key(t, VIM_Replace,  0, vim_replace_undo_char, 0,       KeyCode_Backspace);
  vim_map_bind_key(t, VIM_Replace,  0, vim_replace_undo_word, Ctl,     KeyCode_Backspace);
  vim_map_bind_key(t, VIM_Replace,  0, vim_replace_undo_all,  Ctl|Sft, KeyCode_Backspace);
  vim_map_bind_key(t, VIM_Replace,  0, vim_replace_undo_all,  Ctl,     KeyCode_U);
  vim_map_bind_text(t, VIM_Replace, 0, vim_replace_key);
}

function void
vim_init(Application_Links *app){
  init_keycode_lut();
  vim_reset_bottom_text();
  vim_reset_state();
  vim_state.params.seek.direction = Scan_Forward;
  vim_table = {};
  vim_table_builtin(&vim_table);

  {
    default_color_table.arrays[defcolor_vim_filebar_pop]      = default_color_table.arrays[defcolor_mark];
    default_color_table.arrays[defcolor_vim_chord_text]       = default_color_table.arrays[defcolor_text_default];
    default_color_table.arrays[defcolor_vim_chord_unresolved] = default_color_table.arrays[defcolor_mark];
    default_color_table.arrays[defcolor_vim_chord_error]      = default_color_table.arrays[defcolor_special_character];
  }

#if VIM_USE_REIGSTER_BUFFER
  Buffer_ID reg_buffer = create_buffer(app, string_u8_litexpr("*registers*"),
  BufferCreate_NeverAttachToFile|BufferCreate_AlwaysNew);
  buffer_set_setting(app, reg_buffer, BufferSetting_ReadOnly, true);
  buffer_set_setting(app, reg_buffer, BufferSetting_Unkillable, true);
  buffer_set_setting(app, reg_buffer, BufferSetting_Unimportant, true);
#endif

  vim_request_access[REQUEST_None]       = Access_Always;
  vim_request_access[REQUEST_Yank]       = Access_ReadVisible;
  vim_request_access[REQUEST_Delete]     = Access_ReadWriteVisible;
  vim_request_access[REQUEST_Change]     = Access_ReadWriteVisible;
  vim_request_access[REQUEST_Upper]      = Access_ReadWriteVisible;
  vim_request_access[REQUEST_Lower]      = Access_ReadWriteVisible;
  vim_request_access[REQUEST_Replace]    = Access_ReadWriteVisible;
  vim_request_access[REQUEST_ToggleCase] = Access_ReadWriteVisible;
  vim_request_access[REQUEST_Indent]     = Access_ReadWriteVisible;
  vim_request_access[REQUEST_Outdent]    = Access_ReadWriteVisible;
  vim_request_access[REQUEST_AutoIndent] = Access_ReadWriteVisible;
  vim_request_access[REQUEST_Fold]       = Access_ReadVisible;

  vim_request_vtable[REQUEST_None]       = vim_apply_none;
  vim_request_vtable[REQUEST_Yank]       = vim_apply_yank;
  vim_request_vtable[REQUEST_Delete]     = vim_apply_delete;
  vim_request_vtable[REQUEST_Change]     = vim_apply_change;
  vim_request_vtable[REQUEST_Upper]      = vim_apply_upper;
  vim_request_vtable[REQUEST_Lower]      = vim_apply_lower;
  vim_request_vtable[REQUEST_Replace]    = vim_apply_replace;
  vim_request_vtable[REQUEST_ToggleCase] = vim_apply_toggle_case;
  vim_request_vtable[REQUEST_Indent]     = vim_apply_indent;
  vim_request_vtable[REQUEST_Outdent]    = vim_apply_outdent;
  vim_request_vtable[REQUEST_AutoIndent] = vim_apply_auto_indent;
  vim_request_vtable[REQUEST_Fold]       = vim_apply_fold;

  vim_text_object_vtable[TEXT_OBJECT_para] = {'p', (Vim_Text_Object_Func *)vim_object_para};
  vim_text_object_vtable[TEXT_OBJECT_word] = {'w', (Vim_Text_Object_Func *)vim_object_word};
  vim_text_object_vtable[TEXT_OBJECT_Word] = {'W', (Vim_Text_Object_Func *)vim_object_WORD};

  foreach(i,ArrayCount(vim_request_vtable)){ Assert(vim_request_vtable[i] != 0); }
  foreach(i,ArrayCount(vim_text_object_vtable)){ Assert(vim_text_object_vtable[i].func != 0); }

  vim_register_copy(&vim_registers.small_delete, string_u8_empty);
  vim_register_copy(&vim_registers.insert, string_u8_empty);
  for(i32 i=i32(vim_registers.digit - vim_registers.r); i<ArrayCount(vim_registers.r); i++){
    vim_registers.r[i].flags |= REGISTER_ReadOnly;
  }
}

function String_Const_u8 vim_get_bot_string(){
  String_Const_u8 result = g_qol_bot_string.string;

  if(vim_is_querying_user_key || g_qol_lister != 0){ return result; }

  switch(vim_state.mode){
    case VIM_Insert:        result = string_u8_litexpr("-- INSERT --"); break;
    case VIM_Replace:       result = string_u8_litexpr("-- REPLACE --"); break;
    case VIM_Visual:{
      switch(vim_state.params.edit_type){
        case EDIT_CharWise: result = string_u8_litexpr("-- VISUAL --");       break;
        case EDIT_LineWise: result = string_u8_litexpr("-- VISUAL LINE --");  break;
        case EDIT_Block:    result = string_u8_litexpr("-- VISUAL BLOCK --"); break;
      }
    } break;
  }

  if(vim_state.macro_char){
    local_persist u8 macro_string_buffer[] = "-- RECORDING   --";
    macro_string_buffer[13] = vim_state.macro_char;
    result = SCu8(macro_string_buffer, ArrayCount(macro_string_buffer)-1);
  }

  return result;
}