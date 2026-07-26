
function bool
qol_is_theme_file(String_Const_u8 name){
  String_Const_u8 target_prefix = string_u8_litexpr("theme-");
  String_Const_u8 target_suffix = string_u8_litexpr(".4coder");
  String_Const_u8 actual_prefix = string_prefix(name, target_prefix.size);
  String_Const_u8 actual_suffix = string_postfix(name, target_suffix.size);
  return string_match(actual_prefix, target_prefix) && string_match(actual_suffix, target_suffix);
}

function Color_Table
qol_color_table_init(Application_Links *app){
  Color_Table table = make_color_table(app, &global_permanent_arena);
  for (i64 i = 0; i < table.count; i += 1){
    table.arrays[i].vals = push_array(&global_permanent_arena, ARGB_Color, 15);
    table.arrays[i].count = 15;
  }
  return table;
}

function void
qol_color_table_copy(Color_Table dst, Color_Table src){
  for (i64 i = 0; i < src.count; i += 1){
    Color_Array *dst_array = &dst.arrays[i];
    Color_Array *src_array = &src.arrays[i];
    dst_array->count = src_array->count;
    block_copy(dst_array->vals, src_array->vals, src_array->count*sizeof(ARGB_Color));
  }
}

function void
qol_tick_colors(Application_Links *app, Frame_Info frame_info){
  b32 needs_tick = false;

  if (active_color_table.arrays != qol_cur_colors.arrays){
    qol_color_table_copy(qol_nxt_colors, active_color_table);
    active_color_table = qol_cur_colors;
    needs_tick = true;
  }

  for (i64 i = 0; i < qol_cur_colors.count; i += 1){
    Color_Array cur_array = qol_cur_colors.arrays[i];
    Color_Array nxt_array = qol_nxt_colors.arrays[i];
    for (i64 j = 0; j < cur_array.count; j += 1){
      Vec4_f32 cur = unpack_color(cur_array.vals[j]);
      Vec4_f32 nxt = unpack_color(nxt_array.vals[j]);
      Vec4_f32 dif = nxt - cur;
      qol_interp(cur, nxt, frame_info.animation_dt, 1e-4f);
      if (near_zero(dif, 0.05f)){
        cur = nxt;
      }
      else{
        needs_tick = true;
      }
      cur_array.vals[j] = pack_color(cur);
    }
  }

  if (needs_tick){
    animate_in_n_milliseconds(app, 0);
  }
}