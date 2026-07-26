
#define  PI64 3.14159265358979323846
#define TAU64 6.28318530717958647692
#define   E64 2.71828182845904523536

enum Expr_Op{
  CON, NEG,
  SIN, COS, TAN, SQRT, LOG, LN,
  ADD, SUB, MUL, DIV, MOD, EXP,
};

struct Expr_Node{
  Expr_Op type;
  f64 value;
  Expr_Node *A, *B;
};

function f64 EXPR_Evaluate(Expr_Node *node){
  switch(node->type){
    case(CON):  return node->value; break;
    case(NEG):  return       -EXPR_Evaluate(node->A);  break;
    case(SIN):  return    sin(EXPR_Evaluate(node->A)); break;
    case(COS):  return    cos(EXPR_Evaluate(node->A)); break;
    case(TAN):  return    tan(EXPR_Evaluate(node->A)); break;
    case(LN):   return    log(EXPR_Evaluate(node->A)); break;
    case(SQRT): return   sqrt(EXPR_Evaluate(node->A)); break;
    case(LOG):  return  log10(EXPR_Evaluate(node->A)); break;
    case(ADD):  return  EXPR_Evaluate(node->A) + EXPR_Evaluate(node->B); break;
    case(SUB):  return  EXPR_Evaluate(node->A) - EXPR_Evaluate(node->B); break;
    case(MUL):  return  EXPR_Evaluate(node->A) * EXPR_Evaluate(node->B); break;
    case(DIV):  return  EXPR_Evaluate(node->A) / EXPR_Evaluate(node->B); break;
    case(MOD):  return fmod(EXPR_Evaluate(node->A), EXPR_Evaluate(node->B)); break;
    case(EXP):  return  pow(EXPR_Evaluate(node->A), EXPR_Evaluate(node->B)); break;
  }
  return NAN;
}

enum Expr_Token_Type{
  Token_Unknown,
  Token_EndOfStream,
  Token_OpenParen, Token_CloseParen,
  Token_Number,
  Token_Plus, Token_Minus,
  Token_Asterisk, Token_ForwardSlash,
  Token_Caret,
  Token_Percent,
  Token_Sin, Token_Cos, Token_Tan,
  Token_Log, Token_Ln,
  Token_Sqrt,
  Token_PI, Token_TAU, Token_E,
  Token_ANS,
};

struct Expr_Token{
  Expr_Token_Type type;
  const char *text;
  i32 size;
};

struct Expr_Tokenizer{
  const char *At;
  Expr_Token Cur;
};

function i32 EXPR_OpPrec(Expr_Token expr_token){
  switch(expr_token.type){
    case Token_Plus: case Token_Minus:              return 1;
    case Token_Asterisk: case Token_ForwardSlash:   return 2;
    case Token_Caret:                               return 3;
    case Token_Percent:                             return 4;
    default: return 0;
  }
}

function bool EXPR_IsRight(Expr_Token expr_token){ return expr_token.type==Token_Caret; }

// Used for implicit multiplication, so Token_Minus isn't included
function bool EXPR_IsStartAtom(Expr_Token expr_token){
  switch(expr_token.type){
    case Token_OpenParen:
    case Token_Number:
    case Token_TAU:
    case Token_PI:
    case Token_E:
    case Token_Sin:
    case Token_Cos:
    case Token_Tan:
    case Token_Sqrt:
    case Token_Log:
    case Token_Ln:
    return true;
    default:
    return false;
  }
}

function Expr_Op EXPR_GetOp(Expr_Token expr_token){
  switch(expr_token.type){
    case Token_Plus:          return ADD;
    case Token_Minus:         return SUB;
    case Token_Asterisk:      return MUL;
    case Token_ForwardSlash:  return DIV;
    case Token_Percent:       return MOD;
    case Token_Caret:         return EXP;
    case Token_Sin:           return SIN;
    case Token_Cos:           return COS;
    case Token_Tan:           return TAN;
    case Token_Log:           return LOG;
    case Token_Ln:            return LN;
    case Token_Sqrt:          return SQRT;
    default: Assert(false); return CON;
  }
}

function bool EXPR_IsWhiteSpace(char c){ return (c == ' ' || c == '\r' || c == '\t'); }
function bool EXPR_IsNum(char c){ return ('0' <= c && c <= '9'); }

#define EXPR_StrMatch(Tokenizer, Str) EXPR_StrMatch_(Tokenizer, Str, sizeof(Str)-1)
function bool EXPR_StrMatch_(Expr_Tokenizer *T, const char *Str, const int StrCount){
  for(int i=0; i<StrCount; i++)
    if(T->At[i] == '\0' || T->At[i] != Str[i]){ return false; }
  T->At += StrCount;
  return true;
}
function void EXPR_ConsumeWhiteSpace(Expr_Tokenizer *T){ while(EXPR_IsWhiteSpace(*T->At)){ T->At++; } }

#pragma warning(disable : 4706)

function Expr_Token EXPR_GetToken(Expr_Tokenizer *Tokenizer){
  Expr_Token expr_token = {Token_Unknown, 0, 1};        // Default Expr_Token length of 1
  EXPR_ConsumeWhiteSpace(Tokenizer);
  char c = *(expr_token.text = Tokenizer->At++);
  switch(c){
    case '\0':      // Don't allow consuming EOS
    case '\n':      // '=' is for evaluating and outputting test cases
    case EOF:{ expr_token.type = Token_EndOfStream; expr_token.size=0; --Tokenizer->At;  } break;
    case '(':{ expr_token.type = Token_OpenParen;    } break;
    case ')':{ expr_token.type = Token_CloseParen;   } break;
    case '+':{ expr_token.type = Token_Plus;         } break;
    case '-':{ expr_token.type = Token_Minus;        } break;
    case '*':{ expr_token.type = Token_Asterisk;     } break;
    case '/':{ expr_token.type = Token_ForwardSlash; } break;
    case '^':{ expr_token.type = Token_Caret;        } break;
    case '%':{ expr_token.type = Token_Percent;      } break;
    case 'e':{ expr_token.type = Token_E;            } break;
    default:{
      --Tokenizer->At;
      if(EXPR_IsNum(c) || c == '.'){
        expr_token.type = Token_Number;
        bool D = false;     // Decimal
        while(EXPR_IsNum(*Tokenizer->At) || !D && (D |= (*Tokenizer->At=='.'))){ ++Tokenizer->At; }
      }
      else if(EXPR_StrMatch(Tokenizer, "sin("))  { expr_token.type = Token_Sin;  }
      else if(EXPR_StrMatch(Tokenizer, "cos("))  { expr_token.type = Token_Cos;  }
      else if(EXPR_StrMatch(Tokenizer, "tan("))  { expr_token.type = Token_Tan;  }
      else if(EXPR_StrMatch(Tokenizer, "sqrt(")) { expr_token.type = Token_Sqrt; }
      else if(EXPR_StrMatch(Tokenizer, "log("))  { expr_token.type = Token_Log;  }
      else if(EXPR_StrMatch(Tokenizer, "ln("))   { expr_token.type = Token_Ln;   }
      else if(EXPR_StrMatch(Tokenizer, "PI"))    { expr_token.type = Token_PI;   }
      else if(EXPR_StrMatch(Tokenizer, "TAU"))   { expr_token.type = Token_TAU;  }
      else if(EXPR_StrMatch(Tokenizer, "ANS"))   { expr_token.type = Token_ANS;  }

      else{ expr_token.type = Token_Unknown; }
      expr_token.size = i32(Tokenizer->At - expr_token.text);    // All valid default cases must correctly update Tokenizer->At
    } break;
  }
  return Tokenizer->Cur = expr_token;
}
function Expr_Token EXPR_PeekToken(Expr_Tokenizer *Tokenizer){
  Expr_Tokenizer Temp = *Tokenizer;
  return EXPR_GetToken(&Temp);
}
function void EXPR_ConsumePeeked(Expr_Tokenizer *Tokenizer, Expr_Token Peeked){
  Tokenizer->Cur = Peeked;
  Tokenizer->At = Peeked.text + Peeked.size;
}

function Expr_Node* EXPR_Addnode(Arena *arena, Expr_Op Op, Expr_Node *A=0, Expr_Node *B=0, f64 Value=0.0){
  Expr_Node *node = push_array(arena, Expr_Node, 1);
  *node = {Op, Value, A, B};
  return node;
}

global f64 expr_prev_ans;
function Expr_Node* EXPR_ParseExpression(Arena *arena, Expr_Tokenizer *T, Expr_Token_Type EndToken=Token_EndOfStream, Expr_Node *A=nullptr);

function Expr_Node* EXPR_ParseAtom(Arena *arena, Expr_Tokenizer *Tokenizer){
  Expr_Node *node = 0;
  Expr_Token expr_token = EXPR_GetToken(Tokenizer);
  switch(expr_token.type){
    case Token_OpenParen:{ node = EXPR_ParseExpression(arena, Tokenizer, Token_CloseParen);  } break;
    case Token_Number:{ node = EXPR_Addnode(arena, CON, 0, 0, atof(expr_token.text)); } break;
    case Token_ANS:   { node = EXPR_Addnode(arena, CON, 0, 0, expr_prev_ans); } break;
    case Token_TAU:   { node = EXPR_Addnode(arena, CON, 0, 0,   TAU64); } break;
    case Token_PI:    { node = EXPR_Addnode(arena, CON, 0, 0,    PI64); } break;
    case Token_E:     { node = EXPR_Addnode(arena, CON, 0, 0,     E64); } break;

    case Token_Minus: { node = EXPR_Addnode(arena, NEG, EXPR_ParseAtom(arena, Tokenizer)); } break;

    case Token_Sin:
    case Token_Cos:
    case Token_Tan:
    case Token_Log:
    case Token_Ln:
    case Token_Sqrt:{
      node = EXPR_Addnode(arena, EXPR_GetOp(expr_token), EXPR_ParseExpression(arena, Tokenizer, Token_CloseParen));
    } break;

    default:{ return 0; }
  }

  if(!EXPR_IsWhiteSpace(Tokenizer->At[0]) && EXPR_IsStartAtom(EXPR_PeekToken(Tokenizer))){
    node = EXPR_Addnode(arena, MUL, node, EXPR_ParseAtom(arena, Tokenizer));   // Implicit multiplication
  }
  if(node && node->type != CON){
    f64 Val = EXPR_Evaluate(node);
    node = EXPR_Addnode(arena, CON, 0, 0, Val);
  }
  return node;
}

function Expr_Node* EXPR_ParseExpression(Arena *arena, Expr_Tokenizer *Tokenizer, Expr_Token_Type EndTokenType, Expr_Node *PreCalc){
  Expr_Node *A = (PreCalc ? PreCalc : EXPR_ParseAtom(arena, Tokenizer));
  if(A == nullptr){ return 0; }
  while(true){
    Expr_Token expr_token = EXPR_GetToken(Tokenizer);
    if(expr_token.type == EndTokenType){ break; }

    int Precedence1 = EXPR_OpPrec(expr_token);
    if(!Precedence1){ return 0; }

    Expr_Node *B = EXPR_ParseAtom(arena, Tokenizer);
    if(B == nullptr){ return 0; }

    Expr_Token next_token = EXPR_PeekToken(Tokenizer);
    if(next_token.type == EndTokenType){
      EXPR_ConsumePeeked(Tokenizer, next_token);
      return EXPR_Addnode(arena, EXPR_GetOp(expr_token), A, B);
    }

    int Precendence2 = EXPR_OpPrec(next_token);
    if(!Precendence2){ return 0; }

    if(Precedence1 < Precendence2 || (expr_token.type == next_token.type && EXPR_IsRight(expr_token))){
      B = EXPR_ParseExpression(arena, Tokenizer, EndTokenType, B);
      if(Tokenizer->Cur.type == EndTokenType){ Tokenizer->At = Tokenizer->Cur.text; }
    }

    A = EXPR_Addnode(arena, EXPR_GetOp(expr_token), A, B);
  }
  return A;
}

function f64 EXPR_ParseString(Arena *arena, const char *Expression){
  Expr_Tokenizer Tokenizer = {Expression};
  Expr_Token expr_token = EXPR_PeekToken(&Tokenizer);
  if(expr_token.type == Token_EndOfStream){ return 0.0; }

  Expr_Node *A = (EXPR_OpPrec(expr_token) > 0 ? EXPR_Addnode(arena, CON,0,0,expr_prev_ans) : nullptr);
  Expr_Node *node = EXPR_ParseExpression(arena, &Tokenizer, Token_EndOfStream, A);
  if(node){
    expr_prev_ans = EXPR_Evaluate(node);
    if(isnan(expr_prev_ans)){ expr_prev_ans = 0.0; return NAN; }
    if(isinf(expr_prev_ans)){ expr_prev_ans = 0.0; return INFINITY; }
    if(fabs(expr_prev_ans) <= 6.1232339957367660e-17){ expr_prev_ans = 0.0; }

    return expr_prev_ans;
  }
  expr_prev_ans = 0.0;
  return NAN;
}
