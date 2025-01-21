
function void
byp_reformat_buffer(Application_Links *app, Buffer_ID buffer){
  Token_Array arr = get_token_array_from_buffer(app, buffer);
  if(arr.tokens == 0){ return; }
  Face_ID face = get_face_id(app, buffer);
  Face_Metrics metrics = get_face_metrics(app, face);

  Scratch_Block scratch(app);
  String_Const_u8 spaces = string_u8_litexpr("                                ");

  Batch_Edit *batch_first = 0;
  Batch_Edit *batch_last = 0;
  i64 line_count = buffer_get_line_count(app, buffer);
  for(i64 line = 1; line <= line_count; line += 1){
    i64 line_start = get_line_side_pos(app, buffer, line, Side_Min);
    i64 line_end = get_line_side_pos(app, buffer, line, Side_Max);
    if(line_start == line_end){ continue; }  // line is empty

    Token* token = get_token_from_pos(app, &arr, line_start);
    if(token == 0){ continue; }
    i64 token_end = token->pos + token->size;
    b32 is_whitespace = (token->kind == TokenBaseKind_Whitespace);
    b32 on_same_line  = (token_end < line_end);
    b32 clear_blank   = (is_whitespace && !on_same_line);

    i64 ws_end = (!is_whitespace ? line_start :  // Need to insert spaces
                  on_same_line   ? token_end     // Need to replace ws-token
                  /* - else - */ : line_end);    // Need to clear line
    i64 count = 0;

    if(!clear_blank){
      f32 x = buffer_relative_box_of_pos(app, buffer, 1e9f, face, line, ws_end).x0;
      count = (i64)(x / metrics.space_advance);
      if(count < 0){ continue; }  // impossible, but better safe than sorry

      // line is spaced correctly, might be able to skip
      if(count == ws_end-line_start){
        b32 all_spaces = true;
        for(i64 i=0; i<count; i++){
          all_spaces &= (' ' == buffer_get_char(app, buffer, line_start+i));
        }
        if(all_spaces){ continue; }  // No need to edit, line is already good to go
      }
    }

    if((i64)spaces.size < count){
      spaces = string_const_u8_push(scratch, count);
      block_fill_u8(spaces.str, count, ' ');
    }

    Batch_Edit *batch = push_array(scratch, Batch_Edit, 1);
    sll_queue_push(batch_first, batch_last, batch);
    batch->edit.text = string_prefix(spaces, count);
    batch->edit.range = Ii64(line_start, ws_end);
  }

  if(batch_first != 0){
    buffer_batch_edit(app, buffer, batch_first);
  }
}

function void
trim_leading_whitespace(Application_Links *app, Buffer_ID buffer){

  Scratch_Block scratch(app);
  Batch_Edit *batch_first = 0;
  Batch_Edit *batch_last = 0;
  i64 line_count = buffer_get_line_count(app, buffer);
  for(i64 line_number = 1; line_number <= line_count; line_number += 1){
    i64 line_start = get_line_side_pos(app, buffer, line_number, Side_Min);
    i64 line_end = get_line_side_pos(app, buffer, line_number, Side_Max);
    i64 ws_start = line_start;
    i64 ws_end = -1;

    {
      Temp_Memory_Block temp(scratch);
      String_Const_u8 line = push_buffer_range(app, scratch, buffer, Ii64(line_start, line_end));
      for(u64 i=0; i<line.size; i += 1){
        if(!character_is_whitespace(line.str[i])){ break; }
        ws_end = line_start + i + 1;
      }
    }

    if(ws_end == -1){ continue; }

    Batch_Edit *batch = push_array(scratch, Batch_Edit, 1);
    sll_queue_push(batch_first, batch_last, batch);
    batch->edit.text = string_u8_empty;
    batch->edit.range = Ii64(ws_start, ws_end);
  }

  if (batch_first != 0){
    buffer_batch_edit(app, buffer, batch_first);
  }
}


