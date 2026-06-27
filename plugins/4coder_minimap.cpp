// 4coder_minimap.cpp
// BYP

// NOTE: Expected usage, at the end of render_buffer add
// + Text_Layout_ID minimap_id = MM_begin(app, scratch, view, face, buffer, tokens, region, visible_range, cl_nest);
// + draw_cpp_token_colors(app, buffer, minimap_id);
// + MM_end(app, minimap_id);
// Where you may bring your own version of `draw_cpp_token_colors`
//
// To be a good citizen, you can replace
//  -    paint_text_color (app, text_layout_id, Ii64(token), argb)
//  + MM_paint_token_color(app, text_layout_id, token,       argb)
// To remove a per-token binary search of token_from_pos(...)

function Text_Layout_ID MM_begin(Application_Links *app, Arena *arena, View_ID view, Face_ID face, Buffer_ID buffer, Token_Array tokens, Rect_f32 region, Range_i64 visible_range, ARGB_Color cl_nest);
function void           MM_end  (Application_Links *app, Text_Layout_ID minimap_id);

function void MM_paint_text_color (Application_Links *app, Text_Layout_ID layout_id, Range_i64 range, ARGB_Color color);
function void MM_paint_token_color(Application_Links *app, Text_Layout_ID layout_id, Token    *token, ARGB_Color color);

// Only necessary if you want "minimap_scrollable"
#define MM_Wrap(f) ([](Application_Links *app){ if (!MM_will_handle(app)){ f(app); } })
// BindMouse(MM_Wrap(click_set_cursor_and_mark), MouseCode_Left);
// BindMouseRelease(MM_Wrap(click_set_cursor), MouseCode_Left);
// BindCore(click_set_cursor_and_mark, CoreCode_ClickActivateView);
// BindMouseMove(MM_Wrap(click_set_cursor_if_lbutton));

struct MM_Node{
    MM_Node *next;
    i64 p0;
    i64 p1;
    Vec2_f32 a;
};

struct MM_Context{
    View_ID view;
    Buffer_ID buffer;
    Face_ID face;
    Text_Layout_ID layout;
    i64 line;
    Rect_f32 outer;
    Rect_f32 region;
    Range_i64 range;

    custom_paint_text_color_type *core_paint_text_color;
    Token *hint;
    Token_Array tokens;
    Vec2_f32 s_p0;
    Vec2_f32 s_str;
    f32 line_height;

    ARGB_Color cl_nest;
    MM_Node *head;
    MM_Node *tail;
    MM_Node *stack;

    View_ID  hot_view;
    Rect_f32 hot_rect;
    f32      hot_delta;

    // Cached to minimize calls to Layout_Function*
    i64       _line;  // the further line is from xy, the more expensive view_relative_box_of_pos / view_pos_at_relative_xy
    Vec2_f32  _rel;   // incrementally accumulate into rel to keep line and xy close
    Range_i64 _range; // pos range of _line
    Vec2_f32  _p0;
};
global MM_Context g_mm_ctx;

function Vec2_f32 MM_get_xy(Application_Links *app, i64 pos){
    if (!range_contains(g_mm_ctx._range, pos)){
        i64 line = get_line_number_from_pos(app, g_mm_ctx.buffer, pos);
        g_mm_ctx._range.min = view_pos_at_relative_xy (app, g_mm_ctx.view, line, V2f32(0.f,     0.f));
        g_mm_ctx._range.max = view_pos_at_relative_xy (app, g_mm_ctx.view, line, V2f32(max_f32, 0.f));
        g_mm_ctx._rel.x  =    view_relative_box_of_pos(app, g_mm_ctx.view, line, g_mm_ctx._range.min).x0;
        g_mm_ctx._rel.y += g_mm_ctx.line_height*(line - g_mm_ctx._line);
        g_mm_ctx._line = line;
        g_mm_ctx._p0 = g_mm_ctx.region.p0 + hadamard(g_mm_ctx.s_p0, g_mm_ctx._rel);
    }
    return g_mm_ctx._p0 + g_mm_ctx.s_str.x * V2f32(f32(pos - g_mm_ctx._range.min), 0);
}

function Rect_f32 MM_get_line(i64 line){
    f32 y0 = (1 + line - g_mm_ctx.line)*(g_mm_ctx.s_str.y+1);
    return Rf32(rect_range_x(g_mm_ctx.outer), If32_size(y0, g_mm_ctx.s_str.y+1));
}

function void MM_nest_open(Application_Links *app){
    MM_Node *node = g_mm_ctx.head;
    sll_queue_pop(g_mm_ctx.head, g_mm_ctx.tail);
    node->a = MM_get_xy(app, node->p0);
    sll_stack_push(g_mm_ctx.stack, node);
}

function void MM_nest_close(Application_Links *app){
    MM_Node *node = g_mm_ctx.stack;
    sll_stack_pop(g_mm_ctx.stack);
    Vec2_f32 a = node->a;
    Vec2_f32 b = MM_get_xy(app, node->p1);
    Rect_f32 r = rect_union(Rf32(a,a), Rf32(b,b));
    draw_rectangle(app, Rf32(r.x0, r.y0, r.x0+1, r.y1 + g_mm_ctx.s_str.y), 0.f, g_mm_ctx.cl_nest);
}

function Code_Index_Nest* MM_last_nest(Code_Index_Nest *nest, Code_Index_Nest_Ptr_Array *array, Range_i64 range){
    for (i64 i=0; i < array->count; i++){
        Code_Index_Nest *n = array->ptrs[array->count-1 - i];
        if (range_overlap(range, Ii64(n->open.min, n->close.min))){
            return MM_last_nest(n, &n->nest_array, range);
        }
    }
    return nest;
}

function b32 MM_nest_list(Arena *arena, Code_Index_Nest_Ptr_Array *array, Code_Index_Nest *last, Range_i64 range){
    for (i64 i=0; i < array->count; i++){
        Code_Index_Nest *nest = array->ptrs[i];
        if (range_overlap(range, Ii64(nest->open.min, nest->close.min))){
            if (nest->kind == CodeIndexNest_Scope){
                MM_Node *node = push_array_zero(arena, MM_Node, 1);
                node->p0 = nest->open.min;
                node->p1 = nest->close.min;
                sll_queue_push(g_mm_ctx.head, g_mm_ctx.tail, node);
            }
            if (nest == last || MM_nest_list(arena, &nest->nest_array, last, range)){ return true; }
        }
        if (range.max <= nest->open.min){ break; }
    }
    return false;
}

function void MM_paint_token_color(Application_Links *app, Text_Layout_ID layout_id, Token *token, ARGB_Color color){
    paint_text_color(app, layout_id, Ii64(g_mm_ctx.hint = token), color);
}

function void MM_paint_text_color(Application_Links *app, Text_Layout_ID layout_id, Range_i64 range, ARGB_Color color){
    if (g_mm_ctx.layout == 0){ return paint_text_color(app, layout_id, range, color); }
    if (g_mm_ctx.layout != layout_id){ return g_mm_ctx.core_paint_text_color(app, layout_id, range, color); }
    if (!range_overlap(range, g_mm_ctx.range)){ return; }

    b32 use_hint = (g_mm_ctx.hint && g_mm_ctx.hint->pos == range.min);
    Token *t = (use_hint ? g_mm_ctx.hint : token_from_pos(&g_mm_ctx.tokens, range.min));
    if (t == 0 || range.max <= t->pos || t->kind == TokenBaseKind_Whitespace){ return; }

    while (g_mm_ctx.head  && range_contains(g_mm_ctx._range, g_mm_ctx.head->p0)){ MM_nest_open(app); }
    while (g_mm_ctx.stack && range_contains(g_mm_ctx._range, g_mm_ctx.stack->p1)){ MM_nest_close(app); }

    i64 pos = t->pos;
    Vec2_f32 p = MM_get_xy(app, pos);
    for (i64 i=range.min; i < range.max+1; i++){  // bail if there are somehow more lines than chars
        i64 end = Min(range.max, g_mm_ctx._range.max);
        if (pos != end){
            Rect_f32 rect = Rf32_xy_wh(p, hadamard(g_mm_ctx.s_str, V2f32(Max(2.f/1.1f, f32(end-pos)), 1.f)));
            if (rect_overlap(rect, g_mm_ctx.region)){
                draw_rectangle_fcolor(app, rect, 0.f, fcolor_change_alpha(fcolor_argb(color), 0.8f));
            }
        }

        if (range.max <= end){ break; }  // Which ends first: the visual line, or the token?
        g_mm_ctx._line += 1;
        g_mm_ctx._range.min = view_pos_at_relative_xy (app, g_mm_ctx.view, g_mm_ctx._line, V2f32(0.f,     0));
        g_mm_ctx._range.max = view_pos_at_relative_xy (app, g_mm_ctx.view, g_mm_ctx._line, V2f32(max_f32, 0));
        g_mm_ctx._rel.x  =    view_relative_box_of_pos(app, g_mm_ctx.view, g_mm_ctx._line, g_mm_ctx._range.min).x0;
        g_mm_ctx._rel.y += g_mm_ctx.line_height;
        g_mm_ctx._p0 = g_mm_ctx.region.p0 + hadamard(g_mm_ctx.s_p0, g_mm_ctx._rel);

        pos = g_mm_ctx._range.min;
        p = g_mm_ctx._p0;
    }
}

function Text_Layout_ID MM_begin(Application_Links *app, Arena *arena, View_ID view, Face_ID face, Buffer_ID buffer, Token_Array tokens, Rect_f32 region, Range_i64 visible_range, ARGB_Color cl_nest){
    if (!def_get_config_b32(vars_save_string_lit("minimap_enabled")) || 
        def_get_config_b32(vars_save_string_lit("minimap_only_active")) && view != get_active_view(app, Access_Always)){ 
        return 0;
    }
    g_mm_ctx.stack = g_mm_ctx.head = g_mm_ctx.tail = 0;

    Face_Metrics metrics = get_face_metrics(app, face);
    f32 width = Min(0.3f*rect_width(region), 15.f*metrics.normal_advance);
    Rect_f32 outer = rect_split_left_right_neg(region, width).b;

    f32 minimap_line_height = 4.f;
    f32 minimap_char_width  = 1.1f;
    i64  buf_line_count = buffer_get_line_count(app, buffer);
    f32 view_line_count = rect_height(region) / metrics.line_height;
    f32   mm_line_count = rect_height(region) / minimap_line_height;
    mm_line_count = Min(mm_line_count, f32(buf_line_count));
    i64 mm_scroll_line_count = Max(0, buf_line_count - i64(mm_line_count));

    Buffer_Scroll scroll = view_get_buffer_scroll(app, view);
    f32 y  = metrics.line_height * f32(scroll.position.line_number) + scroll.position.pixel_shift.y;
    f32 y1 = metrics.line_height * f32(buf_line_count) - rect_height(region);

    f32 top = (y1 <= 0.f ? 0.f : clamp(0.f, y/y1, 1.f) * f32(mm_scroll_line_count));
    i64 top_line = i64(top);
    Buffer_Point point = {top_line, V2f32(0,0)};

    g_mm_ctx.view   = view;
    g_mm_ctx.face   = face;
    g_mm_ctx.buffer = buffer;
    g_mm_ctx.tokens = tokens;
    g_mm_ctx.line   = top_line;
    g_mm_ctx.layout = text_layout_create(app, buffer, Rf32(0, 0, rect_width(region), metrics.line_height*mm_line_count), point);
    g_mm_ctx.outer  = outer;
    g_mm_ctx.region = Rf32(outer.x0+8, outer.y0+1, outer.x1, Min(outer.y1-1, outer.y0 + minimap_line_height*mm_line_count));
    g_mm_ctx.range  = text_layout_get_visible_range(app, g_mm_ctx.layout);
    g_mm_ctx.s_p0   = V2f32(minimap_char_width / metrics.normal_advance, minimap_line_height / metrics.line_height);
    g_mm_ctx.s_str  = V2f32(minimap_char_width, minimap_line_height-1.f);
    g_mm_ctx._line  = g_mm_ctx.line;
    g_mm_ctx._rel   = V2f32(0,0);
    g_mm_ctx._range = get_pos_range_from_line_range(app, buffer, Ii64(1));
    g_mm_ctx.cl_nest = cl_nest;
    g_mm_ctx.line_height = metrics.line_height;

    if (paint_text_color != MM_paint_text_color){
        g_mm_ctx.core_paint_text_color = paint_text_color;
        paint_text_color = MM_paint_text_color;
    }

    Rect_f32 r = rect_union(MM_get_line(get_line_number_from_pos(app, buffer, visible_range.min+0)),
                            MM_get_line(get_line_number_from_pos(app, buffer, visible_range.max-1)));
    Rect_f32 ra = MM_get_line(get_line_number_from_pos(app, buffer, view_get_cursor_pos(app, view)));
    Rect_f32 rb = MM_get_line(get_line_number_from_pos(app, buffer, view_get_mark_pos(app, view)));
    draw_rectangle_fcolor(app, outer, 0, fcolor_change_alpha(fcolor_blend(fcolor_id(defcolor_back), 0.4f, fcolor_id(defcolor_highlight_cursor_line)), 0.9f));
    draw_rectangle_fcolor(app, r,     0, fcolor_change_alpha(fcolor_id(defcolor_highlight_cursor_line), 0.9f));
    draw_rectangle_fcolor(app, ra,    0, fcolor_change_alpha(fcolor_id(defcolor_cursor), 0.2f));
    draw_rectangle_fcolor(app, rb,    0, fcolor_change_alpha(fcolor_id(defcolor_mark), 0.2f));

    if (def_get_config_b32(vars_save_string_lit("minimap_render_nests"))){
        code_index_lock();
        Code_Index_File *file = code_index_get_file(buffer);
        if (file){
            Code_Index_Nest *last = MM_last_nest(NULL, &file->nest_array, g_mm_ctx.range);
            MM_nest_list(arena, &file->nest_array, last, g_mm_ctx.range);
        }
        code_index_unlock();
    }

    while (g_mm_ctx.head && !range_contains(g_mm_ctx.range, g_mm_ctx.head->p0)){ MM_nest_open(app); }

    if (def_get_config_b32(vars_save_string_lit("minimap_scrollable"))){
        Mouse_State m = get_mouse_state(app);
        if (g_mm_ctx.hot_view == 0 && rect_contains_point(outer, V2f32(m.p)) && m.press_l){
            g_mm_ctx.hot_view  = view;
            g_mm_ctx.hot_rect  = outer;
            g_mm_ctx.hot_delta = rect_contains_point(r, V2f32(m.p)) ? f32(m.p.y) - rect_center(r).y : 0.f;
        }

        if (g_mm_ctx.hot_view == view){
            if (m.l == false){ g_mm_ctx.hot_view = 0; }
            f32 dy = 0.5f*minimap_line_height*view_line_count;
            f32 t = unlerp(g_mm_ctx.region.y0 + dy, f32(m.p.y) - g_mm_ctx.hot_delta, g_mm_ctx.region.y1 - dy);
            i64 line = i64(clamp(0,t,1)*y1 / metrics.line_height);
            scroll.target.line_number = Max(1, line);
            scroll.target.pixel_shift = V2f32(0,0);
            scroll.position = scroll.target;
            view_set_buffer_scroll(app, view, scroll, SetBufferScroll_SnapCursorIntoView);
            if (0.f <= t && t <= 1.f){
                view_set_cursor_and_preferred_x(app, view, seek_line_col(line + i64(view_line_count)/2, 0));
            }
        }
    }

    return g_mm_ctx.layout;
}

function void MM_end(Application_Links *app, Text_Layout_ID minimap_id){
    while (g_mm_ctx.stack){ MM_nest_close(app); }
    if (g_mm_ctx.layout){ text_layout_free(app, g_mm_ctx.layout); }
    paint_text_color = g_mm_ctx.core_paint_text_color;
}

function b32 MM_will_handle(Application_Links *app){
    Mouse_State m = get_mouse_state(app);
    return (g_mm_ctx.hot_view == get_this_ctx_view(app, Access_Always) || 
            g_mm_ctx.hot_view == 0 && rect_contains_point(g_mm_ctx.hot_rect, V2f32(m.p)));
}