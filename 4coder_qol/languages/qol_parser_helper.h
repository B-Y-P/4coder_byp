
struct QOL_Parse_State{
  Token *just_closed;
  Code_Index_Nest *stack;
  Code_Index_File *index;
  Generic_Parse_State generic;
};

enum{
  TokenBaseKind_StmntClose = TokenBaseKind_StatementClose,
  TokenBaseKind_ParenOpen  = TokenBaseKind_ParentheticalOpen,
  TokenBaseKind_ParenClose = TokenBaseKind_ParentheticalClose,
  TokenBaseKind_Preproc    = TokenBaseKind_Preprocessor
};

function void qol_parse_init(Application_Links *app, Arena *arena, String_Const_u8 contents, Token_Array *tokens, QOL_Parse_State *state){
  state->stack = NULL;
  generic_parse_init(app, arena, contents, tokens, &state->generic);
  generic_parse_skip_soft_tokens(&state->generic);
}

internal void qol_tok_restore(QOL_Parse_State *state, Token *token){
  state->generic.it = token_iterator(&state->generic, token);
}

function void qol_tok_consume(QOL_Parse_State *state){
  generic_parse_inc(&state->generic);
  generic_parse_skip_soft_tokens(&state->generic);
}

function Token* qol_tok_peek (QOL_Parse_State *state){ return token_it_read(&state->generic.it); }
function Token* qol_tok_close(QOL_Parse_State *state){ return state->just_closed = qol_tok_peek(state); }

function b32 qol_tok_peek(QOL_Parse_State *state, Token_Base_Kind kind, Token **out_tok){
  if (state->generic.finished){ return false; }
  Token *token = qol_tok_peek(state);
  if (token->kind == kind){
    *out_tok = token;
    return true;
  }
  return false;
}

function b32 qol_tok_peek(QOL_Parse_State *state, Token_Cpp_Kind kind, Token **out_tok){
  if (state->generic.finished){ return false; }
  Token *token = qol_tok_peek(state);
  if (token->sub_kind == kind){
    *out_tok = token;
    return true;
  }
  return false;
}

function b32 qol_tok_peek(QOL_Parse_State *state, Token_Cpp_Kind  kind){ Token *t; return qol_tok_peek(state, kind, &t); }
function b32 qol_tok_peek(QOL_Parse_State *state, Token_Base_Kind kind){ Token *t; return qol_tok_peek(state, kind, &t); }

function b32 qol_tok_peek_preproc_body(QOL_Parse_State *state){
  Token *token = qol_tok_peek(state);
  return token != 0 && HasFlag(token->flags, TokenBaseFlag_PreprocessorBody);
}

function b32 qol_tok_accept(QOL_Parse_State *state, Token_Base_Kind kind, Token **out_tok){
  Token *token = qol_tok_peek(state);
  if (token != 0 && token->kind == kind){
    *out_tok = token;
    qol_tok_consume(state);
    return true;
  }
  return false;
}

function b32 qol_tok_accept(QOL_Parse_State *state, Token_Base_Kind kind){
  Token *t = 0;
  return qol_tok_accept(state, kind, &t);
}

function b32 qol_tok_accept(QOL_Parse_State *state, Token_Cpp_Kind kind){
  Token *token = qol_tok_peek(state);
  if (token != 0 && token->sub_kind == kind){
    qol_tok_consume(state);
    return true;
  }
  return false;
}

function b32 qol_nest_is_inside(Code_Index_Nest *nest, Code_Index_Nest_Kind kind){
  return nest == 0 ? false : nest->kind == kind || qol_nest_is_inside(nest->parent, kind);
}

function void qol_note_push(QOL_Parse_State *state, Range_i64 range, Code_Index_Note_Kind kind){
  index_new_note(state->index, &state->generic, range, kind, state->stack);
}

function void qol_note_push(QOL_Parse_State *state, Range_i64 range, Code_Index_Note_Kind kind, Code_Index_Nest* parent){
  index_new_note(state->index, &state->generic, range, kind, parent);
}


// NOTE: usage `if(peek(ScopeOpen)){ nest_push(Scope) }` as nest_push peeks and consumes
function Code_Index_Nest* qol_nest_push(QOL_Parse_State *state, Code_Index_Nest_Kind kind){
  Code_Index_Nest *nest = push_array_zero(state->generic.arena, Code_Index_Nest, 1);
  nest->kind   = kind;
  nest->open   = Ii64(qol_tok_peek(state));
  nest->close  = Ii64(max_i64);
  //nest->parent = (kind == CodeIndexNest_Preprocessor ? NULL : state->stack);
  nest->parent = state->stack;
  nest->file   = state->index;
  state->generic.scope_counter  += (kind == CodeIndexNest_Scope);
  state->generic.paren_counter  += (kind == CodeIndexNest_Paren);
  state->generic.in_statement    = qol_nest_is_inside(nest, CodeIndexNest_Statement);
  state->generic.in_preprocessor = qol_nest_is_inside(nest, CodeIndexNest_Preprocessor);
  sll_stack_push(state->stack, nest);
  if (kind != CodeIndexNest_Statement){ qol_tok_consume(state); }
  return nest;
}

function Code_Index_Nest* qol_nest_pop(QOL_Parse_State *state, bool is_closed, Range_i64 close_range){
  if (state->stack == 0){ return 0; }
  Code_Index_Nest *nest = state->stack;
  sll_stack_pop(state->stack);
  nest->next = 0;
  nest->is_closed = is_closed;
  nest->close = close_range;
  state->generic.scope_counter  -= (nest->kind == CodeIndexNest_Scope && state->generic.scope_counter > 0);
  state->generic.paren_counter  -= (nest->kind == CodeIndexNest_Paren && state->generic.paren_counter > 0);
  state->generic.in_statement    = qol_nest_is_inside(nest->parent, CodeIndexNest_Statement);
  state->generic.in_preprocessor = qol_nest_is_inside(nest->parent, CodeIndexNest_Preprocessor);
  nest->nest_array = code_index_nest_ptr_array_from_list(state->generic.arena, &nest->nest_list);
  Code_Index_Nest_List *list = (state->stack ? &state->stack->nest_list : &state->index->nest_list);
  code_index_push_nest(list, nest);
  return nest;
}

// For balanced input, _paren and _scope are symmetric, but for unbalanced input, scopes dominates unclosed parens
function Code_Index_Nest* qol_nest_pop_scope(QOL_Parse_State *state){
  Range_i64 range = Ii64(qol_tok_close(state));
  while (state->stack == 0 || state->stack->kind != CodeIndexNest_Scope){
    qol_nest_pop(state, false, Ii64(range.min));
  }
  defer{ qol_tok_consume(state); };
  return qol_nest_pop(state, true, range);
}

function Code_Index_Nest* qol_nest_pop_paren(QOL_Parse_State *state){
  Range_i64 range = Ii64(qol_tok_close(state));
  if (state->stack == 0 || state->stack->kind != CodeIndexNest_Paren){ return NULL; }
  defer{ qol_tok_consume(state); };
  return qol_nest_pop(state, true, range);
}

function void qol_nest_resolve(QOL_Parse_State *state, Code_Index_Nest *last){
  while (state->stack != last){
    qol_nest_pop(state, false, Ii64(qol_tok_peek(state)));
  }
  state->index->nest_array = code_index_nest_ptr_array_from_list(state->generic.arena, &state->index->nest_list);
  state->index->note_array = code_index_note_ptr_array_from_list(state->generic.arena, &state->index->note_list);
}

function b32 qol_scan_parens(QOL_Parse_State *state){
  Token_Iterator_Array it = state->generic.it;
  if (generic_scan_parens(state->index, &state->generic)){
    qol_tok_consume(state);
    return true;
  }
  state->generic.it = it;
  return false;
}

function Code_Index_Nest* qol_parse_stmnt   (QOL_Parse_State *state);
function Code_Index_Nest* qol_parse_scope   (QOL_Parse_State *state);
function Code_Index_Nest* qol_parse_paren   (QOL_Parse_State *state);
function Code_Index_Nest* qol_parse_preproc (QOL_Parse_State *state);
function void             qol_parse_top     (QOL_Parse_State *state);
function void             qol_parse_file    (Code_Index_File *index, QOL_Parse_State *state, i32 limit);

function Code_Index_Nest* qol_parse_preproc(QOL_Parse_State *state){
  b32 try_macro = qol_tok_peek(state, TokenCppKind_PPDefine);
  qol_nest_push(state, CodeIndexNest_Preprocessor);
  for (;;){
    if (state->generic.finished){ break; }
    if (qol_tok_peek(state, TokenBaseKind_Preprocessor) || !qol_tok_peek_preproc_body(state)){ break; }
    if (qol_tok_peek(state, TokenBaseKind_Identifier) && try_macro){ qol_note_push(state, Ii64(qol_tok_peek(state)), CodeIndexNote_Macro); }
    if (qol_tok_peek(state, TokenBaseKind_ScopeOpen)){ qol_parse_scope(state); continue; }
    if (qol_tok_peek(state, TokenBaseKind_ParenOpen)){ qol_parse_paren(state); continue; }
    qol_tok_consume(state);
    try_macro = false;
  }
  defer{ generic_parse_skip_soft_tokens(&state->generic); };
  return qol_nest_pop(state, true, Ii64(qol_tok_close(state)->pos));
}

function Code_Index_Nest* qol_parse_stmnt(QOL_Parse_State *state){
  qol_nest_push(state, CodeIndexNest_Statement);
  for (;;){
    if (state->generic.finished){ break; }
    if (state->generic.in_preprocessor && (!qol_tok_peek_preproc_body(state) || qol_tok_peek(state, TokenBaseKind_Preprocessor))){ break; }
    if (!state->generic.in_preprocessor && qol_tok_peek(state, TokenBaseKind_Preprocessor)){ break; }
    if (qol_tok_peek(state, TokenBaseKind_ParenOpen )){ qol_parse_paren(state); continue; }
    if (qol_tok_peek(state, TokenBaseKind_ScopeOpen )){ break; } // { qol_parse_scope(state); return qol_nest_pop(state, true, Ii64(state->just_closed)); }
    if (qol_tok_peek(state, TokenBaseKind_ParenClose)){ break; }
    if (qol_tok_peek(state, TokenBaseKind_ScopeClose)){ break; }
    if (qol_tok_peek(state, TokenBaseKind_StmntClose)){ defer{ qol_tok_consume(state); }; return qol_nest_pop(state, true, Ii64(qol_tok_close(state))); }
    qol_tok_consume(state);
  }
  return qol_nest_pop(state, true, Ii64(qol_tok_close(state)->pos));
}

function Code_Index_Nest* qol_parse_scope(QOL_Parse_State *state){
  qol_nest_push(state, CodeIndexNest_Scope);
  for (;;){
    if (state->generic.finished){ break; }
    if (state->generic.in_preprocessor && (!qol_tok_peek_preproc_body(state) || qol_tok_peek(state, TokenBaseKind_Preprocessor))){ break; }
    if (!state->generic.in_preprocessor && qol_tok_peek(state, TokenBaseKind_Preprocessor)){ qol_parse_preproc(state); continue; }
    if (qol_tok_peek(state, TokenBaseKind_ScopeClose)){ break; }
    if (qol_tok_peek(state, TokenBaseKind_ScopeOpen )){ qol_parse_scope(state); continue; }
    if (qol_tok_peek(state, TokenBaseKind_ParenClose)){ qol_tok_consume(state); continue; }
    if (qol_tok_peek(state, TokenBaseKind_ParenOpen )){ qol_parse_paren(state); /**/ }
    qol_parse_stmnt(state);
  }
  return qol_nest_pop_scope(state);
}

function Code_Index_Nest* qol_parse_paren(QOL_Parse_State *state){
  qol_nest_push(state, CodeIndexNest_Paren);
  for (;;){
    if (state->generic.finished){ break; }
    if (state->generic.in_preprocessor && !qol_tok_peek_preproc_body(state) || qol_tok_peek(state, TokenBaseKind_Preprocessor)){ break; }
    if (!state->generic.in_preprocessor && qol_tok_peek(state, TokenBaseKind_Preprocessor)){ qol_parse_preproc(state); continue; }
    if (qol_tok_peek(state, TokenBaseKind_ParenClose)){ break; }
    if (qol_tok_peek(state, TokenBaseKind_ParenOpen )){ qol_parse_paren(state); continue; }
    if (qol_tok_peek(state, TokenBaseKind_ScopeClose)){ break; }
    if (qol_tok_peek(state, TokenBaseKind_ScopeOpen )){ qol_parse_scope(state); continue; }
    qol_parse_stmnt(state);
  }
  return qol_nest_pop_paren(state);
}