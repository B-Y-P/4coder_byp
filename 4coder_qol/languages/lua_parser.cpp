
// NOTE: Only edge case lua explicitly disambiguates which 4coder should do auto-semi inserts
// Function calls and assignments can start with an open parenthesis.
// This possibility leads to an ambiguity in Lua's grammar.
// Consider the following fragment:
//  ` a = b + c
//  ` (print or io.write)('done')
// The grammar could see it in two ways:
//  ` a = b + c(print or io.write)('done')
//  ` a = b + c; (print or io.write)('done')

function void lua_parse_label(QOL_Parse_State *state){
  qol_tok_consume(state);
  Token *iden;
  if (qol_tok_accept(state, TokenBaseKind_Identifier, &iden) &&
      qol_tok_accept(state, TokenLuaKind_Label)){
    qol_note_push(state, Ii64(iden), CodeIndexNote_Macro);
  }
}

function void lua_parse_func(QOL_Parse_State *state) {
  Token *iden;
  qol_nest_push(state, CodeIndexNest_Scope);
  if (qol_tok_accept(state, TokenBaseKind_Identifier, &iden)){
    qol_note_push(state, Ii64(iden), CodeIndexNote_Function);
  }
}

function void lua_parse_else(QOL_Parse_State *state){
  Token *tok = qol_tok_peek(state);
  qol_nest_pop_scope(state);
  qol_tok_restore(state, tok);
  qol_nest_push(state, CodeIndexNest_Scope);
}

function void lua_parse_top(QOL_Parse_State *state){
  for (;;){
    if (state->generic.finished){ break; }
    else if (qol_tok_peek(state, TokenBaseKind_ScopeOpen)) { qol_nest_push(state, CodeIndexNest_Scope); }
    else if (qol_tok_peek(state, TokenBaseKind_ParenOpen)) { qol_nest_push(state, CodeIndexNest_Paren); }
    else if (qol_tok_peek(state, TokenBaseKind_ScopeClose)){ qol_nest_pop_scope(state); }
    else if (qol_tok_peek(state, TokenBaseKind_ParenClose)){ qol_nest_pop_paren(state); }
    else if (qol_tok_peek(state, TokenLuaKind_Label))      { lua_parse_label(state); }
    else if (qol_tok_peek(state, TokenLuaKind_Function))   { lua_parse_func(state); }
    else if (qol_tok_peek(state, TokenLuaKind_Do))         { qol_nest_push(state, CodeIndexNest_Scope); }
    else if (qol_tok_peek(state, TokenLuaKind_Repeat))     { qol_nest_push(state, CodeIndexNest_Scope); }
    else if (qol_tok_peek(state, TokenLuaKind_Then))       { qol_nest_push(state, CodeIndexNest_Scope); }
    else if (qol_tok_peek(state, TokenLuaKind_Else))       { lua_parse_else(state); }
    else if (qol_tok_peek(state, TokenLuaKind_Elseif))     { qol_nest_pop_scope(state); }
    else if (qol_tok_peek(state, TokenLuaKind_End))        { qol_nest_pop_scope(state); }
    else if (qol_tok_peek(state, TokenLuaKind_Until))      { qol_nest_pop_scope(state); }
    else{
      qol_tok_consume(state);
    }
  }
}

function void
lua_parse_file(Application_Links *app, Code_Index_File *index, Arena *arena, String_Const_u8 contents, Token_Array *tokens){
  i32 limit = max_i32;
  QOL_Parse_State state = {};
  qol_parse_init(app, arena, contents, tokens, &state);
  state.index = index;
  state.generic.token_it_index_opl = token_it_index(&state.generic.it) + limit;
  lua_parse_top(&state);
  qol_nest_resolve(&state, NULL);
}