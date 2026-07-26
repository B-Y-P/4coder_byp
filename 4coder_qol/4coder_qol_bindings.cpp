

function void
qol_setup_essential_mapping(Mapping *mapping, i64 global_id, i64 file_id, i64 code_id){
  MappingScope();
  SelectMapping(mapping);

  SelectMap(global_id);
  BindCore(qol_startup, CoreCode_Startup);
  BindCore(qol_try_exit, CoreCode_TryExit);
  BindCore(clipboard_record_clip, CoreCode_NewClipboardContents);
  BindMouseWheel(qol_scroll_hovered);
  BindMouseWheel(mouse_wheel_change_face_size, KeyCode_Control);

  SelectMap(file_id);
  ParentMap(global_id);
  MC_BindTextInput(qol_write_text_input);

  BindMouse(MM_Wrap(click_set_cursor_and_mark), MouseCode_Left);
  BindMouseRelease(MM_Wrap(click_set_cursor), MouseCode_Left);
  BindCore(MM_Wrap(click_set_cursor_and_mark), CoreCode_ClickActivateView);
  BindMouseMove(MM_Wrap(click_set_cursor_if_lbutton));

  SelectMap(code_id);
  ParentMap(file_id);
  MC_BindTextInput(qol_write_text_and_auto_indent);
}

function void
qol_setup_default_mapping(Mapping *mapping, i64 global_id, i64 file_id, i64 code_id){
  MappingScope();
  SelectMapping(mapping);

  SelectMap(global_id);
  Bind(keyboard_macro_start_recording,  KeyCode_U, KeyCode_Control);
  Bind(keyboard_macro_finish_recording, KeyCode_U, KeyCode_Control, KeyCode_Shift);
  Bind(keyboard_macro_replay,           KeyCode_U, KeyCode_Alt);
  Bind(change_active_panel,           KeyCode_Comma, KeyCode_Control);
  Bind(change_active_panel_backwards, KeyCode_Comma, KeyCode_Control, KeyCode_Shift);
  Bind(interactive_new,               KeyCode_N, KeyCode_Control);
  Bind(interactive_open_or_new,       KeyCode_O, KeyCode_Control);
  Bind(open_in_other,                 KeyCode_O, KeyCode_Alt);
  Bind(interactive_kill_buffer,       KeyCode_K, KeyCode_Control);
  Bind(interactive_switch_buffer,     KeyCode_I, KeyCode_Control);
  Bind(project_go_to_root_directory,  KeyCode_H, KeyCode_Control);
  Bind(change_to_build_panel,         KeyCode_Period, KeyCode_Alt);
  Bind(goto_next_jump,                KeyCode_N, KeyCode_Alt);
  Bind(goto_prev_jump,                KeyCode_N, KeyCode_Alt, KeyCode_Shift);
  Bind(goto_first_jump,               KeyCode_M, KeyCode_Alt, KeyCode_Shift);
  Bind(execute_any_cli,               KeyCode_Z, KeyCode_Alt);
  Bind(execute_previous_cli,          KeyCode_Z, KeyCode_Alt, KeyCode_Shift);
  Bind(command_lister,                KeyCode_X, KeyCode_Alt);
  Bind(project_command_lister,        KeyCode_X, KeyCode_Alt, KeyCode_Shift);
  Bind(quick_swap_buffer,             KeyCode_BackwardSlash, KeyCode_Alt);
  Bind(jump_to_last_point,            KeyCode_P, KeyCode_Control);
  Bind(list_all_functions_current_buffer_lister, KeyCode_I, KeyCode_Control, KeyCode_Shift);
  Bind(exit_4coder,          KeyCode_F4, KeyCode_Alt);
  Bind(project_fkey_command, KeyCode_F1);
  Bind(project_fkey_command, KeyCode_F2);
  Bind(project_fkey_command, KeyCode_F3);
  Bind(project_fkey_command, KeyCode_F4);
  Bind(project_fkey_command, KeyCode_F5);
  Bind(project_fkey_command, KeyCode_F6);
  Bind(project_fkey_command, KeyCode_F7);
  Bind(project_fkey_command, KeyCode_F8);
  Bind(project_fkey_command, KeyCode_F9);
  Bind(project_fkey_command, KeyCode_F10);
  Bind(project_fkey_command, KeyCode_F11);
  Bind(project_fkey_command, KeyCode_F12);
  Bind(project_fkey_command, KeyCode_F13);
  Bind(project_fkey_command, KeyCode_F14);
  Bind(project_fkey_command, KeyCode_F15);
  Bind(project_fkey_command, KeyCode_F16);
  MC_Bind(save_all_dirty_buffers,     KeyCode_S, KeyCode_Control, KeyCode_Shift);
  MC_Bind(close_build_panel,          KeyCode_Comma, KeyCode_Alt);
  MC_Bind(build_in_build_panel,       KeyCode_M, KeyCode_Alt);
  MC_Bind(toggle_filebar,             KeyCode_B, KeyCode_Alt);
  MC_Bind(MC_end_multi,               KeyCode_Escape);

  Bind(qol_jump_down,              KeyCode_LeftBracket, KeyCode_Control);
  Bind(qol_jump_up,                KeyCode_RightBracket, KeyCode_Control);

  Bind(qol_bview_active_to_bottom, KeyCode_Return, KeyCode_Alt, KeyCode_Shift);
  Bind(qol_bview_bottom_to_active, KeyCode_Return, KeyCode_Alt);
  Bind(qol_bview_toggle,           KeyCode_Return, KeyCode_Control, KeyCode_Shift);
  Bind(qol_bview_scroll_up,        KeyCode_PageUp, KeyCode_Control, KeyCode_Shift);
  Bind(qol_bview_scroll_down,      KeyCode_PageDown, KeyCode_Control, KeyCode_Shift);

  SelectMap(file_id);
  Bind(qol_seek_char_query, KeyCode_F, KeyCode_Control);
  MC_Bind(delete_char,         KeyCode_Delete);
  MC_Bind(backspace_char,      KeyCode_Backspace);
  MC_Bind(move_up,             KeyCode_Up);
  MC_Bind(move_down,           KeyCode_Down);
  MC_Bind(move_left,           KeyCode_Left);
  MC_Bind(move_right,          KeyCode_Right);
  MC_Bind(seek_end_of_line,    KeyCode_End);
  MC_Bind(qol_home,            KeyCode_Home);
  Bind(page_up,                KeyCode_PageUp);
  Bind(page_down,              KeyCode_PageDown);
  Bind(goto_beginning_of_file, KeyCode_PageUp, KeyCode_Control);
  Bind(goto_end_of_file,       KeyCode_PageDown, KeyCode_Control);
  Bind(move_up_to_blank_line_end,        KeyCode_Up, KeyCode_Control);
  Bind(move_down_to_blank_line_end,      KeyCode_Down, KeyCode_Control);
  Bind(qol_find_divider_up,              KeyCode_Up, KeyCode_Control, KeyCode_Shift);
  Bind(qol_find_divider_down,            KeyCode_Down, KeyCode_Control, KeyCode_Shift);
  MC_Bind(qol_ctrl_forwards,        KeyCode_Right, KeyCode_Control);
  MC_Bind(qol_ctrl_backwards,       KeyCode_Left, KeyCode_Control);
  MC_Bind(qol_ctrl_delete,          KeyCode_Delete, KeyCode_Control);
  MC_Bind(qol_ctrl_backspace,       KeyCode_Backspace, KeyCode_Control);
  MC_Bind(backspace_alpha_numeric_boundary, KeyCode_Backspace, KeyCode_Alt);
  MC_Bind(delete_alpha_numeric_boundary,    KeyCode_Delete, KeyCode_Alt);
  MC_Bind(set_mark,                 KeyCode_Space, KeyCode_Control);
  MC_Bind(copy,                     KeyCode_C, KeyCode_Control);
  MC_Bind(delete_range,             KeyCode_D, KeyCode_Control);
  MC_Bind(delete_line,              KeyCode_D, KeyCode_Control, KeyCode_Shift);
  MC_Bind(center_view,              KeyCode_E, KeyCode_Control);
  MC_Bind(left_adjust_view,         KeyCode_E, KeyCode_Control, KeyCode_Shift);
  Bind(replace_in_range,            KeyCode_A, KeyCode_Control);
  Bind(qol_search,                  KeyCode_F, KeyCode_Control);
  Bind(qol_reverse_search,          KeyCode_R, KeyCode_Control);
  Bind(list_all_locations,          KeyCode_F, KeyCode_Control, KeyCode_Shift);
  Bind(list_all_substring_locations_case_insensitive, KeyCode_F, KeyCode_Alt);
  Bind(goto_line,                   KeyCode_G, KeyCode_Control);
  Bind(list_all_locations_of_selection,  KeyCode_G, KeyCode_Control, KeyCode_Shift);
  Bind(snippet_lister,              KeyCode_J, KeyCode_Control);
  Bind(kill_buffer,                 KeyCode_K, KeyCode_Control, KeyCode_Shift);
  Bind(duplicate_line,              KeyCode_L, KeyCode_Control);
  MC_Bind(cursor_mark_swap,         KeyCode_M, KeyCode_Control);
  Bind(reopen,                      KeyCode_O, KeyCode_Control, KeyCode_Shift);
  Bind(query_replace,               KeyCode_Q, KeyCode_Control);
  Bind(query_replace_identifier,    KeyCode_Q, KeyCode_Control, KeyCode_Shift);
  Bind(query_replace_selection,     KeyCode_Q, KeyCode_Alt);
  Bind(list_all_locations_of_identifier, KeyCode_T, KeyCode_Control, KeyCode_Shift);
  Bind(redo,                        KeyCode_Y, KeyCode_Control);
  Bind(undo,                        KeyCode_Z, KeyCode_Control);
  Bind(view_buffer_other_panel,     KeyCode_1, KeyCode_Control);
  Bind(swap_panels,                 KeyCode_2, KeyCode_Control);
  MC_Bind(qol_modal_return,         KeyCode_Return);
  Bind(if_read_only_goto_position_same_panel, KeyCode_Return, KeyCode_Shift);
  Bind(view_jump_list_with_lister,  KeyCode_Period, KeyCode_Control, KeyCode_Shift);
  Bind(qol_move_selection_up,       KeyCode_Up, KeyCode_Alt);
  Bind(qol_move_selection_down,     KeyCode_Down, KeyCode_Alt);

  MC_Bind(save,                        KeyCode_S, KeyCode_Control);
  MC_Bind(save_all_dirty_buffers,      KeyCode_S, KeyCode_Control, KeyCode_Shift);
  MC_Bind(paste,                       KeyCode_V, KeyCode_Control);
  MC_Bind(cut,                         KeyCode_X, KeyCode_Control);

  Bind(MC_add_at_pos,            KeyCode_BackwardSlash, KeyCode_Control);
  Bind(MC_begin_multi,           KeyCode_Return, KeyCode_Control);
  Bind(MC_begin_multi_block,     KeyCode_L, KeyCode_Control, KeyCode_Shift);
  Bind(MC_up_trail,              KeyCode_Up, KeyCode_Control, KeyCode_Alt);
  Bind(MC_down_trail,            KeyCode_Down, KeyCode_Control, KeyCode_Alt);
  MC_Bind(qol_column_toggle,           KeyCode_BackwardSlash, KeyCode_Control, KeyCode_Shift);
  MC_Bind(qol_char_forward,            KeyCode_Right, KeyCode_Shift);
  MC_Bind(qol_char_backward,           KeyCode_Left, KeyCode_Shift);

  SelectMap(code_id);
  ParentMap(file_id);
  MC_Bind(move_left_alpha_numeric_or_camel_boundary,  KeyCode_Left, KeyCode_Alt);
  MC_Bind(move_right_alpha_numeric_or_camel_boundary, KeyCode_Right, KeyCode_Alt);
  MC_Bind(comment_line_toggle,        KeyCode_Semicolon, KeyCode_Control);
  MC_Bind(auto_indent_range,          KeyCode_Tab, KeyCode_Control);
  MC_Bind(auto_indent_line_at_cursor, KeyCode_Tab, KeyCode_Shift);
  Bind(word_complete,                 KeyCode_Tab);
  Bind(word_complete_prev,         KeyCode_Tab, KeyCode_Shift);
  Bind(word_complete_drop_down,       KeyCode_Tab, KeyCode_Shift, KeyCode_Control);
  MC_Bind(write_block,                KeyCode_R, KeyCode_Alt);
  MC_Bind(write_todo,                 KeyCode_T, KeyCode_Alt);
  MC_Bind(write_note,                 KeyCode_Y, KeyCode_Alt);
  Bind(qol_search_identifier,         KeyCode_F, KeyCode_Alt);
  Bind(list_all_locations_of_type_definition,               KeyCode_D, KeyCode_Alt);
  Bind(list_all_locations_of_type_definition_of_identifier, KeyCode_T, KeyCode_Alt, KeyCode_Shift);
  Bind(select_surrounding_scope,         KeyCode_LeftBracket, KeyCode_Alt);
  Bind(select_surrounding_scope_maximal, KeyCode_LeftBracket, KeyCode_Alt, KeyCode_Shift);
  Bind(select_prev_scope_absolute,       KeyCode_RightBracket, KeyCode_Alt);
  Bind(select_prev_top_most_scope,       KeyCode_RightBracket, KeyCode_Alt, KeyCode_Shift);
  Bind(select_next_scope_absolute,       KeyCode_Quote, KeyCode_Alt);
  Bind(select_next_scope_after_current,  KeyCode_Quote, KeyCode_Alt, KeyCode_Shift);
  Bind(place_in_scope,                   KeyCode_ForwardSlash, KeyCode_Alt);
  Bind(delete_current_scope,             KeyCode_Minus, KeyCode_Alt);
  Bind(if0_off,                          KeyCode_I, KeyCode_Alt);
  Bind(open_file_in_quotes,              KeyCode_1, KeyCode_Alt);
  Bind(open_matching_file_cpp,           KeyCode_2, KeyCode_Alt);
  Bind(write_zero_struct,                KeyCode_0, KeyCode_Control);
  Bind(qol_jump_to_definition,           KeyCode_W, KeyCode_Control);
}