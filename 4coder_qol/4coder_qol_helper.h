
#define qol_interp(cur, nxt, dt, rate) (cur += (((nxt) - (cur))*(1.f - pow_f32(rate, dt))))

function Rect_f32 rect_vsplit(Rect_f32 r, f32 t, b32 c){ return !c ? rect_split_left_right(r, t).min : rect_split_left_right_neg(r, t).max; }
function Rect_f32 rect_hsplit(Rect_f32 r, f32 t, b32 c){ return !c ? rect_split_top_bottom(r, t).min : rect_split_top_bottom_neg(r, t).max; }

function void
printf_message(Application_Links *app, char *format, ...){
  Scratch_Block scratch(app);
  va_list args;
  va_start(args, format);
  String_Const_u8 result = push_stringfv(scratch, format, args);
  print_message(app, result);
  va_end(args);
}

function Range_i64
get_line_range_from_pos(Application_Links *app, Buffer_ID buffer, i64 pos){
  i64 line = get_line_number_from_pos(app, buffer, pos);
  return get_line_pos_range(app, buffer, line);
}

function void
qol_bot_text_set(String_Const_u8 string){
  block_copy(g_qol_bot_buffer, string.str, string.size);
  g_qol_bot_string.size = string.size;
}

function void
qol_bot_text_append(String_Const_u8 string){
  string_append(&g_qol_bot_string, string);
}

function f32
qol_bot_line_count(Application_Links *app){
  f32 line = def_get_config_f32(app, vars_save_string_lit("bot_line_count"));
  return clamp(1.f, line, 5.f);
}

function f32
qol_bot_text_height(Application_Links *app, Face_Metrics metrics){
  f32 height_char     = metrics.ascent - metrics.descent;
  f32 height_initial  = qol_bot_line_count(app)*height_char;
  f32 height_baseline = 0.5f*height_initial;
  f32 height_required = height_baseline + 0.5f*height_char;
  return 2.f + Max(height_initial, height_required) + 2.f;
}

function f32
qol_bot_text_y_offset(Application_Links *app, Face_Metrics metrics){
  f32 height_char     = metrics.ascent - metrics.descent;
  f32 height_initial  = qol_bot_line_count(app)*height_char;
  f32 height_baseline = 0.5f*height_initial;
  return 2.f + height_baseline + 0.5f*height_char;
}

function void qol_scroll(Application_Links *app, View_ID view, f32 delta_y){
  Buffer_Scroll scroll = view_get_buffer_scroll(app, view);
  scroll.target = view_move_buffer_point(app, view, scroll.target, V2f32(0.f, delta_y));

  f32 dy = view_point_difference(app, view, scroll.target, scroll.position).y;
  i64 pos = view_get_cursor_pos(app, view);
  Buffer_Cursor cursor = view_compute_cursor(app, view, seek_pos(pos));

  f32 line_height = get_view_line_height(app, view);
  f32 pad = line_height*def_get_config_f32(app, vars_save_string_lit("scroll_margin_y"));
  f32 y = line_height*cursor.line;
  f32 y0 = scroll.target.line_number*line_height - scroll.target.pixel_shift.y;
  f32 y1 = y0 + rect_height(view_get_buffer_region(app, view));
  y0 = y0 + pad + dy;
  y1 = y1 - pad + dy;

  Vec2_f32 delta = V2f32(0,0);
  delta += (y>y0 ? V2f32(0,0) : V2f32(y0-y+line_height, 1));
  delta += (y<y1 ? V2f32(0,0) : V2f32(y1-y-line_height, 1));
  if (delta.y > 0){
    i64 line_delta = i64(delta.x / (delta.y*line_height));
    view_set_cursor(app, view, seek_line_col(cursor.line + line_delta, cursor.col));
  }
  view_set_buffer_scroll(app, view, scroll, SetBufferScroll_NoCursorChange);
}