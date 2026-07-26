
enum QOL_Cursor_Kind{
  QOL_Cursor_Rect,
  QOL_Cursor_Thin,
  QOL_Cursor_Under,
  QOL_Cursor_Corner,
};

function QOL_Cursor_Kind qol_cursor_kind(String_Const_u8 str){
  return (string_match(str, string_u8_litexpr("|")) ? QOL_Cursor_Thin   :
          string_match(str, string_u8_litexpr("_")) ? QOL_Cursor_Under  :
          string_match(str, string_u8_litexpr("L")) ? QOL_Cursor_Corner : QOL_Cursor_Rect);
}

function void
qol_draw_hex_color(Application_Links *app, View_ID view, Buffer_ID buffer, Text_Layout_ID text_layout_id){
  Scratch_Block scratch(app);
  Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
  String_Const_u8 buffer_string = push_buffer_range(app, scratch, buffer, visible_range);

  for (i64 i = 0; i+9 < range_size(visible_range); i += 1){
    u8 *str = buffer_string.str+i;
    bool s0 = str[0] != '0';
    bool s1 = str[1] != 'x';
    if (s0 || s1){ continue; }

    b32 all_hex = true;
    for (i64 j = 0; j < 8; j += 1){
      u8 c = str[j+2];
      bool is_digit = '0' <= c && c <= '9';
      bool is_lower = 'a' <= c && c <= 'f';
      bool is_upper = 'A' <= c && c <= 'F';
      if (!(is_digit || is_lower || is_upper)) { all_hex=false; break; }
    }
    if (!all_hex){ continue; }

    i64 pos = visible_range.min + i;
    Rect_f32 r0 = text_layout_character_on_screen(app, text_layout_id, pos+0);
    Rect_f32 r1 = text_layout_character_on_screen(app, text_layout_id, pos+9);
    Rect_f32 rect = rect_inner(rect_union(r0, r1), -1.f);

    ARGB_Color color = ARGB_Color(string_to_integer(SCu8(str+2, 8), 16));
    u32 sum = ((color >> 16) & 0xFF) + ((color >> 8) & 0xFF) + (color & 0xFF);
    ARGB_Color contrast = ARGB_Color(0xFF000000 | (i32(sum > 330)-1));

    draw_rectangle_outline(app, rect_inner(rect, -2.f), 10.f, 4.f, contrast);
    draw_rectangle(app, rect, 8.f, color);
    paint_text_color(app, text_layout_id, Ii64_size(pos, 10), contrast);
    i += 9;
  }
}

function void
qol_draw_scopes(Application_Links *app, View_ID view, Buffer_ID buffer, Text_Layout_ID text_layout_id, f32 width){
  Scratch_Block scratch(app);
  i64 pos = view_get_cursor_pos(app, view);
  Rect_f32 prev_clip = draw_set_clip(app, Rect_f32{});
  Rect_f32 view_rect = prev_clip;
  view_rect.x0 -= 3.f;

  b32 on_open = '{' == buffer_get_char(app, buffer, pos);
  Range_i64_Array ranges = get_enclosure_ranges(app, scratch, buffer, pos+on_open, FindNest_Scope);
  for (i64 i = 0; i < ranges.count; i += 1){
    FColor color = fcolor_id(i == 0 ? defcolor_preproc : defcolor_ghost_character);
    Range_i64 range = ranges.ranges[i];
    range.max -= 1;

    u8 c0 = buffer_get_char(app, buffer, range.min);
    u8 c1 = buffer_get_char(app, buffer, range.max);
    if ((c0 != '{' && c0 != '(') || (c1 != '}' && c1 != ')')){ continue; }
    paint_text_color_pos(app, text_layout_id, range.min, color);
    paint_text_color_pos(app, text_layout_id, range.max, color);

    i64 line0 = get_line_number_from_pos(app, buffer, range.min);
    i64 line1 = get_line_number_from_pos(app, buffer, range.max);
    if (line0 == line1){ continue; }

    Rect_f32 scope_rect = qol_get_abs_block_rect(app, view, buffer, text_layout_id, range);
    scope_rect.p0 -= V2f32(3.f, 4.f);
    scope_rect.x1 += 10.f;
    Rect_f32 clip_rect = Rf32(scope_rect.p0, V2f32(scope_rect.x0 + 3.f + width, scope_rect.y1));
    draw_set_clip(app, rect_intersect(view_rect, clip_rect));
    draw_rectangle_outline_fcolor(app, scope_rect, 5.f, 2.f, color);
  }
  draw_set_clip(app, prev_clip);

  Token_Array tokens = get_token_array_from_buffer(app, buffer);
  Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
  for (i64 i=0 ; i < ranges.count; i += 1){
    Range_i64 range = ranges.ranges[ranges.count-1-i];
    i64 line0 = get_line_number_from_pos(app, buffer, range.min);
    i64 line1 = get_line_number_from_pos(app, buffer, range.max);
    if (line0 == line1 || range.max >= visible_range.max){ continue; }

    Token *start_token = 0;
    Token_Iterator_Array it = token_iterator_pos(0, &tokens, range.min-1);
    i64 paren_level = 0;
    for (;;){
      Token *token = token_it_read(&it);
      if (token == 0){ break; }

      paren_level += (token->kind == TokenBaseKind_ParentheticalClose);
      paren_level -= (token->kind == TokenBaseKind_ParentheticalOpen);

      if (paren_level == 0){
        if (token->kind == TokenBaseKind_ScopeClose ||
            (token->kind == TokenBaseKind_StatementClose && token->sub_kind != TokenCppKind_Colon))
        {
          break;
        }
        else if (token->kind == TokenBaseKind_Identifier ||
                 token->kind == TokenBaseKind_Keyword    ||
                 token->kind == TokenBaseKind_Comment    ||
                 token->kind == qol_TokenKind_Control)
        {
          start_token = token;
          break;
        }
      }
      if (!token_it_dec_non_whitespace(&it)){ break; }
    }

    if (start_token){
      i64 line = get_line_number_from_pos(app, buffer, start_token->pos);
      String_Const_u8 text = push_buffer_line(app, scratch, buffer, line);

      i64 ws_count = 0;
      for (u64 j = 0; j < text.size; j += 1){
        if (text.str[j] > 32){ break; }
        ws_count++;
      }
      text.str  += ws_count;
      text.size -= ws_count;
      text.size -= (text.str[text.size-1] == '\r');

      i64 end_pos = get_line_end_pos(app, buffer, line1)-1;
      Rect_f32 end_rect = text_layout_character_on_screen(app, text_layout_id, end_pos);
      Vec2_f32 p0 = V2f32(end_rect.x1 + 3.f, end_rect.y0 + 3.f);

      draw_string(app, qol_small_face, text, p0, fcolor_id(defcolor_ghost_character));
    }
  }
}

function void
qol_draw_cursor_mark(Application_Links *app, View_ID view_id, b32 is_active_view,
                     Buffer_ID buffer, Text_Layout_ID text_layout_id,
                     f32 roundness, f32 outline_thickness){
  b32 has_highlight_range = draw_highlight_range(app, view_id, buffer, text_layout_id, roundness);

  i64 cursor_pos = view_get_cursor_pos(app, view_id);
  i64 mark_pos = view_get_mark_pos(app, view_id);
  Rect_f32 nxt_cursor_rect = text_layout_character_on_screen(app, text_layout_id, cursor_pos);
  Rect_f32 cur_cursor_rect = Rf32_xy_wh(qol_cur_cursor_pos, rect_dim(nxt_cursor_rect));
  if (is_active_view && nxt_cursor_rect.x1 > 0.f){
    qol_nxt_cursor_pos = nxt_cursor_rect.p0;
  }

  if (!has_highlight_range){
    Scratch_Block scratch(app);
    QOL_Cursor_Kind cursor_kind = qol_cursor_kind(def_get_config_string(scratch, vars_save_string_lit("cursor_style")));
    QOL_Cursor_Kind   mark_kind = qol_cursor_kind(def_get_config_string(scratch, vars_save_string_lit("mark_style")));

    ARGB_Color cl_cursor = fcolor_resolve(fcolor_id(defcolor_cursor, default_cursor_sub_id()));
    ARGB_Color cl_mark   = fcolor_resolve(fcolor_id(defcolor_mark));
    if (is_active_view && cursor_kind == QOL_Cursor_Rect && rect_overlap(nxt_cursor_rect, cur_cursor_rect)){
      // NOTE: Only paint once cursor is overlapping (from Jack Punter)
      paint_text_color_pos(app, text_layout_id, cursor_pos, fcolor_id(defcolor_at_cursor));
    }
    else if (!is_active_view){
      draw_rectangle_outline(app, nxt_cursor_rect, roundness, outline_thickness, cl_cursor);
    }

    b32 b = cursor_pos < mark_pos;
    b32 c = mark_pos <= cursor_pos;
    f32 w = rect_width(cur_cursor_rect) - 3.f;

    if (vim_show_cursor(app)){
      Vec2_f32 d = V2f32(c ? w : 0, 0);
      Rect_f32 rect_shifted = Rf32(cur_cursor_rect.p0-d, cur_cursor_rect.p1-d);
      switch (cursor_kind){
        case QOL_Cursor_Rect:    draw_rectangle(app, cur_cursor_rect, roundness, cl_cursor); break;
        case QOL_Cursor_Thin:    draw_rectangle(app, rect_vsplit(cur_cursor_rect, 1.f, 0), 0.f, cl_cursor); break;
        case QOL_Cursor_Under:   draw_rectangle(app, rect_hsplit(cur_cursor_rect, 5.f, 1), roundness, cl_cursor); break;
        case QOL_Cursor_Corner: (draw_rectangle(app, rect_vsplit(cur_cursor_rect, 3.f, 0), roundness, cl_cursor),
                                 draw_rectangle(app, rect_hsplit(rect_shifted,    3.f, c), roundness, cl_cursor)); break;
      }
    }

    if (vim_state.mode == VIM_Insert){
      Rect_f32 mark_rect = text_layout_character_on_screen(app, text_layout_id, mark_pos);
      Vec2_f32 d = V2f32(b ? w : 0, 0);
      Rect_f32 rect_shifted = Rf32(mark_rect.p0-d, mark_rect.p1-d);
      switch (mark_kind){
        case QOL_Cursor_Rect:    draw_rectangle_outline(app, mark_rect, roundness, outline_thickness, cl_mark); break;
        case QOL_Cursor_Thin:    draw_rectangle(app, rect_vsplit(mark_rect, 1.f, 0), 0.f, cl_mark); break;
        case QOL_Cursor_Under:   draw_rectangle(app, rect_hsplit(mark_rect, 5.f, 1), roundness, cl_mark); break;
        case QOL_Cursor_Corner: (draw_rectangle(app, rect_vsplit(mark_rect,    outline_thickness,  0), roundness, cl_mark),
                                 draw_rectangle(app, rect_hsplit(rect_shifted, outline_thickness, !c), roundness, cl_mark));
      }
    }
  }

  MC_render_cursors(app, view_id, text_layout_id);
}

function void
qol_draw_comments(Application_Links *app, Buffer_ID buffer, Text_Layout_ID text_layout_id, Token_Array *array, Rect_f32 rect){
  Scratch_Block scratch(app);
  ARGB_Color cl_annotate = finalize_color(defcolor_comment_pop, 2);
  b32 use_comment_annotations = def_get_config_b32(vars_save_string_lit("use_comment_annotations"));
  Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
  i64 first_index = token_index_from_pos(array, visible_range.first);
  Token_Iterator_Array it = token_iterator_index(buffer, array, first_index);
  for (;;){
    Temp_Memory_Block temp(scratch);
    Token *token = token_it_read(&it);
    if (token->pos >= visible_range.one_past_last){ break; }
    String_Const_u8 tail = {};
    if (token_it_check_and_get_lexeme(app, scratch, &it, TokenBaseKind_Comment, &tail)){

      //- divider comments
      String_Const_u8 match = string_u8_litexpr("//-");
      String_Const_u8 prefix = string_prefix(tail, match.size);
      if (block_match(prefix.str, match.str, match.size)){
        f32 y = text_layout_character_on_screen(app, text_layout_id, token->pos).y0;
        Rect_f32 dividor_line = Rf32(rect.x0, y - 1.f, rect.x1, y);
        draw_rectangle_fcolor(app, dividor_line, 0.f, fcolor_id(defcolor_comment));
      }

      // paint @annotations in comments
      if (use_comment_annotations){
        for (i64 i = 1; i < token->size; i += 1){
          if (tail.str[i] != '@' || !character_is_whitespace(tail.str[i-1])){ continue; }
          Range_i64 range = Ii64(i,i+1);
          for (i64 j = i+1; j < token->size; j += 1){
            char ch = tail.str[j];
            if (character_is_whitespace(ch) || !character_is_alpha_numeric(ch)){
              break;
            }
            range.max++;
          }

          if (range_size(range) > 1){
            paint_text_color(app, text_layout_id, range + token->pos, cl_annotate);
          }
        }
      }
    }
    if (!token_it_inc_non_whitespace(&it)){ break; }
  }
}

function void
qol_draw_function_tooltip_inner(Application_Links *app, Arena *arena, Code_Index_Note *note, Code_Index_Nest* paren_define, Code_Index_Nest* paren_caller, i64 pos, i64 depth){
  i64 param_hovered = 0;
  i64 pos_start = paren_caller->open.min;
  for (Code_Index_Nest *n = paren_caller->nest_list.first; n != 0; n = n->next){
    if (n->next == 0 || range_contains(Ii64(pos_start, n->close.min), pos)){ break; }
    pos_start = n->close.start;
    param_hovered++;
  }

  String8List prefix = {};
  String8List middle = {};
  String8List suffix = {};
  string_list_push(arena, &prefix, note->text);
  string_list_push(arena, &prefix, string_u8_litexpr("("));

  for (Code_Index_Nest *n = paren_define->nest_list.first; n != 0; n = n->next){
    String8List* list = (0<param_hovered ? &prefix : param_hovered==0 ? &middle : &suffix);
    param_hovered--;
    String_Const_u8 param = push_buffer_range(app, arena, note->file->buffer, range_union(n->open, n->close));
    string_list_push(arena, list, string_condense_whitespace(arena, param));
    if (n->next){ string_list_push(arena, list, string_u8_litexpr(" ")); }
  }
  string_list_push(arena, &suffix, string_u8_litexpr(")"));

  Face_Metrics metrics = get_face_metrics(app, qol_small_face);
  f32 char_wid = metrics.normal_advance;
  f32 line_hit = metrics.line_height;
  FColor cl_line = fcolor_id(depth == 1 ? defcolor_cursor : defcolor_ghost_character);
  f32 wid = 2.f;
  f32 pad = 2.f;

  String_Const_u8 pre = string_list_flatten(arena, prefix);
  String_Const_u8 mid = string_list_flatten(arena, middle);
  String_Const_u8 suf = string_list_flatten(arena, suffix);

  Vec2_f32 p0 = V2f32(f32_floor32(qol_cur_cursor_pos.x), f32_floor32(qol_cur_cursor_pos.y) + 2.f + depth*(1.f + metrics.line_height + pad + 2.f*wid));
  Rect_f32 full   = Rf32(p0 - V2f32(pre.size*char_wid, 0.f), p0 + V2f32((mid.size+suf.size)*char_wid, line_hit));
  Rect_f32 r_mid  = Rf32(p0, p0 + V2f32(mid.size*char_wid, wid + line_hit));
  Rect_f32 r_line = Rf32(r_mid.x0, r_mid.y1-4.f, r_mid.x1 + 2.f - (suffix.node_count != 1)*char_wid, r_mid.y1-3.f);
  Vec2_f32 pre_p0 =  full.p0;
  Vec2_f32 mid_p0 = r_mid.p0;
  Vec2_f32 suf_p0 =  full.p1 - V2f32(suf.size*char_wid, line_hit);

  draw_rectangle_fcolor(app, rect_inner(full, -wid), 3.f, fcolor_id(defcolor_back));
  draw_string(app, qol_small_face, pre, pre_p0 + wid*V2f32(1,1), fcolor_id(defcolor_ghost_character));
  draw_string(app, qol_small_face, mid, mid_p0 + wid*V2f32(1,1), fcolor_id(defcolor_text_default));
  draw_string(app, qol_small_face, suf, suf_p0 + wid*V2f32(1,1), fcolor_id(defcolor_ghost_character));
  draw_rectangle_fcolor(app, r_line, 2.f, cl_line);
  draw_rectangle_outline_fcolor(app, rect_inner(full, -wid), 3.f, wid, cl_line);
}

function void
qol_draw_function_tooltip(Application_Links *app, Buffer_ID buffer, i64 pos){
  Token_Array tokens = get_token_array_from_buffer(app, buffer);
  if (tokens.tokens == 0){ return; }
  i64 count = 0;

  code_index_lock();
  Code_Index_File *file = code_index_get_file(buffer);
  for (Code_Index_Nest* n=code_index_get_nest(file, pos); n != 0; n = n->parent){
    Scratch_Block scratch(app);
    if (n->kind != CodeIndexNest_Paren){ continue; }
    Token_Iterator_Array it = token_iterator_pos(0, &tokens, n->open.min);
    token_it_dec_non_whitespace(&it);
    Token *token = token_it_read(&it);

    if (token->kind == TokenBaseKind_Identifier){
      String_Const_u8 lexeme = push_token_lexeme(app, scratch, buffer, token);
      Code_Index_Note *note = code_index_note_from_string(lexeme);
      if (note == NULL){ continue; }
      if (note->note_kind == CodeIndexNote_Function ||
          note->note_kind == CodeIndexNote_Macro)
      {
        Code_Index_Nest *paren_define = note->parent->nest_list.first;
        if (paren_define != NULL && paren_define->kind == CodeIndexNest_Paren){
          qol_draw_function_tooltip_inner(app, scratch, note, paren_define, n, pos, ++count);
        }
      }
    }
  }
  code_index_unlock();
}

function void
qol_draw_compile_errors(Application_Links *app, Buffer_ID buffer, Text_Layout_ID text_layout_id, Buffer_ID jump_buffer){
  if (jump_buffer == 0){ return; }

  Scratch_Block scratch(app);
  Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
  FColor cl_error = fcolor_blend(fcolor_id(defcolor_highlight_junk), 0.6f, fcolor_id(defcolor_text_default));

  Managed_Scope scopes[2];
  scopes[0] = buffer_get_managed_scope(app, jump_buffer);
  scopes[1] = buffer_get_managed_scope(app, buffer);
  Managed_Scope comp_scope = get_managed_scope_with_multiple_dependencies(app, scopes, ArrayCount(scopes));
  Managed_Object *markers_object = scope_attachment(app, comp_scope, sticky_jump_marker_handle, Managed_Object);

  i32 count = managed_object_get_item_count(app, *markers_object);
  Marker *markers = push_array(scratch, Marker, count);
  managed_object_load_data(app, *markers_object, 0, count, markers);
  for (i32 i = 0; i < count; i += 1){
    i64 line_number = get_line_number_from_pos(app, buffer, markers[i].pos);
    Range_i64 line_range = get_line_pos_range(app, buffer, line_number);
    if (!range_overlap(visible_range, line_range)){ continue; }

    String_Const_u8 comp_line_string = push_buffer_line(app, scratch, jump_buffer, markers[i].line);
    Parsed_Jump jump = parse_jump_location(comp_line_string);
    if (!jump.success){ continue; }

    i64 end_pos = get_line_end_pos(app, buffer, line_number)-1;
    Rect_f32 end_rect = text_layout_character_on_screen(app, text_layout_id, end_pos);
    Vec2_f32 p0 = V2f32(end_rect.x1, end_rect.y0 + 4.f);
    String_Const_u8 error_string = string_skip(comp_line_string, jump.colon_position + 2);
    draw_line_highlight(app, text_layout_id, line_number, fcolor_id(defcolor_highlight_junk));
    draw_string(app, qol_small_face, error_string, p0, cl_error);
  }
}

function Rect_f32
qol_draw_query_bars(Application_Links *app, Rect_f32 region, View_ID view_id, Face_ID face_id){
  Face_Metrics face_metrics = get_face_metrics(app, face_id);
  f32 line_height = face_metrics.line_height;

  Query_Bar *space[32];
  Query_Bar_Ptr_Array query_bars = {};
  query_bars.ptrs = space;
  if (get_active_query_bars(app, view_id, ArrayCount(space), &query_bars)){
    for (i32 i = 0; i < query_bars.count; i += 1){
      Rect_f32_Pair pair = layout_query_bar_on_bot(region, line_height, 1);

      Query_Bar *query_bar = query_bars.ptrs[i];
      Rect_f32 bar_rect = pair.max;

      Scratch_Block scratch(app);
      Fancy_Line list = {};
      push_fancy_string(scratch, &list, fcolor_id(defcolor_pop1),         query_bar->prompt);
      push_fancy_string(scratch, &list, fcolor_id(defcolor_text_default), query_bar->string);
      Vec2_f32 p = bar_rect.p0 + V2f32(2.f, 2.f);
      p = draw_fancy_line(app, face_id, fcolor_zero(), &list, p);
      if (i == 0){
        draw_rectangle_fcolor(app, Rf32_xy_wh(p.x, p.y, 2.f, face_metrics.line_height), 0.f, fcolor_id(defcolor_cursor, 0));
      }

      region = pair.min;
    }
  }
  return(region);
}

function void
qol_render_buffer(Application_Links *app, View_ID view_id, Face_ID face_id, Buffer_ID buffer, Text_Layout_ID text_layout_id, Rect_f32 rect){
  ProfileScope(app, "qol render buffer");

  View_ID active_view = get_active_view(app, Access_Always);
  b32 is_active_view = (active_view == view_id);
  Rect_f32 prev_clip = draw_set_clip(app, rect);

  Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);

  // NOTE(allen): Cursor shape
  Face_Metrics metrics = get_face_metrics(app, face_id);
  u64 cursor_roundness_100 = def_get_config_u64(app, vars_save_string_lit("cursor_roundness"));
  f32 cursor_roundness = metrics.normal_advance*cursor_roundness_100*0.01f;
  f32 mark_thickness = (f32)def_get_config_u64(app, vars_save_string_lit("mark_thickness"));

  // NOTE(allen): Token colorizing
  Token_Array token_array = get_token_array_from_buffer(app, buffer);
  if (token_array.tokens != 0){
    qol_draw_cpp_token_colors(app, view_id, buffer, text_layout_id, &token_array);

    // NOTE(allen): Scan for TODOs and NOTEs
    b32 use_comment_keyword = def_get_config_b32(vars_save_string_lit("use_comment_keyword"));
    if (use_comment_keyword){
      Comment_Highlight_Pair pairs[] = {
        {string_u8_litexpr("NOTE"), finalize_color(defcolor_comment_pop, 0)},
        {string_u8_litexpr("TODO"), finalize_color(defcolor_comment_pop, 1)},
      };
      draw_comment_highlights(app, buffer, text_layout_id, &token_array, pairs, ArrayCount(pairs));
    }
    qol_draw_comments(app, buffer, text_layout_id, &token_array, rect);
  }
  else{
    paint_text_color_fcolor(app, text_layout_id, visible_range, fcolor_id(defcolor_text_default));
  }

  i64 cursor_pos = view_correct_cursor(app, view_id);
  view_correct_mark(app, view_id);

  // NOTE(allen): Scope highlight
  b32 use_scope_highlight = def_get_config_b32(vars_save_string_lit("use_scope_highlight"));
  if (use_scope_highlight){
    Color_Array colors = finalize_color_array(defcolor_back_cycle);
    draw_scope_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
  }

  if (qol_col_cursor.pos >= 0 && qol_col_buffer == buffer){
    Rect_f32 r = view_relative_box_of_pos(app, view_id, qol_col_cursor.line, qol_col_cursor.pos);
    f32 dx = view_get_buffer_scroll(app, view_id).position.pixel_shift.x;
    Rect_f32 col_rect = Rf32(rect_range_x(r) + rect.x0 - dx, rect_range_y(rect));
    draw_rectangle_fcolor(app, col_rect, 0.f, fcolor_id(defcolor_highlight_cursor_line));
  }

  // NOTE(allen): Line highlight
  b32 highlight_line_at_cursor = def_get_config_b32(vars_save_string_lit("highlight_line_at_cursor"));
  if (highlight_line_at_cursor && is_active_view){
    i64 line_number = get_line_number_from_pos(app, buffer, cursor_pos);
    draw_line_highlight(app, text_layout_id, line_number, fcolor_id(defcolor_highlight_cursor_line));
  }

  qol_draw_scopes(app, view_id, buffer, text_layout_id, metrics.normal_advance);

  b32 use_error_highlight = def_get_config_b32(vars_save_string_lit("use_error_highlight"));
  b32 use_jump_highlight = def_get_config_b32(vars_save_string_lit("use_jump_highlight"));
  if (use_error_highlight || use_jump_highlight){
    // NOTE(allen): Error highlight
    String_Const_u8 name = string_u8_litexpr("*compilation*");
    Buffer_ID compilation_buffer = get_buffer_by_name(app, name, Access_Always);
    if (use_error_highlight){
      qol_draw_compile_errors(app, buffer, text_layout_id, compilation_buffer);
    }

    // NOTE(allen): Search highlight
    if (use_jump_highlight){
      Buffer_ID jump_buffer = get_locked_jump_buffer(app);
      if (jump_buffer != compilation_buffer){
        draw_jump_highlights(app, buffer, text_layout_id, jump_buffer, fcolor_id(defcolor_highlight_white));
      }
    }
  }

  // NOTE(allen): Color parens
  b32 use_paren_helper = def_get_config_b32(vars_save_string_lit("use_paren_helper"));
  if (use_paren_helper){
    Color_Array colors = finalize_color_array(defcolor_text_cycle);
    draw_paren_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
  }

  // NOTE(allen): Whitespace highlight
  b64 show_whitespace = false;
  view_get_setting(app, view_id, ViewSetting_ShowWhitespace, &show_whitespace);
  if (show_whitespace){
    if (token_array.tokens == 0){
      draw_whitespace_highlight(app, buffer, text_layout_id, cursor_roundness);
    }
    else{
      draw_whitespace_highlight(app, text_layout_id, &token_array, cursor_roundness);
    }
  }

  b32 show_hex_colors = def_get_config_b32(vars_save_string_lit("show_hex_colors"));
  if (show_hex_colors){
    qol_draw_hex_color(app, view_id, buffer, text_layout_id);
  }

  if(is_active_view && vim_state.mode == VIM_Visual){
    vim_draw_visual_mode(app, view_id, buffer, face_id, text_layout_id);
  }

  vim_draw_search_highlight(app, view_id, buffer, text_layout_id, cursor_roundness);

  // NOTE(allen): Cursor
  switch (fcoder_mode){
    case FCoderMode_Original:
    {
      Rect_f32 r = draw_set_clip(app, prev_clip);

      Scratch_Block scratch(app);
      String_ID key = vars_save_string_lit("cursor_style");
      String_Const_u8 prev = def_get_config_string(scratch, key);
      def_set_config_string(key, vim_state.mode == VIM_Insert ? string_u8_litexpr("_") : prev);
      qol_draw_cursor_mark(app, view_id, is_active_view, buffer, text_layout_id, cursor_roundness, mark_thickness);
      def_set_config_string(key, prev);

      draw_set_clip(app, r);
    }break;
    case FCoderMode_NotepadLike:
    {
      draw_notepad_style_cursor_highlight(app, view_id, buffer, text_layout_id, cursor_roundness);
    }break;
  }

  // NOTE(allen): Fade ranges
  paint_fade_ranges(app, text_layout_id, buffer);

  // NOTE(allen): put the actual text on the actual screen
  draw_text_layout_default(app, text_layout_id);

  vim_draw_after_text(app, view_id, is_active_view, buffer, text_layout_id, cursor_roundness, mark_thickness);

  if (rect_contains_point(rect, qol_cur_cursor_pos) &&
      def_get_config_b32(vars_save_string_lit("use_function_tooltip"))){
    qol_draw_function_tooltip(app, buffer, cursor_pos);
  }

  if (token_array.tokens){
    Scratch_Block scratch(app);
    ARGB_Color cl_nest = fcolor_resolve(fcolor_change_alpha(fcolor_id(defcolor_control), 0.8f));
    Text_Layout_ID minimap_id = MM_begin(app, scratch, view_id, face_id, buffer, token_array, rect, visible_range, cl_nest);
    qol_paint_cpp_token_colors(app, buffer, minimap_id);
    MM_end(app, minimap_id);
  }

  draw_set_clip(app, prev_clip);
}

function void
qol_render_caller(Application_Links *app, Frame_Info frame_info, View_ID view_id){
  ProfileScope(app, "qol render caller");
  View_ID active_view = get_active_view(app, Access_Always);
  b32 is_active_view = (active_view == view_id);

  Rect_f32 region = view_get_screen_rect(app, view_id);
  Rect_f32 prev_clip = draw_set_clip(app, region);
  draw_rectangle_fcolor(app, region, 0.f, fcolor_id(defcolor_back));

  Buffer_ID buffer = view_get_buffer(app, view_id, Access_Always);
  Face_ID face_id = get_face_id(app, buffer);
  Face_Metrics face_metrics = get_face_metrics(app, face_id);
  f32 line_height = face_metrics.line_height;
  f32 normal_advance = face_metrics.normal_advance;
  f32 digit_advance = face_metrics.decimal_digit_advance;

  // NOTE(allen): query bars
  region = qol_draw_query_bars(app, region, view_id, face_id);

  // NOTE(allen): file bar
  b64 showing_file_bar = false;
  b64 has_bot_border = false;
  if (view_get_setting(app, view_id, ViewSetting_ShowFileBar, &showing_file_bar) && showing_file_bar){
    b32 on_top = def_get_config_b32(vars_save_string_lit("filebar_on_top"));
    Rect_f32_Pair pair = (on_top ?
                          layout_file_bar_on_top(region, line_height) :
                          layout_file_bar_on_bot(region, line_height));
    vim_draw_filebar(app, view_id, buffer, face_id, pair.e[1-on_top]);
    region = pair.e[on_top];
    has_bot_border = !on_top;
  }

  if (!has_bot_border){
    Rect_f32_Pair pair = rect_split_top_bottom_neg(region, 2.f);
    draw_rectangle_fcolor(app, pair.max, 0.f, fcolor_id(defcolor_bar));
    region = pair.min;
  }

  {
    Rect_f32 r = global_get_screen_rectangle(app);
    ARGB_Color cl = fcolor_resolve(fcolor_id(defcolor_margin));
    if(region.x0 != r.x0){ draw_rectangle(app, Rf32(region.x0,   region.y0, region.x0+2, region.y1), 0.f, cl); region.x0 += 2; }
    if(region.x1 != r.x1){ draw_rectangle(app, Rf32(region.x1-2, region.y0, region.x1,   region.y1), 0.f, cl); region.x1 -= 2; }
  }

  f32 char_count = def_get_config_f32(app, vars_save_string_lit("scroll_margin_x"));
  f32 line_count = def_get_config_f32(app, vars_save_string_lit("scroll_margin_y"));
  Vec2_f32 margin = V2f32(char_count*normal_advance, line_count*line_height);
  view_set_camera_bounds(app, view_id, margin, V2f32(1,1));

  Buffer_Scroll scroll = view_get_buffer_scroll(app, view_id);

  Buffer_Point_Delta_Result delta = delta_apply(app, view_id, frame_info.animation_dt, scroll);
  if (!block_match_struct(&scroll.position, &delta.point)){
    if (is_active_view){
      qol_cur_cursor_pos -= view_point_difference(app, view_id, delta.point, scroll.position);
    }
    block_copy_struct(&scroll.position, &delta.point);
    view_set_buffer_scroll(app, view_id, scroll, SetBufferScroll_NoCursorChange);
  }
  if (delta.still_animating){
    animate_in_n_milliseconds(app, 0);
  }

  // NOTE(allen): FPS hud
  if (show_fps_hud){
    Rect_f32_Pair pair = layout_fps_hud_on_bottom(region, line_height);
    draw_fps_hud(app, frame_info, face_id, pair.max);
    region = pair.min;
    animate_in_n_milliseconds(app, 1000);
  }

  // NOTE(allen): layout line numbers
  b32 show_line_number_margins = def_get_config_b32(vars_save_string_lit("show_line_number_margins"));
  Rect_f32 line_number_rect = {};
  if (show_line_number_margins){
    Rect_f32_Pair pair = vim_line_number_margin(app, buffer, region, digit_advance);
    line_number_rect = pair.min;
    region = pair.max;
  }
  region = rect_split_left_right(region, 4.f).max;

  // NOTE(allen): begin buffer render
  Buffer_Point buffer_point = scroll.position;
  Text_Layout_ID text_layout_id = text_layout_create(app, buffer, region, buffer_point);

  // NOTE(allen): draw line numbers
  if (show_line_number_margins){
    (def_get_config_b32(vars_save_string_lit("vim_line_numbers_relative")) ?
     vim_draw_rel_line_number_margin(app, view_id, buffer, face_id, text_layout_id, line_number_rect) :
     vim_draw_abs_line_number_margin(app, view_id, buffer, face_id, text_layout_id, line_number_rect));
  }

  // NOTE(allen): draw the buffer
  qol_render_buffer(app, view_id, face_id, buffer, text_layout_id, region);

  text_layout_free(app, text_layout_id);
  draw_set_clip(app, prev_clip);
}

function Rect_f32
qol_buffer_region(Application_Links *app, View_ID view_id, Rect_f32 region){
  Buffer_ID buffer = view_get_buffer(app, view_id, Access_Always);
  Face_ID face_id = get_face_id(app, buffer);
  Face_Metrics metrics = get_face_metrics(app, face_id);
  f32 line_height = metrics.line_height;
  f32 digit_advance = metrics.decimal_digit_advance;

  // NOTE(allen): query bars
  {
    Query_Bar *space[32];
    Query_Bar_Ptr_Array query_bars = {};
    query_bars.ptrs = space;
    if (get_active_query_bars(app, view_id, ArrayCount(space), &query_bars)){
      region = layout_query_bar_on_top(region, line_height, query_bars.count).max;
    }
  }

  // NOTE(allen): file bar
  b64 showing_file_bar = false;
  b64 has_bot_border = false;
  if (view_get_setting(app, view_id, ViewSetting_ShowFileBar, &showing_file_bar) && showing_file_bar){
    b32 on_top = def_get_config_b32(vars_save_string_lit("filebar_on_top"));
    Rect_f32_Pair pair = (on_top ?
                          layout_file_bar_on_top(region, line_height) :
                          layout_file_bar_on_bot(region, line_height));
    region = pair.e[on_top];
    has_bot_border = !on_top;
  }

  if (!has_bot_border){
    region = rect_split_top_bottom_neg(region, 2.f).min;
  }

  // NOTE(allen): FPS hud
  if (show_fps_hud){
    region = layout_fps_hud_on_bottom(region, line_height).min;
  }

  // NOTE(allen): line numbers
  if (def_get_config_b32(vars_save_string_lit("show_line_number_margins"))){
    region = vim_line_number_margin(app, buffer, region, digit_advance).max;
  }
  region = rect_split_left_right(region, 4.f).max;

  return(region);
}

function void
qol_draw_peek(Application_Links *app, Frame_Info frame_info){
  Scratch_Block scratch(app);
  String_Const_u8 lexeme = push_token_or_word_under_active_cursor(app, scratch);
  Code_Index_Note *note = code_index_note_from_string(lexeme);
  switch (note ? note->note_kind : -1){
    case CodeIndexNote_Function:
    case CodeIndexNote_Type:
    case CodeIndexNote_Macro: break;
    default: return;
  }

  Rect_f32 region = panel_get_rect(app, TAB_root(app));
  Vec2_f32 center = rect_center(region);
  Vec2_f32 dim = rect_half_dim(region);
  Vec2_f32 bound = V2f32(lerp(rect_x(region), 0.4f),  // x bias since text is left-to-right
                         lerp(rect_y(region), 0.5f));
  f32 x0 = f32_floor32(qol_cur_cursor_pos.x < bound.x ? center.x : region.x0);
  f32 y0 = f32_floor32(Min(qol_cur_cursor_pos.y, region.y1-dim.y));
  Rect_f32 rect = Rf32_xy_wh(V2f32(x0, y0), dim);
  // ^ or iterate panels selecting max via (panel-height, dist-to-cursor)

  Buffer_ID peek_buffer = 0;
  i64 peek_line = 0;

  {
    code_index_lock();
    for (Buffer_ID b = get_buffer_next(app, 0, Access_Always);
         b != 0;
         b = get_buffer_next(app, b, Access_Always)){
      Code_Index_File *file = code_index_get_file(b);
      if (file == 0){ continue; }

      for (i32 i = 0; i < file->note_array.count; i += 1){
        Code_Index_Note *n = file->note_array.ptrs[i];
        if (!string_match(n->text, lexeme)){ continue; }

        peek_buffer = b;
        peek_line = get_line_number_from_pos(app, b, n->pos.first);
        goto done;
      }
    }
    done:;
    code_index_unlock();
  }

  if (peek_buffer == 0){ return; }

  Buffer_Point point = {peek_line};
  Text_Layout_ID text_layout_id = text_layout_create(app, peek_buffer, rect_inner(rect, 10), point);

  draw_rectangle_fcolor(app, rect, 5, fcolor_change_alpha(fcolor_id(defcolor_back), 0.9f));
  draw_rectangle_outline_fcolor(app, rect, 5, 5, fcolor_id(defcolor_bar));
  draw_line_highlight(app, text_layout_id, peek_line, fcolor_id(defcolor_highlight_cursor_line));

  Rect_f32 prev_clip = draw_set_clip(app, text_layout_region(app, text_layout_id));
  qol_paint_cpp_token_colors(app, peek_buffer, text_layout_id);
  draw_text_layout_default(app, text_layout_id);
  text_layout_free(app, text_layout_id);
  draw_set_clip(app, prev_clip);
}

function void
qol_try_exit_render(Application_Links *app, Frame_Info frame_info){
  Rect_f32 region = global_get_screen_rectangle(app);
  Vec2_f32 center = rect_center(region);

  Scratch_Block scratch(app);
  tutorial_init_title_face(app);
  local_persist Face_Description desc = get_face_description(app, get_face_id(app, 0));
  local_persist Face_ID face = try_create_new_face(app, &desc);

  String_Const_u8 s1 = string_u8_litexpr("Trying to Exit with ");
  String_Const_u8 s2 = push_stringf(scratch, "%lld unsaved buffers", qol_dirty_buffer_count(app));
  f32 w1 = get_string_advance(app, tutorial.face, s1);
  f32 w2 = get_string_advance(app, tutorial.face, s2);
  f32 w = w1 + w2;
  f32 h = get_face_metrics(app, tutorial.face).line_height;
  Face_Metrics metrics = get_face_metrics(app, face);
  Mouse_State mouse = get_mouse_state(app);
  Vec2_f32 mp = V2f32(mouse.p);

  Fancy_Line line = {};
  push_fancy_string(scratch, &line, fcolor_id(defcolor_pop1), s1);
  push_fancy_string(scratch, &line, fcolor_id(defcolor_pop2), s2);

  FColor cl_1 = fcolor_id(defcolor_pop1);
  FColor cl_2 = fcolor_id(defcolor_pop2);
  FColor cl_3 = fcolor_id(defcolor_text_default);
  FColor cl_rect = fcolor_blend(fcolor_argb(0xFF000000), 0.8f, fcolor_id(defcolor_back));
  Rect_f32 r = Rf32_xy_wh(center.x - 0.5f*w, 3.5f*h, w, 3.f*h);
  draw_rectangle_fcolor(app, rect_inner(r, -15), 5, fcolor_change_alpha(cl_rect, 0.9f));

  f32 pad = 10.f;
  f32 b_wid = (w - 4*pad) / 3.f;
  Rect_f32 r1 = Rf32_xy_wh(r.x0 + 1*pad + 0*b_wid, r.y1 - 1.25f*h, b_wid, h);
  Rect_f32 r2 = Rf32_xy_wh(r.x0 + 2*pad + 1*b_wid, r.y1 - 1.25f*h, b_wid, h);
  Rect_f32 r3 = Rf32_xy_wh(r.x0 + 3*pad + 2*b_wid, r.y1 - 1.25f*h, b_wid, h);

  draw_fancy_line(app, tutorial.face, fcolor_zero(), &line, r.p0);
  b32 do_1 = draw_button(app, r1, mp, face, cl_1, SCu8("Save all and exit"));
  b32 do_2 = draw_button(app, r2, mp, face, cl_2, SCu8("Exit without saving"));
  b32 do_3 = draw_button(app, r3, mp, face, cl_3, SCu8("Cancel"));

  draw_rectangle_outline_fcolor(app, rect_inner(r, -15), 5, 5, fcolor_id(defcolor_margin_active));

  Key_Code code = (do_1 ? KeyCode_S : do_2 ? KeyCode_Y : do_3 ? KeyCode_N : 0);
  Input_Event event = { InputEventKind_KeyStroke };
  event.key.code = code;

  if (code != 0){
    if (mouse.release_l){ enqueue_virtual_event(app, &event); }
    FColor cl = (do_1 ? cl_1 : do_2 ? cl_2 : do_3 ? cl_3 : fcolor_zero());
    String_Const_u8 str = push_stringf(scratch, "Key: \"%s\"", key_code_name[code]);
    Vec2_f32 dim = V2f32(get_string_advance(app, face, str), metrics.line_height);
    Rect_f32 rect = Rf32_xy_wh(mp + V2f32(pad, pad + metrics.line_height), dim);
    draw_rectangle_fcolor(app, rect_inner(rect, -pad), 5, cl_rect);
    draw_rectangle_outline_fcolor(app, rect_inner(rect, -pad), 5, 2, cl);
    draw_string(app, face, str, 0.5f*(rect.p0 + rect.p1 - dim), cl);
  }

  if (rect_contains_point(Rf32(r.x1 - w2, r.y0, r.x1, r.y0 + h), mp)){
    Fancy_Block block = {};
    for (Buffer_ID b = get_buffer_next(app, 0, Access_Always); b; b=get_buffer_next(app, b, Access_Always)){
      Dirty_State dirty = buffer_get_dirty_state(app, b);
      if (HasFlag(dirty, DirtyState_UnsavedChanges)){
        Fancy_Line *l = push_fancy_line(scratch, &block, cl_3, push_buffer_base_name(app, scratch, b));
        push_fancy_string(scratch, l, cl_2, 0.5f, 0, string_u8_litexpr("*"));
      }
    }
    Rect_f32 rect = draw_tool_tip(app, face, &block, mp, region, 10, 5, cl_rect);
    draw_rectangle_outline_fcolor(app, rect, 5, 3, fcolor_id(defcolor_margin_active));
  }
}

function void
qol_whole_screen_render_caller(Application_Links *app, Frame_Info frame_info){
  if (def_get_config_b32(vars_save_string_lit("use_code_peek"))){
    qol_draw_peek(app, frame_info);
  }

  if (qol_try_exit_view != 0){
    qol_try_exit_render(app, frame_info);
  }
}
