
enum Rect_Edge{ EDGE_X0, EDGE_X1, EDGE_Y0, EDGE_Y1 };
struct View_Val{ View_ID view; f32 val; };
function View_Val max_view(View_Val a, View_Val b){ return a.val >= b.val ? a : b; }

function View_Val vim_window_max(Application_Links *app, View_ID ignore, Rect_f32 edge, Panel_ID p, View_Val max){
  if (panel_is_leaf(app, p)){
    View_ID view = panel_get_view(app, p, Access_Always);
    if (view == ignore || view_get_is_passive(app, view)){ return max; }
    return max_view(max, {view, rect_area(rect_intersect(edge, view_get_screen_rect(app, view)))});
  } else {
    return max_view(vim_window_max(app, ignore, edge, panel_get_child(app, p, Side_Min), max),
                    vim_window_max(app, ignore, edge, panel_get_child(app, p, Side_Max), max));
  }
}

function void vim_window(Application_Links *app, Rect_Edge e){
  View_ID view = get_active_view(app, Access_Always);
  Rect_f32 r = view_get_screen_rect(app, view);
  Rect_f32 s = (e == EDGE_X0 ? Rf32(If32(r.x0), rect_range_y(r)) :
                e == EDGE_X1 ? Rf32(If32(r.x1), rect_range_y(r)) :
                e == EDGE_Y0 ? Rf32(rect_range_x(r), If32(r.y0)) :
                e == EDGE_Y1 ? Rf32(rect_range_x(r), If32(r.y1)) : r);
  Panel_ID root = TAB_root(app);
  View_Val max = vim_window_max(app, view, rect_inner(s, -1.f), root, {view, 1.f});
  view_set_active(app, max.view);
}

VIM_COMMAND_SIG(vim_window_l){ vim_window(app, EDGE_X0); }
VIM_COMMAND_SIG(vim_window_r){ vim_window(app, EDGE_X1); }
VIM_COMMAND_SIG(vim_window_u){ vim_window(app, EDGE_Y0); }
VIM_COMMAND_SIG(vim_window_d){ vim_window(app, EDGE_Y1); }