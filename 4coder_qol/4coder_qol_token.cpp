
function b32 qol_highlight_token(Token_Base_Kind kind){
  switch(kind){
    case TokenBaseKind_Keyword:
    case TokenBaseKind_Identifier:
    case TokenBaseKind_Primitive:
    case TokenBaseKind_Control:
    case TokenBaseKind_Struct:
    return true;
  }
  return false;
}

function Managed_ID qol_get_token_color_base(Token *token){
  switch (token->kind){
    case TokenBaseKind_Preproc:        { return defcolor_preproc;        }break;
    case TokenBaseKind_Keyword:        { return defcolor_keyword;        }break;
    case TokenBaseKind_Comment:        { return defcolor_comment;        }break;
    case TokenBaseKind_LiteralString:  { return defcolor_str_constant;   }break;
    case TokenBaseKind_LiteralInteger: { return defcolor_int_constant;   }break;
    case TokenBaseKind_LiteralFloat:   { return defcolor_float_constant; }break;
    case TokenBaseKind_Operator:       { return defcolor_operator;       }break;
    case TokenBaseKind_ScopeOpen:
    case TokenBaseKind_ScopeClose:
    case TokenBaseKind_ParenOpen:
    case TokenBaseKind_ParenClose:
    case TokenBaseKind_StmntClose:{ return defcolor_non_text;  }break;
    case TokenBaseKind_Control:   { return defcolor_control;   }break;
    case TokenBaseKind_Primitive: { return defcolor_primitive; }break;
    case TokenBaseKind_Struct:    { return defcolor_struct;    }break;
  }
  return defcolor_text_default;
}

function FColor qol_get_token_color_cpp(Token *token){
  switch (token->sub_kind){
    case TokenCppKind_LiteralTrue:
    case TokenCppKind_LiteralFalse:{ return fcolor_id(defcolor_bool_constant); } break;
    case TokenCppKind_LiteralCharacter:
    case TokenCppKind_LiteralCharacterWide:
    case TokenCppKind_LiteralCharacterUTF8:
    case TokenCppKind_LiteralCharacterUTF16:
    case TokenCppKind_LiteralCharacterUTF32: { return fcolor_id(defcolor_char_constant); }break;
    case TokenCppKind_PPIncludeFile: { return fcolor_id(defcolor_include); }break;
  }
  return fcolor_id(qol_get_token_color_base(token));
}

function FColor qol_get_token_color_lua(Token *token){
  switch(token->sub_kind){
    case TokenLuaKind_Reserved:     return fcolor_id(defcolor_global);
    case TokenCppKind_LiteralTrue:  return fcolor_id(defcolor_bool_constant);
    case TokenCppKind_LiteralFalse: return fcolor_id(defcolor_bool_constant);
  }
  return fcolor_id(token->kind == TokenBaseKind_Preproc ? defcolor_global : qol_get_token_color_base(token));
}