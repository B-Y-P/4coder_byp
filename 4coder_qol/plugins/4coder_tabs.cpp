// 4coder_tabs.cpp
// BYP

#if !defined(TAB_MAX_COUNT)
#define TAB_MAX_COUNT 8
#endif

//- Declarations
// inner Hook functions, call from your own custom hooks
function void TAB_tick_inner(Application_Links *app, Frame_Info frame_info);

// Hook functions, only pass to set_custom_hook(...) if using default hooks
function void TAB_tick(Application_Links *app, Frame_Info frame_info);

// inner CoreCode commands, call from your own custom commands
function void TAB_startup_inner(Application_Links *app);
function void TAB_try_exit_inner(Application_Links *app);

// CoreCode commands, only pass to BindCore(...) if using default commands
function void TAB_startup(Application_Links *app);
function void TAB_try_exit(Application_Links *app);

/// Used for top tab bar view context
function void TAB_render_bar(Application_Links *app, Frame_Info frame_info, View_ID view);
function void TAB_view_spin(Application_Links *app);

enum TAB_Kind{
  TAB_Kind_VSplit,
  TAB_Kind_HSplit,
  TAB_Kind_LeafInactive,
  TAB_Kind_LeafActive,
  TAB_Kind_COUNT
};

enum TAB_Split{
  TAB_Split_Current,
  TAB_Split_RootKeep,
  TAB_Split_RootRight,
  TAB_Split_COUNT,
};

struct TAB_Node{
  TAB_Kind kind;
  TAB_Split split;       //- split params
  f32 ratio;
  Buffer_ID buffer;      //- leaf params
  Buffer_Point scroll;
  i64 cursor_pos;
  i64 mark_pos;
  QOL_View_Jumps jumps;  // NOTE: specific to QOL... *gasp*
};

struct TAB_Data{
  TAB_Node nodes[29];    // 4ed(leaves[16] + splits[15]) - TAB(leaf[1] + split[1])
  i64 count;
  f32 cur_x;
};

struct TAB_Context{
  View_ID view;
  TAB_Data tabs[TAB_MAX_COUNT];
  i32 count;
  i32 current;
};

global TAB_Context g_tab_ctx;

// API
function void TAB_save(Application_Links *app);          // Saves current panel layout to context
function void TAB_apply(Application_Links *app, i32 i);  // Applies tab[i] to panel layout
function void TAB_swap(Application_Links *app, i32 i);   // What to call tab[i] is clicked
function void TAB_close_inner(Application_Links *app, i32 i);

// Only saves/loads when project.4coder or config.4coder has `session_file = "<file_name>"`
function void TAB_file_save(Application_Links *app);
function void TAB_file_load(Application_Links *app);

function Rect_f32 panel_get_rect(Application_Links *app, Panel_ID panel){
  if (panel_is_leaf(app, panel)){
    return view_get_screen_rect(app, panel_get_view(app, panel, Access_Always));
  } else {
    Rect_f32 rect_min = panel_get_rect(app, panel_get_child(app, panel, Side_Min));
    Rect_f32 rect_max = panel_get_rect(app, panel_get_child(app, panel, Side_Max));
    return rect_union(rect_min, rect_max);
  }
}

/// We can have a little hex float... as a treat
function u32 hex_from_float(f32 f){ u32 h=0x0; block_copy(&h, &f, sizeof(u32)); return h; }
function f32 float_from_hex(u32 h){ f32 f=0.f; block_copy(&f, &h, sizeof(f32)); return f; }

//- Implementation
function void TAB_startup_inner(Application_Links *app){
  User_Input input = get_current_input(app);
  if (match_core_code(&input, CoreCode_Startup)){
    Panel_ID root = panel_get_root(app);
    panel_split(app, root, Dimension_Y);
    panel_swap_children(app, root);
    panel_set_split(app, root, PanelSplitKind_FixedPixels_Min, 30.f);
    Panel_ID panel = panel_get_child(app, root, Side_Min);
    View_ID view = panel_get_view(app, panel, Access_Always);

    View_Context ctx = view_current_context(app, view);
    ctx.render_caller = TAB_render_bar;
    ctx.hides_buffer = true;

    view_set_passive(app, view, true);
    view_alter_context(app, view, &ctx);
    view_enqueue_command_function(app, view, TAB_view_spin);
  }
}

function void TAB_try_exit_inner(Application_Links *app){
  User_Input input = get_current_input(app);
  if (match_core_code(&input, CoreCode_TryExit)){
    TAB_file_save(app);
  }
}

function void TAB_render_bar(Application_Links *app, Frame_Info frame_info, View_ID view){
  Scratch_Block scratch(app);
  Face_ID face = get_face_id(app, 0);
  Rect_f32 region = view_get_screen_rect(app, view);
  Rect_f32 prev_clip = draw_set_clip(app, region);

  TAB_Data *tabs = g_tab_ctx.tabs;
  local_persist f32 hot_x = 0.f;
  local_persist i32 hot_i = -1;
  ARGB_Color cl_curr = fcolor_resolve(fcolor_id(defcolor_list_item_active, 0));
  ARGB_Color cl_rect = fcolor_resolve(fcolor_id(defcolor_list_item, 0));
  draw_rectangle(app, region, 0.f, fcolor_resolve(fcolor_id(defcolor_back)));

  Face_Metrics metrics = get_face_metrics(app, face);
  f32 x0 = region.x0 + metrics.space_advance;
  f32 y0 = region.y1 - metrics.line_height;
  f32 wid_max = ((region.x1 - x0) / g_tab_ctx.count) - 10.f;
  Vec2_f32 tab_dim = V2f32(Min(32.f*metrics.normal_advance, wid_max), 30.f);
  i32 max_len = (i32)(tab_dim.x / metrics.normal_advance)-1;

  View_ID active_view = get_active_view(app, Access_Always);
  Buffer_ID active_buffer = view_get_buffer(app, active_view, Access_Always);

  Mouse_State mouse = get_mouse_state(app);
  Vec2_f32 mouse_pos = V2f32(mouse.p);

  i32 clicked_i = -1;
  i32 closed_i = -1;
  for (i32 ii = 0; ii < g_tab_ctx.count; ii += 1){
    i32 i = (ii == g_tab_ctx.count-1 ? g_tab_ctx.current : ii + (ii >= g_tab_ctx.current));
    b32 is_current = (i == g_tab_ctx.current);
    Buffer_ID buffer = (is_current ? active_buffer : 0);
    for (i32 j = 0; j < tabs[i].count; j += 1){
      TAB_Node *node = &tabs[i].nodes[j];
      if (node->kind == TAB_Kind_LeafActive){
        buffer = node->buffer;
        break;
      }
    }

    String_Const_u8 full = push_buffer_base_name(app, scratch, buffer);
    String_Const_u8 pre  = string_prefix(full, max_len-1);
    String_Const_u8 name = (full.size <= max_len ? full : push_stringf(scratch, "%.*s…", string_expand(pre)));
    Vec2_f32 p0 = V2f32(tabs[i].cur_x, y0);
    Rect_f32 rect = rect_inner(Rf32_xy_wh(p0, tab_dim), -5.f);
    draw_rectangle(app, rect, 5.f, is_current ? cl_curr : cl_rect);
    draw_string(app, face, name, p0, fcolor_resolve(fcolor_id(defcolor_text_default)));

    if (rect_contains_point(rect, mouse_pos)){
      if ( mouse.press_l && !mouse.press_r){ clicked_i = i; }
      if (!mouse.press_l &&  mouse.press_r){  closed_i = i; }
    }
  }
  //draw_rectangle(app, rect_split_top_bottom_neg(region, 2.f).b, 0.f, fcolor_resolve(fcolor_id(defcolor_margin)));
  draw_rectangle(app, rect_split_top_bottom_neg(region, 2.f).b, 0.f, cl_curr);
  draw_set_clip(app, prev_clip);

  for (i32 i = 0; i < g_tab_ctx.count; i += 1){
    f32 nxt_x = x0 + (tab_dim.x + 10.f)*i + (i==hot_i ? mouse_pos.x-hot_x : 0.f);
    f32 delta = nxt_x - tabs[i].cur_x;
    tabs[i].cur_x += delta*(1.f - powf(1e-14f, frame_info.animation_dt));
    if (!near_zero(delta, 0.5f)){
      animate_in_n_milliseconds(app, 0);
    }
  }
  for (i64 i = g_tab_ctx.count; i < TAB_MAX_COUNT; i += 1){
    tabs[i].cur_x = x0 + (tab_dim.x + 10.f)*(g_tab_ctx.count-1);
  }

  if (clicked_i >= 0){
    hot_x = mouse_pos.x;
    hot_i = clicked_i;
    TAB_swap(app, clicked_i);
  }

  if (closed_i >= 0){
    hot_i = -1;
    TAB_close_inner(app, closed_i);
  }

  if (hot_i >= 0){
    i32 hover_i = (i32)((tabs[hot_i].cur_x - x0)/tab_dim.x + 0.5f);
    if (!mouse.l || !rect_contains_point(global_get_screen_rectangle(app), mouse_pos)){
      hot_i = -1;
    } else if (hover_i != hot_i && 0 <= hover_i && hover_i < g_tab_ctx.count){
      Swap(TAB_Data, tabs[hot_i], tabs[hover_i]);
      hot_x += (hover_i - hot_i)*tab_dim.x;
      hot_i = hover_i;
      g_tab_ctx.current = hover_i;
    }
  }
}

function void TAB_view_spin(Application_Links *app){
  View_ID view = get_this_ctx_view(app, Access_Always);
  g_tab_ctx.view = view;
  g_tab_ctx.count = 1;

  TAB_file_load(app);

  for (;;){
    User_Input input = get_next_input(app, EventPropertyGroup_Any, 0);
    if (input.abort){
      break;
    }

    if (input.event.kind == InputEventKind_CustomFunction){
      input.event.custom_func(app);
    }
  }
}

function void TAB_tick_inner(Application_Links *app, Frame_Info frame_info){
  View_ID active_now = get_active_view(app, Access_Always);
  local_persist View_ID active_last = active_now;

  if (g_tab_ctx.view == active_now){
    view_set_active(app, active_last);
  } else {
    active_last = active_now;
  }

  f32 nxt_height = (g_tab_ctx.count > 1 ? 30.f : 0.f);
  local_persist f32 cur_height = nxt_height;
  f32 delta = nxt_height - cur_height;
  cur_height += delta*(1.f - powf(1e-14f, frame_info.animation_dt));
  if (!near_zero(delta, 0.5f)){
    animate_in_n_milliseconds(app, 0);
  }

  view_set_split_pixel_size(app, g_tab_ctx.view, (i32)cur_height);
}

function Panel_ID TAB_root(Application_Links *app){
  Panel_ID panel = view_get_panel(app, g_tab_ctx.view);
  Panel_ID parent = panel_get_parent(app, panel);
  return panel_get_child(app, parent, Side_Max);
}

//- Config
function void TAB_file_save(Application_Links *app){
  Scratch_Block scratch(app);
  String_Const_u8 file_session = def_get_config_string(scratch, vars_save_string_lit("session_file"));
  if (file_session.size == 0){ return; }
  TAB_save(app);

  String_Const_u8 cstr = push_string_copy(scratch, file_session);
  FILE *f = fopen((char*)cstr.str, "wb");
  if (f == 0){ return; }

  Date_Time date_time = system_now_date_time_universal();
  date_time = system_local_date_time_from_universal(&date_time);
  String_Const_u8 date_string = date_time_format(scratch, "yyyy-mm-dd", &date_time);

  fprintf(f, "// Generated by 4coder_tabs.cpp\n");
  fprintf(f, "// %.*s\n", string_expand(date_string));
  fprintf(f, "current = %d;\n", g_tab_ctx.current);
  fprintf(f, "\n");
  fprintf(f, "tabs = {\n");

  for (i64 i = 0; i < g_tab_ctx.count; i += 1){
    TAB_Data *data = &g_tab_ctx.tabs[i];
    fprintf(f, "\t{\n");
    for (i64 j = 0; j < data->count; j += 1){
      TAB_Node *node = &data->nodes[j];
      fprintf(f, "\t\t{ .kind=%d, ", node->kind);
      if (node->kind >= TAB_Kind_LeafInactive){
        String_Const_u8 name = string_escape(scratch, push_buffer_file_name(app, scratch, node->buffer));
        u32 hex = hex_from_float(node->scroll.pixel_shift.y);
        fprintf(f, ".pixel_y=0x%08X, .line=%d, .cursor=%d, .mark=%d, .file=\"%.*s\", ",
                hex, (i32)node->scroll.line_number, (i32)node->cursor_pos, (i32)node->mark_pos, string_expand(name));
      } else {
        u32 hex = hex_from_float(node->ratio);
        fprintf(f, ".split_t=0x%08X, .split=%d, ", hex, node->split);
      }
      fprintf(f, "},\n");
    }
    fprintf(f, "\t},\n");
  }
  fprintf(f, "};");
  fclose(f);
}

// NOTE: Only minimal effort at validation is made, as the file is expected to be generated
function void TAB_file_load_inner(Application_Links *app, Config_Compound *nodes, TAB_Data *data){
  data->count = 0;
  for (Config_Compound_Element *n = nodes->first; n != 0; n = n->next){
    if (n->r->type != ConfigRValueType_Compound){ continue; }
    if (data->count == ArrayCount(data->nodes)){ break; }

    TAB_Node *node = &data->nodes[data->count++];
    for (Config_Compound_Element *e = n->r->compound->first; e != 0; e = e->next){
      String_Const_u8 iden = e->l.identifier;
      b32 is_int = (e->r->type == ConfigRValueType_Integer);
      i32 i_val  = (e->r->integer);
      f32 f_val  = float_from_hex(e->r->uinteger);
      if (false){
      } else if (string_match(iden, SCu8("file")) && e->r->type == ConfigRValueType_String){
        node->buffer = create_buffer(app, e->r->string, Access_Always);
      } else if (string_match(iden, SCu8("kind")) && is_int){
        node->kind = (range_contains(Ii32(0, TAB_Kind_COUNT), i_val) ? (TAB_Kind)i_val : TAB_Kind_LeafInactive);
      } else if (string_match(iden, SCu8("pixel_y")) && is_int){
        node->scroll.pixel_shift.y = ((0.f < f_val && f_val < 1.f) ? f_val : 0.5f);
      } else if (string_match(iden, SCu8("line")) && is_int){
        node->scroll.line_number = ((0 <= i_val) ? i_val : 0);
      } else if (string_match(iden, SCu8("cursor")) && is_int){
        node->cursor_pos = ((0 <= i_val) ? i_val : 0);
      } else if (string_match(iden, SCu8("mark")) && is_int){
        node->mark_pos = ((0 <= i_val) ? i_val : 0);
      } else if (string_match(iden, SCu8("split_t")) && is_int){
        node->ratio = ((0.f < f_val && f_val < 1.f) ? f_val : 0.5f);
      } else if (string_match(iden, SCu8("split")) && is_int){
        node->split = (range_contains(Ii32(0, TAB_Split_COUNT), i_val) ? (TAB_Split)i_val : TAB_Split_RootKeep);
      }
    }
  }
}

function void TAB_file_load(Application_Links *app){
  Scratch_Block scratch(app);
  String_Const_u8 file_session = def_get_config_string(scratch, vars_save_string_lit("session_file"));
  File_Name_Data data = dump_file(scratch, file_session);
  Config *parsed = def_config_from_text(app, scratch, file_session, data.data);
  if (parsed == 0){ return; }

  i32 current = 0;
  i32 count = 0;
  for (Config_Assignment *node = parsed->first; node != 0; node = node->next){
    if (string_match(node->l->identifier, SCu8("current")) && node->r->type == ConfigRValueType_Integer){
      current = node->r->integer;
    }

    if (string_match(node->l->identifier, SCu8("tabs")) && node->r->type == ConfigRValueType_Compound){
      for (Config_Compound_Element *t = node->r->compound->first; t != 0; t = t->next){
        if (t->r->type != ConfigRValueType_Compound){ continue; }
        if (count == TAB_MAX_COUNT){ break; }
        TAB_file_load_inner(app, t->r->compound, &g_tab_ctx.tabs[count++]);
      }
    }
  }

  if (count > 0 && range_contains(Ii32(0, count), current)){
    g_tab_ctx.count = count;
    TAB_apply(app, current);
  }
}

function TAB_Node TAB_get_node(Application_Links *app, Panel_ID panel, b32 is_root){
  TAB_Node node = {};
  if (panel_is_split(app, panel)){
    b32 use_root = panel_is_split(app, panel_get_child(app, panel, Side_Min));
    Rect_f32 min = panel_get_rect(app, panel_get_child(app, panel, Side_Min));
    Rect_f32 max = panel_get_rect(app, panel_get_child(app, panel, Side_Max));
    b32 is_h = near_zero(min.y1 - max.y0, 0.5f);
    node.kind = (TAB_Kind)is_h;
    node.ratio = rect_dim(min).v[is_h] / rect_dim(rect_union(min, max)).v[is_h];
    node.split = (is_root ? TAB_Split_RootRight :
                  (use_root ? TAB_Split_RootKeep : TAB_Split_Current));
  } else {
    View_ID view = panel_get_view(app, panel, Access_Always);
    View_ID active = get_active_view(app, Access_Always);
    node.kind       = (view == active ? TAB_Kind_LeafActive : TAB_Kind_LeafInactive);
    node.buffer     = view_get_buffer(app, view, Access_Always);
    node.scroll     = view_get_buffer_scroll(app, view).target;
    node.cursor_pos = view_get_cursor_pos(app, view);
    node.mark_pos   = view_get_mark_pos(app, view);
    node.jumps      = *qol_jumps(app, view);
  }
  return node;
}

/// in-order traversal, tracking right-subtree roots
function TAB_Node* TAB_save_inner(Application_Links *app, Panel_ID panel, TAB_Node *node, b32 is_root){
  if (panel_is_leaf(app, panel)){
    *node++ = TAB_get_node(app, panel, false);
  } else {
    node    = TAB_save_inner(app, panel_get_child(app, panel, Side_Min), node, false);
    *node++ = TAB_get_node(app, panel, is_root);
    node    = TAB_save_inner(app, panel_get_child(app, panel, Side_Max), node, is_root);
  }
  return node;
}

function void TAB_save(Application_Links *app){
  TAB_Data *data = &g_tab_ctx.tabs[g_tab_ctx.current];
  TAB_Node *opl = TAB_save_inner(app, TAB_root(app), data->nodes, true);
  data->count = opl - data->nodes;
}

/// Resets current panel tree to a single *scratch* leaf node
function void TAB_scratch(Application_Links *app){
  /// Since view_close re-associates Panels <-> Views, this was the most robust way
  for(;;){
    Panel_ID panel = TAB_root(app);
    if (panel_is_leaf(app, panel)){
      Buffer_Identifier iden = buffer_identifier(string_u8_litexpr("*scratch*"));
      Buffer_ID buffer_scratch = buffer_identifier_to_id(app, iden);
      View_ID view = panel_get_view(app, panel, Access_Always);
      view_set_buffer(app, view, buffer_scratch, 0);
      break;
    }

    for(;;){
      panel = panel_get_child(app, panel, Side_Min);
      if (panel_is_leaf(app, panel)){ break; }
    }
    view_close(app, panel_get_view(app, panel, Access_Always));
  }
}

function void TAB_apply_inner(Application_Links *app){
  Scratch_Block scratch(app);
  TAB_scratch(app);

  TAB_Data *data = &g_tab_ctx.tabs[g_tab_ctx.current];
  Panel_ID root = TAB_root(app);
  View_ID view = panel_get_view(app, root, Access_Always);
  View_ID active = 0;

  for (i32 i = 0; i < data->count; i += 1){
    TAB_Node *node = &data->nodes[i];
    if (node->kind >= TAB_Kind_LeafInactive){
      view_set_buffer(app, view, node->buffer, 0);
      view_set_buffer_scroll(app, view, Buffer_Scroll{node->scroll, node->scroll}, SetBufferScroll_NoCursorChange);
      view_set_mark(app, view, seek_pos(node->mark_pos));
      view_set_cursor_and_preferred_x(app, view, seek_pos(node->cursor_pos));
      *qol_jumps(app, view) = node->jumps;
      if (node->kind == TAB_Kind_LeafActive){ active = view; }
    } else {
      Panel_ID panel = (node->split == TAB_Split_Current ? view_get_panel(app, view) : root);
      panel_split(app, panel, Dimension(node->kind));
      panel_set_split(app, panel, PanelSplitKind_Ratio_Min, node->ratio);
      Panel_ID right = panel_get_child(app, panel, Side_Max);
      if (node->split == TAB_Split_RootRight){ root = right; }
      view = panel_get_view(app, right, Access_Always);
      new_view_settings(app, view);
    }
  }

  if (active){
    view_set_active(app, active);
  }
  data->count = 0;
}

function void TAB_apply(Application_Links *app, i32 i){
  g_tab_ctx.current = i;
  view_enqueue_command_function(app, g_tab_ctx.view, TAB_apply_inner);
}

function void TAB_swap(Application_Links *app, i32 i){
  TAB_save(app);
  TAB_apply(app, i);
}

function void TAB_close_inner(Application_Links *app, i32 i){
  if (g_tab_ctx.count <= 1){ return; }
  i32 count = --g_tab_ctx.count;
  TAB_Data *a = g_tab_ctx.tabs;
  block_copy(a+i, a+i+1, (count-i)*sizeof(*a));
  if (i == g_tab_ctx.current){
    TAB_apply(app, Min(i, count-1));
  }
  g_tab_ctx.current -= (i < g_tab_ctx.current);
}

//- Commands
CUSTOM_COMMAND_SIG(TAB_next)
CUSTOM_DOC("[TAB] switch to next tab in list")
{
  if (g_tab_ctx.count <= 1){ return; }
  TAB_swap(app, (g_tab_ctx.current+1) % g_tab_ctx.count);
}

CUSTOM_COMMAND_SIG(TAB_prev)
CUSTOM_DOC("[TAB] switch to prev tab in list")
{
  if (g_tab_ctx.count <= 1){ return; }
  TAB_swap(app, (g_tab_ctx.current-1+g_tab_ctx.count) % g_tab_ctx.count);
}

CUSTOM_COMMAND_SIG(TAB_new)
CUSTOM_DOC("[TAB] create new tab with current buffer")
{
  if (g_tab_ctx.count >= TAB_MAX_COUNT){ return; }
  i32 i = g_tab_ctx.count++;
  TAB_Data *data = &g_tab_ctx.tabs[i];
  View_ID view = get_active_view(app, Access_Always);
  data->nodes[0] = TAB_get_node(app, view_get_panel(app, view), true);
  data->count = 1;
  TAB_swap(app, i);
}

CUSTOM_COMMAND_SIG(TAB_close)
CUSTOM_DOC("[TAB] closes current tab")
{
  TAB_close_inner(app, g_tab_ctx.current);
}

//- STUB
function void TAB_startup(Application_Links *app){
  default_startup(app);
  TAB_startup_inner(app);
}

function void TAB_try_exit(Application_Links *app){
  TAB_try_exit_inner(app);
  default_try_exit(app);
}

function void TAB_tick(Application_Links *app, Frame_Info frame_info){
  default_tick(app, frame_info);
  TAB_tick_inner(app, frame_info);
}

function void TAB_setup_default_mapping(Mapping *mapping, i64 global_id, i64 file_id, i64 code_id){
  MappingScope();
  SelectMapping(&framework_mapping);

  SelectMap(file_id);
  Bind(TAB_next,   KeyCode_Tab, KeyCode_Control);
  Bind(TAB_prev,   KeyCode_Tab, KeyCode_Control, KeyCode_Shift);
  Bind(TAB_new,    KeyCode_T,   KeyCode_Control);
  Bind(TAB_close,  KeyCode_W,   KeyCode_Control, KeyCode_Shift);

  SelectMap(code_id);
  ParentMap(file_id);
  Bind(TAB_next,   KeyCode_Tab, KeyCode_Control);
  Bind(TAB_prev,   KeyCode_Tab, KeyCode_Control, KeyCode_Shift);
  Bind(TAB_new,    KeyCode_T,   KeyCode_Control);
  Bind(TAB_close,  KeyCode_W,   KeyCode_Control, KeyCode_Shift);
}