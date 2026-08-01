
function void
vim_draw_visual_mode(Application_Links *app, View_ID view, Buffer_ID buffer, Face_ID face_id, Text_Layout_ID text_layout_id){
  Range_i64 range = Ii64(view_get_cursor_pos(app, view), view_get_mark_pos(app, view));
  Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
  visible_range.max++;

  ARGB_Color text_color = fcolor_resolve(fcolor_id(defcolor_at_highlight));
  switch(vim_state.params.edit_type){
    case EDIT_Block:{
      Rect_f32 block_rect = vim_get_abs_block_rect(app, view, buffer, text_layout_id, range);
      draw_rectangle_fcolor(app, block_rect, 5.f, fcolor_id(defcolor_highlight));

      i64 line_min = get_line_number_from_pos(app, buffer, range.min);
      i64 line_max = get_line_number_from_pos(app, buffer, range.max);
      f32 line_advance = rect_height(block_rect)/f32(Max(1, line_max-line_min));
      f32 wid = rect_width(block_rect);

      Range_i64 test_range = range_intersect(visible_range, range);
      i64 test_line_min = get_line_number_from_pos(app, buffer, test_range.min);
      i64 test_line_max = get_line_number_from_pos(app, buffer, test_range.max);

      b32 show_helpers = def_get_config_b32(vars_save_string_lit("vim_show_visual_block_markers"));
      ARGB_Color helper_color = fcolor_resolve(fcolor_id(defcolor_mark));
      for(i64 i=test_line_min; i<=test_line_max; i++){
        if(line_is_valid_and_blank(app, buffer, i) && i != line_min && i != line_max){ continue; }
        Vec2_f32 min_point = block_rect.p0 + V2f32(0, line_advance*(i-line_min));
        Vec2_f32 max_point = min_point + V2f32(wid,0);
        i64 min_pos = view_pos_from_xy(app, view, min_point);
        i64 max_pos = view_pos_from_xy(app, view, max_point);
        paint_text_color(app, text_layout_id, Ii64(min_pos, max_pos), text_color);

        if(!show_helpers || min_pos == max_pos){ continue; }
        Rect_f32 min_rect = text_layout_character_on_screen(app, text_layout_id, min_pos);
        Rect_f32 max_rect = text_layout_character_on_screen(app, text_layout_id, max_pos-1);
        draw_rectangle(app, rect_split_top_bottom_neg(min_rect, 3.f).b, 3.0f, helper_color);
        draw_rectangle(app, rect_split_top_bottom(max_rect,     3.f).a, 3.0f, helper_color);
        draw_rectangle(app, rect_split_left_right(min_rect,     3.f).a, 3.0f, helper_color);
        draw_rectangle(app, rect_split_left_right_neg(max_rect, 3.f).b, 3.0f, helper_color);
      }
    } break;

    case EDIT_LineWise:{
      range = range_intersect(range, visible_range);
      i64 line_min = get_line_number_from_pos(app, buffer, range.min);
      i64 line_max = get_line_number_from_pos(app, buffer, range.max);
      if(def_get_config_b32(vars_save_string_lit("vim_full_visual_lines"))){
        draw_line_highlight(app, text_layout_id, Ii64(line_min, line_max), fcolor_id(defcolor_highlight));
      }else{
        range.min = view_pos_at_relative_xy(app, view, line_min, V2f32(0.f,     view_relative_xy_of_pos(app, view, line_min, range.min).y));
        range.max = view_pos_at_relative_xy(app, view, line_max, V2f32(max_f32, view_relative_xy_of_pos(app, view, line_max, range.max).y))+1;
        draw_character_block(app, text_layout_id, range, 5.f, fcolor_id(defcolor_highlight));
      }
      paint_text_color(app, text_layout_id, range, text_color);
    } break;

    case EDIT_CharWise:{
      range.max++;
      range = range_intersect(range, visible_range);
      draw_character_block(app, text_layout_id, range, 5.f, fcolor_id(defcolor_highlight));
      paint_text_color(app, text_layout_id, range, text_color);
    } break;
  }
}

function void
vim_draw_filebar(Application_Links *app, View_ID view_id, Buffer_ID buffer, Face_ID face_id, Rect_f32 bar){
  Scratch_Block scratch(app);
  String_Const_u8 unique_name = push_buffer_unique_name(app, scratch, buffer);

  draw_rectangle_fcolor(app, bar, 0.f, fcolor_id(defcolor_bar));

  f32 char_wid = get_face_metrics(app, face_id).normal_advance;
  Rect_f32 title_rect = bar;
  title_rect.x1 = bar.x0 + char_wid*unique_name.size;
  draw_rectangle_fcolor(app, title_rect, 0.f, fcolor_id(defcolor_vim_filebar_pop));

  Rect_f32 triangle_rect = title_rect;
  f32 radius_fudge = 5.f;
  triangle_rect.x0 = title_rect.x1 - radius_fudge*char_wid;
  triangle_rect.x1 = title_rect.x1 + radius_fudge*char_wid;
  draw_rectangle_fcolor(app, triangle_rect, radius_fudge*char_wid, fcolor_id(defcolor_vim_filebar_pop));

  FColor base_color = fcolor_id(defcolor_base);
  FColor pop2_color = fcolor_id(defcolor_pop2);

  i64 cursor_position = view_get_cursor_pos(app, view_id);
  Buffer_Cursor cursor = view_compute_cursor(app, view_id, seek_pos(cursor_position));

  u8 space[5];
  String_u8 str = Su8(space, 0, 4);

  Managed_Scope scope = buffer_get_managed_scope(app, buffer);
  Line_Ending_Kind *eol_kind = scope_attachment(app, scope, buffer_eol_setting, Line_Ending_Kind);
  switch(*eol_kind){
    case LineEndingKind_Binary:{ string_append(&str, string_u8_litexpr("bin"));  } break;
    case LineEndingKind_LF:    { string_append(&str, string_u8_litexpr("lf"));   } break;
    case LineEndingKind_CRLF:  { string_append(&str, string_u8_litexpr("crlf")); } break;
  }


  Vec2_f32 p = V2f32(title_rect.x1 + 4.5f*char_wid, bar.y0 + 3.f);
  p = draw_string(app, face_id, str.string, p, base_color);

  str = Su8(space, 0, 5);
  Dirty_State dirty = buffer_get_dirty_state(app, buffer);
  if(dirty != 0){
    string_append(&str, string_u8_litexpr(" ["));
    if(HasFlag(dirty, DirtyState_UnsavedChanges))  string_append_character(&str, '+');
    if(HasFlag(dirty, DirtyState_UnloadedChanges)) string_append_character(&str, '!');
    string_append(&str, string_u8_litexpr("]"));
    draw_string(app, face_id, str.string, p, pop2_color);
  }

  p.x = Max(p.x + 5.f*char_wid, bar.x1 - char_wid*15.f);
  draw_string(app, face_id, push_stringf(scratch, "%d,%d", cursor.line, cursor.col), p, base_color);

  p.x = bar.x0 + 2.f;
  draw_string(app, face_id, unique_name, p, base_color);

  // TODO: vim_state.mode             == VIM_Visual "Selection: %d lines, %d chars"
  // TODO: vim_state.params.edit_type == EDIT_Block "Selection: %d x %d block, %d chars"

  p.x = bar.x1 - char_wid*3.5f;
  i64 N = buffer_get_size(app, buffer);
  String_Const_u8 pos_text = (cursor_position == 0 ? string_u8_litexpr("Top") :
                              cursor_position == N ? string_u8_litexpr("Bot") :
                              push_stringf(scratch, "%d%%", i64(100.f*cursor_position/(N))));
  draw_string(app, face_id, pos_text, p, base_color);
}

function void
vim_draw_search_highlight(Application_Links *app, View_ID view, Buffer_ID buffer, Text_Layout_ID text_layout_id, f32 roundness){
  String_u8 *pattern = &vim_registers.search.data;
  if(pattern->size == 0){ return; }
  Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
  i64 buffer_size = buffer_get_size(app, buffer);
  i64 cur_pos = visible_range.min;
  while(cur_pos < visible_range.max){
    i64 new_pos = 0;
    seek_string_forward(app, buffer, cur_pos, 0, pattern->string, &new_pos);
    if(new_pos == 0 || new_pos == buffer_size){ break; }
    else{
      cur_pos = new_pos;
      Rect_f32 rect = text_layout_character_on_screen(app, text_layout_id, cur_pos);
      rect.x1 = rect.x0 + pattern->size*(rect_width(rect));
      draw_rectangle_fcolor(app, rect, roundness, fcolor_id(defcolor_highlight));
    }
  }
}

function void
vim_draw_after_text(Application_Links *app, View_ID view, b32 is_active_view, Buffer_ID buffer, Text_Layout_ID text_layout_id, f32 cursor_roundness, f32 mark_thickness){
  if(is_active_view && vim_is_selecting_register && vim_state.mode == VIM_Insert){
    i64 cursor_pos = view_get_cursor_pos(app, view);
    Rect_f32 cursor_rect = text_layout_character_on_screen(app, text_layout_id, cursor_pos);
    draw_rectangle_fcolor(app, cursor_rect, 0.f, fcolor_id(defcolor_back));
    if(!def_get_config_b32(vars_save_string_lit("highlight_line_at_cursor"))){
      draw_rectangle_fcolor(app, cursor_rect, 0.f, fcolor_id(defcolor_highlight_cursor_line));
    }
    //vim_draw_cursor(app, view, is_active_view, buffer, text_layout_id, cursor_roundness, mark_thickness);
    draw_string(app, get_face_id(app, 0), string_u8_litexpr("\""), cursor_rect.p0, fcolor_id(defcolor_text_default));
  }
}

function Rect_f32
vim_draw_query_bars(Application_Links *app, Rect_f32 region, View_ID view_id, Face_ID face_id){
  Face_Metrics face_metrics = get_face_metrics(app, face_id);
  f32 line_height = face_metrics.line_height;

  Query_Bar *space[32];
  Query_Bar_Ptr_Array query_bars = {};
  query_bars.ptrs = space;
  if(get_active_query_bars(app, view_id, ArrayCount(space), &query_bars)){
    foreach(i,query_bars.count){
      Rect_f32_Pair pair = layout_query_bar_on_bot(region, line_height, 1);
      draw_rectangle_fcolor(app, pair.max, 0.f, fcolor_id(defcolor_back));
      draw_query_bar(app, query_bars.ptrs[i], face_id, pair.max);
      region = pair.min;
    }
  }
  return region;
}

function Rect_f32_Pair
vim_line_number_margin(Application_Links *app, Buffer_ID buffer, Rect_f32 rect, f32 digit_advance){
  i64 line_count = buffer_get_line_count(app, buffer);
  i64 digit_count = digit_count_from_integer(line_count, 10) + i64(def_get_config_b32(vars_save_string_lit("vim_line_numbers_relative")));

  f32 margin_width = (f32)digit_count*digit_advance + 6.f;
  Rect_f32_Pair pair = rect_split_left_right(rect, margin_width);
  pair.a = rect_split_left_right(pair.a, 6.f).b;
  pair.b = rect_split_left_right(pair.b, 4.f).b;
  return pair;
}


function void
vim_draw_rel_line_number_margin(Application_Links *app, View_ID view, Buffer_ID buffer, Face_ID face, Text_Layout_ID text_layout_id, Rect_f32 margin){
  Rect_f32 prev_clip = draw_set_clip(app, margin);
  draw_rectangle_fcolor(app, margin, 0.f, fcolor_id(defcolor_line_numbers_back));

  const i64 cur_line = get_line_number_from_pos(app, buffer, view_get_cursor_pos(app, view));
  const i64 line_count = buffer_get_line_count(app, buffer);

  i64 cur_line_digit_count = digit_count_from_integer(cur_line, 10);
  i64 bot_line_digit_count = digit_count_from_integer(line_count, 10);
  i64 digit_count = Max(cur_line_digit_count+1, bot_line_digit_count);

  Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
  Buffer_Cursor cursor = view_compute_cursor(app, view, seek_pos(visible_range.first));
  Buffer_Cursor cursor_end = view_compute_cursor(app, view, seek_pos(visible_range.end));
  const i64 first_line_num = cursor.line;
  const i64 one_past_last = cursor_end.line;

  Scratch_Block scratch(app);
  u8 *digit_buffer = push_array(scratch, u8, digit_count);
  String_Const_u8 digit_string = SCu8(digit_buffer, digit_count);
  foreach(i, digit_count){ digit_buffer[i] = ' '; }

  u8 *small_digit = digit_buffer + (digit_count-1) - 1;
  u8 *ptr = small_digit;
  if(cur_line == 0){ *ptr = '0'; }
  else{
    for(u64 X=cur_line; X>0; X/=10){
      *ptr-- = '0' + (X%10);
    }
  }
  small_digit++;

  Range_f32 line_y = text_layout_line_on_screen(app, text_layout_id, cur_line);
  Vec2_f32 p = V2f32(margin.x0, line_y.min);

  // NOTE(BYP): This assumes background is darker than font color
  FColor text_color = fcolor_id(defcolor_line_numbers_text);
  FColor contrast_color = fcolor_blend(text_color, 0.4f, f_white);

  draw_string(app, face, digit_string, p, fcolor_resolve(contrast_color));

  i32 rel_num = 1;
  foreach(i, digit_count-1){ digit_buffer[i] = ' '; }
  digit_buffer[digit_count-1] = '1';

  for(;;){
    i64 bot_line = cur_line+rel_num;
    if(bot_line > one_past_last){ break; }

    for(;;){
      line_y = text_layout_line_on_screen(app, text_layout_id, cur_line+rel_num);
      if(line_y.min != line_y.max){ break; }
      rel_num++;
    }
    p = V2f32(margin.x0, line_y.min);
    draw_string(app, face, digit_string, p, fcolor_resolve(text_color));

    rel_num++;
    ptr = small_digit;
    while(ptr >= digit_buffer){
      if(*ptr == ' '){ *ptr   = '0'; }
      if(*ptr == '9'){ *ptr-- = '0'; }
      else{ (*ptr)++; break; }
    }
  }

  rel_num = 1;
  foreach(i, digit_count-1){ digit_buffer[i] = ' '; }
  digit_buffer[digit_count-1] = '1';

  for(;;){
    i64 top_line = cur_line-rel_num;
    if(top_line < first_line_num){ break; }

    for(;;){
      line_y = text_layout_line_on_screen(app, text_layout_id, cur_line-rel_num);
      if(line_y.min != line_y.max){ break; }
      rel_num++;
    }
    p = V2f32(margin.x0, line_y.min);
    draw_string(app, face, digit_string, p, fcolor_resolve(text_color));

    rel_num++;
    ptr = small_digit;
    while(ptr >= digit_buffer){
      if(*ptr == ' '){ *ptr   = '0'; }
      if(*ptr == '9'){ *ptr-- = '0'; }
      else{ (*ptr)++; break; }
    }
  }
  draw_set_clip(app, prev_clip);
}

function void
vim_draw_abs_line_number_margin(Application_Links *app, View_ID view, Buffer_ID buffer, Face_ID face, Text_Layout_ID text_layout_id, Rect_f32 margin){

  Scratch_Block scratch(app);
  Rect_f32 prev_clip = draw_set_clip(app, margin);
  draw_rectangle_fcolor(app, margin, 0.f, fcolor_id(defcolor_line_numbers_back));

  Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
  i64 line_count = buffer_get_line_count(app, buffer);
  i64 digit_count = digit_count_from_integer(line_count, 10);

  u8 *digit_buffer = push_array(scratch, u8, digit_count);
  String_Const_u8 digit_string = SCu8(digit_buffer, digit_count);
  foreach(i, digit_count){ digit_buffer[i] = ' '; }

  i64 cur_line = view_compute_cursor(app, view, seek_pos(visible_range.min)).line;
  i64 end_line = view_compute_cursor(app, view, seek_pos(visible_range.max)).line+1;

  u8 *small_digit = digit_buffer + (digit_count-1) - 1;
  u8 *ptr = small_digit;
  if(cur_line == 0){ *ptr = '0'; }
  else{
    for(u64 X=cur_line; X>0; X/=10){
      *ptr-- = '0' + (X%10);
    }
  }

  Range_f32 line_y = text_layout_line_on_screen(app, text_layout_id, cur_line);
  Vec2_f32 p = V2f32(margin.x0, line_y.min);

  // NOTE(BYP): This assumes background is darker than font color
  FColor text_color = fcolor_id(defcolor_line_numbers_text);
  FColor contrast_color = fcolor_blend(text_color, 0.4f, f_white);

  draw_string(app, face, digit_string, p, fcolor_resolve(contrast_color));

  for(;;){
    if(cur_line > end_line){ break; }

    line_y = text_layout_line_on_screen(app, text_layout_id, cur_line);
    if(line_y.min != line_y.max){
      p = V2f32(margin.x0, line_y.min);
      draw_string(app, face, digit_string, p, fcolor_resolve(text_color));
    }

    cur_line++;
    ptr = small_digit;
    while(ptr >= digit_buffer){
      if(*ptr == ' '){ *ptr   = '0'; }
      if(*ptr == '9'){ *ptr-- = '0'; }
      else{ (*ptr)++; break; }
    }
  }

  draw_set_clip(app, prev_clip);
}