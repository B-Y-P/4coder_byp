
#define LANG_NAME_LOWER lua
#define LANG_NAME_CAMEL Lua

#include "lexer_generator/4coder_lex_gen_main.cpp"

internal void
build_language_model(void){
  u8 utf8[129];
  smh_utf8_fill(utf8);

  smh_set_base_character_names();
  smh_typical_tokens();

  sm_select_base_kind(TokenBaseKind_Comment);
  sm_direct_token_kind("CommentShort");  // '--' '.'*
  sm_direct_token_kind("CommentLong");   // LONG : '=' LONG '=' | '[' .* ']' ;

  sm_select_base_kind(TokenBaseKind_Identifier);
  sm_direct_token_kind("Reserved");  // _ENV, _VERSION, _G,

  sm_select_base_kind(TokenBaseKind_Keyword);
  sm_direct_token_kind("KeywordGeneric");

  sm_select_base_kind(TokenBaseKind_LiteralString);
  sm_direct_token_kind("LiteralStringSingle");
  sm_direct_token_kind("LiteralStringDouble");
  sm_direct_token_kind("LiteralStringMulti");

  sm_select_base_kind(TokenBaseKind_LiteralFloat);
  sm_direct_token_kind("LiteralFloat");
  sm_direct_token_kind("LiteralFloatHex");

  sm_select_base_kind(TokenBaseKind_LiteralInteger);
  sm_direct_token_kind("LiteralInteger");
  sm_direct_token_kind("LiteralIntegerHex");

  Keyword_Set *main_keys = sm_begin_key_set("main_keys");
  // Lua Keywords
  sm_select_base_kind(TokenBaseKind_Keyword);
  sm_key("Function");
  sm_key("Local");

  // std function
  sm_key("assert");
  sm_key("collectgarbage");
  sm_key("dofile");
  sm_key("error");
  sm_key("getmetatable");
  sm_key("ipairs");
  sm_key("load");
  sm_key("loadfile");
  sm_key("next");
  sm_key("pairs");
  sm_key("pcall");
  sm_key("print");
  sm_key("rawequal");
  sm_key("rawget");
  sm_key("rawlen");
  sm_key("rawset");
  sm_key("require");
  sm_key("select");
  sm_key("setmetatable");
  sm_key("tonumber");
  sm_key("tostring");
  sm_key("type");
  sm_key("xpcall");
  sm_key("warn");

  sm_select_base_kind(TokenBaseKind_Operator);
  sm_key("Not");
  sm_key("And");
  sm_key("Or");
  sm_key("In");

  sm_select_base_kind(TokenBaseKind_Preproc);
  sm_key("bit32");
  sm_key("coroutine");
  sm_key("debug");
  sm_key("file");
  sm_key("io");
  sm_key("math");
  sm_key("os");
  sm_key("package");
  sm_key("string");
  sm_key("table");
  sm_key("utf8");

  sm_select_base_kind(TokenBaseKind_Control);
  sm_key("Then");    // VWS ScopeOpen
  sm_key("Do");      // VWS ScopeOpen
  sm_key("Repeat");  // VWS ScopeOpen
  sm_key("Else");    // VWS ScopeClose ScopeOpen
  sm_key("Elseif");  // VWS ScopeClose
  sm_key("End");     // VWS ScopeClose
  sm_key("Until");   // VWS ScopeClose  NOTE: VWS handled in lua_parser (not the lexer)
  sm_key("Break");
  sm_key("If");
  sm_key("Return");
  sm_key("While");
  sm_key("For");
  sm_key("Goto");

  sm_select_base_kind(TokenBaseKind_LiteralInteger);
  sm_key("LiteralTrue", "true");
  sm_key("LiteralFalse", "false");
  sm_key("Nil");

  sm_select_base_kind(TokenBaseKind_Identifier);
  sm_key_fallback("Identifier");

  Operator_Set *main_ops = sm_begin_op_set();

  sm_select_base_kind(TokenBaseKind_ScopeOpen);
  sm_op("{");
  sm_select_base_kind(TokenBaseKind_ScopeClose);
  sm_op("}");
  sm_select_base_kind(TokenBaseKind_ParenOpen);
  sm_op("(");
  sm_op("[");
  sm_select_base_kind(TokenBaseKind_ParenClose);
  sm_op(")");
  sm_op("]");
  sm_select_base_kind(TokenBaseKind_StmntClose);
  sm_op(";");

  // Explicitly name the operators to avoid overlap with lua keywords
  sm_select_base_kind(TokenBaseKind_Operator);
  sm_op("+",  "OpAdd");
  sm_op("-",  "OpSub");
  sm_op("*",  "OpMul");
  sm_op("/",  "OpDiv");
  sm_op("%",  "OpMod");
  sm_op("^",  "OpExp");
  sm_op("#",  "OpLength");
  sm_op("&",  "OpAnd");
  sm_op("~",  "OpNegXor");  // bitwise xor binop, bitwise neg unop
  sm_op("|",  "OpOr");
  sm_op("<<", "OpShl");
  sm_op("//", "OpFloorDiv");
  sm_op(">>", "OpShr");
  sm_op("==", "OpEq");
  sm_op("~=", "OpNeq");
  sm_op("<=", "OpLeq");
  sm_op(">=", "OpGeq");
  sm_op("<",  "OpLe");
  sm_op("=",  "OpAssign");
  sm_op(">",  "OpGe");
  sm_op(":",  "OpMethod");
  sm_op(",",  "OpComma");
  sm_op(".",  "OpDot");
  sm_op("..", "OpConcat");
  sm_op("::", "Label");
  sm_op("...", "VarArgs");
  //sm_key_fallback("LexError"); // should this be required? (maybe?)

  // State Machine
  State *root = sm_begin_state_machine();
#define AddState(N) State *N = sm_add_state(#N)
  AddState(identifier);
  AddState(reserved_first);
  AddState(reserved);
  AddState(whitespace);
  AddState(number);
  AddState(znumber);
  AddState(number_hex);
  AddState(fnumber_dec);
  AddState(fnumber_hex);
  AddState(fnumber_exp_sign);
  AddState(fnumber_exp);
  AddState(fnumber_hex_exp);
  AddState(fnumber_hex_exp_sign);

  AddState(minus_or_comment);
  AddState(comment_first);
  AddState(comment_short);
  AddState(comment_loop);
  AddState(comment_long);
  AddState(comment_long_close);

  AddState(string_single);
  AddState(string_double);
  AddState(string_single_esc);
  AddState(string_double_esc);
  AddState(string_or_brace);
  AddState(string_loop);
  AddState(string_multi);
  AddState(string_multi_close);

  // TODO:
  //Flag *enter = sm_add_counter();
  //Flag *exit  = sm_add_counter();

  sm_select_state(root);

  {
    Emit_Rule *emit = sm_emit_rule();
    sm_emit_handler_direct("EOF");
    sm_case_eof(emit);
  }

  sm_case("abcdefghijklmnopqrstuvwxyz"
          "ABCDEFGHIJKLMNOPQRSTUVWXYZ", identifier);
  sm_case(utf8,          identifier);
  sm_case("_",           reserved_first);
  sm_case(" \r\t\f\v\n", whitespace);
  sm_case("123456789",   number);
  sm_case("0",           znumber);
  sm_case("-",           minus_or_comment);
  sm_case("[",           string_or_brace);
  sm_case("'",           string_single);
  sm_case("\"",          string_double);

  {
    Character_Set *char_set = smo_new_char_set();
    smo_char_set_union_ops_firsts(char_set, main_ops);
    char *char_set_array = smo_char_set_get_array(char_set);
    State *operator_state = smo_op_set_lexer_root(main_ops, root, "LexError");
    sm_case_peek(char_set_array, operator_state);
  }

  {
    Emit_Rule *emit = sm_emit_rule();
    sm_emit_handler_direct("LexError");
    sm_fallback(emit);
  }

#define S(s,n) for(int _I_=(sm_select_state(s),0); _I_==0; _I_ += (sm_fallback_peek(n),1))
#define D(n) sm_emit_rule_direct(n)

  S(whitespace, D("Whitespace")){
    sm_case(" \t\r\f\v\n", whitespace);
  }
  S(reserved_first, D("Identifier")){
    sm_case("ABCDEFGHIJKLMNOPQRSTUVWXYZ", reserved);
    sm_case("abcdefghijklmnopqrstuvwxyz0123456789_", identifier);
  }
  S(reserved, D("Reserved")){
    sm_case("ABCDEFGHIJKLMNOPQRSTUVWXYZ", reserved);
    sm_case("abcdefghijklmnopqrstuvwxyz0123456789_", identifier);
  }

  //S(identifier, sm_emit_rule_keys(main_keys)){
  //sm_case(utf8, identifier);
  //sm_case("ABCDEFGHIJKMNOPQSTVWXYZabcdefghijkmnopqstvwxyz0123456789_", identifier);
  //}
  sm_select_state(identifier);
  sm_case(utf8, identifier);
  sm_case("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
          "abcdefghijklmnopqrstuvwxyz"
          "0123456789_", identifier);
  {
    Emit_Rule *emit = sm_emit_rule();
    sm_emit_handler_keys(main_keys);
    sm_fallback_peek(emit);
  }

  S(number, D("LiteralInteger")){
    sm_case("0123456789", number_hex);
    sm_case(".", fnumber_dec);
  }
  S(znumber, D("LiteralInteger")){
    sm_case("xX", number_hex);
  }
  S(number_hex, D("LiteralIntegerHex")){
    sm_case("0123456789ABCDEFabcdef", number_hex);
    sm_case(".", fnumber_hex);
  }

  S(fnumber_dec, D("LiteralFloat")){
    sm_case("0123456789", fnumber_dec);
    sm_case("eE", fnumber_exp_sign);
  }
  S(fnumber_exp_sign, D("LiteralFloat")){
    sm_case("-+0123456789", fnumber_exp);
  }
  S(fnumber_exp, D("LiteralFloat")){
    sm_case("0123456789", fnumber_exp);
  }
  S(fnumber_hex, D("LiteralFloatHex")){
    sm_case("0123456789ABCDEFabcdef", fnumber_hex);
    sm_case("pP", fnumber_hex_exp_sign);
  }
  S(fnumber_hex_exp_sign, D("LiteralFloatHex")){
    sm_case("-+0123456789", fnumber_hex_exp);
  }
  S(fnumber_hex_exp, D("LiteralFloatHex")){
    sm_case("0123456789", fnumber_hex_exp);
  }

  sm_select_state(string_single);
  sm_case("'", D("LiteralStringSingle"));
  sm_case("\\", string_single_esc);
  sm_fallback(string_single);

  sm_select_state(string_double);
  sm_case("\"", D("LiteralStringSingle"));
  sm_case("\\", string_double_esc);
  sm_fallback(string_double);

  sm_select_state(string_single_esc);
  sm_case("'", string_single);
  sm_fallback(string_single);

  sm_select_state(string_double_esc);
  sm_case("\"", string_single);
  sm_fallback(string_double);

  sm_select_state(string_or_brace);
  sm_case_peek("=", string_loop);
  sm_case("[", string_multi);
  sm_fallback_peek(D("BrackOp"));

  sm_select_state(string_loop);
  sm_case("=", string_loop); // TODO: inc enter counter
  sm_case("[", string_multi);
  sm_fallback_peek(D("LexError"));

  sm_select_state(string_multi);
  sm_case("]", string_multi_close);
  sm_fallback(string_multi);

  sm_select_state(string_multi_close);
  sm_case("=", string_multi_close);  // TODO: inc exit counter
  sm_case("]", D("LiteralStringMulti"));  // TODO: enter==exit ? (-{emit}-> root) :  (-{exit=0}-> string_multi)
  sm_fallback(string_multi);

  S(minus_or_comment, D("OpSub")){
    sm_case("-", comment_first);
  }

  sm_select_state(comment_first);
  sm_case("[", comment_loop);
  sm_fallback_peek(comment_short);

  sm_select_state(comment_short);
  sm_case("\n", D("CommentShort"));
  sm_fallback(comment_short);

  sm_select_state(comment_loop);
  sm_case("=", comment_loop); // TODO: inc enter counter
  sm_case("[", comment_long);
  sm_fallback(comment_short);

  sm_select_state(comment_long);
  sm_case("]", comment_long_close);
  sm_fallback(comment_long);

  sm_select_state(comment_long_close);
  sm_case("=", comment_long_close);  // TODO: inc exit counter
  sm_case("]", D("CommentLong"));  // TODO: enter==exit ? (-{emit}-> root) :  (-{exit=0}-> comment_long)
  sm_fallback(comment_long);
}