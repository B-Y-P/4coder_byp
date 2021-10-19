
struct Game_State{
   b32 buttonA;
   b32 buttonD;
   b32 buttonSpace;
   i32 Frame;
   Vec2_f32 Pos;
   Vec2_f32 Dim;
   Vec2_f32 b_p;
   Vec2_f32 b_v;
   Vec2_f32 trail[10];
   i32 trail_index;
   void render_game_byp(Application_Links*, Frame_Info, Face_ID);
   void bounce_rect(f32 r, f32 step, Rect_f32 rect);
};

global b32 byp_game_is_running;
global Game_State byp_game_state;

function f32 bounce_angle_out(){ return pi_f32*0.8f*(pcg_rand() / f32(max_u32)) + pi_f32*0.1f; }
function Vec2_f32 bounce_u(){ f32 a = bounce_angle_out(); return {cosf(a), -sinf(a)}; }
function Vec2_f32 bounce_d(){ f32 a = bounce_angle_out(); return {cosf(a),  sinf(a)}; }
function Vec2_f32 bounce_l(){ f32 a = bounce_angle_out(); return {-sinf(a), cosf(a)}; }
function Vec2_f32 bounce_r(){ f32 a = bounce_angle_out(); return { sinf(a), cosf(a)}; }


function void normalize(Vec2_f32 &v){
	if(v.x != 0.f && v.y != 0.f){ v *= 1.f/sqrtf(v.x*v.x + v.y*v.y); }
}

void Game_State::bounce_rect(f32 r, f32 step, Rect_f32 rect){
	rect = rect_inner(rect, -r);
	Vec2_f32 new_p = b_p;
	new_p.x += b_v.x*step;
	if(rect_contains_point(rect, new_p)){
		b_v.x *= -1.0f;
		//b_v = (b_v.x < 0.0f ? bounce_r() : bounce_l());
	}
	new_p = b_p;
	new_p.y += b_v.y*step;
	if(rect_contains_point(rect, new_p)){
		b_v.y *= -1.0f;
		//b_v = (b_v.y < 0.0f ? bounce_d() : bounce_u());
		if(buttonA){ b_v.x = -fabsf(b_v.x); }
		if(buttonD){ b_v.x =  fabsf(b_v.x); }
	}

}

void Game_State::render_game_byp(Application_Links *app, Frame_Info frame_info, Face_ID face_id){
	Rect_f32 global_rect = global_get_screen_rectangle(app);
	Rect_f32 prev_rect = draw_set_clip(app, global_rect);
	f32 max_x = global_rect.x1;
	f32 max_y = global_rect.y1;

	f32 delta = frame_info.animation_dt;

	if(buttonA){ Pos.x -= delta*625.0f; }
	if(buttonD){ Pos.x += delta*625.0f; }
	Pos.x = clamp(global_rect.x0, Pos.x, global_rect.x1-Dim.x);

	f32 b_r = 20.0f;
	f32 scale = delta*880.0f;
	Vec2_f32 new_p = b_p + scale*b_v;

	// Bounce screen edge
#if 0
	if(0){}
	else if(new_p.x - b_r <  0.0f){ b_v = bounce_r(); }
	else if(new_p.x + b_r > max_x){ b_v = bounce_l(); }
	if(0){}
	else if(new_p.y - b_r <  0.0f){ b_v = bounce_d(); }
	else if(new_p.y + b_r > max_y){ b_v = bounce_u(); }
#else
	if(new_p.x - b_r <       0.0f){ b_v = (b_v.x*=-1.0f, lerp(b_v, 0.3f, bounce_r())); }
	else if(new_p.x + b_r > max_x){ b_v = (b_v.x*=-1.0f, lerp(b_v, 0.3f, bounce_l())); }
	if(new_p.y - b_r <       0.0f){ b_v = (b_v.y*=-1.0f, lerp(b_v, 0.3f, bounce_d())); }
	else if(new_p.y + b_r > max_y){ byp_game_is_running=false; }
#endif

	// Bounce paddle
	bounce_rect(b_r, scale, Rf32_xy_wh(Pos, Dim));

	// Bounce block region
	// for(){ bounce_rect(); }

	normalize(b_v);

	b_p = b_p + scale*b_v;

	const i32 count = ArrayCount(trail);
	for(i32 i=0; i<count; i++){
		i32 index = trail_index-i + count;
		f32 percent = (index/f32(trail_index+count));
		FColor color = fcolor_change_alpha(f_white, 0.5f*percent);
		draw_circle_fcolor(app, trail[index%count], b_r*(percent*percent), color);
	}

	draw_rectangle(app, Rf32_xy_wh(Pos+V2f32(5.0f, 5.0f), Dim), 0.f, 0xAA000000);
	draw_rectangle_fcolor(app, Rf32_xy_wh(Pos, Dim), 0.f, f_red);
	if(buttonSpace){
		draw_circle_fcolor(app, b_p, b_r, f_blue);
	}else{
		draw_circle_gradient(app, b_p, b_r, fcolor_resolve(f_pink), 0xFF8E2DE2);
	}


	trail_index = (trail_index+1) % count;
	trail[trail_index] = b_p;

	draw_set_clip(app, prev_rect);
}

// change from command sig once implemented in lister
CUSTOM_COMMAND_SIG(byp_start_game_loop)
CUSTOM_DOC("Starts game loop")
{
	View_ID view = get_this_ctx_view(app, Access_Always);
	View_Context ctx = view_current_context(app, view);
	ctx.hides_buffer = true;
	View_Context_Block ctx_block(app, view, &ctx);


	Rect_f32 global_rect = global_get_screen_rectangle(app);

	byp_game_is_running = 1;
	byp_game_state = {};
	byp_game_state.b_p = {rect_center(global_rect).x, global_rect.y0+20.0f};
	byp_game_state.b_v = bounce_d();
	byp_game_state.Dim = {400.0f, 30.0f};
	byp_game_state.Pos.x = byp_game_state.b_p.x;
	byp_game_state.Pos.y = global_rect.y1 -= 90.0f;

	for(i32 i=0; i<ArrayCount(byp_game_state.trail); i++){
		byp_game_state.trail[i] = byp_game_state.b_p;
	}

	while(byp_game_is_running){
		byp_game_state.Frame++;
		animate_in_n_milliseconds(app, 0);

		User_Input in = get_next_input(app, EventPropertyGroup_Any, EventProperty_Escape);
		if(in.abort){ break; }

		switch(in.event.kind){
			case InputEventKind_KeyRelease:
			case InputEventKind_KeyStroke:{
				b32 IsDown = in.event.kind == InputEventKind_KeyStroke;
				switch(in.event.key.code){
					case KeyCode_A:{ byp_game_state.buttonA = IsDown; } break;
					case KeyCode_D:{ byp_game_state.buttonD = IsDown; } break;
					case KeyCode_Space:{ byp_game_state.buttonSpace = IsDown; } break;
				} break;
			}

			default:{}
		}
	}
	byp_game_is_running = 0;
	vim_reset_bottom_text();
}
