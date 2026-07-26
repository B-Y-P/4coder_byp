
CUSTOM_COMMAND_SIG(qol_startup)
CUSTOM_DOC("QOL command for responding to a startup event")
{
  ProfileScope(app, "qol startup");
  User_Input input = get_current_input(app);
  if (match_core_code(&input, CoreCode_Startup)){
    String_Const_u8_Array file_names = input.event.core.file_names;
    load_themes_default_folder(app);

    {
      Face_Description description = get_face_description(app, 0);
      i32 override_font_size = description.parameters.pt_size;
      b32 override_hinting   = description.parameters.hinting;

      Scratch_Block scratch(app);
      load_config_and_apply(app, &global_config_arena, override_font_size, override_hinting);

      String_Const_u8 bindings_file_name = string_u8_litexpr("bindings.4coder");
      String_Const_u8 mapping = def_get_config_string(scratch, vars_save_string_lit("mapping"));

      if (string_match(mapping, string_u8_litexpr("mac-default"))){
        bindings_file_name = string_u8_litexpr("mac-bindings.4coder");
      }
      else if (OS_MAC && string_match(mapping, string_u8_litexpr("choose"))){
        bindings_file_name = string_u8_litexpr("mac-bindings.4coder");
      }

      String_ID global_map_id = vars_save_string_lit("keys_global");
      String_ID file_map_id = vars_save_string_lit("keys_file");
      String_ID code_map_id = vars_save_string_lit("keys_code");

      if (dynamic_binding_load_from_file(app, &framework_mapping, bindings_file_name)){
        qol_setup_essential_mapping(&framework_mapping, global_map_id, file_map_id, code_map_id);
      }
      else{
        vim_default_bindings(app, KeyCode_BackwardSlash);
        TAB_setup_default_mapping(&framework_mapping, global_map_id, file_map_id, code_map_id);
        qol_setup_default_mapping(&framework_mapping, global_map_id, file_map_id, code_map_id);
        qol_setup_essential_mapping(&framework_mapping, global_map_id, file_map_id, code_map_id);
      }

      // open command line files
      String_Const_u8 hot_directory = push_hot_directory(app, scratch);
      for (i32 i = 0; i < file_names.count; i += 1){
        Temp_Memory_Block temp(scratch);
        String_Const_u8 input_name = file_names.vals[i];
        String_Const_u8 full_name = push_u8_stringf(scratch, "%S/%S", hot_directory, input_name);
        Buffer_ID new_buffer = create_buffer(app, full_name, BufferCreate_NeverNew|BufferCreate_MustAttachToFile);
        if (new_buffer == 0){
          create_buffer(app, input_name, 0);
        }
      }
    }

    //default_4coder_initialize(app, file_names);
    default_4coder_side_by_side_panels(app, file_names);

    if (def_get_config_b32(vars_save_string_lit("automatically_load_project"))){
      load_project(app);
    }

    qol_temp_buffer = create_buffer(app, string_u8_litexpr("*qol_temp*"),
                                    BufferCreate_Background | BufferCreate_AlwaysNew | BufferCreate_NeverAttachToFile);
    buffer_set_setting(app, qol_temp_buffer, BufferSetting_Unimportant, true);
    buffer_set_setting(app, qol_temp_buffer, BufferSetting_Unkillable, true);
    buffer_set_setting(app, qol_temp_buffer, BufferSetting_ReadOnly, false);

    qol_snippet_init(app);
    TAB_startup_inner(app);
    qol_bview_init(app);
  }

  {
    def_audio_init();
  }

  {
    def_enable_virtual_whitespace = def_get_config_b32(vars_save_string_lit("enable_virtual_whitespace"));
    clear_all_layouts(app);
  }

  Face_Description desc = get_global_face_description(app);
  desc.parameters.pt_size -= 4;
  qol_small_face = try_create_new_face(app, &desc);

  String_Const_u8 non_word_chars = string_u8_litexpr(" \t\n/\\()\"':,.;<>~!@#$%^&*|+=[]{}`?-_");
  character_predicate_non_word = character_predicate_from_chars(non_word_chars);
  character_predicate_word     = character_predicate_not(&character_predicate_non_word);

  Scratch_Block scratch(app);
  set_active_color(get_color_table_by_name(def_get_config_string(scratch, vars_save_string_lit("default_theme_name"))));
  qol_cur_colors = qol_color_table_init(app);
  qol_nxt_colors = qol_color_table_init(app);
  qol_color_table_copy(qol_cur_colors, active_color_table);
  qol_color_table_copy(qol_nxt_colors, active_color_table);
}

function void
qol_tick(Application_Links *app, Frame_Info frame_info){
  default_tick(app, frame_info);

  f32 dt = frame_info.animation_dt;

  vim_tick(app, frame_info);

  qol_interp(qol_cur_cursor_pos, qol_nxt_cursor_pos, dt, 1e-14f);
  if (!near_zero(qol_cur_cursor_pos - qol_nxt_cursor_pos, 0.5f)){
    animate_in_n_milliseconds(app, 0);
  }

  b32 anchor = def_get_config_b32(vars_save_string_lit("anchor_pproc"));
  if (anchor != g_anchor_pproc){
    g_anchor_pproc = anchor;
    clear_all_layouts(app);
  }

  if (qol_try_exit_view != 0){
    view_set_active(app, qol_try_exit_view);
  }

  qol_tick_colors(app, frame_info);

  qol_tick_bview(app, frame_info);

  MC_tick_inner(app, frame_info);
  TAB_tick_inner(app, frame_info);
}

BUFFER_HOOK_SIG(qol_file_save){
  //default_file_save(app, buffer_id);
  if (def_get_config_b32(vars_save_string_lit("automatically_indent_text_on_save"))){
    qol_reformat_buffer(app, buffer_id);
  }

  vim_file_save(app, buffer_id);

  Scratch_Block scratch(app);
  String_Const_u8 path = push_buffer_file_name(app, scratch, buffer_id);
  String_Const_u8 name = string_front_of_path(path);

  if (qol_is_theme_file(name)){
    Color_Table color_table = make_color_table(app, &global_theme_arena);
    Config *config = theme_parse__buffer(app, scratch, buffer_id, &global_theme_arena, &color_table);
    String_Const_u8 error_text = config_stringize_errors(app, scratch, config);
    comp_error(app, error_text);

    if (error_text.size > 0){
      print_message(app, error_text);
    }
    else{
      qol_color_table_copy(qol_nxt_colors, color_table);
    }
  }

  if (string_match(name, string_u8_litexpr("config.4coder"))){
    View_ID view = get_active_view(app, Access_Always);
    view_enqueue_command_function(app, view, qol_reload_config);
  }

  if (string_match(name, string_u8_litexpr("project.4coder"))){
    View_ID view = get_active_view(app, Access_Always);
    view_enqueue_command_function(app, view, qol_reload_project);
  }

  if (string_match(name, string_u8_litexpr("bindings.4coder"))){
    View_ID view = get_active_view(app, Access_Always);
    view_enqueue_command_function(app, view, qol_reload_bindings);
  }

  return 0;
}

CUSTOM_COMMAND_SIG(qol_view_input_handler)
CUSTOM_DOC("QOL Input consumption loop for views")
{
  Scratch_Block scratch(app);
  default_input_handler_init(app, scratch);

  View_ID view = get_this_ctx_view(app, Access_Always);
  Managed_Scope scope = view_get_managed_scope(app, view);

  for (;;){
    // NOTE(allen): Get input
    User_Input input = get_next_input(app, EventPropertyGroup_Any, 0);
    if (input.abort){
      break;
    }

    ProfileScopeNamed(app, "before view input", view_input_profile);

    // NOTE(allen): Mouse Suppression
    Event_Property event_properties = get_event_properties(&input.event);
    if (suppressing_mouse && (event_properties & EventPropertyGroup_AnyMouseEvent) != 0){
      continue;
    }

    // NOTE(allen): Get binding
    if (implicit_map_function == 0){
      implicit_map_function = default_implicit_map;
    }
    Implicit_Map_Result map_result = implicit_map_function(app, 0, 0, &input.event);
    vim_pre_command(key_from_event(&input.event), map_result);

    if (map_result.command == 0){
      leave_current_input_unhandled(app);
      continue;
    }

    // NOTE(allen): Run the command and pre/post command stuff
    qol_pre_command(app, scope);
    ProfileCloseNow(view_input_profile);
    map_result.command(app);
    ProfileScope(app, "after view input");
    vim_post_command();
    qol_post_command(app, scope);
  }
}

function i32 qol_buffer_edit_range(Application_Links *app, Buffer_ID buffer_id, Range_i64 new_range, Range_Cursor old_cursor_range){
  i64 pos = qol_col_cursor.pos;
  if (pos >= 0 && qol_col_buffer == buffer_id){
    Range_i64 old_range = Ii64(old_cursor_range.min.pos, old_cursor_range.max.pos);
    i64 insert_size = range_size(new_range);
    index_shift(&pos, old_range, insert_size);
    qol_col_cursor = buffer_compute_cursor(app, buffer_id, seek_pos(pos));
  }

  vim_buffer_edit_range_inner(app, buffer_id, new_range, old_cursor_range);

  return MC_buffer_edit_range(app, buffer_id, new_range, old_cursor_range);
}
