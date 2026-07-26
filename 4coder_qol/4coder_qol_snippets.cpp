
global Buffer_ID qol_snippet_buffer;

function void
qol_snippet_init(Application_Links *app){
  qol_snippet_buffer = create_buffer(app, string_u8_litexpr("*qol_snippet*"),
                                     BufferCreate_Background | BufferCreate_AlwaysNew | BufferCreate_NeverAttachToFile);
  buffer_set_setting(app, qol_snippet_buffer, BufferSetting_Unimportant, true);
  buffer_set_setting(app, qol_snippet_buffer, BufferSetting_Unkillable, true);
  buffer_set_setting(app, qol_snippet_buffer, BufferSetting_ReadOnly, false);
}

CUSTOM_COMMAND_SIG(qol_snippet_begin)
CUSTOM_DOC("[QOL] Opens *qol_snippet* empty buffer")
{
  View_ID view = open_view(app, get_next_view_after_active(app, Access_Always), ViewSplit_Bottom);
  view_set_split_proportion(app, view, 0.2f);
  view_set_active(app, view);
  view_set_buffer(app, view, qol_snippet_buffer, 0);
  buffer_replace_range(app, qol_snippet_buffer, buffer_range(app, qol_snippet_buffer), string_u8_empty);
}

CUSTOM_COMMAND_SIG(qol_snippet_end)
CUSTOM_DOC("[QOL] ")
{
  Scratch_Block scratch(app);
  View_ID view = get_first_view_with_buffer(app, qol_snippet_buffer);
  if (view == 0){ return; }

  Buffer_ID buffer = qol_snippet_buffer;
  Range_i64 range = buffer_range(app, qol_snippet_buffer);
  String_Const_u8 str = push_buffer_range(app, scratch, buffer, range);
  String_Const_u8 name = string_u8_empty;
  String_Const_u8 text = string_u8_empty;
  i64 cursor = view_get_cursor_pos(app, view);
  i64 mark = view_get_mark_pos(app, view);

  // Snippet Name
  {
    Query_Bar_Group group(app);
    u8 string_space[256];
    Query_Bar bar = {};
    bar.prompt = string_u8_litexpr("Snippet Name: ");
    bar.string = SCu8(string_space, (u64)0);
    bar.string_capacity = sizeof(string_space);
    if (!query_user_string(app, &bar)){ return; }
    name = bar.string;
  }

  // Snippet Text
  {
    Character_Predicate pred_esc = character_predicate_from_chars(string_u8_litexpr("\"\\\n\t"));
    String_Const_u8 quote     = string_u8_litexpr("\\\"");
    String_Const_u8 tab       = string_u8_litexpr("\\t");
    String_Const_u8 newline   = string_u8_litexpr("\\n");
    String_Const_u8 backslash = string_u8_litexpr("\\\\");

    String8List list = {};
    for (i64 pos = 0;;){
      String_Match match = buffer_seek_character_class(app, buffer, &pred_esc, Scan_Forward, pos-1);
      if (match.buffer == 0){
        string_list_push(scratch, &list, string_substring(str, Ii64(pos, range.end)));
        break;
      }

      String_Const_u8 esc_str = string_u8_empty;
      switch (buffer_get_char(app, buffer, match.range.min)){
        case  '"':{ esc_str = quote;     } break;
        case '\t':{ esc_str = tab;       } break;
        case '\n':{ esc_str = newline;   } break;
        case '\\':{ esc_str = backslash; } break;
      }
      string_list_push(scratch, &list, string_substring(str, Ii64(pos, match.range.min)));
      string_list_push(scratch, &list, esc_str);
      pos = match.range.max;
      if (!range_contains(range, pos)){ break; }
    }
    text = string_list_flatten(scratch, list);
  }

  String_Const_u8 snippet = push_stringf(scratch, "\t{\"%S\", \"%S\", %lld, %lld},\n", name, text, cursor, mark);
  String_Const_u8 full_path = push_stringf(scratch, "%Ssnippets.4coder", system_get_path(scratch, SystemPath_Binary));
  Buffer_ID buffer_snippets = 0;
  if (!open_file(app, &buffer_snippets, full_path, 0, 0)){ return; }

  b32 did_write = 0;
  Range_i64 snippet_range = buffer_range(app, buffer_snippets);
  String_Const_u8 str_snippets = push_buffer_range(app, scratch, buffer_snippets, snippet_range);

  Config *parsed = def_config_from_text(app, scratch, full_path, str_snippets);
  if (parsed != 0){
    for (Config_Assignment *node = parsed->first; node != 0; node = node->next){
      if (string_match(node->l->identifier, SCu8("snippets")) && node->r->type == ConfigRValueType_Compound){
        i64 pos = node->r->range.end-1;
        buffer_replace_range(app, buffer_snippets, Ii64(pos), snippet);
        buffer_save(app, buffer_snippets, full_path, 0);
        did_write = true;
        break;
      }
    }
  }

  if (!did_write){
    String_Const_u8 string = push_stringf(scratch, "snippets = {\n%S};", snippet);
    Range_i64 replace = (parsed != 0 ? Ii64(0,0) : snippet_range);
    buffer_replace_range(app, buffer_snippets, replace, string);
    buffer_save(app, buffer_snippets, full_path, 0);
  }
  view_enqueue_command_function(app, view, close_panel);
}
