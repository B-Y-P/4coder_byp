
global Face_ID byp_small_italic_face;
global Face_ID byp_minimal_face;


global b32 byp_show_hex_colors;
global b32 byp_relative_numbers;
global b32 byp_show_scrollbars;
global b32 byp_drop_shadow;

CUSTOM_COMMAND_SIG(byp_toggle_show_hex_colors)
CUSTOM_DOC("Toggles value for `show_hex_colors`")
{ byp_show_hex_colors ^= 1; }

CUSTOM_COMMAND_SIG(byp_toggle_relative_numbers)
CUSTOM_DOC("Toggles value for `relative_numbers`")
{ byp_relative_numbers ^= 1; }

CUSTOM_COMMAND_SIG(byp_toggle_show_scrollbars)
CUSTOM_DOC("Toggles value for `show_scrollbars`")
{ byp_show_scrollbars ^= 1; }

CUSTOM_COMMAND_SIG(byp_toggle_drop_shadow)
CUSTOM_DOC("Toggles value for `drop_shadow`")
{ byp_drop_shadow ^= 1; }


CUSTOM_COMMAND_SIG(byp_test)
CUSTOM_DOC("Just bound to the key I spam to execute whatever test code I'm working on")
{
	toggle_virtual_whitespace(app);
}

CUSTOM_COMMAND_SIG(byp_close_all_buffers)
CUSTOM_DOC("Reloads buffers")
{
	for (Buffer_ID buffer = get_buffer_next(app, 0, Access_Always);
		 buffer != 0;
		 buffer = get_buffer_next(app, buffer, Access_Always)){
        if (buffer_has_name_with_star(app, buffer)){ continue; }
		buffer_kill(app, buffer, BufferKill_AlwaysKill);
	}
}

CUSTOM_COMMAND_SIG(byp_reload_config)
CUSTOM_DOC("Reloads config.4coder file")
{
	View_ID view = get_active_view(app, Access_Always);
	Buffer_ID buffer = view_get_buffer(app, view, Access_Always);
	Face_ID face = get_face_id(app, buffer);
	Face_Description face_desc = get_face_description(app, face);
	load_config_and_apply(app, &global_config_arena, face_desc.parameters.pt_size, face_desc.parameters.hinting);
}

CUSTOM_COMMAND_SIG(byp_reload_project)
CUSTOM_DOC("Reloads the project.4coder file")
{
    Scratch_Block scratch(app);
	View_ID view = get_active_view(app, Access_Always);
	Buffer_ID buffer = view_get_buffer(app, view, Access_Always);
    String_Const_u8 path = push_buffer_file_name(app, scratch, buffer);
    String_Const_u8 file = string_front_of_path(path);
    if (string_match(file, string_u8_litexpr("project.4coder"))){
        File_Name_Data dump = dump_file(scratch, path);
        parse_project(app, scratch, dump);
    }
}

CUSTOM_COMMAND_SIG(byp_reopen_all_buffers)
CUSTOM_DOC("Reload current buffer")
{
	View_ID views[32];
	Buffer_Scroll scrolls[32];
	i64 count = 0;

	for (View_ID view = get_view_next(app, 0, Access_Always);
		 view != 0;
		 view = get_view_next(app, view, Access_Always)){
		i64 idx = count++;
		views[idx] = view;
		scrolls[idx] = view_get_buffer_scroll(app, view);
	}

	for (Buffer_ID buffer = get_buffer_next(app, 0, Access_Always);
		 buffer != 0;
		 buffer = get_buffer_next(app, buffer, Access_Always)){
        if (buffer_has_name_with_star(app, buffer)){ continue; }
		if (buffer_get_dirty_state(app, buffer) == DirtyState_UpToDate){ continue; }
		buffer_reopen(app, buffer, 0);
	}

	foreach(i, count){
		view_set_buffer_scroll(app, views[i], scrolls[i], SetBufferScroll_NoCursorChange);
	}
}

CUSTOM_COMMAND_SIG(format_all_buffers)
CUSTOM_DOC("Auto-indent and remove blank lines for all loaded buffers")
{
	for (Buffer_ID buffer = get_buffer_next(app, 0, Access_ReadWrite);
		 buffer != 0;
		 buffer = get_buffer_next(app, buffer, Access_ReadWrite)){
        if (buffer_has_name_with_star(app, buffer)){ continue; }
		auto_indent_buffer(app, buffer, buffer_range(app, buffer), Indent_ClearLine|Indent_UseTab|Indent_FullTokens);
		clean_all_lines_buffer(app, buffer, CleanAllLinesMode_RemoveBlankLines);
	}
	save_all_dirty_buffers(app);
}


CUSTOM_COMMAND_SIG(byp_reset_face_size)
CUSTOM_DOC("Resets face size to default")
{
	View_ID view = get_active_view(app, Access_Always);
	Buffer_ID buffer = view_get_buffer(app, view, Access_Always);
	Face_ID face_id = get_face_id(app, buffer);
	Face_Description description = get_face_description(app, face_id);
	description.parameters.pt_size = (i32)def_get_config_u64(app, vars_save_string_lit("default_font_size"));
	try_modify_face(app, face_id, &description);
}

// TODO: This should probably be scoped per_view and know the buffer it refers to,
// but that's to much of a fuss for something so small
global Buffer_Cursor byp_col_cursor;

CUSTOM_COMMAND_SIG(byp_toggle_set_col_ruler)
CUSTOM_DOC("Toggles the column ruler. Set to cursor column when on.")
{
	View_ID view = get_active_view(app, Access_ReadVisible);
	if(byp_col_cursor.pos == 0){
		Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
		i64 pos = view_get_cursor_pos(app, view);
		byp_col_cursor = view_compute_cursor(app, view, seek_pos(pos));
		vim_state.params.seek = {buffer_get_char(app, buffer, pos), VIM_Inclusive, -1};
	}else{
		byp_col_cursor = Buffer_Cursor{};
	}
}

CUSTOM_COMMAND_SIG(byp_space)
CUSTOM_DOC("When column ruler is set, spaces towards that, else just inserts one space")
{
	Scratch_Block scratch(app);
	if(byp_col_cursor.pos > 0){
		View_ID view = get_active_view(app, Access_ReadVisible);
		Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);

		byp_col_cursor = buffer_compute_cursor(app, buffer, seek_line_col(byp_col_cursor.line, byp_col_cursor.col));

		i64 pos = view_get_cursor_pos(app, view);
		i64 line = get_line_number_from_pos(app, buffer, pos);
		f32 col_x = view_relative_xy_of_pos(app, view, line, byp_col_cursor.pos).x;
		f32 cur_x = view_relative_xy_of_pos(app, view, line, pos).x;
		Face_ID face = get_face_id(app, buffer);
		f32 space_advance = get_face_metrics(app, face).space_advance;

		i64 N = i64((col_x - cur_x) / space_advance);
		if(N < 0){ N = 1; }
		foreach(i,N){ write_space(app); }
	}else{ write_space(app); }
}


global b32 byp_bracket_opened;

CUSTOM_COMMAND_SIG(byp_write_text_input)
CUSTOM_DOC("Inserts whatever text was used to trigger this command.")
{
	User_Input in = get_current_input(app);
	String_Const_u8 insert = to_writable(&in);
	byp_bracket_opened = insert.str[insert.size-1] == '{';
	write_text(app, insert);
}

CUSTOM_COMMAND_SIG(byp_auto_complete_bracket)
CUSTOM_DOC("Sets the right size of the view near the x position of the cursor.")
{
	View_ID view = get_active_view(app, Access_ReadWriteVisible);
	i64 pos = view_get_character_legal_pos_from_pos(app, view, view_get_cursor_pos(app, view));
	Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
	Token_Array token_array = get_token_array_from_buffer(app, buffer);
	do{
		if(token_array.tokens == 0){
			if(byp_bracket_opened){
				write_text(app, string_u8_litexpr("\n\n}"));
				move_up(app);
				byp_bracket_opened = 0;
				return;
			}else{
				break;
			}
		}

		i64 first_index = token_index_from_pos(&token_array, pos);
		Token_Iterator_Array it = token_iterator_index(0, token_array.tokens, token_array.count, first_index);
		if(!token_it_dec(&it)){ break; }

		Token *token = token_it_read(&it);
		if(token && byp_bracket_opened && buffer_get_char(app, buffer, token->pos) == '{'){
			token_it_dec(&it);
			token = token_it_read(&it);
			if(token->kind == TokenBaseKind_Identifier){
				if(!token_it_dec(&it)){ break; }
				token = token_it_read(&it);
			}
			String_Const_u8 insert = string_u8_litexpr("\n\n};");
			insert.size -= (token->kind != byp_TokenKind_Struct);
			write_text(app, insert);
			move_up(app);
			byp_bracket_opened = 0;
			return;
		}
	}while(0);

	write_text(app, string_u8_litexpr("\n"));
	byp_bracket_opened = 0;
}

function bool
byp_is_divider(String_Const_u8 lexeme){
	return (string_has_prefix(lexeme, string_u8_litexpr("//-"))  ||
			string_has_prefix(lexeme, string_u8_litexpr("// -")) ||
			string_has_prefix(lexeme, string_u8_litexpr("//--")));
}

function void
byp_find_divider(Application_Links *app, Scan_Direction direction){
	View_ID view = get_active_view(app, Access_ReadVisible);
	Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
	Token_Array tokens = get_token_array_from_buffer(app, buffer);
	if (tokens.tokens == 0){ return; }

	i64 pos = view_get_cursor_pos(app, view);
	Token_Iterator_Array it = token_iterator_pos(buffer, &tokens, pos);
	for (;;){
		Scratch_Block scratch(app);
		Token *token = token_it_read(&it);
		String_Const_u8 lexeme = {};

		b32 valid_dir = ((token->pos < pos && direction == Scan_Backward) ||
						 (token->pos > pos && direction == Scan_Forward));
		b32 has_lexeme = token_it_check_and_get_lexeme(app, scratch, &it, TokenBaseKind_Comment, &lexeme);

		if (valid_dir && has_lexeme && byp_is_divider(lexeme)){
			view_set_cursor(app, view, seek_pos(token->pos));
			return;
		}

		b32 has_next = (direction == Scan_Forward ?
						token_it_inc_non_whitespace(&it) :
						token_it_dec_non_whitespace(&it));
		if (!has_next){ break; }
	}
}

CUSTOM_COMMAND_SIG(byp_find_divider_up)
CUSTOM_DOC("Find //- divider above cursor")
{
	byp_find_divider(app, Scan_Backward);
}

CUSTOM_COMMAND_SIG(byp_find_divider_down)
CUSTOM_DOC("Find //- divider below cursor")
{
	byp_find_divider(app, Scan_Forward);
}


CUSTOM_COMMAND_SIG(explorer)
CUSTOM_DOC("Opens file explorer in hot directory")
{
	Scratch_Block scratch(app);
	String_Const_u8 hot = push_hot_directory(app, scratch);
	exec_system_command(app, 0, buffer_identifier(0), hot, string_u8_litexpr("explorer ."), 0);
}

function void
byp_load_theme(String_Const_u8 theme_name){
	Color_Table *table_ptr = get_color_table_by_name(theme_name);
	if(table_ptr){ byp_copy_color_table(&target_color_table, *table_ptr); }
}

global Vim_Buffer_Peek_Entry BYP_peek_list[VIM_ADDITIONAL_PEEK] = {
	{ buffer_identifier(string_u8_litexpr("*scratch*")), 1.f, 1.f },
	{ buffer_identifier(string_u8_litexpr("todo.txt")),  1.f, 1.f },
};

CUSTOM_COMMAND_SIG(byp_list_all_locations_selection)
CUSTOM_DOC("Lists locations of selection range")
{
	vim_normal_mode(app);
	View_ID view = get_active_view(app, Access_ReadVisible);
	Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
	Range_i64 range = get_view_range(app, view);
	range.max++;

	Scratch_Block scratch(app);
	String_Const_u8 range_string = push_buffer_range(app, scratch, buffer, range);
	list_all_locations__generic(app, range_string, ListAllLocationsFlag_CaseSensitive|ListAllLocationsFlag_MatchSubstring);
}

CUSTOM_COMMAND_SIG(byp_open_current_peek)
CUSTOM_DOC("Sets the active view to the current peeked buffer")
{
	View_ID view = get_active_view(app, Access_ReadWriteVisible);
	Buffer_ID buffer = buffer_identifier_to_id(app, vim_buffer_peek_list[vim_buffer_peek_index].buffer_id);
	view_set_buffer(app, view, buffer, SetBuffer_KeepOriginalGUI);
	vim_show_buffer_peek = 1;
	vim_toggle_show_buffer_peek(app);
}

VIM_TEXT_OBJECT_SIG(byp_object_param){
	u8 c = buffer_get_char(app, buffer, cursor_pos);
	Range_i64 range = Ii64(cursor_pos + (c == ',' || c == ';'));

	for(; range.min>0; range.min--){
		c = buffer_get_char(app, buffer, range.min);
		if(c == ',' || c == ';'){ break; }
		Scan_Direction bounce = vim_bounce_direction(c);
		if(bounce == Scan_Forward){ break; }
		if(bounce == Scan_Backward){
			range.min = vim_bounce_pair(app, buffer, range.min, c)-1;
			continue;
		}
	}

	i64 buffer_size = buffer_get_size(app, buffer);
	for(; range.max < buffer_size; range.max++){
		c = buffer_get_char(app, buffer, range.max);
		if(c == ',' || c == ';'){ break; }
		Scan_Direction bounce = vim_bounce_direction(c);
		if(bounce == Scan_Backward){ break; }
		if(bounce == Scan_Forward){
			range.max = vim_bounce_pair(app, buffer, range.max, c);
			continue;
		}
	}
	range.min++; range.max--;
	if(range.min >= range.max){ range = {}; }

	return range;
}

VIM_TEXT_OBJECT_SIG(byp_object_camel){
	Range_i64 range = {};
	Scratch_Block scratch(app);
	Range_i64 line_range = get_line_range_from_pos(app, buffer, cursor_pos);
	i64 s = line_range.min;
	u8 *line_text = push_buffer_range(app, scratch, buffer, line_range).str;
	u8 c = line_text[cursor_pos-s];
	if(!character_is_alpha_numeric(c)){ return {}; }
	cursor_pos += line_text[cursor_pos-s] == '_';
	range.min = range.max = cursor_pos;

	b32 valid=false;
	for(; range.min>0; range.min--){
		c = line_text[range.min-s];
		if(!character_is_alpha_numeric(c) || c == '_'){ valid = true; break; }
	}
	if(!valid){ return {}; }

	valid=false;
	for(; range.max>0; range.max++){
		c = line_text[range.max-s];
		if(!character_is_alpha_numeric(c) || c == '_'){ valid = true; break; }
	}
	if(!valid){ return {}; }

	range.min += (vim_state.params.clusivity != VIM_Inclusive || line_text[range.min-s] != '_');
	range.max -= (vim_state.params.clusivity != VIM_Inclusive || line_text[range.max-s] != '_');
	if(range.min >= range.max){ range = {}; }

	return range;
}

function void byp_make_vim_request(Application_Links *app, BYP_Vim_Request request){
	vim_make_request(app, Vim_Request_Type(VIM_REQUEST_COUNT + request));
}

VIM_REQUEST_SIG(byp_apply_title){
	Scratch_Block scratch(app);
	String_Const_u8 text = push_buffer_range(app, scratch, buffer, range);
	u8 prev = buffer_get_char(app, buffer, range.min-1);
	foreach(i, text.size){
		text.str[i] += u8(i32('A' - 'a')*((!character_is_alpha(prev) || prev == '_') &&
										  character_is_lower(text.str[i])));
		prev = text.str[i];
	}
	buffer_replace_range(app, buffer, range, text);
	buffer_post_fade(app, buffer, 0.667f, range, fcolor_resolve(fcolor_id(defcolor_paste)));
}

VIM_REQUEST_SIG(byp_apply_comment){
	i64 line0 = get_line_number_from_pos(app, buffer, range.min);
	i64 line1 = get_line_number_from_pos(app, buffer, range.max);
	line1 += (line0 == line1);
	History_Group history_group = history_group_begin(app, buffer);
	for(i64 l=line0; l<line1; l++){
		i64 line_start = get_pos_past_lead_whitespace_from_line_number(app, buffer, l);
		b32 has_comment = c_line_comment_starts_at_position(app, buffer, line_start);
		if(!has_comment){
			buffer_replace_range(app, buffer, Ii64(line_start), string_u8_litexpr("//"));
			buffer_post_fade(app, buffer, 0.667f, Ii64_size(line_start,2), fcolor_resolve(fcolor_id(defcolor_paste)));
		}
	}
	history_group_end(history_group);
}

VIM_REQUEST_SIG(byp_apply_uncomment){
	i64 line0 = get_line_number_from_pos(app, buffer, range.min);
	i64 line1 = get_line_number_from_pos(app, buffer, range.max);
	line1 += (line0 == line1);
	History_Group history_group = history_group_begin(app, buffer);
	for(i64 l=line0; l<line1; l++){
		i64 line_start = get_pos_past_lead_whitespace_from_line_number(app, buffer, l);
		b32 has_comment = c_line_comment_starts_at_position(app, buffer, line_start);
		if(has_comment){
			buffer_replace_range(app, buffer, Ii64_size(line_start,2), string_u8_empty);
		}
	}
	history_group_end(history_group);
}

VIM_COMMAND_SIG(byp_request_title){ byp_make_vim_request(app, BYP_REQUEST_Title); }
VIM_COMMAND_SIG(byp_request_comment){ byp_make_vim_request(app, BYP_REQUEST_Comment); }
VIM_COMMAND_SIG(byp_request_uncomment){ byp_make_vim_request(app, BYP_REQUEST_UnComment); }
VIM_COMMAND_SIG(byp_visual_comment){
	if(vim_state.mode == VIM_Visual){
		Vim_Edit_Type edit = vim_state.params.edit_type;
		byp_request_comment(app);
		vim_state.mode = VIM_Visual;
		vim_state.params.edit_type = edit;
	}
}
VIM_COMMAND_SIG(byp_visual_uncomment){
	if(vim_state.mode == VIM_Visual){
		Vim_Edit_Type edit = vim_state.params.edit_type;
		byp_request_uncomment(app);
		vim_state.mode = VIM_Visual;
		vim_state.params.edit_type = edit;
	}
}

