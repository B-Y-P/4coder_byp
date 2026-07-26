function void qol_reformat_buffer(Application_Links *app, Buffer_ID buffer){
  Scratch_Block scratch(app);
  Face_ID face = get_face_id(app, buffer);
  Face_Metrics metrics = get_face_metrics(app, face);
  b32 use_tabs     = def_get_config_b32(vars_save_string_lit("indent_with_tabs"));
  b32 clear_blanks = def_get_config_b32(vars_save_string_lit("clear_blank_lines"));
  u64 tab_size     = def_get_config_u64(app, vars_save_string_lit("indent_width"));
  f32 tab_width    = metrics.space_advance * clamp_bot(1, tab_size);

  code_index_lock();
  defer{ code_index_unlock(); };
  Code_Index_File *file = code_index_get_file(buffer);
  if (file == 0){ return; }

  String_ID key = vars_save_string_lit("virtual_whitespace_regular_indent");
  u64 prev_indent = def_get_config_u64(app, key);
  def_set_config_u64(app, key, tab_size);
  Layout_Item_List list = layout_index__inner(app, scratch, buffer, buffer_range(app, buffer), face, max_f32, file, Layout_Unwrapped);
  def_set_config_u64(app, key, prev_indent);
  Layout_Reflex reflex = get_layout_reflex(&list, buffer, max_f32, face);

  String_Const_u8 chars = string_u8_empty;
  Character_Predicate cc = character_predicate_non_whitespace | character_predicate_from_character('\n');

  Batch_Edit *batch_first = 0;
  Batch_Edit *batch_last = 0;
  i64 line_count = buffer_get_line_count(app, buffer);
  for (i64 line_num=1; line_num <= line_count; line_num += 1){
    i64 line_start = get_line_side_pos(app, buffer, line_num, Side_Min);
    Indent_Info line = get_indent_info_line_number_and_start(app, buffer, line_num, line_start, i32(tab_width));
    Range_i64 range = Ii64(line_start, line.first_char_pos);

    if (clear_blanks && line.is_blank){
      if (range_size(range) != 0){
        Batch_Edit *batch = push_array(scratch, Batch_Edit, 1);
        sll_queue_push(batch_first, batch_last, batch);
        batch->edit.text = string_u8_empty;
        batch->edit.range = range;
      }
      continue;
    }

    i64 shift = i64(layout_index_x_shift(app, &reflex, file, line.first_char_pos, tab_width) / metrics.space_advance);
    if (shift < 0){ continue; }  // sanity
    String_Const_u8 ws = string_u8_empty;
    if (shift != 0){
      if (use_tabs){
        u64   tab_count = shift / tab_size;
        u64 space_count = shift % tab_size;
        if (chars.size < tab_count + tab_size-1){
          chars = string_const_u8_push(scratch, 2*tab_count + tab_size-1);
          block_fill_u8(chars.str, 2*tab_count, '\t');
          block_fill_u8(chars.str + 2*tab_count, tab_size-1, ' ');
        }
        ws = string_postfix(chars, tab_count + tab_size-1);
        ws = string_prefix(ws, tab_count + space_count);
      } else {
        u64 space_count = shift;
        if (chars.size < space_count){
          chars = string_const_u8_push(scratch, 2*space_count);
          block_fill_u8(chars.str, chars.size, ' ');
        }
        ws = string_prefix(chars, space_count);
      }
    }

    b32 skip_leading = false;
    if (i64(ws.size) == range_size(range)){
      Temp_Memory_Block temp(scratch);
      String_Const_u8 str = push_buffer_range(app, scratch, buffer, range);
      skip_leading = string_match(str, ws);
    }

    if (!skip_leading){
      Batch_Edit *batch = push_array(scratch, Batch_Edit, 1);
      sll_queue_push(batch_first, batch_last, batch);
      batch->edit.text = ws;
      batch->edit.range = range;
    }

    // trailing whitespace
    if (line.is_blank){ continue; }
    i64 p1 = line.line_end_pos;
    i64 p0 = 1+buffer_seek_character_class(app, buffer, &cc, Scan_Backward, p1).range.min;
    if (p0 < p1){
      Batch_Edit *batch = push_array(scratch, Batch_Edit, 1);
      sll_queue_push(batch_first, batch_last, batch);
      batch->edit.text = string_u8_empty;
      batch->edit.range = Ii64(p0, p1);
    }
  }
  if (batch_first != 0){
    buffer_batch_edit(app, buffer, batch_first);
    buffer_clear_layout_cache(app, buffer);
  }
}

CUSTOM_COMMAND_SIG(qol_reformat_current)
CUSTOM_DOC("[QOL] reformat buffer via code index")
{
  View_ID view = get_active_view(app, Access_ReadWrite);
  qol_reformat_buffer(app, view_get_buffer(app, view, Access_ReadWrite));
}

CUSTOM_COMMAND_SIG(qol_format_all_buffers)
CUSTOM_DOC("[QOL] Auto-indent and remove blank lines for all loaded buffers")
{
  for (Buffer_ID b=0; b=get_buffer_next(app, b, Access_ReadWrite);){
    qol_reformat_buffer(app, b);
  }
  save_all_dirty_buffers(app);
}