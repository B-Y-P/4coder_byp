
global Color_Table target_color_table = {};
global Color_Table cached_color_table = {};

// TODO(BYP): Check for way to get highest color sub_id instead of hard-coding 14
function Color_Table
byp_init_color_table(Application_Links *app){
	Color_Table result = make_color_table(app, &global_permanent_arena);
	foreach(i, result.count){
		result.arrays[i].vals = push_array(&global_permanent_arena, u32, 14);
		result.arrays[i].count = 14;
	}
	return result;
}

function void
byp_copy_color_table(Color_Table *dst, Color_Table table){
	foreach(i, table.count){
		Color_Array *nxt_array = &dst->arrays[i];
		Color_Array *array = &table.arrays[i];
		foreach(j, array->count){
			nxt_array->vals[j] = array->vals[j];
		}
	}
}

function void
byp_tick_colors(Application_Links *app, Frame_Info frame_info){
	// If another function sets active, correct this to not write into a loaded theme
	if(active_color_table.arrays != cached_color_table.arrays){
		if(active_color_table.arrays && active_color_table.arrays->vals && active_color_table.arrays->count > 0){
			byp_copy_color_table(&target_color_table, active_color_table);
			active_color_table = cached_color_table;
		}
	}
	b32 needs_animate = false;
	foreach(i, active_color_table.count){
		Color_Array *nxt_array = &target_color_table.arrays[i];
		Color_Array *cur_array = &active_color_table.arrays[i];
		foreach(j, cur_array->count){
			Vec4_f32 nxt_v = unpack_color(nxt_array->vals[j]);
			Vec4_f32 cur_v = unpack_color(cur_array->vals[j]);
			Vec4_f32 diff = nxt_v - cur_v;
			cur_v += diff*frame_info.animation_dt*10.f;
			cur_array->vals[j] = pack_color(cur_v);

			needs_animate |= (!near_zero(diff, 0.001f));
		}
	}
	if(needs_animate){ animate_in_n_milliseconds(app, 0); }
}

function void
byp_theme_navigate(Application_Links *app, View_ID view, Lister *lister, i32 index_delta){
	lister__navigate__default(app, view, lister, index_delta);
	Color_Table *result = (Color_Table *)lister_get_user_data(lister, lister->raw_item_index);
	if(result){ byp_copy_color_table(&target_color_table, *result); }
}


CUSTOM_UI_COMMAND_SIG(byp_theme_lister)
CUSTOM_DOC("Opens an interactive list of all registered themes.")
{
	Color_Table_List *color_table_list = &global_theme_list;

	Scratch_Block scratch(app);
	Lister_Block lister(app, scratch);
	vim_lister_set_default_handlers(lister);
	vim_reset_bottom_text();
	string_append(&vim_bot_text, string_u8_litexpr("Theme:"));

	lister_add_item(lister, string_u8_litexpr("4coder"), string_u8_litexpr(""),
					(void*)&default_color_table, 0);

	if(color_table_list->first){
		byp_copy_color_table(&target_color_table, color_table_list->first->table);
	}
	for(Color_Table_Node *node = color_table_list->first; node; node = node->next){
		lister_add_item(lister, node->name, string_u8_litexpr(""), (void*)&node->table, 0);
	}

	lister.lister.current->handlers.navigate = byp_theme_navigate;
	Lister_Result l_result = vim_run_lister(app, lister);

	Color_Table *result = 0;
	if(!l_result.canceled){ result = (Color_Table*)l_result.user_data; }

	if(result != 0){ byp_copy_color_table(&target_color_table, *result); }
}
