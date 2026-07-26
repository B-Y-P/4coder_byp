
#define foreach(i,N) for(i64 i=0; i<N; i++)
#define in_range(bot,mid,top) ((bot) <= (mid) && (mid) < (top))

// NOTE(BYP): Used in vim_request_vtable
#define VIM_REQUEST_SIG(name) void name(Application_Links *app, View_ID view, Buffer_ID buffer, Range_i64 range)
typedef void Vim_Apply_Request(Application_Links *app, View_ID view, Buffer_ID buffer, Range_i64 range);

// NOTE(BYP): Used in vim_text_object_vtable
#define VIM_TEXT_OBJECT_SIG(name) Range_i64 name(Application_Links *app, Buffer_ID buffer, i64 cursor_pos)
typedef Range_i64 Vim_Text_Object_Func(Application_Links *app, Buffer_ID buffer, i64 cursor_pos);

// NOTE: Same as CUSTOM_COMMAND_SIG but won't clutter command lister
#if !defined(META_PASS)
#define VIM_COMMAND_SIG(name) void name(Application_Links *app)
#else
#define VIM_COMMAND_SIG(name) CUSTOM_COMMAND(name, __FILE__, __LINE__, Normal) CUSTOM_DOC(#name)
#endif


struct Vim_Text_Object{
  u8 character;
  Vim_Text_Object_Func *func;
};

enum Vim_Mode{
  VIM_Normal,
  VIM_Insert,
  VIM_Visual,
  VIM_Replace,
  VIM_MODE_COUNT,
};

enum Vim_Sub_Mode{
  SUB_None,
  SUB_G,
  SUB_Z,
  SUB_Leader,
  SUB_Window,
  VIM_SUBMODE_COUNT,
};

typedef u32 Vim_Request_Type;
enum{
  REQUEST_None,
  REQUEST_Yank,
  REQUEST_Delete,
  REQUEST_Change,
  REQUEST_Upper, REQUEST_Lower,
  REQUEST_ToggleCase,
  REQUEST_Replace,
  REQUEST_Indent, REQUEST_Outdent,
  REQUEST_AutoIndent,
  REQUEST_Fold,
  VIM_REQUEST_COUNT
};

enum Vim_Default_Text_Objects{
  TEXT_OBJECT_word,
  TEXT_OBJECT_Word,
  TEXT_OBJECT_para,

  VIM_TEXT_OBJECT_COUNT
};

enum Vim_Edit_Type{
  EDIT_CharWise,
  EDIT_LineWise,
  EDIT_Block,         // Only use for Visual Block
};

enum Vim_Clusivity{
  VIM_Inclusive,
  VIM_Exclusive,
};

typedef u32 Vim_Register_Flags;
enum{
  REGISTER_ReadOnly = bit_1,
  REGISTER_Append   = bit_2,
  REGISTER_Set      = bit_3,
  REGISTER_Updated  = bit_4,
};

enum Vim_Chord{
  VIM_CHORD_Resolved,
  VIM_CHORD_Pending,
  VIM_CHORD_Error,
};

struct Vim_Register{
  Vim_Edit_Type edit_type;
  String_u8 data;
  Vim_Register_Flags flags;
};

struct Vim_Seek_Params{
  u8 character;
  Vim_Clusivity clusivity;
  Scan_Direction direction;
};

struct Vim_Params{
  i32 number;
  i32 count;
  Vim_Request_Type request;
  Vim_Edit_Type edit_type;
  Vim_Clusivity clusivity;
  Vim_Seek_Params seek;
  Vim_Register *selected_reg;
  u8 consume_char;
  b8 do_insert;
  Custom_Command_Function *command;
};

struct Vim_State{
  Vim_Mode mode;
  Vim_Sub_Mode sub_mode;

  Vim_Chord chord_state;
  u8 macro_char;
  u8 prev_macro;

  Buffer_Cursor insert_cursor;
  History_Record_Index insert_index;

  i32 number;
  Vim_Params params;
  Vim_Params prev_params;
  Custom_Command_Function *active_command;
};

union Vim_Registers{
  struct{
    Vim_Register named[26];
    Vim_Register unnamed;
    Vim_Register system;
    Vim_Register expression;
    Vim_Register search;
    union{
      Vim_Register digit[10];
      struct{
        Vim_Register yank;
        Vim_Register cycle[9];
      };
    };
    Vim_Register small_delete;
    Vim_Register insert;
    Vim_Register command;
    Vim_Register file;
  };
  Vim_Register r[44];
};

struct Vim_Prev_Visual{
  i64 cursor_pos, mark_pos;
  Vim_Edit_Type edit_type;
};

struct Vim_Global_Mark{
  Buffer_Identifier buffer_id;
  i64 pos;
};

global_const u32 KeyMod_Ctl = bit_32;
global_const u32 KeyMod_Sft = bit_31;
global_const u32 KeyMod_Alt = bit_30;
global_const u32 KeyMod_Cmd = bit_29;
global_const u32 KeyMod_Mnu = bit_28;
