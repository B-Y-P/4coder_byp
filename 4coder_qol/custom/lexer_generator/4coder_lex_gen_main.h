
#if !defined(LANG_NAME_LOWER) || !defined(LANG_NAME_CAMEL)
#error 4coder_lex_get_main.cpp not correctly included.
#endif

#include "4coder_base_types.h"
#include "4coder_table.h"
#include "4coder_token.h"
#include "pcg_basic.h"

#include "4coder_base_types.cpp"
#include "4coder_stringf.cpp"
#include "4coder_malloc_allocator.cpp"
#include "4coder_hash_functions.cpp"
#include "4coder_table.cpp"
#include "pcg_basic.c"

#define for_ll(n,F) for(auto n=(F); n; n = n->next)

#define LANG_NAME_LOWER_STR stringify(LANG_NAME_LOWER)
#define LANG_NAME_CAMEL_STR stringify(LANG_NAME_CAMEL)

////////////////////////////////

// NOTE(allen): PRIMARY MODEL

struct Token_Kind_Node{
  Token_Kind_Node *next;
  b32 optimized_in;
  String_Const_u8 name;
  Token_Base_Kind base_kind;
};

struct Token_Kind_Set{
  Token_Kind_Node *first;
  Token_Kind_Node *last;
  i32 count;
  Table_Data_u64 name_to_ptr;
};

struct Keyword{
  Keyword *next;
  String_Const_u8 name;
  String_Const_u8 lexeme;
};

struct Keyword_Set{
  Keyword_Set *next;
  Keyword *first;
  Keyword *last;
  i32 count;
  b32 has_fallback_token_kind;
  String_Const_u8 fallback_name;
  Table_Data_u64 name_to_ptr;
  Table_Data_u64 lexeme_to_ptr;
  String_Const_u8 pretty_name;
};

struct Keyword_Set_List{
  Keyword_Set *first;
  Keyword_Set *last;
  i32 count;
};

struct Keyword_Layout{
  u64 seed;
  u64 error_score;
  u64 max_single_error_score;
  f32 iterations_per_lookup;
  u64 *hashes;
  u64 *contributed_error;
  Keyword **slots;
  i32 slot_count;
};

typedef i32 Flag_Reset_Rule;
enum{
  FlagResetRule_AutoZero,
  FlagResetRule_KeepState,
  FlagResetRule_COUNT,
};

struct Flag{
  Flag *next;
  Flag_Reset_Rule reset_rule;
  Token_Base_Flag emit_flags;
  u16 emit_sub_flags;

  b32 optimized_in;
  String_Const_u8 base_name;
  i32 number;
  i32 index;
  u32 value;
};

struct Flag_Set{
  Flag *first;
  Flag *last;
  i32 count;
};

typedef i32 Emit_Handler_Kind;
enum{
  EmitHandlerKind_Direct,
  EmitHandlerKind_Keywords,
  EmitHandlerKind_KeywordsDelim,
};

struct Emit_Handler{
  Emit_Handler *next;
  Emit_Handler_Kind kind;
  Flag *flag_check;
  union{
    String_Const_u8 token_name;
    Keyword_Set *keywords;
  };
};

struct Emit_Check{
  Emit_Check *next;
  String_Const_u8 emit_check;
  Flag *flag;
  b32 value;
};

struct Emit_Check_List{
  Emit_Check *first;
  Emit_Check *last;
  i32 count;
};

struct Emit_Rule{
  Emit_Check_List emit_checks;
  Emit_Handler *first;
  Emit_Handler *last;
  i32 count;
};

typedef i32 Action_Kind;
enum{
  ActionKind_SetFlag,
  ActionKind_ZeroFlags,
  ActionKind_DelimMarkFirst,
  ActionKind_DelimMarkOnePastLast,
  ActionKind_Consume,
  ActionKind_Emit,
};

struct Action{
  Action *next;
  Action *prev;
  Action_Kind kind;
  union{
    struct{
      Flag *flag;
      b32 value;
    } set_flag;
    Emit_Rule *emit_rule;
  };
};

struct Action_List{
  Action *first;
  Action *last;
  i32 count;
};

typedef i32 Action_Context;
enum{
  ActionContext_Normal,
  ActionContext_EndOfFile,
};

typedef i32 Transition_Consume_Rule;
enum{
  Transition_Consume,
  Transition_NoConsume,
};

global u16 smi_eof = 256;

struct Field_Pin{
  Field_Pin *next;

  // This represents the set of flags with the particular /flag/ set to /flag/
  // exactly half of all flag state possibilities.
  Flag *flag;
  b32 value;
};

struct Field_Pin_List{
  Field_Pin_List *next;

  // This set is the intersection of the set represented by each pin.
  // A list with nothing in it is _always_ the "full set".
  Field_Pin *first;
  Field_Pin *last;
  i32 count;
};

struct Field_Set{
  // This set is the union of the set represented by each list.
  Field_Pin_List *first;
  Field_Pin_List *last;
  i32 count;
};

struct Input_Set{
  u16 *inputs;
  i32 count;
};

struct Condition_Node{
  Condition_Node *next;
  Field_Set fields;
  Input_Set inputs;
};

struct Condition_Set{
  Condition_Node *first;
  Condition_Node *last;
  i32 count;
};

typedef i32 Transition_Case_Kind;
enum{
  TransitionCaseKind_NONE,

  // intermediates only
  TransitionCaseKind_CharaterArray,
  TransitionCaseKind_EOF,
  TransitionCaseKind_Fallback,

  // actually stored in Transition_Case "kind" field
  TransitionCaseKind_DelimMatch,
  TransitionCaseKind_DelimMatchFail,
  TransitionCaseKind_ConditionSet,
};

struct Transition_Case{
  Transition_Case_Kind kind;
  union{
    Condition_Set condition_set;
  };
};

struct Transition{
  Transition *next;
  Transition *prev;
  struct State *parent_state;
  Transition_Case condition;
  Action_List activation_actions;
  struct State *dst_state;
};

struct Transition_List{
  Transition *first;
  Transition *last;
  i32 count;
};

struct Transition_Ptr_Node{
  Transition_Ptr_Node *next;
  Transition *ptr;
};

struct Transition_Ptr_Set{
  Transition_Ptr_Node *first;
  Transition_Ptr_Node *last;
  i32 count;
};

struct State{
  State *next;
  Transition_List transitions;
  String_Const_u8 pretty_name;
  char *source;

  b32 optimized_in;
  i32 number;
  Transition_Ptr_Set back_references;

  Action_List on_entry_actions;
};

struct State_Set{
  State *first;
  State *last;
  i32 count;
};

struct Lexer_Model{
  State *root;
  Flag_Set flags;
  State_Set states;
};

struct Lexer_Primary_Context{
  Base_Allocator *allocator;
  Arena arena;
  Token_Kind_Set tokens;
  Keyword_Set_List keywords;
  Lexer_Model model;
  b32 has_error;
};

////////////////////////////////

struct Flag_Ptr_Node{
  Flag_Ptr_Node *next;
  Flag *flag;
};

struct Flag_Bucket{
  String_Const_u8 pretty_name;
  Flag_Ptr_Node *first;
  Flag_Ptr_Node *last;
  i32 max_bits;
  i32 count;

  i32 number_of_variables;
};

typedef i32 Flag_Bind_Property;
enum{
  FlagBindProperty_Free,
  FlagBindProperty_Bound,
  FlagBindProperty_COUNT,
};

struct Flag_Bucket_Set{
  Flag_Bucket buckets[FlagBindProperty_COUNT][FlagResetRule_COUNT];
};

struct Partial_Transition{
  Partial_Transition *next;
  Field_Set fields;
  Action_List actions;
  State *dst_state;
};

struct Partial_Transition_List{
  Partial_Transition *first;
  Partial_Transition *last;
  i32 count;
};

struct Grouped_Input_Handler{
  Grouped_Input_Handler *next;

  u8 inputs[256];
  i32 input_count;
  b8 inputs_used[256];

  Partial_Transition_List partial_transitions;
};

struct Grouped_Input_Handler_List{
  Grouped_Input_Handler *first;
  Grouped_Input_Handler *last;
  i32 count;

  Grouped_Input_Handler *group_with_biggest_input_set;
};

////////////////////////////////

// NOTE(allen): MODELING SYNTAX HELPERS

struct Operator{
  Operator *next;
  String_Const_u8 name;
  String_Const_u8 op;
};

struct Operator_Set{
  Operator *first;
  Operator *last;
  i32 count;
  Table_Data_u64 lexeme_to_ptr;
};

struct Lexer_Helper_Context{
  Lexer_Primary_Context primary_ctx;
  Table_u64_Data char_to_name;
  Token_Base_Kind selected_base_kind;
  State *selected_state;
  Operator_Set *selected_op_set;
  Keyword_Set *selected_key_set;
  Emit_Rule *selected_emit_rule;
  Transition *selected_transition;
  char* selected_source;

  // convenience pointer to primary's arena.
  Arena *arena;
};

struct Character_Set{
  Table_u64_u64 table;
};
