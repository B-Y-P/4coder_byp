
function b32
qol_highlight_token(Token_Base_Kind kind){
  switch(kind){
    case TokenBaseKind_Keyword:
    case TokenBaseKind_Identifier:
    case qol_TokenKind_Primitive:
    case qol_TokenKind_Control:
    case qol_TokenKind_Struct:
    return true;
  }
  return false;
}

function FColor
qol_get_token_color_cpp(Token token){
  Managed_ID color = defcolor_text_default;
  switch (token.kind){
    case TokenBaseKind_Preprocessor:   { color = defcolor_preproc;        }break;
    case TokenBaseKind_Keyword:        { color = defcolor_keyword;        }break;
    case TokenBaseKind_Comment:        { color = defcolor_comment;        }break;
    case TokenBaseKind_LiteralString:  { color = defcolor_str_constant;   }break;
    case TokenBaseKind_LiteralInteger: { color = defcolor_int_constant;   }break;
    case TokenBaseKind_LiteralFloat:   { color = defcolor_float_constant; }break;
    case TokenBaseKind_Operator:       { color = defcolor_operator;       }break;

    case TokenBaseKind_ScopeOpen:
    case TokenBaseKind_ScopeClose:
    case TokenBaseKind_ParentheticalOpen:
    case TokenBaseKind_ParentheticalClose:
    case TokenBaseKind_StatementClose:{ color = defcolor_non_text; } break;

    case qol_TokenKind_Control:{ color = defcolor_control; }break;
    case qol_TokenKind_Primitive:{ color = defcolor_primitive; }break;
    case qol_TokenKind_Struct:{ color = defcolor_struct; }break;
    }
    // specifics override generals
    switch (token.sub_kind){
    case TokenCppKind_LiteralTrue:
    case TokenCppKind_LiteralFalse:
    {
    color = defcolor_bool_constant;
    }break;
    case TokenCppKind_LiteralCharacter:
    case TokenCppKind_LiteralCharacterWide:
    case TokenCppKind_LiteralCharacterUTF8:
    case TokenCppKind_LiteralCharacterUTF16:
    case TokenCppKind_LiteralCharacterUTF32:
    {
    color = defcolor_char_constant;
    }break;
    case TokenCppKind_PPIncludeFile:
    {
    color = defcolor_include;
    }break;
    }
    return fcolor_id(color);
    }

    function void
    qol_draw_cpp_token_colors(Application_Links *app, View_ID view, Buffer_ID buffer, Text_Layout_ID text_layout_id, Token_Array *array){
    Scratch_Block scratch(app);
    Token *cursor_token = token_from_pos(array, view_get_cursor_pos(app, view));
    b32 do_highlight_cur_token = qol_highlight_token(cursor_token->kind);

    ARGB_Color cl_type   = fcolor_resolve(fcolor_id(defcolor_type));
    ARGB_Color cl_func   = fcolor_resolve(fcolor_id(defcolor_function));
    ARGB_Color cl_macro  = fcolor_resolve(fcolor_id(defcolor_macro));
    ARGB_Color cl_global = fcolor_resolve(fcolor_id(defcolor_global));
    ARGB_Color cl_enum   = fcolor_resolve(fcolor_id(defcolor_enum));
    ARGB_Color cl_back   = fcolor_resolve(fcolor_id(defcolor_back));
    ARGB_Color cur_tok_color = fcolor_resolve(qol_get_token_color_cpp(*cursor_token));

    if (cursor_token->kind == TokenBaseKind_Identifier){
    String_Const_u8 lexeme = push_token_lexeme(app, scratch, buffer, cursor_token);
    Code_Index_Note *note = code_index_note_from_string(lexeme);

    if (note != 0){
    switch (note->note_kind){
    case CodeIndexNote_Function: cur_tok_color = cl_func;   break;
    case CodeIndexNote_Type:     cur_tok_color = cl_type;   break;
    case CodeIndexNote_Macro:    cur_tok_color = cl_macro;  break;
    case CodeIndexNote_Global:   cur_tok_color = cl_global; break;
    case CodeIndexNote_Enum:     cur_tok_color = cl_enum;   break;
    }
    }
    }

    String_Const_u8 token_string = push_token_lexeme(app, scratch, buffer, cursor_token);
    Rect_f32 cursor_tok_rect = text_layout_character_on_screen(app, text_layout_id, cursor_token->pos);
    Vec2_f32 tok_rect_dim = V2f32(cursor_token->size*rect_width(cursor_tok_rect), 2.f);
    cursor_tok_rect = Rf32_xy_wh(V2f32(cursor_tok_rect.x0, cursor_tok_rect.y1 - 2.f), tok_rect_dim);

    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    Token_Iterator_Array it = token_iterator_pos(0, array, visible_range.first);
    for (;;){
    Token *token = token_it_read(&it);
    if (token->pos >= visible_range.max){ break; }
    ARGB_Color argb = fcolor_resolve(qol_get_token_color_cpp(*token));
    String_Const_u8 lexeme = push_token_lexeme(app, scratch, buffer, token);
    Code_Index_Note *note = code_index_note_from_string(lexeme);

    if (do_highlight_cur_token && string_match(lexeme, token_string)){
    Rect_f32 cur_tok_rect = text_layout_character_on_screen(app, text_layout_id, token->pos);
    cur_tok_rect = Rf32_xy_wh(V2f32(cur_tok_rect.x0, cur_tok_rect.y1 - 2.f), tok_rect_dim);
    draw_rectangle(app, cur_tok_rect, 5.f, argb_color_blend(cur_tok_color, 0.7f, cl_back));
    }

    if (note != 0){
    switch (note->note_kind){
    case CodeIndexNote_Function: argb = cl_func;   break;
    case CodeIndexNote_Type:     argb = cl_type;   break;
    case CodeIndexNote_Macro:    argb = cl_macro;  break;
    case CodeIndexNote_Global:   argb = cl_global; break;
    case CodeIndexNote_Enum:     argb = cl_enum;   break;
    }
    }

    paint_text_color(app, text_layout_id, Ii64(token), argb);
    if(!token_it_inc_all(&it)){ break; }
    }

    if (do_highlight_cur_token){
    draw_rectangle(app, cursor_tok_rect, 5.f, cur_tok_color);
    }
    }

    function void
    qol_paint_cpp_token_colors(Application_Links *app, Buffer_ID buffer, Text_Layout_ID text_layout_id){
    Token_Array array = get_token_array_from_buffer(app, buffer);
    ARGB_Color cl_type   = fcolor_resolve(fcolor_id(defcolor_type));
    ARGB_Color cl_func   = fcolor_resolve(fcolor_id(defcolor_function));
    ARGB_Color cl_macro  = fcolor_resolve(fcolor_id(defcolor_macro));
    ARGB_Color cl_global = fcolor_resolve(fcolor_id(defcolor_global));
    ARGB_Color cl_enum   = fcolor_resolve(fcolor_id(defcolor_enum));

    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    Token_Iterator_Array it = token_iterator_pos(0, &array, visible_range.first);
    for (;;){
    Scratch_Block scratch(app);
    Token *token = token_it_read(&it);
    if (token == 0 || token->pos >= visible_range.max){ break; }
    ARGB_Color argb = fcolor_resolve(qol_get_token_color_cpp(*token));
    String_Const_u8 lexeme = push_token_lexeme(app, scratch, buffer, token);
    Code_Index_Note *note = code_index_note_from_string(lexeme);

    if (note != 0){
    switch (note->note_kind){
    case CodeIndexNote_Function: argb = cl_func;   break;
    case CodeIndexNote_Type:     argb = cl_type;   break;
    case CodeIndexNote_Macro:    argb = cl_macro;  break;
    case CodeIndexNote_Global:   argb = cl_global; break;
    case CodeIndexNote_Enum:     argb = cl_enum;   break;
    }
    }

    MM_paint_token_color(app, text_layout_id, token, argb);
    if(!token_it_inc_all(&it)){ break; }
  }
}