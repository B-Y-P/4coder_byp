/*
4coder_code_index.h - Generic code indexing system for layout, definition jumps, etc.
*/

// TOP

#if !defined(FCODER_CODE_INDEX_H)
#define FCODER_CODE_INDEX_H

struct Code_Index_Nest_List{
  struct Code_Index_Nest *first;
  struct Code_Index_Nest *last;
  i32 count;
};

struct Code_Index_Nest_Ptr_Array{
  struct Code_Index_Nest **ptrs;
  i32 count;
};

typedef i32 Code_Index_Nest_Kind;
enum{
  CodeIndexNest_Scope,
  CodeIndexNest_Paren,
  CodeIndexNest_Chain,
  CodeIndexNest_Preprocessor,
  CodeIndexNest_Statement,
  CodeIndexNest_PProc = CodeIndexNest_Preprocessor,
  CodeIndexNest_Stmnt = CodeIndexNest_Statement,
};

enum Code_Index_Nest_SubKind{
  NestChain_Switch,
  NestChain_Method,
  NestChain_Ternary,
};

struct Code_Index_Nest{
  Code_Index_Nest *next;

  Code_Index_Nest_Kind kind;
  Code_Index_Nest_SubKind chain_kind;
  b32 is_closed;
  b32 is_chain_head;
  Range_i64 open;
  Range_i64 close;

  struct Code_Index_File *file;
  Code_Index_Nest *parent;

  Code_Index_Nest_List nest_list;
  Code_Index_Nest_Ptr_Array nest_array;
};

typedef i64 Code_Index_Note_Kind;
enum{
  CodeIndexNote_Type,
  CodeIndexNote_Function,
  CodeIndexNote_Macro,
  CodeIndexNote_Enum,
  CodeIndexNote_Global,
  CodeIndexNote_4coderCommand,
};

struct Code_Index_Note{
  Code_Index_Note *next;
  Code_Index_Note_Kind note_kind;
  Range_i64 pos;
  String_Const_u8 text;
  struct Code_Index_File *file;
  Code_Index_Nest *parent;

  Code_Index_Note *prev_in_hash;
  Code_Index_Note *next_in_hash;
};

struct Code_Index_Note_List{
  Code_Index_Note *first;
  Code_Index_Note *last;
  i32 count;
};

struct Code_Index_Note_Ptr_Array{
  Code_Index_Note **ptrs;
  i32 count;
};

struct Code_Index_File{
  Code_Index_Nest_List nest_list;
  Code_Index_Nest_Ptr_Array nest_array;
  Code_Index_Note_List note_list;
  Code_Index_Note_Ptr_Array note_array;
  Buffer_ID buffer;
};

struct Code_Index_File_Storage{
  Code_Index_File_Storage *next;
  Code_Index_File_Storage *prev;
  Arena arena;
  Code_Index_File *file;
};

struct Code_Index{
  System_Mutex mutex;
  Arena node_arena;
  Table_u64_u64 buffer_to_index_file;
  Code_Index_File_Storage *free_storage;
  Code_Index_File_Storage *storage_first;
  Code_Index_File_Storage *storage_last;
  i32 storage_count;

  Code_Index_Note_List name_hash[4099];
};

////////////////////////////////

struct Generic_Parse_State{
  Application_Links *app;
  Arena *arena;
  String_Const_u8 contents;
  Token_Iterator_Array it;
  u8 *prev_line_start;
  i64 token_it_index_opl;
  b32 finished;

  i32 scope_counter;
  i32 paren_counter;
  b32 in_preprocessor;
  b32 in_statement;

  b32 do_cpp_parse;
};

internal Token* token_it_read(Token_Iterator_Array *it);
function Range_i64 Ii64(Token *token);
function void code_index_push_nest(Code_Index_Nest_List *list, Code_Index_Nest *nest);
internal Token_Iterator_Array token_iterator(Generic_Parse_State *state, Token *token);
function Code_Index_Nest_Ptr_Array code_index_nest_ptr_array_from_list(Arena *arena, Code_Index_Nest_List *list);
function Code_Index_Note_Ptr_Array code_index_note_ptr_array_from_list(Arena *arena, Code_Index_Note_List *list);

function void generic_parse_inc(Generic_Parse_State *state);
function void generic_parse_skip_soft_tokens(Generic_Parse_State *state);
function b32 generic_scan_parens(Code_Index_File *index, Generic_Parse_State *state);
function Code_Index_Nest* generic_parse_statement         (Code_Index_File *index, Generic_Parse_State *state, Code_Index_Nest *parent);
function Code_Index_Nest* generic_parse_preproc           (Code_Index_File *index, Generic_Parse_State *state, Code_Index_Nest *parent);
function Code_Index_Nest* generic_parse_scope             (Code_Index_File *index, Generic_Parse_State *state, Code_Index_Nest *parent);
function Code_Index_Nest* generic_parse_paren             (Code_Index_File *index, Generic_Parse_State *state, Code_Index_Nest *parent);
function b32              generic_parse_top               (Code_Index_File *index, Generic_Parse_State *state, Code_Index_Nest *parent);
function b32              generic_parse_full_input_breaks (Code_Index_File *index, Generic_Parse_State *state, i32 limit);
function void             generic_parse_init(Application_Links *app, Arena *arena, String_Const_u8 contents, Token_Array *tokens, Generic_Parse_State *state);

function Code_Index_Note* index_new_note(Code_Index_File *index, Generic_Parse_State *state, Range_i64 range, Code_Index_Note_Kind kind, Code_Index_Nest *parent);

#endif

// BOTTOM
