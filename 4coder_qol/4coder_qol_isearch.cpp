
// TODO(BYP): regex

function void
qol_isearch(Application_Links *app, Scan_Direction scan, i64 first_pos, String_Const_u8 query_prompt[2], String_Const_u8 query_init, b32 use_bot_string){
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  if (!buffer_exists(app, buffer)){ return; }

  Query_Bar_Group group(app);
  Query_Bar bar = {};
  if (!use_bot_string && start_query_bar(app, &bar, 0) == 0){ return; }

  Vec2_f32 old_margin = {};
  Vec2_f32 old_push_in = {};
  view_get_camera_bounds(app, view, &old_margin, &old_push_in);

  Vec2_f32 margin = V2f32(old_margin.x, clamp_bot(200.f, old_margin.y));
  view_set_camera_bounds(app, view, margin, old_push_in);

  u8 bar_string_space[256];
  bar.string = SCu8(bar_string_space, query_init.size);
  block_copy(bar.string.str, query_init.str, query_init.size);
  u64 match_size = bar.string.size;
  i64 pos = first_pos;

  Range_i64 range = buffer_range(app, buffer);
  View_Context ctx = view_current_context(app, view);
  Command_Map *map = mapping_get_map(ctx.mapping, default_get_map_id(app, view));

  User_Input in = {};
  for (;;){
    bar.prompt = query_prompt[scan != Scan_Forward];
    if (use_bot_string){
      qol_bot_text_set(bar.prompt);
      qol_bot_text_append(bar.string);
    }
    vim_register_copy(&vim_registers.search, bar.string);
    isearch__update_highlight(app, view, Ii64_size(pos, match_size));

    in = get_next_input(app, EventPropertyGroup_Any, EventProperty_Escape);
    if (in.abort){
      break;
    }

    String_Const_u8 string = to_writable(&in);

    b32 string_change = false;
    if (match_key_code(&in, KeyCode_Return) ||
        match_key_code(&in, KeyCode_Tab)){
      if (has_modifier(&in.event.key.modifiers, KeyCode_Control)){
        bar.string.size = cstring_length(previous_isearch_query);
        block_copy(bar.string.str, previous_isearch_query, bar.string.size);
      }
      else{
        u64 size = bar.string.size;
        size = clamp_top(size, sizeof(previous_isearch_query) - 1);
        block_copy(previous_isearch_query, bar.string.str, size);
        previous_isearch_query[size] = 0;
        break;
      }
    }
    else if (string.str != 0 && string.size > 0){
      String_u8 bar_string = Su8(bar.string, sizeof(bar_string_space));
      string_append(&bar_string, string);
      bar.string = bar_string.string;
      string_change = true;
    }
    else if (match_key_code(&in, KeyCode_Backspace)){
      String_Const_u8 old_string = bar.string;
      b32 mod_ctl = has_modifier(&in.event.key.modifiers, KeyCode_Control);
      b32 mod_sft = has_modifier(&in.event.key.modifiers, KeyCode_Shift);
      bar.string = (mod_ctl && !mod_sft ? qol_ctrl_backspace_string(app, bar.string) :
                    mod_ctl &&  mod_sft ? string_prefix(bar.string, 0) :
                    backspace_utf8(bar.string));
      string_change = (bar.string.size < old_string.size);
    }

    b32 do_scan_action = false;
    b32 do_scroll_wheel = false;
    Scan_Direction change_scan = scan;
    if (!string_change){
      if (match_key_code(&in, KeyCode_Down) || match_key_code(&in, KeyCode_PageDown)){
        change_scan = Scan_Forward;
        do_scan_action = true;
      }
      else if (match_key_code(&in, KeyCode_Up) || match_key_code(&in, KeyCode_PageUp)){
        change_scan = Scan_Backward;
        do_scan_action = true;
      }
      else{
        Custom_Command_Function *f = map_get_binding_recursive(ctx.mapping, map, &in.event);
        if (f == qol_search || f == qol_reverse_search){
          change_scan = (f == qol_search ? Scan_Forward : Scan_Backward);
          do_scan_action = true;
        }
        else if (f == qol_write_space || MCi_kind(f) == MC_Command_CursorPaste){
          String_u8 bar_string = Su8(bar.string, sizeof(bar_string_space));
          String_Const_u8 append = (f==qol_write_space ? string_u8_litexpr(" ") : get_clipboard_index(&clipboard0, 0));
          string_append(&bar_string, append);
          bar.string = bar_string.string;
          string_change = true;
        }
        else if (f != 0){
          Command_Metadata *metadata = get_command_metadata(f);
          if (metadata != 0 && metadata->is_ui){
            view_enqueue_command_function(app, view, f);
            break;
          }
          if (MCi_kind(f) == MC_Command_Global){
            f(app);
          }
        }
        else {
          leave_current_input_unhandled(app);
        }
      }
    }

    if (string_change || do_scan_action){
      scan = change_scan;
      i64 new_pos = 0;
      (scan == Scan_Forward ?
       seek_string_insensitive_forward(app, buffer, pos - string_change, 0, bar.string, &new_pos) :
       seek_string_insensitive_backward(app, buffer, pos + string_change, 0, bar.string, &new_pos));
      if (range_contains(range, new_pos)){
        pos = new_pos;
        match_size = bar.string.size;
      }
    }
    else if (do_scroll_wheel){
      mouse_wheel_scroll(app);
    }
  }

  view_disable_highlight_range(app, view);

  if (in.abort){
    u64 size = bar.string.size;
    size = clamp_top(size, sizeof(previous_isearch_query) - 1);
    block_copy(previous_isearch_query, bar.string.str, size);
    previous_isearch_query[size] = 0;
    view_set_cursor_and_preferred_x(app, view, seek_pos(first_pos));
  }

  view_set_camera_bounds(app, view, old_margin, old_push_in);
}

function void
qol_isearch(Application_Links *app, Scan_Direction start_scan){
  View_ID view = get_active_view(app, Access_ReadVisible);
  i64 pos = view_get_cursor_pos(app, view);
  String_Const_u8 prompts[] = { string_u8_litexpr("I-Search: "), string_u8_litexpr("Reverse-I-Search: ") };
  qol_isearch(app, start_scan, pos, prompts, string_u8_empty, false);
}

CUSTOM_COMMAND_SIG(qol_search_identifier)
CUSTOM_DOC("[QOL] I-search identifier under cursor")
{
  Scratch_Block scratch(app);
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  Token_Array array = get_token_array_from_buffer(app, buffer);
  i64 cursor = view_get_cursor_pos(app, view);
  Token *token = get_token_from_pos(app, &array, cursor);
  String_Const_u8 query = push_token_lexeme(app, scratch, buffer, token);
  String_Const_u8 prompts[] = { string_u8_litexpr("I-Search: "), string_u8_litexpr("Reverse-I-Search: ") };
  qol_isearch(app, Scan_Forward, token->pos, prompts, query, false);
}

CUSTOM_COMMAND_SIG(qol_search)
CUSTOM_DOC("[QOL] I-search down")
{
  qol_isearch(app, Scan_Forward);
}

CUSTOM_COMMAND_SIG(qol_reverse_search)
CUSTOM_DOC("[QOL] I-search up")
{
  qol_isearch(app, Scan_Backward);
}