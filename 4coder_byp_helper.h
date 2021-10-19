

#define string_starts_with(src, str) block_match(src, str, sizeof(str)-1)

function b32
string_ends_with(String_Const_u8 string, String_Const_u8 pred){
	return block_match(string.str+string.size-pred.size, pred.str, pred.size);
}

function void
draw_circle_outline(Application_Links *app, Vec2_f32 center, f32 radius, ARGB_Color color){
	Rect_f32 rect = Rect_f32{center.x-radius, center.y-radius, center.x+radius, center.y+radius};
	draw_rectangle(app, rect, radius, color);
}

function void
draw_circle(Application_Links *app, Vec2_f32 center, f32 radius, ARGB_Color color){
	Rect_f32 rect = Rect_f32{center.x-radius, center.y-radius, center.x+radius, center.y+radius};
	draw_rectangle(app, rect, radius, color);
}

function void
draw_circle_fcolor(Application_Links *app, Vec2_f32 center, f32 radius, FColor color){
	Rect_f32 rect = Rect_f32{center.x-radius, center.y-radius, center.x+radius, center.y+radius};
	draw_rectangle_fcolor(app, rect, radius, color);
}

function void
draw_circle_outline_fcolor(Application_Links *app, Vec2_f32 center, f32 radius, f32 thickness, FColor color){
	Rect_f32 rect = Rect_f32{center.x-radius, center.y-radius, center.x+radius, center.y+radius};
	draw_rectangle_outline_fcolor(app, rect, radius, thickness, color);
}

function void
draw_circle_gradient(Application_Links *app, Vec2_f32 center, f32 radius, ARGB_Color c1, ARGB_Color c2){
	Rect_f32 rect = Rf32(center - radius*V2f32(1,1), center + radius*V2f32(1,1));
	for(f32 y=rect.y0; y<=rect.y1; y+=1.f){
		ARGB_Color color = argb_color_blend(c1, (y-rect.y0)/(rect.y1-rect.y0), c2);
		draw_set_clip(app, Rf32(rect.x0, y, rect.x1, y+1.f));
		draw_rectangle(app, rect, radius, color);
	}
}

function Audio_Clip
load_wav(Arena *arena, char *file_name){
	Audio_Clip result = {};
	FILE *file = def_search_normal_fopen(arena, file_name, "rb");
	if(file != 0){
		result = audio_clip_from_wav_FILE(&global_permanent_arena, file);
		fclose(file);
	}
	return result;
}


#ifndef PCG_SEED
#define PCG_SEED time(NULL)
#endif
#ifndef PCG_INC
#define PCG_INC 1
#endif

function u32 pcg_rand(){
	static u64 state = PCG_SEED;
	state = state * 6364136223846793005ULL + (PCG_INC|1);
#ifdef PCG_SPEED
	return (state ^ (state >> 22)) >> (22 + (state >> 61));
#else
	u64 xsh = (state ^ (state >> 18)) >> 27;
	i32 r_bits = i32(state >> 59);
	return u32((xsh >> r_bits) | (xsh << ((-r_bits) & 31)));
#endif
}

function f32 rand01(){ return pcg_rand()/f32(max_u32); }

#define rand_array(a) ArraySafe(a, i32(rand01()*ArrayCount(a)))
