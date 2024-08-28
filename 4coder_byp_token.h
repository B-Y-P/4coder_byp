
#if !defined(FCODER_BYP_TOKEN_H)
#define FCODER_BYP_TOKEN_H

enum{
    byp_TokenKind_Primitive = 16,
    byp_TokenKind_ControlFlow = 17,
    byp_TokenKind_Struct = 18,
};

char *byp_token_base_kind_names[] ={
    "EOF",
    "Whitespace",
    "LexError",
    "Comment",
    "Keyword",
    "Preprocessor",
    "Identifier",
    "Operator",
    "LiteralInteger",
    "LiteralFloat",
    "LiteralString",
    "ScopeOpen",
    "ScopeClose",
    "ParentheticalOpen",
    "ParentheticalClose",
    "Primitive",
    "ControlFlow",
    "Struct",
};


#endif
