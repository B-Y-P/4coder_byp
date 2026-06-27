
function void cpp_parse_struct(QOL_Parse_State *state){
  Token *iden;
  if (qol_tok_accept(state, TokenBaseKind_Identifier, &iden)){
    if (qol_tok_peek(state, TokenBaseKind_StmntClose)){ qol_note_push(state, Ii64(iden), CodeIndexNote_Type); }
    if (qol_tok_peek(state, TokenBaseKind_ScopeOpen )){ qol_note_push(state, Ii64(iden), CodeIndexNote_Type); }
  }
  if (qol_tok_peek(state, TokenBaseKind_ScopeOpen)){
    qol_parse_scope(state);  // TODO: recursively call into qol_parse_top
    //qol_nest_push(state, CodeIndexNest_Scope);
    //cpp_parse_top(state);
    //if (qol_tok_peek(state, TokenBaseKind_ScopeClose)){ qol_nest_pop_scope(state); }
  }
}

function void cpp_parse_enum(QOL_Parse_State *state){
  Token *iden;
  if (qol_tok_accept(state, TokenCppKind_Struct) ||
      qol_tok_accept(state, TokenCppKind_Class)){}
  if (qol_tok_accept(state, TokenBaseKind_Identifier, &iden)){
    qol_note_push(state, Ii64(iden), CodeIndexNote_Type);
  }
  if (qol_tok_accept(state, TokenCppKind_Colon)){
    while (qol_tok_accept(state, byp_TokenKind_Primitive) ||
           qol_tok_accept(state, TokenBaseKind_Identifier));
  }
  if (qol_tok_peek(state, TokenBaseKind_ScopeOpen)){
    qol_nest_push(state, CodeIndexNest_Scope);
    for (;;){
      if (state->generic.finished){ break; }
      if (qol_tok_peek(state, TokenBaseKind_ScopeClose)){ break; }
      if (qol_tok_peek(state, TokenBaseKind_Identifier)){
        qol_note_push(state, Ii64(qol_tok_peek(state)), CodeIndexNote_Enum);
        qol_parse_stmnt(state);
        continue;
      }
      qol_tok_consume(state);
    }
    qol_nest_pop_scope(state);
  }
}

function void cpp_parse_using(QOL_Parse_State* state){
  Token *iden;
  if (qol_tok_accept(state, TokenBaseKind_Identifier, &iden) &&
      qol_tok_accept(state, TokenCppKind_Eq)){
    qol_note_push(state, Ii64(iden), CodeIndexNote_Type);
  }
}

function void cpp_parse_extern(QOL_Parse_State *state){
  if (qol_tok_accept(state, TokenBaseKind_LiteralString) &&
      qol_tok_peek(state, TokenBaseKind_ScopeOpen)){
    qol_nest_push(state, CodeIndexNest_Scope);
    cpp_parse_top(state);
    qol_nest_pop_scope(state);
  }
}

function b32 cpp_parse_func(QOL_Parse_State *state){
  Token *iden, *begin, *end;
  if (!qol_tok_accept(state, TokenBaseKind_Identifier, &iden)){ return false; }
  if (qol_tok_peek(state, TokenCppKind_ParenOp, &begin) && qol_scan_parens(state)){
    if (qol_tok_peek(state, TokenBaseKind_ScopeOpen, &end) || qol_tok_peek(state, TokenBaseKind_StmntClose, &end)) {
      qol_tok_restore(state, iden);
      qol_note_push(state, Ii64(iden), CodeIndexNote_Function, qol_parse_stmnt(state));
      return true;
    }
  }
  qol_tok_restore(state, iden);
  return false;
}

function void cpp_parse_global(QOL_Parse_State *state){
loop:;
  qol_tok_consume(state);
  Token_Iterator_Array reset_it = state->generic.it;
  Token *parens[16], *iden;  // im ok disallowing over 16-dimensional global arrays...
  i64 paren_count = 0;

  for (;;){
    if (state->generic.finished){ goto fail; }
    if (qol_tok_peek(state, TokenCppKind_BrackOp, &parens[paren_count])){
      if (!qol_scan_parens(state) || ++paren_count >= ArrayCount(parens)){ goto fail; }
    }
    else if (!qol_tok_peek(state, TokenBaseKind_Operator) || qol_tok_peek(state, TokenCppKind_Eq)){ break; }
    else { qol_tok_consume(state); }
  }

  if (!qol_tok_accept(state, TokenBaseKind_Identifier, &iden)){ goto fail; }

  for (;;){
    if (state->generic.finished){ goto fail; }
    if (qol_tok_peek(state, TokenCppKind_BrackOp, &parens[paren_count])){
      if (!qol_scan_parens(state) || ++paren_count >= ArrayCount(parens)){ goto fail; }
    }
    else if (!qol_tok_peek(state, TokenBaseKind_Operator) || qol_tok_peek(state, TokenCppKind_Eq)){ break; }
    else { qol_tok_consume(state); }
  }

  if (qol_tok_peek(state, TokenBaseKind_StmntClose) || qol_tok_peek(state, TokenCppKind_Eq)){
    qol_note_push(state, Ii64(iden), CodeIndexNote_Global);
    Token_Iterator_Array it = state->generic.it;
    for (i64 i=0; i<paren_count; i += 1){
      qol_tok_restore(state, parens[i]);
      qol_parse_paren(state);
    }
    state->generic.it = it;
    if (qol_tok_peek(state, TokenCppKind_Eq)){ qol_parse_stmnt(state); qol_tok_restore(state, state->just_closed); }
    if (qol_tok_peek(state, TokenCppKind_Comma)){ goto loop; }
    return;
  }
fail:
  state->generic.it = reset_it;
}

function void cpp_parse_typedef(QOL_Parse_State *state){
  Token *iden, *paren;
  for(;;){
    if (state->generic.finished){ break; }
    else if (qol_tok_peek(state, TokenBaseKind_ScopeOpen)){ break; }
    else if (qol_tok_peek(state, TokenBaseKind_ScopeClose)){ break; }
    else if (qol_tok_peek(state, TokenBaseKind_StmntClose)){ break; }
    else if (qol_tok_accept(state, TokenBaseKind_Identifier, &iden)){
      if (qol_tok_accept(state, TokenCppKind_Comma)){ qol_note_push(state, Ii64(iden), CodeIndexNote_Type); continue; } // typedef char *LPSTR, *PSTR; 
      if (qol_tok_peek(state, TokenBaseKind_StmntClose)){ qol_note_push(state, Ii64(iden), CodeIndexNote_Type); break; } // typedef uint64_t u64;
      if (qol_tok_peek(state, TokenBaseKind_ParenOpen, &paren)) {
        if (qol_scan_parens(state) && qol_tok_peek(state, TokenBaseKind_StmntClose)){
          qol_note_push(state, Ii64(iden), CodeIndexNote_Type); // typedef void Void_Func( ... );
          qol_tok_restore(state, paren);
          break;
        }
        qol_tok_restore(state, paren);
      } 
    }
    else if (qol_tok_accept(state, TokenCppKind_ParenOp, &paren)){
      while (qol_tok_accept(state, TokenBaseKind_Identifier)){}
      if (qol_tok_accept(state, TokenCppKind_Star) &&
          qol_tok_accept(state, TokenBaseKind_Identifier, &iden) &&
          qol_tok_accept(state, TokenCppKind_ParenCl)){
        qol_note_push(state, Ii64(iden), CodeIndexNote_Type); // typedef bool (__stdcall* Compare_Func)( ... );
      }
      qol_tok_restore(state, paren);
      break;
    }
    else if (qol_tok_accept(state, TokenCppKind_Struct)) { cpp_parse_struct(state); }
    else if (qol_tok_accept(state, TokenCppKind_Union))  { cpp_parse_struct(state); }
    else if (qol_tok_accept(state, TokenCppKind_Enum))   { cpp_parse_enum(state); }
    else{
      qol_tok_consume(state);
    }
  }
}

function void cpp_parse_top(QOL_Parse_State *state){
  for (;;){
    if (state->generic.finished || state->stack != 0 && qol_tok_peek(state, TokenBaseKind_ScopeClose)){ break; }
    else if (qol_tok_peek(state, TokenBaseKind_ScopeOpen )){ qol_parse_scope   (state); }
    else if (qol_tok_peek(state, TokenBaseKind_ParenOpen )){ qol_parse_paren   (state); }
    else if (qol_tok_peek(state, TokenBaseKind_Preproc   )){ qol_parse_preproc (state); }
    else if (qol_tok_accept(state, TokenCppKind_Enum     )){ cpp_parse_enum    (state); }
    else if (qol_tok_accept(state, TokenCppKind_Struct   )){ cpp_parse_struct  (state); }
    else if (qol_tok_accept(state, TokenCppKind_Union    )){ cpp_parse_struct  (state); }
    else if (qol_tok_accept(state, TokenCppKind_Typedef  )){ cpp_parse_typedef (state); }
    else if (qol_tok_accept(state, TokenCppKind_Using    )){ cpp_parse_using   (state); }
    else if (qol_tok_accept(state, TokenCppKind_Extern   )){ cpp_parse_extern  (state); }
    else if (qol_tok_peek(state, TokenCppKind_Identifier) && cpp_parse_func    (state)){}
    else if (qol_tok_peek(state, TokenCppKind_Identifier )){ cpp_parse_global  (state); }
    else if (qol_tok_peek(state, byp_TokenKind_Primitive )){ cpp_parse_global  (state); }
    else{
      qol_tok_consume(state);
    }
  }
}

function void cpp_parse_file(Code_Index_File *index, QOL_Parse_State *state, i32 limit){
  state->index = index;
  state->generic.token_it_index_opl = token_it_index(&state->generic.it) + limit;
  cpp_parse_top(state);
  qol_nest_resolve(state, NULL);
}