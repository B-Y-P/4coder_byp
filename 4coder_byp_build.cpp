
function void
byp_prj_exec_command(Application_Links *app, Variable_Handle cmd_var){
	Scratch_Block scratch(app);

	String_ID os_id = vars_save_string_lit(OS_NAME);

	String8 cmd = vars_string_from_var(scratch, vars_read_key(cmd_var, os_id));
	if (cmd.size > 0){
		String_ID out_id = vars_save_string_lit("out");
		String_ID footer_panel_id = vars_save_string_lit("footer_panel");
		String_ID save_dirty_files_id = vars_save_string_lit("save_dirty_files");
		String_ID cursor_at_end_id = vars_save_string_lit("cursor_at_end");

		b32 save_dirty_files = vars_b32_from_var(vars_read_key(cmd_var, save_dirty_files_id));
		if (save_dirty_files){
			save_all_dirty_buffers(app);
		}

		u32 flags = CLI_OverlapWithConflict|CLI_SendEndSignal;
		b32 cursor_at_end = vars_b32_from_var(vars_read_key(cmd_var, cursor_at_end_id));
		if (cursor_at_end){
			flags |= CLI_CursorAtEnd;
		}

		View_ID view = 0;
		Buffer_Identifier buffer_id = {};
		b32 set_fancy_font = false;
		String8 out = vars_string_from_var(scratch, vars_read_key(cmd_var, out_id));
		if(out.size > 0){
			buffer_id = buffer_identifier(out);

			b32 footer_panel = vars_b32_from_var(vars_read_key(cmd_var, footer_panel_id));
			if(footer_panel){
				set_fancy_font = true;
				vim_show_buffer_peek = false;
				vim_toggle_show_buffer_peek(app);
				vim_buffer_peek_index = 0;
				buffer_id = vim_buffer_peek_list[0].buffer_id;
                vim_buffer_peek_list[0] = {buffer_id, 1.f, 1.f};

				View_Context ctx = view_current_context(app, build_footer_panel_view_id);
				view_set_split_pixel_size(app, build_footer_panel_view_id, 0);
				ctx.render_caller = 0;
				view_alter_context(app, build_footer_panel_view_id, &ctx);
			}
			else{
				Buffer_ID buffer = buffer_identifier_to_id(app, buffer_id);
				view = get_first_view_with_buffer(app, buffer);
				if (view == 0){
					view = get_active_view(app, Access_Always);
				}
			}

			block_zero_struct(&prev_location);
			lock_jump_buffer(app, out);
		}
		else{
			// TODO(allen): fix the exec_system_command call so it can take a null buffer_id.
			buffer_id = buffer_identifier(string_u8_litexpr("*dump*"));
		}

		Variable_Handle command_list_var = vars_parent(cmd_var);
		Variable_Handle prj_var = vars_parent(command_list_var);
		String8 prj_dir = prj_path_from_project(scratch, prj_var);
		exec_system_command(app, view, buffer_id, prj_dir, cmd, flags);

	}
}

function void
byp_prj_exec_command_fkey_index(Application_Links *app, i32 fkey_index){
	// setup fkey string
	Scratch_Block scratch(app);
	String8 fkey_index_str = push_stringf(scratch, "F%d", fkey_index + 1);
	String_ID fkey_index_id = vars_save_string(fkey_index_str);

	// get command variable
	Variable_Handle cmd_name_var = vars_get_nil();

	// try user override
	{
		Variable_Handle fkey_override = def_get_config_var(vars_save_string_lit("fkey_command_override"));
		if(!vars_is_nil(fkey_override)){
			String_Const_u8 name = def_get_config_string(scratch, vars_save_string_lit("user_name"));
			String_ID user_name_id = vars_save_string(name);
			Variable_Handle user_var = vars_read_key(fkey_override, user_name_id);
			cmd_name_var = vars_read_key(user_var, fkey_index_id);
		}
	}

	// try defaults
	if(vars_is_nil(cmd_name_var)){
		Variable_Handle fkeys = def_get_config_var(vars_save_string_lit("fkey_command"));
		cmd_name_var = vars_read_key(fkeys, fkey_index_id);
	}

	String_ID cmd_id = vars_save_string(vars_string_from_var(scratch, cmd_name_var));
	Variable_Handle cmd_list = def_get_config_var(vars_save_string_lit("commands"));
	Variable_Handle cmd = vars_read_key(cmd_list, cmd_id);
	byp_prj_exec_command(app, cmd);
}


CUSTOM_COMMAND_SIG(byp_project_fkey_command)
CUSTOM_DOC("Runs project commands but uses proper footer build panel")
{
	ProfileScope(app, "project fkey command");
	User_Input input = get_current_input(app);
	b32 got_ind = false;
	i32 ind = 0;
	if(input.event.kind == InputEventKind_KeyStroke){
		if(KeyCode_F1 <= input.event.key.code && input.event.key.code <= KeyCode_F16){
			ind = (input.event.key.code - KeyCode_F1);
			got_ind = true;
		}
		else if(KeyCode_1 <= input.event.key.code && input.event.key.code <= KeyCode_9){
			ind = (input.event.key.code - '1');
			got_ind = true;
		}
		else if(input.event.key.code == KeyCode_0){
			ind = 9;
			got_ind = true;
		}
		if(got_ind){
			byp_prj_exec_command_fkey_index(app, ind);
		}
	}
}

CUSTOM_COMMAND_SIG(byp_build_project)
CUSTOM_DOC("Compiles project")
{
	View_ID view = get_active_view(app, Access_Always);
	Buffer_ID buffer = view_get_buffer(app, view, Access_Always);

	View_ID build_view = get_or_open_build_panel(app);
	standard_search_and_build(app, build_view, buffer);

	vim_show_buffer_peek = false;
	vim_toggle_show_buffer_peek(app);
	vim_buffer_peek_index = 0;
	vim_buffer_peek_list[0] = {buffer_identifier(string_u8_litexpr("*compilation*")), 1.f, 1.f};

	View_Context ctx = view_current_context(app, build_footer_panel_view_id);
	view_set_split_pixel_size(app, build_footer_panel_view_id, 0);
	ctx.render_caller = 0;
	view_alter_context(app, build_footer_panel_view_id, &ctx);

	block_zero_struct(&prev_location);
	lock_jump_buffer(app, string_u8_litexpr("*compilation*"));
	//close_build_footer_panel(app);
}
