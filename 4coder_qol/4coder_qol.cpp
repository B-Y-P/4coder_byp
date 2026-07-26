
#include "4coder_default_include.h"

//#define SNIPPET_EXPANSION "path/to/snippet.inc"

//#define AUTO_CENTER_AFTER_JUMPS false

#include "4coder_qol_token.h"
#include "4coder_qol_lister.h"
#include "4coder_qol_bview.h"
#define run_lister qol_run_lister

CUSTOM_ID(colors, defcolor_type);
CUSTOM_ID(colors, defcolor_function);
CUSTOM_ID(colors, defcolor_macro);
CUSTOM_ID(colors, defcolor_enum);
CUSTOM_ID(colors, defcolor_global);
CUSTOM_ID(colors, defcolor_control);
CUSTOM_ID(colors, defcolor_primitive);
CUSTOM_ID(colors, defcolor_struct);
CUSTOM_ID(colors, defcolor_non_text);
CUSTOM_ID(colors, defcolor_operator);

function b32 vim_parse_vim_keys(Application_Links *app, Arena *arena, Config *parsed, Config_Assignment *node);
function b32 MC_filter_command(Custom_Command_Function *func);

// Ideally this could be defined in 4coder_multi_cursor.cpp
#if !defined(META_PASS)
#define CUSTOM_COMMAND_MC_SIG(name, kind)  void name(struct Application_Links *app)
#else
#define CUSTOM_COMMAND_MC_SIG(name, kind)  CUSTOM_COMMAND(name, __FILE__, __LINE__, Normal, kind)
#endif
#define CUSTOM_COMMAND_MC_CURSOR_SIG(name) CUSTOM_COMMAND_MC_SIG(name, 1) // 1 == MC_Command_Cursor
#define CUSTOM_COMMAND_MC_GLOBAL_SIG(name) CUSTOM_COMMAND_MC_SIG(name, 2) // 2 == MC_Command_Global
#define CUSTOM_COMMAND_MC_COPY_SIG(name)   CUSTOM_COMMAND_MC_SIG(name, 3) // 3 == MC_Command_CursorCopy
#define CUSTOM_COMMAND_MC_PASTE_SIG(name)  CUSTOM_COMMAND_MC_SIG(name, 4) // 4 == MC_Command_CursorPaste

#include "languages/qol_parser_helper.h"
#include "4coder_default_include.cpp"
#include "languages/cpp_parser.cpp"

#include "4coder_qol_jumps.cpp"

global b32 qol_opened_brace = false;
global u8 qol_target_char;
global Buffer_Cursor qol_col_cursor = {-1};
global Buffer_ID qol_col_buffer;

global Vec2_f32 qol_cur_cursor_pos;
global Vec2_f32 qol_nxt_cursor_pos;

#include "plugins/4coder_multi_cursor.cpp"
#include "plugins/4coder_tabs.cpp"
#include "plugins/4coder_minimap.cpp"

global Color_Table qol_cur_colors;
global Color_Table qol_nxt_colors;

global Face_ID qol_small_face;

global Buffer_ID qol_temp_buffer;

global View_ID qol_try_exit_view;

global u8 g_qol_bot_buffer[1024];
global String_u8 g_qol_bot_string = Su8(g_qol_bot_buffer, 0, sizeof(g_qol_bot_buffer));

global Character_Predicate character_predicate_word = {};
global Character_Predicate character_predicate_non_word = {};

global View_ID g_qol_lister_view;
global Lister* g_qol_lister;
global Lister_Node* g_qol_mouse_node;

#include "4coder_qol_helper.h"
#include "4coder_qol_block.cpp"

#include "4coder_qol_colors.cpp"
#include "4coder_qol_token.cpp"

#include "../4coder_vim/4coder_vim_include.h"
#include "../4coder_vim/4coder_vim_include.cpp"

#include "4coder_qol_bindings.cpp"
#include "4coder_qol_commands.cpp"
#include "4coder_qol_reformat.cpp"

#include "4coder_qol_isearch.cpp"
#include "4coder_qol_draw.cpp"

#include "4coder_qol_lister.cpp"
#include "4coder_qol_bview.cpp"
#include "4coder_qol_snippets.cpp"

#include "4coder_qol_hooks.cpp"

void custom_layer_init(Application_Links *app){
  Thread_Context *tctx = get_thread_context(app);

  default_framework_init(app);

  vim_init(app);
  MC_init(app);
  implicit_map_function = vim_input_map;

  MC_register(command_lister,   MC_Command_Global);
  MC_register(theme_lister,     MC_Command_Global);
  MC_register(vim_cut,          MC_Command_CursorCopy);
  MC_register(vim_normal_mode,  MC_Command_Global);
  MC_register(vim_paste_before, MC_Command_Cursor);

  // Set up custom layer hooks
  {
    set_custom_hook(app, HookID_BufferViewerUpdate, default_view_adjust);

    set_custom_hook(app, HookID_ViewEventHandler, qol_view_input_handler);
    set_custom_hook(app, HookID_Tick, qol_tick);
    set_custom_hook(app, HookID_RenderCaller, qol_render_caller);
    set_custom_hook(app, HookID_WholeScreenRenderCaller, qol_whole_screen_render_caller);

    set_custom_hook(app, HookID_DeltaRule, fixed_time_cubic_delta);
    set_custom_hook_memory_size(app, HookID_DeltaRule,
                                delta_ctx_size(fixed_time_cubic_delta_memory_size));

    set_custom_hook(app, HookID_BufferNameResolver, default_buffer_name_resolution);

    set_custom_hook(app, HookID_BeginBuffer, vim_begin_buffer);
    set_custom_hook(app, HookID_EndBuffer, end_buffer_close_jump_list);
    set_custom_hook(app, HookID_NewFile, default_new_file);
    set_custom_hook(app, HookID_SaveFile, qol_file_save);
    set_custom_hook(app, HookID_BufferEditRange, qol_buffer_edit_range);
    set_custom_hook(app, HookID_BufferRegion, qol_buffer_region);
    set_custom_hook(app, HookID_ViewChangeBuffer, default_view_change_buffer);

    set_custom_hook(app, HookID_Layout, layout_unwrapped);
  }

  def_set_config_b32(vars_save_string_lit("use_function_tooltip"), true);

  mapping_init(tctx, &framework_mapping);

  String_ID global_map_id = vars_save_string_lit("keys_global");
  String_ID file_map_id = vars_save_string_lit("keys_file");
  String_ID code_map_id = vars_save_string_lit("keys_code");
  qol_setup_essential_mapping(&framework_mapping, global_map_id, file_map_id, code_map_id);
  qol_setup_default_mapping(&framework_mapping, global_map_id, file_map_id, code_map_id);
}