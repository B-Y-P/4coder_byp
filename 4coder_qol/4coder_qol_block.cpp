
typedef void QOL_Range_Func(Application_Links *app, View_ID view, Buffer_ID buffer, Range_i64 range);

function Rect_f32
qol_get_rel_block_rect(Application_Links *app, View_ID view, Buffer_ID buffer, Range_i64 range, i64 line){
  Vec2_f32 p0 = view_relative_xy_of_pos(app, view, line, range.min);
  Vec2_f32 p1 = view_relative_xy_of_pos(app, view, line, range.max);
  return Rf32(Min(p0.x, p1.x), Min(p0.y, p1.y), Max(p0.x, p1.x), Max(p0.y, p1.y));
}

function Rect_f32
qol_get_abs_block_rect(Application_Links *app, View_ID view, Buffer_ID buffer, Text_Layout_ID text_layout_id, Range_i64 range){
  Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);

  i64 rel_pos = range.min;
  if (!range_contains(visible_range, rel_pos)){ rel_pos = range.max; }
  if (!range_contains(visible_range, rel_pos)){ rel_pos = scan(app, boundary_alpha_numeric, buffer, Scan_Forward, visible_range.min); }
  if (!range_contains(visible_range, rel_pos)){ return Rect_f32{}; }   // This is nearly impossible to hit... nearly

  i64 line = get_line_number_from_pos(app, buffer, rel_pos);
  Vec2_f32 rel_p = view_relative_xy_of_pos(app, view, line, rel_pos);
  Rect_f32 rel_rect = text_layout_character_on_screen(app, text_layout_id, rel_pos);
  Vec2_f32 offset0 = rel_p - rel_rect.p0;
  Vec2_f32 offset1 = rel_p - rel_rect.p1;
  Rect_f32 min_rect, max_rect;

  if (range_contains(visible_range, range.min)){
    min_rect = text_layout_character_on_screen(app, text_layout_id, range.min);
  }
  else{
    Vec2_f32 min_p = view_relative_xy_of_pos(app, view, line, range.min);
    min_rect = Rf32(min_p - offset0, min_p - offset1);
  }

  if (range_contains(visible_range, range.max)){
    max_rect = text_layout_character_on_screen(app, text_layout_id, range.max);
  }
  else{
    Vec2_f32 max_p = view_relative_xy_of_pos(app, view, line, range.max);
    max_rect = Rf32(max_p - offset0, max_p - offset1);
  }

  return rect_union(min_rect, max_rect);
}

struct Block_Iter{
  Application_Links *app;
  View_ID view;
  Buffer_ID buffer;
  i64 line;
  i64 line_min;
  i64 line_max;
  Rect_f32 rect;
  Range_i64 range;
};

function bool
qol_block_iter_next(Block_Iter *iter){
  Application_Links *app = iter->app;
  View_ID view = iter->view;
  Buffer_ID buffer = iter->buffer;
  f32 line_advance = rect_height(iter->rect)/f32(Max(1.f, iter->line_max - iter->line_min));
  for (; iter->line >= iter->line_min; iter->line -= 1){
    if (line_is_valid_and_blank(app, buffer, iter->line)){ continue; }
    f32 y = line_advance*(iter->line - iter->line_min);
    Vec2_f32 min_point = V2f32(iter->rect.x0, y);
    Vec2_f32 max_point = V2f32(iter->rect.x1, y);
    i64 min_pos = view_pos_at_relative_xy(app, view, iter->line_min, min_point);
    i64 max_pos = view_pos_at_relative_xy(app, view, iter->line_min, max_point);

    Vec2_f32 min_p = view_relative_xy_of_pos(app, view, iter->line_min, min_pos);
    Vec2_f32 max_p = view_relative_xy_of_pos(app, view, iter->line_min, max_pos);
    bool contains_min = rect_contains_point(iter->rect, min_p);
    bool contains_max = rect_contains_point(iter->rect, max_p);
    if (!contains_min || !contains_max){ continue; }

    iter->range = Ii64(min_pos, max_pos+1);
    iter->line -= 1;
    return true;
  }
  return false;
}

function Block_Iter
qol_block_iter_init(Application_Links *app, View_ID view, Range_i64 range){
  Block_Iter result;
  result.app = app;
  result.view = view;
  result.buffer = view_get_buffer(app, view, Access_ReadVisible);
  result.line_min = get_line_number_from_pos(app, result.buffer, range.min);
  result.line_max = get_line_number_from_pos(app, result.buffer, range.max);
  result.line = result.line_max;
  result.rect = qol_get_rel_block_rect(app, view, result.buffer, range, result.line_min);
  result.rect.x1 += 0.01f;
  result.rect.y1 += 0.01f;
  return result;
}

function void
qol_block_apply(Application_Links *app, View_ID view, Buffer_ID buffer, Range_i64 block, QOL_Range_Func *func){
  for (Block_Iter iter = qol_block_iter_init(app, view, block); qol_block_iter_next(&iter);){
    func(app, view, buffer, iter.range);
  }
}

function void
qol_range_delete(Application_Links *app, View_ID view, Buffer_ID buffer, Range_i64 range){
  buffer_replace_range(app, buffer, range, string_u8_empty);
}

function void
qol_range_fade(Application_Links *app, View_ID view, Buffer_ID buffer, Range_i64 range){
  buffer_post_fade(app, buffer, 0.667f, range, fcolor_resolve(fcolor_id(defcolor_pop2)));
}

function void
qol_block_delete(Application_Links *app, View_ID view, Buffer_ID buffer, Range_i64 range){
  History_Group group = history_group_begin(app, buffer);
  qol_block_apply(app, view, buffer, range, qol_range_delete);
  history_group_end(group);
}