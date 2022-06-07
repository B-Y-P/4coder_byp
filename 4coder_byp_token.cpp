
function b32
byp_highlight_token(Token_Base_Kind kind){
	switch(kind){
		case TokenBaseKind_Keyword:
		//case TokenBaseKind_Preprocessor:
		case TokenBaseKind_Identifier:
		case byp_TokenKind_Primitive:
		case byp_TokenKind_ControlFlow:
		case byp_TokenKind_Struct:
		return true;
	}
	return false;
}


function ARGB_Color
byp_get_token_color_cpp(Token token){
	Managed_ID color = defcolor_text_default;
	switch (token.kind){
		case TokenBaseKind_Preprocessor:{ color = defcolor_preproc; }break;
		case TokenBaseKind_Keyword:{ color = defcolor_keyword; }break;
		case TokenBaseKind_Comment:{ color = defcolor_comment; }break;
		case TokenBaseKind_LiteralString:{ color = defcolor_str_constant; }break;
		case TokenBaseKind_LiteralInteger:{ color = defcolor_int_constant; }break;
		case TokenBaseKind_LiteralFloat:{ color = defcolor_float_constant; }break;

		case TokenBaseKind_Operator:
		case TokenBaseKind_ScopeOpen:
		case TokenBaseKind_ScopeClose:
		case TokenBaseKind_ParentheticalOpen:
		case TokenBaseKind_ParentheticalClose:
		case TokenBaseKind_StatementClose:{ color = defcolor_non_text; } break;

		case byp_TokenKind_ControlFlow:{ color = defcolor_control; }break;
		case byp_TokenKind_Primitive:{ color = defcolor_primitive; }break;
		case byp_TokenKind_Struct:{ color = defcolor_struct; }break;
	}
	// specifics override generals
	switch (token.sub_kind){
		case TokenCppKind_LiteralTrue:
		case TokenCppKind_LiteralFalse:
		{ color = defcolor_bool_constant; }break;

		case TokenCppKind_LiteralCharacter:
		case TokenCppKind_LiteralCharacterWide:
		case TokenCppKind_LiteralCharacterUTF8:
		case TokenCppKind_LiteralCharacterUTF16:
		case TokenCppKind_LiteralCharacterUTF32:
		{ color = defcolor_char_constant; }break;

		case TokenCppKind_PPIncludeFile:
		{ color = defcolor_include; }break;
	}
	return fcolor_resolve(fcolor_id(color));
}

function void
byp_draw_cpp_token_colors(Application_Links *app, Text_Layout_ID text_layout_id, Token_Array *array){
	Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
	i64 first_index = token_index_from_pos(array, visible_range.first);
	Token_Iterator_Array it = token_iterator_index(0, array, first_index);
	for(;;){
		Token *token = token_it_read(&it);
		if(token->pos >= visible_range.one_past_last){ break; }
		ARGB_Color argb = byp_get_token_color_cpp(*token);
		paint_text_color(app, text_layout_id, Ii64_size(token->pos, token->size), argb);
		if(!token_it_inc_all(&it)){ break; }
	}
}

function void byp_draw_token_colors(Application_Links *app, View_ID view, Buffer_ID buffer, Text_Layout_ID text_layout_id){
	Token_Array token_array = get_token_array_from_buffer(app, buffer);
	Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);

	Scratch_Block scratch(app);
	byp_draw_cpp_token_colors(app, text_layout_id, &token_array);

	i64 cursor_index = token_index_from_pos(&token_array, view_get_cursor_pos(app, view));
	Token_Iterator_Array it = token_iterator_index(0, token_array.tokens, token_array.count, cursor_index);
	Token *cursor_token = token_it_read(&it);
	b32 do_cursor_tok_highlight = byp_highlight_token(cursor_token->kind);

	String_Const_u8 token_string = {};
	Rect_f32 cursor_tok_rect = {};
	Vec2_f32 tok_rect_dim = {};

	if(do_cursor_tok_highlight){
		token_string = push_token_lexeme(app, scratch, buffer, cursor_token);
		cursor_tok_rect = text_layout_character_on_screen(app, text_layout_id, cursor_token->pos);
		tok_rect_dim = V2f32(cursor_token->size*rect_width(cursor_tok_rect), 2.f);
		cursor_tok_rect = Rf32_xy_wh(V2f32(cursor_tok_rect.x0, cursor_tok_rect.y1 - 2.f), tok_rect_dim);
	}

	ARGB_Color function_color = fcolor_resolve(fcolor_id(defcolor_function));
	ARGB_Color type_color     = fcolor_resolve(fcolor_id(defcolor_type));
	ARGB_Color macro_color    = fcolor_resolve(fcolor_id(defcolor_macro));
	ARGB_Color back_color     = fcolor_resolve(fcolor_id(defcolor_back));
	ARGB_Color cursor_tok_color = byp_get_token_color_cpp(*cursor_token);

	if(cursor_token->kind == TokenBaseKind_Identifier){
		String_Const_u8 lexeme = push_token_lexeme(app, scratch, buffer, cursor_token);
		Code_Index_Note *note = code_index_note_from_string(lexeme);

		if(note != 0){
			switch(note->note_kind){
				case CodeIndexNote_Function: cursor_tok_color = function_color; break;
				case CodeIndexNote_Type:     cursor_tok_color = type_color;     break;
				case CodeIndexNote_Macro:    cursor_tok_color = macro_color;    break;
			}
		}
	}

	// NOTE(BYP): @Annotations
	{
		i64 first_index = token_index_from_pos(&token_array, visible_range.first);
		Token_Iterator_Array comment_it = token_iterator_index(buffer, &token_array, first_index);
		for(;;){
			Token *token = token_it_read(&comment_it);
			if(token->pos >= visible_range.max){ break; }
			String_Const_u8 tail = {};
			if(token_it_check_and_get_lexeme(app, scratch, &comment_it, TokenBaseKind_Comment, &tail)){
				foreach(i, token->size){
					if(tail.str[i] == '@'){
						Range_i64 annot_range = Ii64(i);
						i32 j=i+1;
						for(; j<token->size; j++){
							if(character_is_whitespace(tail.str[j]) || !character_is_alpha_numeric(tail.str[j])){
								break;
							}
						}
						annot_range.max = j;
						if(annot_range.min != annot_range.max-1){
							annot_range += token->pos;
							paint_text_color(app, text_layout_id, annot_range, 0xFFFF0000);
						}
					}
				}
			}
			if(!token_it_inc_non_whitespace(&comment_it)){ break; }
		}
	}

	// NOTE(allen): Scan for TODOs and NOTEs
	b32 use_comment_keyword = def_get_config_b32(vars_save_string_lit("use_comment_keyword"));
	if(use_comment_keyword){
		Comment_Highlight_Pair pairs[] = {
			{string_u8_litexpr("NOTE"), finalize_color(defcolor_comment_pop, 0)},
			{string_u8_litexpr("TODO"), finalize_color(defcolor_comment_pop, 1)},
		};
		draw_comment_highlights(app, buffer, text_layout_id, &token_array, pairs, ArrayCount(pairs));
	}

	it = token_iterator_pos(0, &token_array, Max(0, visible_range.first-1));
	for(;;){
		Token *token = token_it_read(&it);
		if(token->pos > visible_range.max){ break; }
		String_Const_u8 lexeme = push_token_lexeme(app, scratch, buffer, token);
		Code_Index_Note *note = code_index_note_from_string(lexeme);

		if(do_cursor_tok_highlight){
			if(data_match(lexeme, token_string)){
				Rect_f32 cur_tok_rect = text_layout_character_on_screen(app, text_layout_id, token->pos);
				cur_tok_rect = Rf32_xy_wh(V2f32(cur_tok_rect.x0, cur_tok_rect.y1 - 2.f), tok_rect_dim);
				draw_rectangle(app, cur_tok_rect, 5.f, argb_color_blend(cursor_tok_color, 0.7f, back_color));
			}
		}

		if(!token_it_inc_non_whitespace(&it)){ break; }
		if(note == 0){ continue; }
		switch(note->note_kind){
			case CodeIndexNote_Function:
			paint_text_color(app, text_layout_id, Ii64_size(token->pos, token->size), function_color); break;
			case CodeIndexNote_Type:
			paint_text_color(app, text_layout_id, Ii64_size(token->pos, token->size), type_color); break;
			case CodeIndexNote_Macro:
			paint_text_color(app, text_layout_id, Ii64_size(token->pos, token->size), macro_color); break;
		}

	}
	if(do_cursor_tok_highlight){ draw_rectangle(app, cursor_tok_rect, 5.f, cursor_tok_color); }

}
