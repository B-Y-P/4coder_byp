
function Rect_f32
vim_get_rel_block_rect(Application_Links *app, View_ID view, Buffer_ID buffer, Range_i64 range, i64 line){
  Vec2_f32 p0 = view_relative_xy_of_pos(app, view, line, range.min);
  Vec2_f32 p1 = view_relative_xy_of_pos(app, view, line, range.max);
  return Rf32(Min(p0.x, p1.x), Min(p0.y, p1.y), Max(p0.x, p1.x), Max(p0.y, p1.y));
}

function Rect_f32
vim_get_abs_block_rect(Application_Links *app, View_ID view, Buffer_ID buffer, Text_Layout_ID text_layout_id, Range_i64 range){
  Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);

  i64 rel_pos = range.min;
  if(!range_contains(visible_range, rel_pos)){ rel_pos = range.max; }
  if(!range_contains(visible_range, rel_pos)){ rel_pos = scan(app, boundary_alpha_numeric, buffer, Scan_Forward, visible_range.min); }
  if(!range_contains(visible_range, rel_pos)){ return Rect_f32{}; }   // This is nearly impossible to hit... nearly

  i64 line = get_line_number_from_pos(app, buffer, rel_pos);
  Vec2_f32 rel_p = view_relative_xy_of_pos(app, view, line, rel_pos);
  Rect_f32 rel_rect = text_layout_character_on_screen(app, text_layout_id, rel_pos);
  Vec2_f32 offset0 = rel_p - rel_rect.p0;
  Vec2_f32 offset1 = rel_p - rel_rect.p1;
  Rect_f32 min_rect, max_rect;

  if(range_contains(visible_range, range.min)){
    min_rect = text_layout_character_on_screen(app, text_layout_id, range.min);
  }else{
    Vec2_f32 min_p = view_relative_xy_of_pos(app, view, line, range.min);
    min_rect = Rf32(min_p - offset0, min_p - offset1);
  }

  if(range_contains(visible_range, range.max)){
    max_rect = text_layout_character_on_screen(app, text_layout_id, range.max);
  }else{
    Vec2_f32 max_p = view_relative_xy_of_pos(app, view, line, range.max);
    max_rect = Rf32(max_p - offset0, max_p - offset1);
  }

  return rect_union(min_rect, max_rect);
}

function void
vim_block_copy(Application_Links *app, View_ID view, Buffer_ID buffer, Range_i64 range, Vim_Register *reg){
  if(reg->flags & REGISTER_ReadOnly){
    vim_state.chord_state = VIM_CHORD_Error;
    Scratch_Block scratch(app);
    vim_set_bottom_text(push_stringf(scratch, "Register %c is Read Only", vim_get_register_char(reg)));
    return;
  }
  reg->edit_type = EDIT_Block;
  reg->flags |= (REGISTER_Set|REGISTER_Updated);
  print_message(app, string_u8_litexpr("Block copy\n"));
  i64 line_min = get_line_number_from_pos(app, buffer, range.min);
  i64 line_max = get_line_number_from_pos(app, buffer, range.max);
  Rect_f32 block_rect = vim_get_rel_block_rect(app, view, buffer, range, line_min);

  f32 line_advance = rect_height(block_rect)/f32(Max(1, line_max-line_min));
  f32 wid = rect_width(block_rect);

  u64 size = 0;
  for(i64 i=line_max; i>=line_min; i--){
    Vec2_f32 min_point = block_rect.p0 + V2f32(0, line_advance*(i-line_min));
    Vec2_f32 max_point = min_point + V2f32(wid,0);
    i64 min_pos = view_pos_at_relative_xy(app, view, line_min, min_point);
    i64 max_pos = view_pos_at_relative_xy(app, view, line_min, max_point)+1;
    size += (max_pos - min_pos) + 1;
  }

  b32 valid = true;
  if(size >= reg->data.cap){ valid = vim_realloc_string(&reg->data, size); }
  if(!valid){ return; }
  Scratch_Block scratch(app);

  reg->data.size = 0;
  for(i64 i=line_max; i>=line_min; i--){
    Vec2_f32 min_point = block_rect.p0 + V2f32(0, line_advance*(i-line_min));
    Vec2_f32 max_point = min_point + V2f32(wid,0);
    i64 min_pos = view_pos_at_relative_xy(app, view, line_min, min_point);
    i64 max_pos = view_pos_at_relative_xy(app, view, line_min, max_point)+1;

    Range_i64 line_range = Ii64(min_pos, max_pos);
    buffer_read_range(app, buffer, line_range, reg->data.str + reg->data.size);
    buffer_post_fade(app, buffer, 0.667f, line_range, fcolor_resolve(fcolor_id(defcolor_paste)));
    reg->data.size += (max_pos - min_pos) + 1;
    reg->data.str[reg->data.size-1] = '\n';
  }
}

function void
vim_block_paste(Application_Links *app, View_ID view, Buffer_ID buffer, Vim_Register *reg){
  i64 cursor_pos = view_get_cursor_pos(app, view);

  History_Group group = history_group_begin(app, buffer);

  i64 line_min = get_line_number_from_pos(app, buffer, cursor_pos);
  i64 line_max = line_min-1;
  foreach(i, i64(reg->data.size)){ line_max += reg->data.str[i] == '\n'; }

  // TODO(BYP): Still doesn't handle empty lines gracefully
  Range_i64 range = Ii64(cursor_pos, buffer_compute_cursor(app, buffer, seek_line_col(line_max, 0)).pos);
  Rect_f32 block_rect = vim_get_rel_block_rect(app, view, buffer, range, line_min);

  //f32 line_advance = get_face_metrics(app, get_face_id(app, buffer)).line_height - 0.01f;
  f32 line_advance = rect_height(block_rect)/f32(Max(1, line_max-line_min));
  f32 wid = rect_width(block_rect);

  Range_i64 substring = {};
  substring.max = -1;
  for(i64 i=line_max; i>=line_min; i--){
    Vec2_f32 point = block_rect.p0 + V2f32(wid, line_advance*(i-line_min));
    i64 pos = view_pos_at_relative_xy(app, view, line_min, point);

    b32 valid=true;
    substring.min = substring.max + 1;
    while(reg->data.str[++substring.max] != '\n'){
      if(substring.max >= i64(reg->data.size)){ valid=false; break; }
    }
    if(!valid){ break; }

    Range_i64 sub = substring;
    //sub.max += (line_is_valid_and_blank(app, buffer, i));
    //pos -= (line_is_valid_and_blank(app, buffer, i));
    buffer_replace_range(app, buffer, Ii64(pos), string_substring(reg->data.string, sub));
    ARGB_Color argb = fcolor_resolve(fcolor_id(defcolor_paste));
    buffer_post_fade(app, buffer, 0.667f, Ii64_size(pos, range_size(sub)), argb);
  }

  history_group_end(group);


  vim_default_register();
}

function void
vim_block_edit(Application_Links *app, View_ID view, Buffer_ID buffer, Range_i64 range){
  Vim_Params *params = &vim_state.params;
  if(params->request == REQUEST_Yank ||
     params->request == REQUEST_Change ||
     params->request == REQUEST_Delete)
  {
    if(vim_state.params.selected_reg){
      vim_block_copy(app, view, buffer, range, vim_state.params.selected_reg);
      vim_update_registers(app);
      vim_state.params.selected_reg = 0;
    }
    if(params->request == REQUEST_Yank){ return; }
  }

  History_Group group = history_group_begin(app, buffer);

  i64 line_min = get_line_number_from_pos(app, buffer, range.min);
  i64 line_max = get_line_number_from_pos(app, buffer, range.max);
  Rect_f32 block_rect = vim_get_rel_block_rect(app, view, buffer, range, line_min);

  f32 line_advance = rect_height(block_rect)/f32(Max(1, line_max-line_min));
  f32 wid = rect_width(block_rect);
  block_rect = rect_inner(block_rect, -0.1f);

  for(i64 i=line_max; i>=line_min; i--){
    if(line_is_valid_and_blank(app, buffer, i)){ continue; }
    Vec2_f32 min_point = block_rect.p0 + V2f32(0, line_advance*(i-line_min));
    Vec2_f32 max_point = min_point + V2f32(wid,0);
    i64 min_pos = view_pos_at_relative_xy(app, view, line_min, min_point);
    i64 max_pos = view_pos_at_relative_xy(app, view, line_min, max_point);

    Vec2_f32 min_p = view_relative_xy_of_pos(app, view, line_min, min_pos);
    Vec2_f32 max_p = view_relative_xy_of_pos(app, view, line_min, max_pos);
    if(!rect_contains_point(block_rect, min_p) || !rect_contains_point(block_rect, max_p)){ continue; }

    Range_i64 line_range = Ii64(min_pos, max_pos+1);

    vim_request_vtable[params->request](app, view, buffer, line_range);
  }

  history_group_end(group);
}