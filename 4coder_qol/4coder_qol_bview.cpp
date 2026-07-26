
CUSTOM_COMMAND_MC_GLOBAL_SIG(qol_bview_close)
CUSTOM_DOC("[QOL] Closes bottom view")
{
  g_qol_nxt_bot_height = 0.f;
}

CUSTOM_COMMAND_MC_GLOBAL_SIG(qol_bview_open)
CUSTOM_DOC("[QOL] Opens bottom view")
{
  Scratch_Block scratch(app);
  g_qol_nxt_bot_height = 0.35f*rect_height(global_get_screen_rectangle(app));
  qol_bot_text_set(push_buffer_unique_name(app, scratch, g_qol_b_buffer));
}

CUSTOM_COMMAND_MC_GLOBAL_SIG(qol_bview_toggle)
CUSTOM_DOC("[QOL] Toggles bottom view open/close")
{
  g_qol_nxt_bot_height > 0.5f ? qol_bview_close(app) : qol_bview_open(app);
}

function void qol_bview_set_buffer(Application_Links *app, Buffer_ID buffer){
  g_qol_b_buffer = buffer;
  view_set_buffer(app, g_qol_b_view, buffer, SetBuffer_KeepOriginalGUI);
}

CUSTOM_COMMAND_MC_GLOBAL_SIG(qol_bview_active_to_bottom)
CUSTOM_DOC("[QOL] Sets opens active buffer in bottom view")
{
  View_ID view = get_active_view(app, Access_ReadVisible);
  qol_bview_set_buffer(app, view_get_buffer(app, view, Access_ReadVisible));
  qol_bview_open(app);
}

CUSTOM_COMMAND_SIG(qol_bview_bottom_to_active)
CUSTOM_DOC("[QOL] Opens bottom view buffer in active view")
{
  View_ID view = get_active_view(app, Access_ReadVisible);
  view_set_buffer(app, view, g_qol_b_buffer, 0);
  qol_bview_close(app);
}

CUSTOM_COMMAND_MC_GLOBAL_SIG(qol_bview_scroll_up)
CUSTOM_DOC("[QOL] Scrolls bottom view up")
{
  Buffer_Scroll scroll = view_get_buffer_scroll(app, g_qol_b_view);
  scroll.target = view_move_buffer_point(app, g_qol_b_view, scroll.target, V2f32(0.f, -g_qol_nxt_bot_height));
  move_vertical_pixels(app, g_qol_b_view, -g_qol_nxt_bot_height);
  view_set_buffer_scroll(app, g_qol_b_view, scroll, SetBufferScroll_SnapCursorIntoView);
}

CUSTOM_COMMAND_MC_GLOBAL_SIG(qol_bview_scroll_down)
CUSTOM_DOC("[QOL] Scolls bottom view down")
{
  Buffer_Scroll scroll = view_get_buffer_scroll(app, g_qol_b_view);
  scroll.target = view_move_buffer_point(app, g_qol_b_view, scroll.target, V2f32(0.f, g_qol_nxt_bot_height));
  move_vertical_pixels(app, g_qol_b_view, g_qol_nxt_bot_height);
  view_set_buffer_scroll(app, g_qol_b_view, scroll, SetBufferScroll_SnapCursorIntoView);
}

function void
qol_bview_render(Application_Links *app, Frame_Info frame_info, View_ID view){
  Rect_f32 region = view_get_screen_rect(app, view);
  Face_ID face_id = get_face_id(app, 0);
  Face_Metrics metrics = get_face_metrics(app, face_id);
  f32 text_field_height = qol_bot_text_height(app, metrics);
  Rect_f32_Pair pair = rect_split_top_bottom_neg(region, text_field_height);
  region = pair.min;

  qol_lister_render(app, frame_info, view);

  if (g_qol_lister == 0 && g_qol_b_buffer && g_qol_nxt_bot_height > 0.5f){
    Rect_f32 rect = rect_split_left_right(region, 4.f).max;
    Rect_f32 prev_clip = draw_set_clip(app, rect);
    Face_ID face = get_face_id(app, g_qol_b_buffer);

    Buffer_Scroll scroll = view_get_buffer_scroll(app, view);
    Buffer_Point_Delta_Result delta = delta_apply(app, view, frame_info.animation_dt, scroll);
    if (!block_match_struct(&scroll.position, &delta.point)){
      block_copy_struct(&scroll.position, &delta.point);
      view_set_buffer_scroll(app, view, scroll, SetBufferScroll_NoCursorChange);
    }
    if (delta.still_animating){
      animate_in_n_milliseconds(app, 0);
    }

    Text_Layout_ID text_layout_id = text_layout_create(app, g_qol_b_buffer, rect, scroll.position);
    qol_render_buffer(app, view, face, g_qol_b_buffer, text_layout_id, rect);
    text_layout_free(app, text_layout_id);
    draw_set_clip(app, prev_clip);
  }

  if (g_qol_lister == 0 && g_qol_cur_bot_height > 0.5f){
    Rect_f32 dividor_rect = rect_split_top_bottom_neg(region, 2.f).max;
    draw_rectangle_fcolor(app, dividor_rect, 0.f, get_item_margin_color(UIHighlight_Active));
  }
}

function void
qol_tick_bview(Application_Links *app, Frame_Info frame_info){
  Face_ID face_id = get_face_id(app, 0);
  Face_Metrics metrics = get_face_metrics(app, face_id);
  f32 block_height = lister_get_block_height(metrics.line_height);

  if (g_qol_lister){
    Vec2_i32 grid = qol_lister_grid_dim(app, g_qol_lister);
    g_qol_nxt_bot_height = grid.y*block_height;
    view_set_active(app, g_qol_lister_view);
  }

  f32 text_field_height = qol_bot_text_height(app, metrics);
  f32 max_height = 0.5f*rect_height(global_get_screen_rectangle(app));
  f32 nxt_height = Min(text_field_height + g_qol_nxt_bot_height, max_height);

  qol_interp(g_qol_cur_bot_height, nxt_height, frame_info.animation_dt, 1e-14f);
  if (abs_f32(g_qol_cur_bot_height - nxt_height) >= 1.f){
    animate_in_n_milliseconds(app, 0);
  }
  else{
    g_qol_cur_bot_height = nxt_height;
  }
  view_set_split_pixel_size(app, g_qol_b_view, (i32)g_qol_cur_bot_height);
}

function void
qol_bview_spin(Application_Links *app){
  View_ID view = get_this_ctx_view(app, Access_Always);
  View_Context ctx = view_current_context(app, view);
  ctx.render_caller = qol_bview_render;
  ctx.hides_buffer = true;
  View_Context_Block ctx_block(app, view, &ctx);
  change_active_panel_backwards(app);

  for (;;){
    User_Input in = get_next_input(app, EventPropertyGroup_Any, 0);
    if (in.abort){ break; }
    leave_current_input_unhandled(app);
  }
}

function void
qol_bview_init(Application_Links *app){
  Panel_ID root = panel_get_root(app);
  Panel_ID split = panel_split(app, root, Dimension_Y);
  if (split){
    panel_set_split(app, split, PanelSplitKind_FixedPixels_Max, 1.f);
    g_qol_b_view = panel_get_view(app, panel_get_child(app, split, Side_Max), Access_Always);
    view_enqueue_command_function(app, g_qol_b_view, qol_bview_spin);
    view_set_setting(app, g_qol_b_view, ViewSetting_ShowFileBar, false);
    view_set_passive(app, g_qol_b_view, true);
    view_set_camera_bounds(app, g_qol_b_view, V2f32(max_f32, max_f32), V2f32(1,1));
    qol_bview_set_buffer(app, buffer_identifier_to_id_create_out_buffer(app, standard_build_build_buffer_identifier));
  }
}
