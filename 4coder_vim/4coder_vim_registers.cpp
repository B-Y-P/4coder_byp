
#include "calc.hpp"

function void
vim_update_registers(Application_Links *app){
#if VIM_USE_REIGSTER_BUFFER
  Buffer_ID buffer = buffer_identifier_to_id(app, buffer_identifier(string_u8_litexpr("*registers*")));
  i64 buffer_size = buffer_get_size(app, buffer);

  f32 advance = get_face_metrics(app, get_face_id(app, buffer)).normal_advance;
  i64 max_line_size = i64(rect_width(global_get_screen_rectangle(app))/advance) - 4;
  max_line_size = Max(max_line_size, 30);

  Scratch_Block scratch(app);
  Batch_Edit head = {};
  Batch_Edit *last = &head;

  String_Const_u8 top_text = string_u8_litexpr("Vim Registers\n-+-----------");

  head.edit.range = Ii64(0, top_text.size);
  head.edit.text = top_text;

  i64 total_size = top_text.size;
  foreach(i, ArrayCount(vim_registers.r)){
  Vim_Register *reg = vim_registers.r + i;
  if(reg->flags & REGISTER_Set){
  u8 reg_char = vim_get_register_char(reg);
  i64 size = Min(i64(reg->data.size), max_line_size);
  size -= (reg->data.str[size-1] == '\n');

  last = (last->next = push_array(scratch, Batch_Edit, 1));
  u8 *str = push_array(scratch, u8, 4);
  str[0] = '\n';
  str[1] = reg_char;
  str[2] = '|';
  str[3] = ' ';

  last->edit.text = SCu8(str, 4);
  last->edit.range = Ii64_size(total_size, 4);
  total_size += 4;

  last = (last->next = push_array(scratch, Batch_Edit, 1));
  last->edit.text = string_substring(reg->data.string, Ii64(0, size));
  last->edit.range = Ii64_size(total_size, size);
  total_size += size;
  if(reg->flags & REGISTER_Updated){
  reg->flags &= (~REGISTER_Updated);
  buffer_post_fade(app, buffer, 0.667f, last->edit.range, fcolor_resolve(fcolor_id(defcolor_cursor)));
  }
  }
  }
  last->next = 0;
  String_Const_u8 blank_string = SCu8(push_array(scratch, u8, total_size+1), total_size+1);
  blank_string.str[total_size] = ' ';
  buffer_replace_range(app, buffer, Ii64(0, buffer_size), blank_string);
  buffer_batch_edit(app, buffer, &head);
#endif
}

function void vim_eval_register(Application_Links *app, Vim_Register *reg){
  reg->data.str[reg->data.size] = 0;
  Scratch_Block scratch(app);
  vim_register_copy(reg, push_stringf(scratch, "%g", EXPR_ParseString(scratch, (char *)reg->data.str)));
}

function void vim_push_reg_cycle(Application_Links *app){
  Scratch_Block scratch(app);
  for(i32 i=ArrayCount(vim_registers.cycle)-1; i>0; i--){
    if(vim_registers.cycle[i-1].flags & REGISTER_Set){
      vim_register_copy(&vim_registers.cycle[i], &vim_registers.cycle[i-1]);
    }
  }
}

function void
vim_copy(Application_Links *app, View_ID view, Buffer_ID buffer, Range_i64 range, Vim_Register *reg){
  if(reg->flags & REGISTER_ReadOnly){
    vim_state.chord_state = VIM_CHORD_Error;
    Scratch_Block scratch(app);
    vim_set_bottom_text(push_stringf(scratch, "Register %c is Read Only", vim_get_register_char(reg)));
    return;
  }

  u32 append = ((reg->flags & REGISTER_Append) != 0);
  u64 size = u64(range_size(range)) + append*reg->data.size;

  b32 valid = true;
  if(size >= reg->data.cap){ valid = vim_realloc_string(&reg->data, size); }

  if(!valid){ return; }

  buffer_read_range(app, buffer, range, reg->data.str + append*reg->data.size);
  reg->data.size = size;

  buffer_post_fade(app, buffer, 0.667f, range, fcolor_resolve(fcolor_id(defcolor_paste)));
  reg->flags |= (REGISTER_Set|REGISTER_Updated);

  if(reg != &vim_registers.yank && vim_state.params.request == REQUEST_Yank){
    vim_register_copy(&vim_registers.yank, reg);
  }

  if(0){}
  else if(reg == &vim_registers.system){ clipboard_post(0, reg->data.string); }
  else if(reg == &vim_registers.expression){ vim_eval_register(app, reg); }

  vim_update_registers(app);
}

function void
vim_paste(Application_Links *app, View_ID view, Buffer_ID buffer, Vim_Register *reg){
  if(reg->edit_type == EDIT_Block){ vim_block_paste(app, view, buffer, reg); return; }

  i64 pos = view_get_cursor_pos(app, view);
  if(reg == &vim_registers.system){
    clipboard_update_history_from_system(app, 0);
    clipboard_update_history_from_system(app, 0);
    i32 count = clipboard_count(0);
    if(count > 0){
      Managed_Scope scope = view_get_managed_scope(app, view);
      i32 *paste_index = scope_attachment(app, scope, view_paste_index_loc, i32);
      if(paste_index){
        Scratch_Block scratch(app);
        vim_register_copy(reg, push_clipboard_index(app, scratch, 0, *paste_index=0));
        vim_update_registers(app);
      }
    }
  }
  buffer_replace_range(app, buffer, Ii64(pos), reg->data.string);
  view_set_mark(app, view, seek_pos(pos));
  i64 cursor_pos = pos + ((i32)reg->data.string.size - (vim_state.mode != VIM_Insert));
  view_set_cursor_and_preferred_x(app, view, seek_pos(cursor_pos));

  ARGB_Color argb = fcolor_resolve(fcolor_id(defcolor_paste));
  buffer_post_fade(app, buffer, 0.667f, Ii64_size(pos, reg->data.string.size), argb);

  vim_state.prev_params.selected_reg = vim_state.params.selected_reg;
  vim_default_register();
}


VIM_COMMAND_SIG(vim_select_register){
  vim_is_selecting_register = true;
  u8 c = vim_query_user_key(app, string_u8_litexpr("-- SELECT REGISTER --"));
  vim_is_selecting_register = false;
  if(c == 0){ vim_state.chord_state = VIM_CHORD_Error; return; }
  Vim_Register *reg = vim_state.params.selected_reg;
  u32 append = 0;

  if(0){}
  else if(c == '_'){ reg = 0; }
  else if(c == '"'){ reg = &vim_registers.unnamed;      }
  else if(c == '/'){ reg = &vim_registers.search;       }
  else if(c == '.'){ reg = &vim_registers.insert;       }
  else if(c == ':'){ reg = &vim_registers.command;      }
  else if(c == '='){ reg = &vim_registers.expression;   }
  else if(c == '-'){ reg = &vim_registers.small_delete; }
  else if(c == '%'){ reg = &vim_registers.file;         }
  else if(c == '0'){ reg = &vim_registers.yank;         }
  else if(c == '*'){ reg = &vim_registers.system;       }
  else if(c == '+'){ reg = &vim_registers.system;       }
  else if(in_range('1', c, '9'+1)){ reg = &vim_registers.cycle[c-'1']; }
  else if(in_range('a', c, 'z'+1)){ reg = &vim_registers.named[c-'a']; }
  else if(in_range('A', c, 'Z'+1)){ reg = &vim_registers.named[c-'A']; append = REGISTER_Append; }
  else{ vim_state.chord_state = VIM_CHORD_Error; return; }

  vim_state.params.selected_reg = reg;
  if(reg){
    reg->flags &= (~REGISTER_Append);
    reg->flags |= append;
  }

  if(vim_state.mode == VIM_Insert){
    if(reg){
      View_ID view = get_active_view(app, Access_ReadVisible);
      Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
      reg->flags &= (~REGISTER_Append);
      vim_paste(app, view, buffer, reg);
    }
  }else{
    vim_state.chord_state = VIM_CHORD_Pending;
  }
}

function b32 vim_insert_record_accept(Application_Links *app, Record_Info record, i64 *pos){
  if(record.error != RecordError_NoError){ return false; }
  i64 pos_after = record.pos_before_edit - record.single_string_backward.size + record.single_string_forward.size;
  if(*pos != pos_after){ return false; }
  *pos = record.pos_before_edit;
  return true;
}

function void
vim_set_insert_register(Application_Links *app){
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  i64 max = view_get_cursor_pos(app, view);
  i64 min = max;

  History_Record_Index i = buffer_history_get_current_state_index(app, buffer);
  for(; i != vim_state.insert_index; i -= 1){
    Record_Info record = buffer_history_get_record_info(app, buffer, i);
    if(record.kind == RecordKind_Single){
      if(!vim_insert_record_accept(app, record, &min)){ goto done; }
    }
    else if(record.kind == RecordKind_Group){
      foreach(j, record.group_count){
        Record_Info r = buffer_history_get_group_sub_record(app, buffer, i, record.group_count-1-i32(j));
        if(!vim_insert_record_accept(app, r, &min)){ goto done; }
      }
    }
  }

  done:;
  vim_registers.insert.data.size = 0;
  if (min < max){
    Scratch_Block scratch(app);
    String_Const_u8 str = push_buffer_range(app, scratch, buffer, Ii64(min, max));
    vim_register_copy(&vim_registers.insert, str);
  }

  vim_state.prev_params.do_insert = true;
  vim_registers.insert.flags &= (~REGISTER_Append);
  vim_registers.insert.flags |= (REGISTER_Set|REGISTER_Updated);
  vim_update_registers(app);

  vim_history_group.app = app;
  history_group_end(vim_history_group);
}