/*
4coder_tutorial.cpp - Guided graphical tutorial system.
*/

// TOP

global Tutorial_State tutorial = {};
global f32 tutorial_padding = 44.f;

CUSTOM_COMMAND_MC_GLOBAL_SIG(kill_tutorial)
CUSTOM_DOC("If there is an active tutorial, kill it.")
{
  if (!tutorial.in_tutorial){
    return;
  }

  tutorial.in_tutorial = false;
  view_close(app, tutorial.view);
}

CUSTOM_COMMAND_MC_GLOBAL_SIG(tutorial_maximize)
CUSTOM_DOC("Expand the tutorial window")
{
  if (!tutorial.in_tutorial){
    return;
  }

  Panel_ID panel = view_get_panel(app, tutorial.view);
  Panel_ID parent = panel_get_parent(app, panel);
  panel_set_split(app, parent, PanelSplitKind_Ratio_Min, 0.5f);

  tutorial.is_active = true;
}

CUSTOM_COMMAND_MC_GLOBAL_SIG(tutorial_minimize)
CUSTOM_DOC("Shrink the tutorial window")
{
  if (!tutorial.in_tutorial){
    return;
  }

  Face_ID face = get_face_id(app, 0);
  Face_Metrics metrics = get_face_metrics(app, face);
  f32 line_height = metrics.line_height;

  Panel_ID panel = view_get_panel(app, tutorial.view);
  Panel_ID parent = panel_get_parent(app, panel);
  panel_set_split(app, parent, PanelSplitKind_FixedPixels_Min, line_height*4.f);

  tutorial.is_active = false;
}

function void
tutorial_action(Application_Links *app, Tutorial_Action action){
  switch (action){
    case TutorialAction_Minimize: { tutorial_minimize(app);    }break;
    case TutorialAction_Maximize: { tutorial_maximize(app);    }break;
    case TutorialAction_Prev:     { tutorial.slide_index -= 1; }break;
    case TutorialAction_Next:     { tutorial.slide_index += 1; }break;
    case TutorialAction_Index:    { tutorial.slide_index = tutorial.hover_index; }break;
    case TutorialAction_Exit:     { kill_tutorial(app);        }break;
    case TutorialAction_Restart:  { tutorial.slide_index = 0;  }break;
  }
}

function void
tutorial_init_title_face(Application_Links *app){
  if (tutorial.face == 0){
    Face_ID face = get_face_id(app, 0);
    Face_Description face_description = get_face_description(app, face);
    face_description.parameters.pt_size *= 2;
    tutorial.face = try_create_new_face(app, &face_description);
    if (tutorial.face == 0){
      tutorial.face = face;
    }
  }
}

//-

function void
tutorial_render(Application_Links *app, Frame_Info frame_info, View_ID view_id){
  Face_ID face = get_face_id(app, 0);
  tutorial_init_title_face(app);
  Face_Metrics metrics = get_face_metrics(app, face);
  Mouse_State mouse = get_mouse_state(app);

  ////////

  Scratch_Block scratch(app);
  tutorial.slide_index = clamp(0, tutorial.slide_index, tutorial.slide_count - 1);
  Tutorial_Desc desc = tutorial.slides[tutorial.slide_index];
  Tutorial_Slide slide = desc.func(app, scratch);

  ////////

  f32 h0 = get_fancy_line_height(app, 0, &slide.short_details);
  f32 h1 = get_fancy_line_height(app, 0, slide.long_details.first);
  f32 title_height = Max(h0, h1);

  ////////

  View_ID active_view = get_active_view(app, Access_Always);
  b32 is_active_view = (active_view == view_id);

  FColor margin_color = get_panel_margin_color(is_active_view?UIHighlight_Active:UIHighlight_None);
  Rect_f32 region = draw_background_and_margin(app, view_id, margin_color, margin_color);
  Rect_f32 prev_clip = draw_set_clip(app, region);

  f32 panel_y0 = region.y0 - 3.f;

  region = rect_inner(region, 3.f);
  draw_rectangle_fcolor(app, region, 20.f, fcolor_id(defcolor_back));
  region = rect_inner(region, 10.f);

  f32 s_width = metrics.normal_advance*3.5f;
  Vec2_f32 m_p = V2f32(mouse.p);
  Vec2_f32 title_p = V2f32(region.x0, panel_y0 + (metrics.line_height*2.f) - title_height*0.5f);

  tutorial.hover_action = TutorialAction_None;
  if (tutorial.is_active){
    push_fancy_string(scratch, slide.long_details.first, tutorial.face, fcolor_id(defcolor_pop1), string_u8_litexpr(" - "));
    push_fancy_string(scratch, slide.long_details.first, tutorial.face, fcolor_id(defcolor_keyword), desc.title);
    draw_fancy_block(app, 0, fcolor_zero(), &slide.long_details, title_p);

    // NOTE(allen): buttons
    Rect_f32_Pair footer_pair = rect_split_top_bottom_neg(region, metrics.line_height*2.f);
    Rect_f32 footer = footer_pair.max;
    footer.x0 += 10.f;
    footer.y0 -= 5.f;
    footer.y1 -= 5.f;

    {
      Rect_f32_Pair pair = rect_split_left_right(footer, 1.2f*s_width);
      footer = pair.max;
      footer.x0 += 5.f;
      if (draw_button(app, pair.min, m_p, face, string_u8_litexpr("<")) && tutorial.slide_index > 0){
        tutorial.hover_action = TutorialAction_Prev;
      }
    }

    for (i32 i = 0; i < tutorial.slide_count; i += 1)
    {
      Rect_f32_Pair pair = rect_split_left_right(footer, s_width);
      footer = pair.max;
      footer.x0 += 5.f;
      if (draw_button(app, pair.min, m_p, face, push_stringf(scratch, "%d", i+1))){
        tutorial.hover_action = TutorialAction_Index;
        tutorial.hover_index  = i;

        Fancy_Block block = {};
        push_fancy_line(scratch, &block, tutorial.face, fcolor_id(defcolor_keyword), tutorial.slides[i].title);
        Rect_f32 tool_rect = draw_tool_tip(app, face, &block, V2f32(m_p.x, pair.min.y0 - 4.f), region, 8.f, 4.f, fcolor_id(defcolor_back));
        draw_rectangle_outline_fcolor(app, rect_inner(tool_rect, -2.f), 5.f, 4.f, fcolor_id(defcolor_list_item_active, 0));
      }
      if (tutorial.slide_index == i){
        draw_rectangle_outline_fcolor(app, pair.min, 3.f, 2.f, fcolor_id(defcolor_list_item_active, 0));
      }
    }

    {
      Rect_f32_Pair pair = rect_split_left_right(footer, 1.2f*s_width);
      footer = pair.max;
      footer.x0 += 10.f;
      if (draw_button(app, pair.min, m_p, face, string_u8_litexpr(">")) && tutorial.slide_index < tutorial.slide_count - 1){
        tutorial.hover_action = TutorialAction_Next;
      }
    }
  }
  else{
    draw_fancy_line(app, 0, fcolor_zero(), &slide.short_details, title_p);
  }

  // header
  {
    Rect_f32_Pair header_pair = rect_split_top_bottom(region, metrics.line_height*1.5f);
    Rect_f32 header = header_pair.min;
    header.x1 -= 10.f;

    {
      Rect_f32_Pair pair = rect_split_left_right_neg(header, s_width);
      header = pair.min;
      header.x1 -= 5.f;
      if (draw_button(app, pair.max, m_p, face, string_u8_litexpr("X"))){
        tutorial.hover_action = TutorialAction_Exit;
      }
    }

    {
      Rect_f32_Pair pair = rect_split_left_right_neg(header, s_width);
      header = pair.min;
      header.x1 -= 5.f;

      b32 in_button = draw_button(app, pair.max, m_p, face, string_u8_litexpr(" "));
      if (in_button){
        tutorial.hover_action = (tutorial.is_active ? TutorialAction_Minimize : TutorialAction_Maximize);
      }

      if (tutorial.is_active){
        Rect_f32 icon = rect_inner(pair.max, 12.f);
        icon.p0 += V2f32(-2.f, 2.f);
        icon.p1 += V2f32(-2.f, 2.f);
        draw_rectangle_outline_fcolor(app, icon, 2.f, 2.f, fcolor_id(defcolor_text_default));

        Rect_f32 mask = rect_inner(icon, 1.f);
        icon.p0 += V2f32(5.f, -5.f);
        icon.p1 += V2f32(5.f, -5.f);
        draw_rectangle_outline_fcolor(app, icon, 2.f, 2.f, fcolor_id(defcolor_text_default));
        draw_rectangle_fcolor(app, mask, 2.f, in_button ? fcolor_id(defcolor_list_item_active, 0) : fcolor_id(defcolor_back));
      }
      else{
        draw_rectangle_outline_fcolor(app, rect_inner(pair.max, 11.f), 2.f, 2.f, fcolor_id(defcolor_text_default));
      }
    }
  }

  draw_set_clip(app, prev_clip);
}

function void
tutorial_run_loop(Application_Links *app){
  View_ID view = get_this_ctx_view(app, Access_Always);
  View_Context ctx = view_current_context(app, view);
  ctx.render_caller = tutorial_render;
  ctx.hides_buffer = true;
  View_Context_Block ctx_block(app, view, &ctx);

  tutorial.in_tutorial = true;
  tutorial.view = view;
  tutorial_maximize(app);

  change_active_panel(app);

  for (;;){
    User_Input in = get_next_input(app, EventPropertyGroup_Any, 0);
    if (in.abort){
      break;
    }

    b32 handled = true;
    switch (in.event.kind){
      case InputEventKind_MouseButton:
      {
        tutorial_maximize(app);
        if (in.event.mouse.code == MouseCode_Left){
          tutorial_action(app, tutorial.hover_action);
        }
      }break;

      case InputEventKind_Core:
      {
        switch (in.event.core.code){
          case CoreCode_ClickActivateView:
          {
            tutorial_maximize(app);
            tutorial_action(app, tutorial.hover_action);
            change_active_panel(app);
          }break;

          default:
          {
            handled = false;
          }break;
        }
      }break;

      default:
      {
        handled = false;
      }break;
    }

    if (!handled){
      Mapping *mapping = ctx.mapping;
      Command_Map *map = mapping_get_map(mapping, ctx.map_id);

      Fallback_Dispatch_Result disp_result =
        fallback_command_dispatch(app, mapping, map, &in);
      if (disp_result.code == FallbackDispatch_DelayedUICall){
        call_after_ctx_shutdown(app, view, disp_result.func);
        break;
      }
      if (disp_result.code == FallbackDispatch_Unhandled){
        leave_current_input_unhandled(app);
      }
    }
  }

  tutorial.in_tutorial = false;
}

function void
run_tutorial(Application_Links *app, Tutorial_Desc *slides, i32 slide_count){
  if (slide_count > 0){
    kill_tutorial(app);
    Panel_ID root_panel = panel_get_root(app);
    if (panel_split(app, root_panel, Dimension_Y)){
      panel_swap_children(app, root_panel);
      Panel_ID tutorial_panel = panel_get_child(app, root_panel, Side_Min);
      tutorial.view = panel_get_view(app, tutorial_panel, Access_Always);
      view_set_passive(app, tutorial.view, true);
      tutorial.slide_index = 0;
      tutorial.slides = slides;
      tutorial.slide_count = slide_count;
      view_enqueue_command_function(app, tutorial.view, tutorial_run_loop);
    }
  }
}

////////////////////////////////

function void
tutorial_short_details(Application_Links *app, Arena *arena, Fancy_Line *short_details){
  Face_ID face = get_face_id(app, 0);
  push_fancy_string(arena, short_details, tutorial.face, fcolor_id(defcolor_pop1), string_u8_litexpr("4coder Tutorial"));
  push_fancy_string(arena, short_details, face, fcolor_id(defcolor_pop2), 8.f, 0.f, string_u8_litexpr("Click panel to see more."));
  push_fancy_string(arena, short_details, face, fcolor_id(defcolor_text_default), 8.f, 0.f, string_u8_litexpr("Run "));
  push_fancy_string(arena, short_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr("kill_tutorial"));
  push_fancy_string(arena, short_details, face, fcolor_id(defcolor_text_default), string_u8_litexpr(" to close."));
}

function void
tutorial_long_start(Application_Links *app, Arena *arena, Fancy_Block *long_details){
  Fancy_Line *line = push_fancy_line(arena, long_details, tutorial.face, fcolor_id(defcolor_pop1), string_u8_litexpr("4coder Tutorial"));

  Face_ID face = get_face_id(app, 0);
  line = push_fancy_line(arena, long_details, face, fcolor_id(defcolor_text_default));
  push_fancy_string(arena, line, string_u8_litexpr("If you want more information than what you can find here, please "));
  push_fancy_string(arena, line, fcolor_id(defcolor_pop2), string_u8_litexpr("ask!"));

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr(""));
}

function void
tutorial_push_description(Application_Links *app, Arena *arena, Fancy_Line *line, Face_ID face, char* description){
  f32 width = get_fancy_line_width(app, face, line);
  push_fancy_stringf(arena, line, "%s", description);
  Face_Metrics metrics = get_face_metrics(app, face);
  f32 advanced = width / metrics.normal_advance;
  line->first->pre_margin = Max(0, tutorial_padding - advanced);
}

function void
tutorial_binding_line(Application_Links *app, Arena *arena, Fancy_Block *long_details, Face_ID face, String_Const_u8 m, String_Const_u8 k, char *description){
  Fancy_Line *line = line = push_fancy_line(arena, long_details, face, fcolor_id(defcolor_text_default));
  push_fancy_stringf(arena, line, 0, 0.5f, "<");
  if (m.size > 0){
    push_fancy_stringf(arena, line, fcolor_id(defcolor_keyword), 0, 0.5f, "%S", m);
  }
  push_fancy_stringf(arena, line, fcolor_id(defcolor_pop2), "%S", k);
  push_fancy_stringf(arena, line, 0.5f, 1.f, ">");
  tutorial_push_description(app, arena, line, face, description);
}

function void
tutorial_binding_line(Application_Links *app, Arena *arena, Fancy_Block *long_details, Face_ID face, char *modifiers, char *key, char *description){
  tutorial_binding_line(app, arena, long_details, face, SCu8(modifiers), SCu8(key), description);
}

function Command_Trigger*
tutorial_binding(Custom_Command_Function *func){
  for (Command_Map *m = framework_mapping.first_map; m != 0; m = m->next){
    Command_Trigger_List *val = 0;
    if (table_read(&m->cmd_to_binding_trigger, PtrAsInt(func), (u64*)&val)){
      return val->first;
    }
  }
  return 0;
}

function u8
mod_bitfield(Input_Modifier_Set mods){
  u8 m = 0;
  for (i64 i = 0; i < mods.count; i += 1){
    m |= mod_bit_from_key(mods.mods[i]);
  }
  return m;
}

function void
tutorial_push_mods(Arena *arena, Fancy_Line *line, u8 mods){
  for (u32 b=1; b <= mods; b <<= 1){
    if (!HasFlag(b, mods)){ continue; }
    String_Const_u8 string = SCu8(key_mod_name(mod_key_from_bit(b)));
    push_fancy_string(arena, line, fcolor_id(defcolor_keyword), 0, 0.5f, string);
  }
}

function void
tutorial_push_trigger(Arena *arena, Fancy_Line *line, Command_Trigger *trigger){
  Command_Trigger temp = *trigger;
  temp.mods = {};
  List_String_Const_u8 list = {};
  command_trigger_stringize(arena, &list, &temp);
  push_fancy_string(arena, line, fcolor_id(defcolor_pop2), list.first->next->string);
}

function void
tutorial_push_meta(Arena *arena, Fancy_Line *line, Custom_Command_Function *f){
  Command_Metadata *meta = get_command_metadata(f);
  String_Const_u8 string = (meta ? SCu8(meta->name, meta->name_len) : string_u8_litexpr("null"));
  push_fancy_string(arena, line, fcolor_id(defcolor_pop2), string);
}

function void
tutorial_binding_line(Application_Links *app, Arena *arena, Fancy_Block *long_details, Face_ID face, Custom_Command_Function *a, Custom_Command_Function *b, char *description){
  Fancy_Line *line = push_fancy_line(arena, long_details, face, fcolor_id(defcolor_text_default));
  Command_Trigger *ta = tutorial_binding(a);
  Command_Trigger *tb = tutorial_binding(b);

  push_fancy_stringf(arena, line, 0, 0.5f, "<");

  if (ta == 0 || tb == 0){
    tutorial_push_meta(arena, line, a);
    push_fancy_stringf(arena, line, "/");
    tutorial_push_meta(arena, line, b);
  }
  else{
    u8 mod_a = mod_bitfield(ta->mods);
    u8 mod_b = mod_bitfield(tb->mods);
    u8 diff = mod_a ^ mod_b;
    u8 mod_lo = mod_a & (diff - 1);
    u8 mod_hi = mod_a & ~(mod_lo|diff);

    u64 diff_count = (0x8DA691691448ULL >> (diff * 3)) & 0x7;  // 4-bit popcnt lut
    diff_count += (ta->kind != tb->kind || ta->sub_code != tb->sub_code);
    if (diff_count > 1){
      tutorial_push_mods(arena, line, mod_a);
      tutorial_push_trigger(arena, line, ta);
      push_fancy_stringf(arena, line, 0.5f, 0.5f, "> / <");
      tutorial_push_mods(arena, line, mod_b);
      tutorial_push_trigger(arena, line, tb);
    }
    else {
      tutorial_push_mods(arena, line, mod_lo);
      if (diff != 0){
        push_fancy_stringf(arena, line,  0.0f, 0.0f, "[");
        tutorial_push_mods(arena, line, diff);
        push_fancy_stringf(arena, line, -0.5f, 0.5f, "]");
      }
      tutorial_push_mods(arena, line, mod_hi);

      tutorial_push_trigger(arena, line, ta);
      if (diff == 0){
        push_fancy_stringf(arena, line, "/");
        tutorial_push_trigger(arena, line, tb);
      }
    }
  }

  push_fancy_stringf(arena, line, 0.5f, 1.f, ">");
  tutorial_push_description(app, arena, line, face, description);
}

function void
tutorial_binding_line(Application_Links *app, Arena *arena, Fancy_Block *long_details, Face_ID face, Custom_Command_Function *func, char *description){
  Fancy_Line *line = push_fancy_line(arena, long_details, face, fcolor_id(defcolor_text_default));
  Command_Trigger *t = tutorial_binding(func);

  push_fancy_stringf(arena, line, 0, 0.5f, "<");

  if (t == 0){
    tutorial_push_meta(arena, line, func);
  }
  else{
    tutorial_push_mods(arena, line, mod_bitfield(t->mods));
    tutorial_push_trigger(arena, line, t);
  }

  push_fancy_stringf(arena, line, 0.5f, 1.f, ">");
  tutorial_push_description(app, arena, line, face, description);
}

//-

function Tutorial_Slide
tutorial_slide_basic(Application_Links *app, Arena *arena){
  Tutorial_Slide result = {};

  Face_ID face = get_face_id(app, 0);
  tutorial_init_title_face(app);

  tutorial_short_details(app, arena, &result.short_details);
  Fancy_Block *long_details = &result.long_details;
  tutorial_long_start(app, arena, long_details);

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr("Let's start with a few basic commands:"));

  tutorial_binding_line(app, arena, long_details, face,
                        write_text_input, "Insert text at cursor");

  tutorial_binding_line(app, arena, long_details, face,
                        "", "ArrowKey", "Move cursor one character or line");

  tutorial_binding_line(app, arena, long_details, face,
                        backspace_char, delete_char, "Delete previous/next character");

  tutorial_binding_line(app, arena, long_details, face,
                        move_left_whitespace_boundary, move_right_whitespace_boundary, "Move left/right to the next *word*");

  tutorial_binding_line(app, arena, long_details, face,
                        seek_beginning_of_line, seek_end_of_line, "Move to the start/end of the line");

  tutorial_binding_line(app, arena, long_details, face,
                        page_up, page_down, "Move up/down by one page");

  tutorial_binding_line(app, arena, long_details, face,
                        goto_beginning_of_file, goto_end_of_file, "Move up/down by one page");

  tutorial_binding_line(app, arena, long_details, face,
                        center_view, "Center scroll on cursor");

  tutorial_binding_line(app, arena, long_details, face,
                        build_in_build_panel, "Build in *compilation* (Windows -> `build.bat`; Unix -> `build.sh`");

  tutorial_binding_line(app, arena, long_details, face,
                        goto_next_jump, goto_prev_jump, "Goto next/prev jump (search results or build errors)");

  tutorial_binding_line(app, arena, long_details, face,
                        goto_line, "Query line number to jump to");

  tutorial_binding_line(app, arena, long_details, face,
                        save, save_all_dirty_buffers, "Save current/all files");

  tutorial_binding_line(app, arena, long_details, face,
                        reopen, "Reopen current file (discard unsaved edits and/or load external changes)");

  return(result);
}

function Tutorial_Slide
tutorial_slide_edit(Application_Links *app, Arena *arena){
  Tutorial_Slide result = {};

  Face_ID face = get_face_id(app, 0);
  tutorial_init_title_face(app);

  tutorial_short_details(app, arena, &result.short_details);

  Fancy_Block *long_details = &result.long_details;
  tutorial_long_start(app, arena, long_details);

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr("Now a look at basic editing:"));

  tutorial_binding_line(app, arena, long_details, face,
                        word_complete, word_complete_prev, "Suggest next/prev auto-complete entry for current identifier");

  tutorial_binding_line(app, arena, long_details, face,
                        undo, redo, "Undo/Redo");

  tutorial_binding_line(app, arena, long_details, face,
                        set_mark, "Place `mark` at cursor position. Text between cursor/mark is the `range`");

  tutorial_binding_line(app, arena, long_details, face,
                        cursor_mark_swap, "Swap cursor and mark positions");

  tutorial_binding_line(app, arena, long_details, face,
                        delete_range, "Delete text inside `range`");

  tutorial_binding_line(app, arena, long_details, face,
                        copy, "Copy text inside `range`");

  tutorial_binding_line(app, arena, long_details, face,
                        cut, "Cut text inside `range`");

  tutorial_binding_line(app, arena, long_details, face,
                        paste_and_indent, "Paste copied text");

  tutorial_binding_line(app, arena, long_details, face,
                        duplicate_line, "Duplicate the current line");

  tutorial_binding_line(app, arena, long_details, face,
                        delete_line, "Delete the current line");

  tutorial_binding_line(app, arena, long_details, face,
                        move_line_up, move_line_down, "Move the current line up/down");

  tutorial_binding_line(app, arena, long_details, face,
                        keyboard_macro_start_recording, keyboard_macro_finish_recording, "Start/Stop recording macro");

  tutorial_binding_line(app, arena, long_details, face,
                        keyboard_macro_replay, "Replay macro");

  return(result);
}

function Tutorial_Slide
tutorial_slide_panel(Application_Links *app, Arena *arena){
  Tutorial_Slide result = {};

  Face_ID face = get_face_id(app, 0);
  tutorial_init_title_face(app);

  tutorial_short_details(app, arena, &result.short_details);

  Fancy_Block *long_details = &result.long_details;
  tutorial_long_start(app, arena, long_details);

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr("Now to take a look at panels:"));

  tutorial_binding_line(app, arena, long_details, face,
                        change_active_panel, "Change active panel");

  tutorial_binding_line(app, arena, long_details, face,
                        vsplit, hsplit, "Splits the current panel vertically/horizontally");

  tutorial_binding_line(app, arena, long_details, face,
                        close_panel, "Closes the current panel");

  tutorial_binding_line(app, arena, long_details, face,
                        change_active_panel, change_active_panel_backwards, "Focus next/prev panel");

  tutorial_binding_line(app, arena, long_details, face,
                        change_to_build_panel, "Focus *compilation* panel");

  tutorial_binding_line(app, arena, long_details, face,
                        close_build_panel, "Close *compilation* panel");

  tutorial_binding_line(app, arena, long_details, face,
                        view_buffer_other_panel, "Open current buffer in neighboring panel");

  tutorial_binding_line(app, arena, long_details, face,
                        swap_panels, "Swap neighboring panels");

  push_fancy_line(arena, long_details, face, string_u8_empty);
  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_text_default), string_u8_litexpr("Panels can be resized by clicking and dragging the dividers"));

  {
    Fancy_Line *line = push_fancy_line(arena, long_details, face, fcolor_id(defcolor_text_default), string_u8_litexpr("There's an upper limit of "));
    push_fancy_string(arena, line, face, fcolor_id(defcolor_int_constant), string_u8_litexpr("16"));
    push_fancy_string(arena, line, face, fcolor_id(defcolor_text_default), string_u8_litexpr(" for panels. Subsequent splits will no-op"));
  }

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_text_default), string_u8_litexpr("Custom layers may use panels for their own elements. For example, this tutorial"));

  return(result);
}

function Tutorial_Slide
tutorial_slide_lists(Application_Links *app, Arena *arena){
  Tutorial_Slide result = {};

  Face_ID face = get_face_id(app, 0);
  tutorial_init_title_face(app);

  tutorial_short_details(app, arena, &result.short_details);

  Fancy_Block *long_details = &result.long_details;
  tutorial_long_start(app, arena, long_details);

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr("Here are the different lists available"));

  tutorial_binding_line(app, arena, long_details, face,
                        command_lister, "List available editor commands to run");

  tutorial_binding_line(app, arena, long_details, face,
                        project_command_lister, "List available project commands to run");

  tutorial_binding_line(app, arena, long_details, face,
                        interactive_open_or_new, "List current files/directories to switch/navigate to, creating new as needed");

  tutorial_binding_line(app, arena, long_details, face,
                        interactive_switch_buffer, "List loaded files/directories to switch to");

  tutorial_binding_line(app, arena, long_details, face,
                        theme_lister, "List available themes to switch to");

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr("Once you're in a lister:"));

  tutorial_padding = 25.f;
  tutorial_binding_line(app, arena, long_details, face,
                        "", "*TextInsert*", "Filters out less specific entries");

  tutorial_binding_line(app, arena, long_details, face,
                        "", "*ArrowKey*", "Move active lister entry");

  //tutorial_binding_line(app, arena, long_details, face,
  //                      page_up, page_down, "Move active lister up/down by one page");

  tutorial_binding_line(app, arena, long_details, face,
                        "", "MouseWheel", "Scroll lister");

  tutorial_binding_line(app, arena, long_details, face,
                        "", "LeftClick", "Select hovered lister entry");

  tutorial_binding_line(app, arena, long_details, face,
                        "", "Return", "Select active lister entry");

  tutorial_binding_line(app, arena, long_details, face,
                        "", "Escape", "Cancel lister");
  tutorial_padding = 44.f;

  return(result);
}

function Tutorial_Slide
tutorial_slide_jumps(Application_Links *app, Arena *arena){
  Tutorial_Slide result = {};

  Face_ID face = get_face_id(app, 0);
  tutorial_init_title_face(app);

  tutorial_short_details(app, arena, &result.short_details);

  Fancy_Block *long_details = &result.long_details;
  tutorial_long_start(app, arena, long_details);

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr("Common navigation commands:"));

  tutorial_binding_line(app, arena, long_details, face,
                        search, "query for string in current buffer, Up/Down Arrow to go next/prev occurrence");

  tutorial_binding_line(app, arena, long_details, face,
                        jump_to_definition_at_cursor, "jump to definition at cursor");

  tutorial_binding_line(app, arena, long_details, face,
                        jump_to_last_point, "jump back");

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr("Fast navigation by jump lists:"));

  tutorial_binding_line(app, arena, long_details, face,
                        list_all_substring_locations_case_insensitive, "query for string to generate a jump list of matches");

  tutorial_binding_line(app, arena, long_details, face,
                        list_all_locations_of_identifier, "using the identifier under the cursor, generate a jump list of matches");

  tutorial_binding_line(app, arena, long_details, face,
                        list_all_functions_current_buffer, "generate a jump list of all functions in current buffer");

  tutorial_binding_line(app, arena, long_details, face,
                        list_all_functions_all_buffers, "generate a jump list of all functions in all loaded buffers");

  tutorial_binding_line(app, arena, long_details, face,
                        list_all_locations_of_selection, "using current selection, generate a jump list");

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr("Build commands which output to *compilation* create jump list for error messages"));
  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr("After generating a jump list, the following commands are enabled:"));

  tutorial_binding_line(app, arena, long_details, face,
                        goto_next_jump, goto_prev_jump, "goto next/prev jump (search results or build errors)");

  tutorial_binding_line(app, arena, long_details, face,
                        goto_first_jump, "goto first jump in jump list");

  return(result);
}

function Tutorial_Slide
tutorial_slide_vws(Application_Links *app, Arena *arena){
  Tutorial_Slide result = {};

  Face_ID face = get_face_id(app, 0);
  tutorial_init_title_face(app);

  tutorial_short_details(app, arena, &result.short_details);

  Fancy_Block *long_details = &result.long_details;
  tutorial_long_start(app, arena, long_details);

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr("Virtual whitespace:"));

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_text_default),
                  string_u8_litexpr("\tBuffers that are indexed with information about nest structures can be equiped with the virtual whitespace layout algorithm."));

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_text_default),
                  string_u8_litexpr("\tThe on screen layout of text is independent of the actual whitespace contents of the underlying text."));

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_text_default), string_u8_litexpr(""));

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_keyword),
                  string_u8_litexpr("\tTry inserting new scopes and parenthetical sections in a code file."));

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_text_default),
                  string_u8_litexpr("\t\tObserve that indentation is updated automatically."));

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_keyword),
                  string_u8_litexpr("\tTry creating a line that is long enough to wrap around the edge."));

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_text_default),
                  string_u8_litexpr("\t\tObserve that wrapped lines obey the same indentation rules as literal lines."));

  {
    Fancy_Line *line = push_fancy_line(arena, long_details, face, fcolor_id(defcolor_text_default));
    push_fancy_stringf(arena, line, "\tIn `config.4coder` set `");
    push_fancy_stringf(arena, line, fcolor_id(defcolor_pop2), "enable_virtual_whitespace");
    push_fancy_stringf(arena, line, " = " );
    push_fancy_stringf(arena, line, fcolor_id(defcolor_int_constant), "true");
    push_fancy_stringf(arena, line, ";" );
    push_fancy_stringf(arena, line, "` turn this feature on by default");
  }

  {
    Fancy_Line *line = push_fancy_line(arena, long_details, face, fcolor_id(defcolor_text_default));
    push_fancy_stringf(arena, line, "\tUse the command ");
    push_fancy_stringf(arena, line, fcolor_id(defcolor_pop2), "toggle_virtual_whitespace");
    push_fancy_stringf(arena, line, " to turn this feature on and off");
  }

  {
    Fancy_Line *line = push_fancy_line(arena, long_details, face, fcolor_id(defcolor_text_default));
    push_fancy_stringf(arena, line, "\tUse the command ");
    push_fancy_stringf(arena, line, fcolor_id(defcolor_pop2), "toggle_line_wrap");
    push_fancy_stringf(arena, line, " to see how layout changes with line wrapping on and off");
  }

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr("Auto Indentation:"));

  tutorial_padding = 20.f;
  tutorial_binding_line(app, arena, long_details, face,
                        auto_indent_range, "Auto indent the lines marked by the range; the effect is only visible with virtual whitespace off.");
  tutorial_padding = 44.f;

  return(result);
}

function Tutorial_Slide
tutorial_slide_sys(Application_Links *app, Arena *arena){
  Tutorial_Slide result = {};

  Face_ID face = get_face_id(app, 0);
  tutorial_init_title_face(app);

  tutorial_short_details(app, arena, &result.short_details);

  Fancy_Block *long_details = &result.long_details;
  tutorial_long_start(app, arena, long_details);

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr("Builds, scripts, and projects:"));

  tutorial_binding_line(app, arena, long_details, face,
                        build_in_build_panel, "Searches for and runs a build script (Windows -> `build.bat`; Unix -> `build.sh`)");

  tutorial_binding_line(app, arena, long_details, face,
                        execute_any_cli, "Query for a buffer name and a cli command, then run it, piping output to the buffer");

  tutorial_binding_line(app, arena, long_details, face,
                        execute_previous_cli, "Repeats last `execute_any_cli` command with the same command and ouptut buffer");

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr("If a script/command generates output that can be parsed as jumps (e.g. compilation errors) then it becomes the active jump buffer:"));

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr("The project system enables rich bindings of arbitrary system scripts (when a project is loaded):"));

  tutorial_binding_line(app, arena, long_details, face,
                        "", "F#", "run a script bound to the corresponding index in the loaded project");

  tutorial_binding_line(app, arena, long_details, face,
                        project_command_lister, "List available project commands to run");

  tutorial_binding_line(app, arena, long_details, face,
                        load_project, "Searches for and loads a 'project.4coder' file");

  tutorial_binding_line(app, arena, long_details, face,
                        setup_new_project, "Generate a new 'project.4coder' file and build scripts");

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_text_default),
                  string_u8_litexpr("\tCheckout 'project.4coder' to see more about what is in a project."));

  return(result);
}

function Tutorial_Slide
tutorial_slide_end(Application_Links *app, Arena *arena){
  Tutorial_Slide result = {};

  Face_ID face = get_face_id(app, 0);
  tutorial_init_title_face(app);

  tutorial_short_details(app, arena, &result.short_details);

  Fancy_Block *long_details = &result.long_details;
  tutorial_long_start(app, arena, long_details);

  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr(""));
  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr("\t\tCongrats, you've made it to the end ^.^"));
  push_fancy_line(arena, long_details, face, fcolor_id(defcolor_pop1), string_u8_litexpr("\t\tThanks for checking out 4coder. Best of luck!"));

  return(result);
}

CUSTOM_COMMAND_MC_GLOBAL_SIG(begin_tutorial)
CUSTOM_DOC("Tutorial for built in 4coder bindings and features.")
{
  local_persist Tutorial_Desc slides[] = {
    { tutorial_slide_basic, string_u8_litexpr("Basics") },
    { tutorial_slide_edit,  string_u8_litexpr("Editing") },
    { tutorial_slide_panel, string_u8_litexpr("Panels") },
    { tutorial_slide_lists, string_u8_litexpr("Lists") },
    { tutorial_slide_jumps, string_u8_litexpr("Jumps") },
    { tutorial_slide_vws,   string_u8_litexpr("Virtual Whitespace") },
    { tutorial_slide_sys,   string_u8_litexpr("System Commands") },
    { tutorial_slide_end,   string_u8_litexpr("End") },
  };
  run_tutorial(app, slides, ArrayCount(slides));
}

// BOTTOM
