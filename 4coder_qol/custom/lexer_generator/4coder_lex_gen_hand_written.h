struct Lexeme_Table_Value{
  Token_Base_Kind base_kind;
  u16 sub_kind;
};

struct Lexeme_Table_Lookup{
  b32 found_match;
  Token_Base_Kind base_kind;
  u16 sub_kind;
};