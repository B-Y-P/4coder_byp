
function void
byp_essential_mapping(Mapping *mapping, i64 global_id, i64 file_id, i64 code_id){
	MappingScope();
	SelectMapping(mapping);

	SelectMap(global_id);
	BindCore(byp_startup, CoreCode_Startup);
	BindCore(vim_try_exit, CoreCode_TryExit);
	BindCore(clipboard_record_clip, CoreCode_NewClipboardContents);
	BindMouseWheel(mouse_wheel_scroll);
	BindMouseWheel(mouse_wheel_change_face_size, KeyCode_Control);

	SelectMap(file_id);
	ParentMap(global_id);
	BindTextInput(byp_write_text_input);
	BindMouse(click_set_cursor_and_mark, MouseCode_Left);
	BindMouseRelease(click_set_cursor, MouseCode_Left);
	BindCore(click_set_cursor_and_mark, CoreCode_ClickActivateView);
	BindMouseMove(click_set_cursor_if_lbutton);

	SelectMap(code_id);
	ParentMap(file_id);
}

function void
byp_vim_bindings(Application_Links *app){

#define N bit_1
#define I bit_2
#define V bit_3
#define MAP 0
	u32 Ctl = KeyMod_Ctl;
	u32 Sft = KeyMod_Sft;
	u32 Alt = KeyMod_Alt;

	VimBind(N|MAP, redo,                            (Ctl|KeyCode_Y));
	VimBind(N|MAP, redo,                        (Ctl|Sft|KeyCode_Z));
	VimBind(N|MAP, save,                            (Ctl|KeyCode_S));
	VimBind(N|MAP, vim_line_start,                  (Sft|KeyCode_0));
	VimBind(N|MAP, byp_space,                            KeyCode_Space);
	VimBind(N|MAP, move_line_up,                    (Alt|KeyCode_K));
	VimBind(N|MAP, move_line_down,                  (Alt|KeyCode_J));
	VimBind(N|MAP, change_active_panel_backwards,   (Ctl|KeyCode_H));
	VimBind(N|MAP, change_active_panel,             (Ctl|KeyCode_L));
	VimBind(N|MAP, goto_next_jump,                  (Alt|KeyCode_N));
	VimBind(N|MAP, goto_prev_jump,              (Alt|Sft|KeyCode_N));

	VimBind(N|V|MAP, vim_bounce,                    (Ctl|KeyCode_5));
	VimBind(N|V|MAP, byp_open_current_peek,         (Alt|KeyCode_Return));

	VimBind(N|V|MAP, byp_request_title, SUB_Leader, (Sft|KeyCode_U));
	VimBind(N|MAP,   byp_request_comment,    SUB_G,      KeyCode_ForwardSlash);
	VimBind(N|MAP,   byp_request_uncomment,  SUB_G, (Sft|KeyCode_ForwardSlash));
	VimBind(V|MAP,   byp_visual_comment,                 KeyCode_ForwardSlash);
	VimBind(V|MAP,   byp_visual_uncomment,          (Sft|KeyCode_ForwardSlash));

	VimBind(N|V|MAP, vim_leader_d, SUB_Leader,           KeyCode_D);
	VimBind(N|V|MAP, vim_leader_c, SUB_Leader,           KeyCode_C);
	VimBind(N|V|MAP, vim_leader_D, SUB_Leader,      (Sft|KeyCode_D));
	VimBind(N|V|MAP, vim_leader_C, SUB_Leader,      (Sft|KeyCode_C));

#undef I
#undef N
#undef V
#undef MAP
}


function void
byp_default_bindings(Mapping *mapping, i64 global_id, i64 file_id, i64 code_id){
	MappingScope();
	SelectMapping(mapping);

	SelectMap(global_id);

	Bind(project_go_to_root_directory,  KeyCode_H, KeyCode_Control);
	Bind(save_all_dirty_buffers,        KeyCode_S, KeyCode_Control, KeyCode_Shift);
	Bind(byp_build_project,             KeyCode_M, KeyCode_Alt);
	Bind(execute_any_cli,               KeyCode_Z, KeyCode_Alt);
	Bind(execute_previous_cli,          KeyCode_Z, KeyCode_Alt, KeyCode_Shift);
	Bind(quick_swap_buffer,             KeyCode_BackwardSlash, KeyCode_Alt);
	Bind(exit_4coder,                   KeyCode_F4, KeyCode_Alt);

	Bind(byp_project_fkey_command, KeyCode_F1);
	Bind(byp_project_fkey_command, KeyCode_F2);
	Bind(byp_project_fkey_command, KeyCode_F3);
	Bind(byp_project_fkey_command, KeyCode_F4);
	Bind(byp_project_fkey_command, KeyCode_F5);
	Bind(byp_project_fkey_command, KeyCode_F6);
	Bind(byp_project_fkey_command, KeyCode_F7);
	Bind(byp_project_fkey_command, KeyCode_F8);
	Bind(byp_project_fkey_command, KeyCode_F9);
	Bind(byp_project_fkey_command, KeyCode_F10);
	Bind(byp_project_fkey_command, KeyCode_F11);
	Bind(byp_project_fkey_command, KeyCode_F12);
	Bind(byp_project_fkey_command, KeyCode_F13);
	Bind(byp_project_fkey_command, KeyCode_F14);
	Bind(byp_project_fkey_command, KeyCode_F15);
	Bind(byp_project_fkey_command, KeyCode_F16);
	Bind(toggle_fullscreen, KeyCode_F11);

	Bind(vim_interactive_open_or_new,                   KeyCode_O, KeyCode_Control);
	Bind(byp_test,                                      KeyCode_BackwardSlash, KeyCode_Control);
	Bind(increase_face_size,                            KeyCode_Equal, KeyCode_Control);
	Bind(decrease_face_size,                            KeyCode_Minus, KeyCode_Control);
	Bind(byp_reset_face_size,                           KeyCode_0, KeyCode_Control);
	Bind(vim_proj_cmd_lister,                           KeyCode_X, KeyCode_Alt, KeyCode_Shift);
	Bind(byp_toggle_set_col_ruler,                      KeyCode_BackwardSlash, KeyCode_Control, KeyCode_Shift);


	SelectMap(file_id);
	ParentMap(global_id);

	Bind(delete_char,                                   KeyCode_Delete);
	Bind(backspace_char,                                KeyCode_Backspace);
	Bind(move_up,                                       KeyCode_Up);
	Bind(move_down,                                     KeyCode_Down);
	Bind(move_left,                                     KeyCode_Left);
	Bind(move_right,                                    KeyCode_Right);
	Bind(seek_end_of_line,                              KeyCode_End);
	Bind(right_adjust_view,                             KeyCode_Home);
	Bind(move_up_to_blank_line_end,                     KeyCode_Up, KeyCode_Control);
	Bind(move_down_to_blank_line_end,                   KeyCode_Down, KeyCode_Control);
	Bind(backspace_alpha_numeric_boundary,              KeyCode_Backspace, KeyCode_Control);
	Bind(delete_alpha_numeric_boundary,                 KeyCode_Delete, KeyCode_Control);
	Bind(snipe_backward_whitespace_or_token_boundary,   KeyCode_Backspace, KeyCode_Alt);
	Bind(snipe_forward_whitespace_or_token_boundary,    KeyCode_Delete, KeyCode_Alt);
	Bind(set_mark,                                      KeyCode_Space, KeyCode_Control);
	Bind(delete_range,                                  KeyCode_D, KeyCode_Control);
	Bind(delete_line,                                   KeyCode_D, KeyCode_Control, KeyCode_Shift);
	Bind(search,                                        KeyCode_F, KeyCode_Control);
	Bind(list_all_locations,                            KeyCode_F, KeyCode_Control, KeyCode_Shift);
	Bind(list_all_substring_locations_case_insensitive, KeyCode_F, KeyCode_Alt);
	Bind(list_all_locations_of_selection,               KeyCode_G, KeyCode_Control, KeyCode_Shift);
	Bind(snippet_lister,                                KeyCode_J, KeyCode_Control);
	Bind(kill_buffer,                                   KeyCode_K, KeyCode_Control, KeyCode_Shift);
	Bind(duplicate_line,                                KeyCode_L, KeyCode_Control);
	Bind(cursor_mark_swap,                              KeyCode_M, KeyCode_Control);
	Bind(query_replace,                                 KeyCode_Q, KeyCode_Control);
	Bind(query_replace_identifier,                      KeyCode_Q, KeyCode_Control, KeyCode_Shift);
	Bind(query_replace_selection,                       KeyCode_Q, KeyCode_Alt);
	Bind(save,                                          KeyCode_S, KeyCode_Control);
	Bind(save_all_dirty_buffers,                        KeyCode_S, KeyCode_Control, KeyCode_Shift);
	Bind(search_identifier,                             KeyCode_T, KeyCode_Control);
	Bind(list_all_locations_of_identifier,              KeyCode_T, KeyCode_Control, KeyCode_Shift);
	Bind(paste_and_indent,                              KeyCode_V, KeyCode_Control);
	Bind(cut,                                           KeyCode_X, KeyCode_Control);
	Bind(redo,                                          KeyCode_Y, KeyCode_Control);
	Bind(undo,                                          KeyCode_Z, KeyCode_Control);
	Bind(if_read_only_goto_position,                    KeyCode_Return);
	Bind(if_read_only_goto_position_same_panel,         KeyCode_Return, KeyCode_Shift);
	Bind(view_jump_list_with_lister,                    KeyCode_Period, KeyCode_Control, KeyCode_Shift);

	SelectMap(code_id);
	ParentMap(file_id);

	Bind(byp_auto_complete_bracket,                     KeyCode_Return);
	Bind(byp_auto_complete_bracket,                     KeyCode_Return, KeyCode_Shift);
	Bind(comment_line_toggle,                           KeyCode_Semicolon, KeyCode_Control);
	Bind(word_complete,                                 KeyCode_Tab);
	Bind(if0_off,                                       KeyCode_I, KeyCode_Alt);
	Bind(open_matching_file_cpp,                        KeyCode_2, KeyCode_Alt);
}
