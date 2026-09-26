
CUSTOM_ID(attachment, buffer_lang);

enum Lang_ID{
  Lang_None,
  Lang_Cpp,
  Lang_Lua,
  Lang_COUNT,
};

typedef Token_List Lex_Async_Func_Type(Async_Context*, Arena*, String_Const_u8, i32, b32*);
typedef Token_List Lex_Sync_Func_Type (Arena*, String_Const_u8);
typedef void Parse_Func_Type(Application_Links*, Code_Index_File*, Arena*, String_Const_u8, Token_Array*);
typedef FColor Token_Color_Func(Token*);

function Token_List lang_lex_async_nop(Async_Context *actx, Arena *arena, String_Const_u8 contents, i32 limit, b32 *canceled){ return {}; }
function Token_List lang_lex_sync_nop (Arena *arena, String_Const_u8 contents){ return {}; }
function void       lang_parse_nop(Application_Links *app, Code_Index_File *index, Arena *arena, String_Const_u8 contents, Token_Array *tokens){}
function FColor     lang_paint_nop(Token* token){ return fcolor_id(defcolor_text_default); }
// parse_async__inner

struct Lang_Spec{
  Lang_ID id;
  Lex_Async_Func_Type *lex_async;
  Lex_Sync_Func_Type  *lex_full;
  Parse_Func_Type     *parse;
  Token_Color_Func    *token_color;
};

global Lang_Spec qol_languages[Lang_COUNT];

function void qol_lang_register(Lang_ID id, Lex_Async_Func_Type *lex_async, Lex_Sync_Func_Type *lex_full, Parse_Func_Type *parse, Token_Color_Func *token_color){
  qol_languages[id] = {id, lex_async, lex_full, parse, token_color};
}

function Lang_Spec* qol_lang_for_buffer(Application_Links *app, Buffer_ID buffer){
  Managed_Scope scope = buffer_get_managed_scope(app, buffer);
  return &qol_languages[*scope_attachment(app, scope, buffer_lang, Lang_ID)];
}

function void qol_lang_full_lex_async(Async_Context *actx, String_Const_u8 data){
  if (data.size != sizeof(Buffer_ID)){ return; }
  Buffer_ID buffer_id = *(Buffer_ID*)data.str;

  Application_Links *app = actx->app;
  ProfileScope(app, "async lex");
  Scratch_Block scratch(app);

  String_Const_u8 contents = {};
  {
    ProfileBlock(app, "async lex contents (before mutex)");
    acquire_global_frame_mutex(app);
    ProfileBlock(app, "async lex contents (after mutex)");
    contents = push_whole_buffer(app, scratch, buffer_id);
    release_global_frame_mutex(app);
  }

  i32 limit = 10000;
  b32 canceled = false;
  Lang_Spec *lang = qol_lang_for_buffer(app, buffer_id);
  Token_List list = lang->lex_async(actx, scratch, contents, limit, &canceled);

  if (!canceled){
    ProfileBlock(app, "async lex save results (before mutex)");
    acquire_global_frame_mutex(app);
    ProfileBlock(app, "async lex save results (after mutex)");
    Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
    if (scope != 0){
      Base_Allocator *allocator = managed_scope_allocator(app, scope);
      Token_Array *tokens_ptr = scope_attachment(app, scope, attachment_tokens, Token_Array);
      base_free(allocator, tokens_ptr->tokens);
      Token_Array tokens = {};
      tokens.tokens = base_array(allocator, Token, list.total_count);
      tokens.count = list.total_count;
      tokens.max = list.total_count;
      token_fill_memory_from_list(tokens.tokens, &list);
      block_copy_struct(tokens_ptr, &tokens);
    }
    buffer_mark_as_modified(buffer_id);
    release_global_frame_mutex(app);
  }
}

function i32 qol_lang_buffer_edit_range(Application_Links *app, Buffer_ID buffer_id, Range_i64 new_range, Range_Cursor old_cursor_range){
  ProfileScope(app, "qol edit range");

  Range_i64 old_range = Ii64(old_cursor_range.min.pos, old_cursor_range.max.pos);

  buffer_shift_fade_ranges(buffer_id, old_range.max, (new_range.max - old_range.max));

  {
    code_index_lock();
    Code_Index_File *file = code_index_get_file(buffer_id);
    if (file != 0){
      code_index_shift(file, old_range, range_size(new_range));
    }
    code_index_unlock();
  }

  i64 insert_size = range_size(new_range);
  i64 text_shift = replace_range_shift(old_range, insert_size);

  Scratch_Block scratch(app);

  Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
  Async_Task *lex_task_ptr = scope_attachment(app, scope, buffer_lex_task, Async_Task);

  Base_Allocator *allocator = managed_scope_allocator(app, scope);
  b32 do_full_relex = false;

  if (async_task_is_running_or_pending(&global_async_system, *lex_task_ptr)){
    async_task_cancel(app, &global_async_system, *lex_task_ptr);
    buffer_unmark_as_modified(buffer_id);
    do_full_relex = true;
    *lex_task_ptr = 0;
  }

  Token_Array *ptr = scope_attachment(app, scope, attachment_tokens, Token_Array);
  if (ptr != 0 && ptr->tokens != 0){
    ProfileBlockNamed(app, "attempt resync", profile_attempt_resync);

    i64 token_index_first = token_relex_first(ptr, old_range.first, 1);
    i64 token_index_resync_guess = token_relex_resync(ptr, old_range.one_past_last, 16);

    if (token_index_resync_guess - token_index_first >= 4000){
      do_full_relex = true;
    }
    else{
      Token *token_first = ptr->tokens + token_index_first;
      Token *token_resync = ptr->tokens + token_index_resync_guess;

      Range_i64 relex_range = Ii64(token_first->pos, token_resync->pos + token_resync->size + text_shift);
      String_Const_u8 partial_text = push_buffer_range(app, scratch, buffer_id, relex_range);

      Lang_Spec *lang = qol_lang_for_buffer(app, buffer_id);
      Token_List relex_list = lang->lex_full(scratch, partial_text);
      if (relex_range.one_past_last < buffer_get_size(app, buffer_id)){
        token_drop_eof(&relex_list);
      }

      Token_Relex relex = token_relex(relex_list, relex_range.first - text_shift, ptr->tokens, token_index_first, token_index_resync_guess);

      ProfileCloseNow(profile_attempt_resync);

      if (!relex.successful_resync){
        do_full_relex = true;
      }
      else{
        ProfileBlock(app, "apply resync");

        i64 token_index_resync = relex.first_resync_index;

        Range_i64 head = Ii64(0, token_index_first);
        Range_i64 replaced = Ii64(token_index_first, token_index_resync);
        Range_i64 tail = Ii64(token_index_resync, ptr->count);
        i64 resynced_count = (token_index_resync_guess + 1) - token_index_resync;
        i64 relexed_count = relex_list.total_count - resynced_count;
        i64 tail_shift = relexed_count - (token_index_resync - token_index_first);

        i64 new_tokens_count = ptr->count + tail_shift;
        Token *new_tokens = base_array(allocator, Token, new_tokens_count);

        Token *old_tokens = ptr->tokens;
        block_copy_array_shift(new_tokens, old_tokens, head, 0);
        token_fill_memory_from_list(new_tokens + replaced.first, &relex_list, relexed_count);
        for (i64 i = 0, index = replaced.first; i < relexed_count; i += 1, index += 1){
          new_tokens[index].pos += relex_range.first;
        }
        for (i64 i = tail.first; i < tail.one_past_last; i += 1){
          old_tokens[i].pos += text_shift;
        }
        block_copy_array_shift(new_tokens, ptr->tokens, tail, tail_shift);

        base_free(allocator, ptr->tokens);

        ptr->tokens = new_tokens;
        ptr->count = new_tokens_count;
        ptr->max = new_tokens_count;

        buffer_mark_as_modified(buffer_id);
      }
    }
  }

  if (do_full_relex){
    *lex_task_ptr = async_task_no_dep(&global_async_system, qol_lang_full_lex_async, make_data_struct(&buffer_id));
  }

  // no meaning for return
  return(0);
}

function void qol_code_index_update_tick(Application_Links *app){
  Scratch_Block scratch(app);
  for (Buffer_Modified_Node *node = global_buffer_modified_set.first;
       node != 0;
       node = node->next){
    Temp_Memory_Block temp(scratch);
    Buffer_ID buffer_id = node->buffer;

    String_Const_u8 contents = push_whole_buffer(app, scratch, buffer_id);
    Token_Array tokens = get_token_array_from_buffer(app, buffer_id);
    if (tokens.count == 0){
      continue;
    }

    Lang_Spec *lang = qol_lang_for_buffer(app, buffer_id);
    Arena arena = make_arena_system(KB(16));
    Code_Index_File *index = push_array_zero(&arena, Code_Index_File, 1);
    index->buffer = buffer_id;
    index->lang_id = lang->id;

    lang->parse(app, index, &arena, contents, &tokens);

    code_index_lock();
    code_index_set_file(buffer_id, arena, index);
    code_index_unlock();
    buffer_clear_layout_cache(app, buffer_id);
  }

  buffer_modified_set_clear();
}

function Code_Index_Note* code_index_note_from_string(Lang_ID id, String_Const_u8 string){
  Code_Index_Note_List *list = code_index__list_from_string(string);
  for (Code_Index_Note *n = list->first; n; n = n->next_in_hash){
    if (id == n->file->lang_id && string_match(string, n->text)){
      return n;
    }
  }
  return NULL;
}

function void qol_draw_token_colors(Application_Links *app, View_ID view, Buffer_ID buffer, Text_Layout_ID text_layout_id, Token_Array *array){
  Lang_Spec *lang = qol_lang_for_buffer(app, buffer);
  Token *cursor_token = token_from_pos(array, view_get_cursor_pos(app, view));
  b32 do_highlight_cur_token = qol_highlight_token(cursor_token->kind);

  ARGB_Color cl_type   = fcolor_resolve(fcolor_id(defcolor_type));
  ARGB_Color cl_func   = fcolor_resolve(fcolor_id(defcolor_function));
  ARGB_Color cl_macro  = fcolor_resolve(fcolor_id(defcolor_macro));
  ARGB_Color cl_global = fcolor_resolve(fcolor_id(defcolor_global));
  ARGB_Color cl_enum   = fcolor_resolve(fcolor_id(defcolor_enum));
  ARGB_Color cl_back   = fcolor_resolve(fcolor_id(defcolor_back));
  ARGB_Color cur_tok_color = fcolor_resolve(lang->token_color(cursor_token));

  Scratch_Block scratch(app);
  String_Const_u8 token_string = push_token_lexeme(app, scratch, buffer, cursor_token);
  if (cursor_token->kind == TokenBaseKind_Identifier){
    Code_Index_Note *note = code_index_note_from_string(lang->id, token_string);

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

  Rect_f32 cursor_tok_rect = text_layout_character_on_screen(app, text_layout_id, cursor_token->pos);
  Vec2_f32 tok_rect_dim = V2f32(cursor_token->size*rect_width(cursor_tok_rect), 2.f);
  cursor_tok_rect = Rf32_xy_wh(V2f32(cursor_tok_rect.x0, cursor_tok_rect.y1 - 2.f), tok_rect_dim);

  Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
  Token_Iterator_Array it = token_iterator_pos(0, array, visible_range.first);
  for (;;){
    Token *token = token_it_read(&it);
    if (token->pos >= visible_range.max){ break; }
    ARGB_Color argb = fcolor_resolve(lang->token_color(token));

    Temp_Memory_Block temp(scratch);
    if (do_highlight_cur_token){
      String_Const_u8 lexeme = push_token_lexeme(app, scratch, buffer, token);
      if (string_match(lexeme, token_string)){
        Rect_f32 cur_tok_rect = text_layout_character_on_screen(app, text_layout_id, token->pos);
        cur_tok_rect = Rf32_xy_wh(V2f32(cur_tok_rect.x0, cur_tok_rect.y1 - 2.f), tok_rect_dim);
        draw_rectangle(app, cur_tok_rect, 5.f, argb_color_blend(cur_tok_color, 0.7f, cl_back));
      }
    }

    if (token->kind == TokenBaseKind_Identifier){
      String_Const_u8 lexeme = push_token_lexeme(app, scratch, buffer, token);
      Code_Index_Note *note = code_index_note_from_string(lang->id, lexeme);

      if (note != 0){
        switch (note->note_kind){
          case CodeIndexNote_Function: argb = cl_func;   break;
          case CodeIndexNote_Type:     argb = cl_type;   break;
          case CodeIndexNote_Macro:    argb = cl_macro;  break;
          case CodeIndexNote_Global:   argb = cl_global; break;
          case CodeIndexNote_Enum:     argb = cl_enum;   break;
        }
      }
    }

    paint_text_color(app, text_layout_id, Ii64(token), argb);
    if(!token_it_inc_all(&it)){ break; }
  }

  if (do_highlight_cur_token){
    draw_rectangle(app, cursor_tok_rect, 5.f, cur_tok_color);
  }
}

function void qol_paint_token_colors(Application_Links *app, Buffer_ID buffer, Text_Layout_ID text_layout_id){
  Lang_Spec *lang = qol_lang_for_buffer(app, buffer);
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
    ARGB_Color argb = fcolor_resolve(lang->token_color(token));

    if (token->kind == TokenBaseKind_Identifier){
      Temp_Memory_Block temp(scratch);
      String_Const_u8 lexeme = push_token_lexeme(app, scratch, buffer, token);
      Code_Index_Note *note = code_index_note_from_string(lang->id, lexeme);

      if (note != 0){
        switch (note->note_kind){
          case CodeIndexNote_Function: argb = cl_func;   break;
          case CodeIndexNote_Type:     argb = cl_type;   break;
          case CodeIndexNote_Macro:    argb = cl_macro;  break;
          case CodeIndexNote_Global:   argb = cl_global; break;
          case CodeIndexNote_Enum:     argb = cl_enum;   break;
        }
      }
    }

    MM_paint_token_color(app, text_layout_id, token, argb);
    if(!token_it_inc_all(&it)){ break; }
  }
}