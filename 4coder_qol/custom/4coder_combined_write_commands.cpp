/*
4coder_combined_write_commands.cpp - Commands for writing text specialized for particular contexts.
*/

// TOP

function void
write_string(Application_Links *app, View_ID view, Buffer_ID buffer, String_Const_u8 string){
  i64 pos = view_get_cursor_pos(app, view);
  buffer_replace_range(app, buffer, Ii64(pos), string);
  view_set_cursor_and_preferred_x(app, view, seek_pos(pos + string.size));
}

function void
write_string(Application_Links *app, String_Const_u8 string){
  View_ID view = get_active_view(app, Access_ReadWriteVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
  write_string(app, view, buffer, string);
}

function void
write_named_comment_string(Application_Links *app, char *type_string){
  Scratch_Block scratch(app);
  String_Const_u8 name = def_get_config_string(scratch, vars_save_string_lit("user_name"));
  String_Const_u8 str = {};
  if (name.size > 0){
    str = push_u8_stringf(scratch, "// %s(%S): ", type_string, name);
  }
  else{
    str = push_u8_stringf(scratch, "// %s: ", type_string);
  }
  write_string(app, str);
}

function void
long_braces(Application_Links *app, char *text, i32 size){
  View_ID view = get_active_view(app, Access_ReadWriteVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
  i64 pos = view_get_cursor_pos(app, view);
  buffer_replace_range(app, buffer, Ii64(pos), SCu8(text, size));
  view_set_cursor_and_preferred_x(app, view, seek_pos(pos + 2));
  auto_indent_buffer(app, buffer, Ii64_size(pos, size));
  move_past_lead_whitespace(app, view, buffer);
}

CUSTOM_COMMAND_MC_CURSOR_SIG(open_long_braces)
CUSTOM_DOC("At the cursor, insert a '{' and '}' separated by a blank line.")
{
  char text[] = "{\n\n}";
  i32 size = sizeof(text) - 1;
  long_braces(app, text, size);
}

CUSTOM_COMMAND_MC_CURSOR_SIG(open_long_braces_semicolon)
CUSTOM_DOC("At the cursor, insert a '{' and '};' separated by a blank line.")
{
  char text[] = "{\n\n};";
  i32 size = sizeof(text) - 1;
  long_braces(app, text, size);
}

CUSTOM_COMMAND_MC_CURSOR_SIG(open_long_braces_break)
CUSTOM_DOC("At the cursor, insert a '{' and '}break;' separated by a blank line.")
{
  char text[] = "{\n\n}break;";
  i32 size = sizeof(text) - 1;
  long_braces(app, text, size);
}

CUSTOM_COMMAND_MC_CURSOR_SIG(if0_off)
CUSTOM_DOC("Surround the range between the cursor and mark with an '#if 0' and an '#endif'")
{
  place_begin_and_end_on_own_lines(app, "#if 0", "#endif");
}

CUSTOM_COMMAND_MC_CURSOR_SIG(write_todo)
CUSTOM_DOC("At the cursor, insert a '// TODO' comment, includes user name if it was specified in config.4coder.")
{
  write_named_comment_string(app, "TODO");
}

CUSTOM_COMMAND_MC_CURSOR_SIG(write_hack)
CUSTOM_DOC("At the cursor, insert a '// HACK' comment, includes user name if it was specified in config.4coder.")
{
  write_named_comment_string(app, "HACK");
}

CUSTOM_COMMAND_MC_CURSOR_SIG(write_note)
CUSTOM_DOC("At the cursor, insert a '// NOTE' comment, includes user name if it was specified in config.4coder.")
{
  write_named_comment_string(app, "NOTE");
}

CUSTOM_COMMAND_MC_CURSOR_SIG(write_block)
CUSTOM_DOC("At the cursor, insert a block comment.")
{
  place_begin_and_end_on_own_lines(app, "/* ", " */");
}

CUSTOM_COMMAND_MC_CURSOR_SIG(write_zero_struct)
CUSTOM_DOC("At the cursor, insert a ' = {};'.")
{
  write_string(app, string_u8_litexpr(" = {};"));
}

function i64
get_start_of_line_at_cursor(Application_Links *app, View_ID view, Buffer_ID buffer){
  i64 pos = view_get_cursor_pos(app, view);
  i64 line = get_line_number_from_pos(app, buffer, pos);
  return(get_pos_past_lead_whitespace_from_line_number(app, buffer, line));
}

function b32
c_line_comment_starts_at_position(Application_Links *app, Buffer_ID buffer, i64 pos){
  b32 alread_has_comment = false;
  u8 check_buffer[2];
  if (buffer_read_range(app, buffer, Ii64(pos, pos + 2), check_buffer)){
    if (check_buffer[0] == '/' && check_buffer[1] == '/'){
      alread_has_comment = true;
    }
  }
  return(alread_has_comment);
}

CUSTOM_COMMAND_MC_CURSOR_SIG(comment_line)
CUSTOM_DOC("Insert '//' at the beginning of the line after leading whitespace.")
{
  View_ID view = get_active_view(app, Access_ReadWriteVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
  i64 pos = get_start_of_line_at_cursor(app, view, buffer);
  b32 alread_has_comment = c_line_comment_starts_at_position(app, buffer, pos);
  if (!alread_has_comment){
    buffer_replace_range(app, buffer, Ii64(pos), string_u8_litexpr("//"));
  }
}

CUSTOM_COMMAND_MC_CURSOR_SIG(uncomment_line)
CUSTOM_DOC("If present, delete '//' at the beginning of the line after leading whitespace.")
{
  View_ID view = get_active_view(app, Access_ReadWriteVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
  i64 pos = get_start_of_line_at_cursor(app, view, buffer);
  b32 alread_has_comment = c_line_comment_starts_at_position(app, buffer, pos);
  if (alread_has_comment){
    buffer_replace_range(app, buffer, Ii64(pos, pos + 2), string_u8_empty);
  }
}

CUSTOM_COMMAND_MC_CURSOR_SIG(comment_line_toggle)
CUSTOM_DOC("Turns uncommented lines into commented lines and vice versa for comments starting with '//'.")
{
  View_ID view = get_active_view(app, Access_ReadWriteVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
  i64 pos = get_start_of_line_at_cursor(app, view, buffer);
  b32 alread_has_comment = c_line_comment_starts_at_position(app, buffer, pos);
  if (alread_has_comment){
    buffer_replace_range(app, buffer, Ii64(pos, pos + 2), string_u8_empty);
  }
  else{
    buffer_replace_range(app, buffer, Ii64(pos), string_u8_litexpr("//"));
  }
}

////////////////////////////////

static Snippet default_snippets[] = {
  // general (for Allen's style)
  {"if",     "if (){\n\n}\n", 4, 7},
  {"ifelse", "if (){\n\n}\nelse{\n\n}", 4, 7},
  {"forn",   "for (node = ;\nnode != 0;\nnode = node->next){\n\n}\n", 5, 38},
  {"fori",   "for (i = 0; i < ; i += 1){\n\n}\n", 5, 16},
  {"forj",   "for (j = 0; j < ; j += 1){\n\n}\n", 5, 16},
  {"fork",   "for (k = 0; k < ; k += 1){\n\n}\n", 5, 16},
  {"for",    "for (;;){\n\n}\n", 5, 10},
  {"///",    "////////////////////////////////", 32, 32},
  {"#guard", "#if !defined(Z)\n#define Z\n#endif\n", 0, 26},

  {"op+",  "Z\noperator+(Z a, Z b){\n,\n}\n", 0, 23},
  {"op-",  "Z\noperator-(Z a, Z b){\n,\n}\n", 0, 23},
  {"op*",  "Z\noperator*(Z a, Z b){\n,\n}\n", 0, 23},
  {"op/",  "Z\noperator/(Z a, Z b){\n,\n}\n", 0, 23},
  {"op+=", "Z&\noperator+=(Z &a, Z b){\n,\n}\n", 0, 26},
  {"op-=", "Z&\noperator-=(Z &a, Z b){\n,\n}\n", 0, 26},
  {"op*=", "Z&\noperator*=(Z &a, Z b){\n,\n}\n", 0, 26},
  {"op/=", "Z&\noperator/=(Z &a, Z b){\n,\n}\n", 0, 26},

  // for 4coder development
  {"4command", "CUSTOM_COMMAND_SIG()\nCUSTOM_DOC()\n{\n\n}\n", 19, 32},
  {"4app", "Application_Links *app", 22, 22},

#if defined(SNIPPET_EXPANSION)
#include SNIPPET_EXPANSION
#endif
};

function void
write_snippet(Application_Links *app, View_ID view, Buffer_ID buffer,
              i64 pos, Snippet *snippet){
  if (snippet != 0){
    String_Const_u8 snippet_text = SCu8(snippet->text);
    buffer_replace_range(app, buffer, Ii64(pos), snippet_text);
    i64 new_cursor = pos + snippet->cursor_offset;
    view_set_cursor_and_preferred_x(app, view, seek_pos(new_cursor));
    i64 new_mark = pos + snippet->mark_offset;
    view_set_mark(app, view, seek_pos(new_mark));
    auto_indent_buffer(app, buffer, Ii64_size(pos, snippet_text.size));
  }
}

function b32
lister_fill_snippets_file(Application_Links *app, Lister *lister, Arena *arena){
  String8List list = {};
  def_search_list_add_system_path(arena, &list, SystemPath_Binary);
  String_Const_u8 path = def_search_get_full_path(arena, &list, string_u8_litexpr("snippets.4coder"));
  File_Name_Data file = dump_file(arena, path);
  Config *parsed = def_config_from_text(app, arena, path, file.data);
  if (parsed == 0){ return false; }

  b32 did_fill = false;
  for (Config_Assignment *node = parsed->first; node != 0; node = node->next){
    if (!string_match(node->l->identifier, SCu8("snippets")) || node->r->type != ConfigRValueType_Compound){
      continue;
    }

    for (Config_Compound_Element *n = node->r->compound->first; n != 0; n = n->next){
      if (n->r->type != ConfigRValueType_Compound){ continue; }
      Snippet s = {};
      Config_Compound_Element *e = n->r->compound->first;
      if (e && e->r->type == ConfigRValueType_String)  { s.name = (char*)e->r->string.str; e=e->next; } else { continue; }
      if (e && e->r->type == ConfigRValueType_String)  { s.text = (char*)e->r->string.str; e=e->next; } else { continue; }
      if (e && e->r->type == ConfigRValueType_Integer) { s.cursor_offset = e->r->integer;  e=e->next; } else { continue; }
      if (e && e->r->type == ConfigRValueType_Integer) { s.mark_offset   = e->r->integer;  e=e->next; } else { continue; }

      lister_add_item(lister, SCu8(s.name), SCu8(s.text), push_array_write(arena, Snippet, 1, &s), 0);
      did_fill = true;
    }
    break;
  }
  return did_fill;
}

global History_Record_Index g_history_idx;

function void
snippet_lister_navigate(Application_Links *app, View_ID view, Lister *lister, i32 delta){
  lister__navigate__default(app, view, lister, delta);

  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
  buffer_history_set_current_state_index(app, buffer, g_history_idx);

  i64 idx = lister->item_index;
  if (!range_contains(Ii64(0, lister->filtered.count), idx)){ return; }
  Snippet *snippet = (Snippet*)lister->filtered.node_ptrs[idx]->user_data;
  if (snippet == 0){ return; }

  String_Const_u8 text = SCu8(snippet->text);
  i64 pos = view_get_cursor_pos(app, view);

  buffer_replace_range(app, buffer, Ii64(pos), text);
  view_set_cursor_and_preferred_x(app, view, seek_pos(pos + snippet->cursor_offset));
  view_set_mark(app, view, seek_pos(pos + snippet->mark_offset));
}

function void
get_snippet_from_user(Application_Links *app, View_ID view, Arena *arena, String_Const_u8 query){
  Lister_Block lister(app, arena);
  lister_set_query(lister, query);
  lister_set_default_handlers(lister);
  lister.lister.current->handlers.navigate = snippet_lister_navigate;

  if (!lister_fill_snippets_file(app, lister, arena)){
    for (i32 i = 0; i < ArrayCount(default_snippets); i += 1){
      Snippet *snippet = &default_snippets[i];
      lister_add_item(lister, SCu8(snippet->name), SCu8(snippet->text), snippet, 0);
    }
  }

  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
  g_history_idx = buffer_history_get_current_state_index(app, buffer);
  Lister_Result l_result = run_lister(app, lister);
  if (l_result.canceled){
    buffer_history_set_current_state_index(app, buffer, g_history_idx);
  }
}

CUSTOM_UI_COMMAND_SIG(snippet_lister)
CUSTOM_DOC("Opens a snippet lister for inserting whole pre-written snippets of text.")
{
  View_ID view = get_this_ctx_view(app, Access_ReadWrite);
  if (view != 0){
    Scratch_Block scratch(app);
    get_snippet_from_user(app, view, scratch, string_u8_litexpr("Snippet:"));
  }
}

// BOTTOM
