#if !defined(FCODER_LEX_GEN_HAND_WRITTEN_TYPES)
#define FCODER_LEX_GEN_HAND_WRITTEN_TYPES

struct Lexeme_Table_Value{
    Token_Base_Kind base_kind;
    u16 sub_kind;
};

struct Lexeme_Table_Lookup{
    b32 found_match;
    Token_Base_Kind base_kind;
    u16 sub_kind;
};

#endif
typedef u16 Token_Cpp_Kind;
enum{
TokenCppKind_EOF = 0,
TokenCppKind_Whitespace = 1,
TokenCppKind_LexError = 2,
TokenCppKind_BlockComment = 3,
TokenCppKind_LineComment = 4,
TokenCppKind_Backslash = 5,
TokenCppKind_LiteralInteger = 6,
TokenCppKind_LiteralIntegerU = 7,
TokenCppKind_LiteralIntegerL = 8,
TokenCppKind_LiteralIntegerUL = 9,
TokenCppKind_LiteralIntegerLL = 10,
TokenCppKind_LiteralIntegerULL = 11,
TokenCppKind_LiteralIntegerHex = 12,
TokenCppKind_LiteralIntegerHexU = 13,
TokenCppKind_LiteralIntegerHexL = 14,
TokenCppKind_LiteralIntegerHexUL = 15,
TokenCppKind_LiteralIntegerHexLL = 16,
TokenCppKind_LiteralIntegerHexULL = 17,
TokenCppKind_LiteralIntegerOct = 18,
TokenCppKind_LiteralIntegerOctU = 19,
TokenCppKind_LiteralIntegerOctL = 20,
TokenCppKind_LiteralIntegerOctUL = 21,
TokenCppKind_LiteralIntegerOctLL = 22,
TokenCppKind_LiteralIntegerOctULL = 23,
TokenCppKind_LiteralFloat32 = 24,
TokenCppKind_LiteralFloat64 = 25,
TokenCppKind_LiteralString = 26,
TokenCppKind_LiteralStringWide = 27,
TokenCppKind_LiteralStringUTF8 = 28,
TokenCppKind_LiteralStringUTF16 = 29,
TokenCppKind_LiteralStringUTF32 = 30,
TokenCppKind_LiteralStringRaw = 31,
TokenCppKind_LiteralStringWideRaw = 32,
TokenCppKind_LiteralStringUTF8Raw = 33,
TokenCppKind_LiteralStringUTF16Raw = 34,
TokenCppKind_LiteralStringUTF32Raw = 35,
TokenCppKind_LiteralCharacter = 36,
TokenCppKind_LiteralCharacterWide = 37,
TokenCppKind_LiteralCharacterUTF8 = 38,
TokenCppKind_LiteralCharacterUTF16 = 39,
TokenCppKind_LiteralCharacterUTF32 = 40,
TokenCppKind_PPIncludeFile = 41,
TokenCppKind_PPErrorMessage = 42,
TokenCppKind_KeywordGeneric = 43,
TokenCppKind_BraceOp = 44,
TokenCppKind_BraceCl = 45,
TokenCppKind_ParenOp = 46,
TokenCppKind_BrackOp = 47,
TokenCppKind_ParenCl = 48,
TokenCppKind_BrackCl = 49,
TokenCppKind_Semicolon = 50,
TokenCppKind_Colon = 51,
TokenCppKind_DotDotDot = 52,
TokenCppKind_ColonColon = 53,
TokenCppKind_PlusPlus = 54,
TokenCppKind_MinusMinus = 55,
TokenCppKind_Dot = 56,
TokenCppKind_Arrow = 57,
TokenCppKind_Plus = 58,
TokenCppKind_Minus = 59,
TokenCppKind_Not = 60,
TokenCppKind_Tilde = 61,
TokenCppKind_Star = 62,
TokenCppKind_And = 63,
TokenCppKind_DotStar = 64,
TokenCppKind_ArrowStar = 65,
TokenCppKind_Div = 66,
TokenCppKind_Mod = 67,
TokenCppKind_LeftLeft = 68,
TokenCppKind_RightRight = 69,
TokenCppKind_Compare = 70,
TokenCppKind_Less = 71,
TokenCppKind_LessEq = 72,
TokenCppKind_Grtr = 73,
TokenCppKind_GrtrEq = 74,
TokenCppKind_EqEq = 75,
TokenCppKind_NotEq = 76,
TokenCppKind_Xor = 77,
TokenCppKind_Or = 78,
TokenCppKind_AndAnd = 79,
TokenCppKind_OrOr = 80,
TokenCppKind_Ternary = 81,
TokenCppKind_Eq = 82,
TokenCppKind_PlusEq = 83,
TokenCppKind_MinusEq = 84,
TokenCppKind_StarEq = 85,
TokenCppKind_DivEq = 86,
TokenCppKind_ModEq = 87,
TokenCppKind_LeftLeftEq = 88,
TokenCppKind_RightRightEq = 89,
TokenCppKind_Comma = 90,
TokenCppKind_PPStringify = 91,
TokenCppKind_PPConcat = 92,
TokenCppKind_Const = 93,
TokenCppKind_ConstExpr = 94,
TokenCppKind_Volatile = 95,
TokenCppKind_Asm = 96,
TokenCppKind_StaticAssert = 97,
TokenCppKind_ConstCast = 98,
TokenCppKind_DynamicCast = 99,
TokenCppKind_ReinterpretCast = 100,
TokenCppKind_StaticCast = 101,
TokenCppKind_Template = 102,
TokenCppKind_Typename = 103,
TokenCppKind_Friend = 104,
TokenCppKind_Namespace = 105,
TokenCppKind_Private = 106,
TokenCppKind_Protected = 107,
TokenCppKind_Public = 108,
TokenCppKind_Using = 109,
TokenCppKind_Extern = 110,
TokenCppKind_Export = 111,
TokenCppKind_Inline = 112,
TokenCppKind_Static = 113,
TokenCppKind_Virtual = 114,
TokenCppKind_AlignAs = 115,
TokenCppKind_Explicit = 116,
TokenCppKind_NoExcept = 117,
TokenCppKind_Operator = 118,
TokenCppKind_Register = 119,
TokenCppKind_This = 120,
TokenCppKind_ThreadLocal = 121,
TokenCppKind_SizeOf = 122,
TokenCppKind_AlignOf = 123,
TokenCppKind_DeclType = 124,
TokenCppKind_TypeID = 125,
TokenCppKind_New = 126,
TokenCppKind_Delete = 127,
TokenCppKind_Class = 128,
TokenCppKind_Enum = 129,
TokenCppKind_Struct = 130,
TokenCppKind_Typedef = 131,
TokenCppKind_Union = 132,
TokenCppKind_Auto = 133,
TokenCppKind_Void = 134,
TokenCppKind_Bool = 135,
TokenCppKind_Char = 136,
TokenCppKind_Int = 137,
TokenCppKind_Float = 138,
TokenCppKind_Double = 139,
TokenCppKind_Long = 140,
TokenCppKind_Short = 141,
TokenCppKind_Unsigned = 142,
TokenCppKind_Signed = 143,
TokenCppKind_Break = 144,
TokenCppKind_Case = 145,
TokenCppKind_Catch = 146,
TokenCppKind_Continue = 147,
TokenCppKind_Default = 148,
TokenCppKind_Do = 149,
TokenCppKind_Else = 150,
TokenCppKind_For = 151,
TokenCppKind_Goto = 152,
TokenCppKind_If = 153,
TokenCppKind_Return = 154,
TokenCppKind_Switch = 155,
TokenCppKind_Try = 156,
TokenCppKind_While = 157,
TokenCppKind_LiteralTrue = 158,
TokenCppKind_LiteralFalse = 159,
TokenCppKind_LiteralNull = 160,
TokenCppKind_NullPtr = 161,
TokenCppKind_Identifier = 162,
TokenCppKind_PPInclude = 163,
TokenCppKind_PPVersion = 164,
TokenCppKind_PPDefine = 165,
TokenCppKind_PPUndef = 166,
TokenCppKind_PPIf = 167,
TokenCppKind_PPIfDef = 168,
TokenCppKind_PPIfNDef = 169,
TokenCppKind_PPElse = 170,
TokenCppKind_PPElIf = 171,
TokenCppKind_PPEndIf = 172,
TokenCppKind_PPError = 173,
TokenCppKind_PPImport = 174,
TokenCppKind_PPUsing = 175,
TokenCppKind_PPLine = 176,
TokenCppKind_PPPragma = 177,
TokenCppKind_PPUnknown = 178,
TokenCppKind_PPDefined = 179,
TokenCppKind_COUNT = 180,
};
char *token_cpp_kind_names[] = {
"EOF",
"Whitespace",
"LexError",
"BlockComment",
"LineComment",
"Backslash",
"LiteralInteger",
"LiteralIntegerU",
"LiteralIntegerL",
"LiteralIntegerUL",
"LiteralIntegerLL",
"LiteralIntegerULL",
"LiteralIntegerHex",
"LiteralIntegerHexU",
"LiteralIntegerHexL",
"LiteralIntegerHexUL",
"LiteralIntegerHexLL",
"LiteralIntegerHexULL",
"LiteralIntegerOct",
"LiteralIntegerOctU",
"LiteralIntegerOctL",
"LiteralIntegerOctUL",
"LiteralIntegerOctLL",
"LiteralIntegerOctULL",
"LiteralFloat32",
"LiteralFloat64",
"LiteralString",
"LiteralStringWide",
"LiteralStringUTF8",
"LiteralStringUTF16",
"LiteralStringUTF32",
"LiteralStringRaw",
"LiteralStringWideRaw",
"LiteralStringUTF8Raw",
"LiteralStringUTF16Raw",
"LiteralStringUTF32Raw",
"LiteralCharacter",
"LiteralCharacterWide",
"LiteralCharacterUTF8",
"LiteralCharacterUTF16",
"LiteralCharacterUTF32",
"PPIncludeFile",
"PPErrorMessage",
"KeywordGeneric",
"BraceOp",
"BraceCl",
"ParenOp",
"BrackOp",
"ParenCl",
"BrackCl",
"Semicolon",
"Colon",
"DotDotDot",
"ColonColon",
"PlusPlus",
"MinusMinus",
"Dot",
"Arrow",
"Plus",
"Minus",
"Not",
"Tilde",
"Star",
"And",
"DotStar",
"ArrowStar",
"Div",
"Mod",
"LeftLeft",
"RightRight",
"Compare",
"Less",
"LessEq",
"Grtr",
"GrtrEq",
"EqEq",
"NotEq",
"Xor",
"Or",
"AndAnd",
"OrOr",
"Ternary",
"Eq",
"PlusEq",
"MinusEq",
"StarEq",
"DivEq",
"ModEq",
"LeftLeftEq",
"RightRightEq",
"Comma",
"PPStringify",
"PPConcat",
"Const",
"ConstExpr",
"Volatile",
"Asm",
"StaticAssert",
"ConstCast",
"DynamicCast",
"ReinterpretCast",
"StaticCast",
"Template",
"Typename",
"Friend",
"Namespace",
"Private",
"Protected",
"Public",
"Using",
"Extern",
"Export",
"Inline",
"Static",
"Virtual",
"AlignAs",
"Explicit",
"NoExcept",
"Operator",
"Register",
"This",
"ThreadLocal",
"SizeOf",
"AlignOf",
"DeclType",
"TypeID",
"New",
"Delete",
"Class",
"Enum",
"Struct",
"Typedef",
"Union",
"Auto",
"Void",
"Bool",
"Char",
"Int",
"Float",
"Double",
"Long",
"Short",
"Unsigned",
"Signed",
"Break",
"Case",
"Catch",
"Continue",
"Default",
"Do",
"Else",
"For",
"Goto",
"If",
"Return",
"Switch",
"Try",
"While",
"LiteralTrue",
"LiteralFalse",
"LiteralNull",
"NullPtr",
"Identifier",
"PPInclude",
"PPVersion",
"PPDefine",
"PPUndef",
"PPIf",
"PPIfDef",
"PPIfNDef",
"PPElse",
"PPElIf",
"PPEndIf",
"PPError",
"PPImport",
"PPUsing",
"PPLine",
"PPPragma",
"PPUnknown",
"PPDefined",
};
