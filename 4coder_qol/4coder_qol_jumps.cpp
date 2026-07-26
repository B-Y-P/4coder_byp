
CUSTOM_ID(attachment, qol_view_jumps);

global b32 g_qol_will_jump;

struct QOL_Point{
  Buffer_ID buffer;
  i64 pos;
};

struct QOL_View_Jumps{
  b32 check;
  QOL_Point point;
  i64 bot, pos, top;
  Point_Stack_Slot ring[128];
};

function QOL_View_Jumps* qol_jumps(Application_Links *app, View_ID view){
  Managed_Scope scope = view_get_managed_scope(app, view);
  return scope_attachment(app, scope, qol_view_jumps, QOL_View_Jumps);
}

function QOL_Point qol_current_point(Application_Links *app){
  QOL_Point point = {};
  View_ID view = get_active_view(app, Access_ReadVisible);
  point.buffer = view_get_buffer(app, view, Access_ReadVisible);
  point.pos = view_get_cursor_pos(app, view);
  return point;
}

function QOL_Point qol_point_from_slot(Application_Links *app, Point_Stack_Slot slot){
  Marker *marker = (Marker*)managed_object_get_pointer(app, slot.object);
  return marker == 0 ? QOL_Point{-1,-1} : QOL_Point{slot.buffer, marker->pos};
}

function Point_Stack_Slot qol_alloc_slot_from_point(Application_Links *app, QOL_Point point){
  Point_Stack_Slot slot = {};
  Managed_Object object = alloc_buffer_markers_on_buffer(app, point.buffer, 1, 0);
  Marker *marker = (Marker*)managed_object_get_pointer(app, object);
  marker->pos = point.pos;
  marker->lean_right = false;
  slot.buffer = point.buffer;
  slot.object = object;
  return slot;
}

function void qol_free_slot(Application_Links *app, Point_Stack_Slot slot){
  managed_object_free(app, slot.object);
}

function b32 qol_is_jump(Application_Links *app, QOL_Point a, QOL_Point b){
  if (g_qol_will_jump){ return true; }
  if (!buffer_exists(app, a.buffer)){ return false; }
  return a.buffer != b.buffer || 1 < range_size(get_line_range_from_pos_range(app, a.buffer, Ii64(a.pos, b.pos)));
}

function void qol_pre_command_inner(Application_Links *app, Managed_Scope scope){
  QOL_View_Jumps *jumps = scope_attachment(app, scope, qol_view_jumps, QOL_View_Jumps);
  jumps->check = true;
  jumps->point = qol_current_point(app);
  g_qol_will_jump = false;
}

function void qol_post_command_inner(Application_Links *app, Managed_Scope scope){
  QOL_View_Jumps *jumps = scope_attachment(app, scope, qol_view_jumps, QOL_View_Jumps);
  QOL_Point point = qol_current_point(app);
  if (jumps != 0 && jumps->check && qol_is_jump(app, jumps->point, point)){
    i64 cap = ArrayCount(jumps->ring);
    if (jumps->pos - jumps->bot == cap-1){
      qol_free_slot(app, jumps->ring[jumps->bot++ % cap]);
    }
    if (2*cap <= jumps->pos){
      jumps->top -= cap;
      jumps->bot -= cap;
    }
    jumps->ring[jumps->pos++ % cap] = qol_alloc_slot_from_point(app, jumps->point);
    jumps->top = jumps->pos;
  }
}

function void qol_pre_command(Application_Links *app, Managed_Scope scope){
  qol_pre_command_inner(app, scope);
  default_pre_command(app, scope);
}

function void qol_post_command(Application_Links *app, Managed_Scope scope){
  default_post_command(app, scope);
  qol_post_command_inner(app, scope);
}

function void qol_jump_to_point(Application_Links *app, View_ID view, QOL_Point point){
  if (point.buffer != view_get_buffer(app, view, Access_Always)){
    view_set_buffer(app, view, point.buffer, 0);
    view_set_cursor(app, view, seek_pos(point.pos));
    center_view(app);
  }
  else{
    Range_i64 range = Ii64(view_get_cursor_pos(app, view), point.pos);
    Range_i64 lines = get_line_range_from_pos_range(app, point.buffer, range);
    f32 y_diff = view_line_y_difference(app, view, lines.max, lines.min);
    view_set_cursor(app, view, seek_pos(point.pos));
    if (rect_height(view_get_buffer_region(app, view)) < y_diff){
      center_view(app);
    }
  }
}

CUSTOM_COMMAND_SIG(qol_jump_down)
CUSTOM_DOC("[QOL] Jump down the view's jump stack")
{
  View_ID view = get_active_view(app, Access_ReadVisible);
  QOL_View_Jumps *jumps = qol_jumps(app, view);
  jumps->check = false;

  if (jumps->pos == jumps->bot){ return; }
  QOL_Point point = qol_point_from_slot(app, ArraySafe(jumps->ring, --jumps->pos));
  if (!buffer_exists(app, point.buffer)){ return; }

  if (jumps->pos+1 == jumps->top){
    ArraySafe(jumps->ring, jumps->top++) = qol_alloc_slot_from_point(app, qol_current_point(app));
  }
  qol_jump_to_point(app, view, point);
}

CUSTOM_COMMAND_SIG(qol_jump_up)
CUSTOM_DOC("[QOL] Jump back up the view's jump stack")
{
  View_ID view = get_active_view(app, Access_ReadVisible);
  QOL_View_Jumps *jumps = qol_jumps(app, view);
  jumps->check = false;

  if (jumps->top <= jumps->pos+1){ return; }
  QOL_Point point = qol_point_from_slot(app, ArraySafe(jumps->ring, ++jumps->pos));
  if (!buffer_exists(app, point.buffer)){ return; }
  qol_jump_to_point(app, view, point);
}
