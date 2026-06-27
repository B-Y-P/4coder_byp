#if !defined(FCODER_LEX_GEN_HAND_WRITTEN)
#define FCODER_LEX_GEN_HAND_WRITTEN

internal u64
lexeme_hash(u64 seed, u8 *ptr, u64 size){
  u64 result = 0;
  for (u64 i = 0; i < size; i += 1, ptr += 1){
    result ^= ((*ptr) ^ result*59) + seed;
  }
  return(result);
}

internal Lexeme_Table_Lookup
lexeme_table_lookup(u64 *hash_array, String_Const_u8 *key_array,
                    Lexeme_Table_Value *value_array, i32 slot_count, u64 seed,
                    u8 *ptr, u64 size){
  Lexeme_Table_Lookup result = {};
  u64 hash = lexeme_hash(seed, ptr, size);
  u64 comparison_hash = hash | 1;
  i32 first_index = (hash % slot_count);
  i32 index = first_index;
  for (;;){
    if (hash_array[index] == comparison_hash){
      if (string_match(SCu8(ptr, size), key_array[index])){
        result.found_match = true;
        result.base_kind = value_array[index].base_kind;
        result.sub_kind = value_array[index].sub_kind;
        break;
      }
    }
    else if (hash_array[index] == 0){
      break;
    }
    index += 1;
    if (index == slot_count){
      index = 0;
    }
    if (index == first_index){
      break;
    }
  }
  return(result);
}

#endif
u64 cpp_main_keys_hash_array[157] = {
  0x0000000000000000,0x0000000000000000,0x8b11b5ad5b598fe9,0x95e09a05de3118ef,
  0x95e09922f99eaa6f,0x0000000000000000,0x0000000000000000,0x0000000000000000,
  0x95e1dbfae2aaf9df,0x777ab1cefd7e0c91,0xcc4ba93118cbe631,0xcc4ba9311ecf3261,
  0x95e0be2e45fd6241,0x0000000000000000,0x8b11b5804bf01607,0x8b11b5917bf35e3b,
  0x0000000000000000,0xb5ef4f9880459037,0x95e090f011c846d1,0x0000000000000000,
  0x95e0856bbda1fd71,0x0000000000000000,0x902a29cd1bdfb267,0xec17eb53d48ce513,
  0x0000000000000000,0x0000000000000000,0x0000000000000000,0xb5ef4f98804590df,
  0x0000000000000000,0xec17eb53d48b6c8b,0xec17eb53d48c4295,0x0000000000000000,
  0xcc4ba93118e14075,0x0000000000000000,0x0000000000000000,0x0000000000000000,
  0xcc4ba9312b00b357,0x8b11b5917b1e6c5d,0xb5ef432d4622acc9,0x0000000000000000,
  0x95e0afc27eec5627,0x0000000000000000,0x0000000000000000,0x0000000000000000,
  0xcc4ba931299d2601,0x777ab1cefd21c481,0x0000000000000000,0x0000000000000000,
  0x0000000000000000,0x0000000000000000,0x8b11b5917bf3aba5,0x777ab1cefec24461,
  0x29554c8ba6cd3e65,0x0000000000000000,0x0000000000000000,0x0000000000000000,
  0xb5ef46aead605ff5,0x0000000000000000,0x8b11b5817060e8f7,0x0000000000000000,
  0x0000000000000000,0xb5ef4869a36364c1,0x777ab1cef8fedee7,0x0000000000000000,
  0x0000000000000000,0x7efc2dd3ab1c1e95,0x8b11b5ac7025808f,0xb5ef475e2108378f,
  0x777ab1cefd27a941,0x833507d59ca7471b,0x0000000000000000,0x0000000000000000,
  0x0000000000000000,0xb5ef4862ad161dfb,0xb5ef46a1551008c1,0xec17eb53d48b7727,
  0x95e0be2ce4c6a39d,0x0000000000000000,0x0000000000000000,0x95e0b455e950c4e7,
  0x777ab1cefe9c5841,0x0000000000000000,0x0000000000000000,0x0000000000000000,
  0xb5ef48c8a34aa441,0xb5ef4919b5344041,0xb5ef46a1556a8781,0xb5ef46bec9458b0f,
  0x0000000000000000,0x8b11b5ad58e082d7,0x295b64a9027859f1,0xb5ef46a15568d241,
  0x0000000000000000,0xcc4ba9311bd9bed3,0xb5ef4935b068d947,0x8b11b5ae69895c69,
  0xcc4ba93126d34103,0x0000000000000000,0x0000000000000000,0x777ab1cefe99fa67,
  0xcc4ba9311e3a23c1,0x0000000000000000,0x95e0ac406cad5a41,0x0000000000000000,
  0x0000000000000000,0x777ab1cefec2a095,0x777ab1cefec9efb5,0x8b11b5aaa322bd4f,
  0x0000000000000000,0x8b11b5afc4442ea1,0x8b11b5ad3bbfb90f,0x8b11b5ae4fb6bc4d,
  0x0000000000000000,0x8b11b5aeb035d435,0x8b11b5ae410f5db7,0x0000000000000000,
  0xec17eb53d48d83ab,0x0000000000000000,0x833507d59ca74129,0x95e0be2e45f3af81,
  0x0000000000000000,0x777ab1cefec94f17,0x30ba1d84b06a2e15,0x0000000000000000,
  0x0000000000000000,0x0000000000000000,0x0000000000000000,0x91c78ef032734e95,
  0x0000000000000000,0x0000000000000000,0xcc4ba93119ffef97,0x295b076bcc0f803d,
  0x95e0ac406c841b81,0x95e0d5644682bd2b,0x0000000000000000,0x95e0ac42147c9c5d,
  0x777ab1cefe9b447d,0x0000000000000000,0x777ab1cefecb57d5,0x0000000000000000,
  0xece7c2b48d169b39,0x0000000000000000,0x033cb6d161d07e15,0x0000000000000000,
  0x0000000000000000,0xcc4ba93127544d6f,0x0000000000000000,0x0000000000000000,
  0x0000000000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000,
  0x0000000000000000,0x2972fc268a66fddd,0x8b11b58170af60c1,0x95e0ac406cf348c1,
  0x0000000000000000,
};
u8 cpp_main_keys_key_array_2[] = {0x65,0x78,0x74,0x65,0x72,0x6e,};
u8 cpp_main_keys_key_array_3[] = {0x76,0x6f,0x6c,0x61,0x74,0x69,0x6c,0x65,};
u8 cpp_main_keys_key_array_4[] = {0x74,0x65,0x6d,0x70,0x6c,0x61,0x74,0x65,};
u8 cpp_main_keys_key_array_8[] = {0x6e,0x6f,0x65,0x78,0x63,0x65,0x70,0x74,};
u8 cpp_main_keys_key_array_9[] = {0x61,0x75,0x74,0x6f,};
u8 cpp_main_keys_key_array_10[] = {0x63,0x6f,0x6e,0x73,0x74,};
u8 cpp_main_keys_key_array_11[] = {0x66,0x6c,0x6f,0x61,0x74,};
u8 cpp_main_keys_key_array_12[] = {0x63,0x68,0x61,0x72,0x31,0x36,0x5f,0x74,};
u8 cpp_main_keys_key_array_14[] = {0x70,0x75,0x62,0x6c,0x69,0x63,};
u8 cpp_main_keys_key_array_15[] = {0x73,0x69,0x7a,0x65,0x6f,0x66,};
u8 cpp_main_keys_key_array_17[] = {0x61,0x6c,0x69,0x67,0x6e,0x61,0x73,};
u8 cpp_main_keys_key_array_18[] = {0x65,0x78,0x70,0x6c,0x69,0x63,0x69,0x74,};
u8 cpp_main_keys_key_array_20[] = {0x74,0x79,0x70,0x65,0x6e,0x61,0x6d,0x65,};
u8 cpp_main_keys_key_array_22[] = {0x73,0x74,0x61,0x74,0x69,0x63,0x5f,0x61,0x73,0x73,0x65,0x72,0x74,};
u8 cpp_main_keys_key_array_23[] = {0x74,0x72,0x79,};
u8 cpp_main_keys_key_array_27[] = {0x61,0x6c,0x69,0x67,0x6e,0x6f,0x66,};
u8 cpp_main_keys_key_array_29[] = {0x6e,0x65,0x77,};
u8 cpp_main_keys_key_array_30[] = {0x61,0x73,0x6d,};
u8 cpp_main_keys_key_array_32[] = {0x63,0x61,0x74,0x63,0x68,};
u8 cpp_main_keys_key_array_36[] = {0x73,0x68,0x6f,0x72,0x74,};
u8 cpp_main_keys_key_array_37[] = {0x73,0x69,0x67,0x6e,0x65,0x64,};
u8 cpp_main_keys_key_array_38[] = {0x76,0x69,0x72,0x74,0x75,0x61,0x6c,};
u8 cpp_main_keys_key_array_40[] = {0x72,0x65,0x67,0x69,0x73,0x74,0x65,0x72,};
u8 cpp_main_keys_key_array_44[] = {0x75,0x73,0x69,0x6e,0x67,};
u8 cpp_main_keys_key_array_45[] = {0x6c,0x6f,0x6e,0x67,};
u8 cpp_main_keys_key_array_50[] = {0x73,0x69,0x7a,0x65,0x5f,0x74,};
u8 cpp_main_keys_key_array_51[] = {0x65,0x6c,0x73,0x65,};
u8 cpp_main_keys_key_array_52[] = {0x63,0x6f,0x6e,0x73,0x74,0x65,0x78,0x70,0x72,};
u8 cpp_main_keys_key_array_56[] = {0x6e,0x75,0x6c,0x6c,0x70,0x74,0x72,};
u8 cpp_main_keys_key_array_58[] = {0x69,0x6e,0x6c,0x69,0x6e,0x65,};
u8 cpp_main_keys_key_array_61[] = {0x77,0x63,0x68,0x61,0x72,0x5f,0x74,};
u8 cpp_main_keys_key_array_62[] = {0x4e,0x55,0x4c,0x4c,};
u8 cpp_main_keys_key_array_65[] = {0x72,0x65,0x69,0x6e,0x74,0x65,0x72,0x70,0x72,0x65,0x74,0x5f,0x63,0x61,0x73,0x74,};
u8 cpp_main_keys_key_array_66[] = {0x66,0x72,0x69,0x65,0x6e,0x64,};
u8 cpp_main_keys_key_array_67[] = {0x70,0x72,0x69,0x76,0x61,0x74,0x65,};
u8 cpp_main_keys_key_array_68[] = {0x67,0x6f,0x74,0x6f,};
u8 cpp_main_keys_key_array_69[] = {0x69,0x66,};
u8 cpp_main_keys_key_array_73[] = {0x74,0x79,0x70,0x65,0x64,0x65,0x66,};
u8 cpp_main_keys_key_array_74[] = {0x69,0x6e,0x74,0x36,0x34,0x5f,0x74,};
u8 cpp_main_keys_key_array_75[] = {0x69,0x6e,0x74,};
u8 cpp_main_keys_key_array_76[] = {0x63,0x6f,0x6e,0x74,0x69,0x6e,0x75,0x65,};
u8 cpp_main_keys_key_array_79[] = {0x64,0x65,0x63,0x6c,0x74,0x79,0x70,0x65,};
u8 cpp_main_keys_key_array_80[] = {0x76,0x6f,0x69,0x64,};
u8 cpp_main_keys_key_array_84[] = {0x75,0x69,0x6e,0x74,0x38,0x5f,0x74,};
u8 cpp_main_keys_key_array_85[] = {0x63,0x68,0x61,0x72,0x38,0x5f,0x74,};
u8 cpp_main_keys_key_array_86[] = {0x69,0x6e,0x74,0x33,0x32,0x5f,0x74,};
u8 cpp_main_keys_key_array_87[] = {0x6d,0x75,0x74,0x61,0x62,0x6c,0x65,};
u8 cpp_main_keys_key_array_89[] = {0x65,0x78,0x70,0x6f,0x72,0x74,};
u8 cpp_main_keys_key_array_90[] = {0x70,0x74,0x72,0x64,0x69,0x66,0x66,0x5f,0x74,};
u8 cpp_main_keys_key_array_91[] = {0x69,0x6e,0x74,0x31,0x36,0x5f,0x74,};
u8 cpp_main_keys_key_array_93[] = {0x62,0x72,0x65,0x61,0x6b,};
u8 cpp_main_keys_key_array_94[] = {0x64,0x65,0x66,0x61,0x75,0x6c,0x74,};
u8 cpp_main_keys_key_array_95[] = {0x72,0x65,0x74,0x75,0x72,0x6e,};
u8 cpp_main_keys_key_array_96[] = {0x75,0x6e,0x69,0x6f,0x6e,};
u8 cpp_main_keys_key_array_99[] = {0x74,0x68,0x69,0x73,};
u8 cpp_main_keys_key_array_100[] = {0x66,0x61,0x6c,0x73,0x65,};
u8 cpp_main_keys_key_array_102[] = {0x75,0x69,0x6e,0x74,0x31,0x36,0x5f,0x74,};
u8 cpp_main_keys_key_array_105[] = {0x65,0x6e,0x75,0x6d,};
u8 cpp_main_keys_key_array_106[] = {0x63,0x61,0x73,0x65,};
u8 cpp_main_keys_key_array_107[] = {0x64,0x65,0x6c,0x65,0x74,0x65,};
u8 cpp_main_keys_key_array_109[] = {0x74,0x79,0x70,0x65,0x69,0x64,};
u8 cpp_main_keys_key_array_110[] = {0x64,0x6f,0x75,0x62,0x6c,0x65,};
u8 cpp_main_keys_key_array_111[] = {0x73,0x74,0x72,0x75,0x63,0x74,};
u8 cpp_main_keys_key_array_113[] = {0x73,0x77,0x69,0x74,0x63,0x68,};
u8 cpp_main_keys_key_array_114[] = {0x73,0x74,0x61,0x74,0x69,0x63,};
u8 cpp_main_keys_key_array_116[] = {0x66,0x6f,0x72,};
u8 cpp_main_keys_key_array_118[] = {0x64,0x6f,};
u8 cpp_main_keys_key_array_119[] = {0x63,0x68,0x61,0x72,0x33,0x32,0x5f,0x74,};
u8 cpp_main_keys_key_array_121[] = {0x63,0x68,0x61,0x72,};
u8 cpp_main_keys_key_array_122[] = {0x64,0x79,0x6e,0x61,0x6d,0x69,0x63,0x5f,0x63,0x61,0x73,0x74,};
u8 cpp_main_keys_key_array_127[] = {0x63,0x6f,0x6e,0x73,0x74,0x5f,0x63,0x61,0x73,0x74,};
u8 cpp_main_keys_key_array_130[] = {0x63,0x6c,0x61,0x73,0x73,};
u8 cpp_main_keys_key_array_131[] = {0x70,0x72,0x6f,0x74,0x65,0x63,0x74,0x65,0x64,};
u8 cpp_main_keys_key_array_132[] = {0x75,0x69,0x6e,0x74,0x33,0x32,0x5f,0x74,};
u8 cpp_main_keys_key_array_133[] = {0x6f,0x70,0x65,0x72,0x61,0x74,0x6f,0x72,};
u8 cpp_main_keys_key_array_135[] = {0x75,0x6e,0x73,0x69,0x67,0x6e,0x65,0x64,};
u8 cpp_main_keys_key_array_136[] = {0x74,0x72,0x75,0x65,};
u8 cpp_main_keys_key_array_138[] = {0x62,0x6f,0x6f,0x6c,};
u8 cpp_main_keys_key_array_140[] = {0x74,0x68,0x72,0x65,0x61,0x64,0x5f,0x6c,0x6f,0x63,0x61,0x6c,};
u8 cpp_main_keys_key_array_142[] = {0x73,0x74,0x61,0x74,0x69,0x63,0x5f,0x63,0x61,0x73,0x74,};
u8 cpp_main_keys_key_array_145[] = {0x77,0x68,0x69,0x6c,0x65,};
u8 cpp_main_keys_key_array_153[] = {0x6e,0x61,0x6d,0x65,0x73,0x70,0x61,0x63,0x65,};
u8 cpp_main_keys_key_array_154[] = {0x69,0x6e,0x74,0x38,0x5f,0x74,};
u8 cpp_main_keys_key_array_155[] = {0x75,0x69,0x6e,0x74,0x36,0x34,0x5f,0x74,};
String_Const_u8 cpp_main_keys_key_array[157] = {
  {0, 0},
  {0, 0},
  {cpp_main_keys_key_array_2, 6},
  {cpp_main_keys_key_array_3, 8},
  {cpp_main_keys_key_array_4, 8},
  {0, 0},
  {0, 0},
  {0, 0},
  {cpp_main_keys_key_array_8, 8},
  {cpp_main_keys_key_array_9, 4},
  {cpp_main_keys_key_array_10, 5},
  {cpp_main_keys_key_array_11, 5},
  {cpp_main_keys_key_array_12, 8},
  {0, 0},
  {cpp_main_keys_key_array_14, 6},
  {cpp_main_keys_key_array_15, 6},
  {0, 0},
  {cpp_main_keys_key_array_17, 7},
  {cpp_main_keys_key_array_18, 8},
  {0, 0},
  {cpp_main_keys_key_array_20, 8},
  {0, 0},
  {cpp_main_keys_key_array_22, 13},
  {cpp_main_keys_key_array_23, 3},
  {0, 0},
  {0, 0},
  {0, 0},
  {cpp_main_keys_key_array_27, 7},
  {0, 0},
  {cpp_main_keys_key_array_29, 3},
  {cpp_main_keys_key_array_30, 3},
  {0, 0},
  {cpp_main_keys_key_array_32, 5},
  {0, 0},
  {0, 0},
  {0, 0},
  {cpp_main_keys_key_array_36, 5},
  {cpp_main_keys_key_array_37, 6},
  {cpp_main_keys_key_array_38, 7},
  {0, 0},
  {cpp_main_keys_key_array_40, 8},
  {0, 0},
  {0, 0},
  {0, 0},
  {cpp_main_keys_key_array_44, 5},
  {cpp_main_keys_key_array_45, 4},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {cpp_main_keys_key_array_50, 6},
  {cpp_main_keys_key_array_51, 4},
  {cpp_main_keys_key_array_52, 9},
  {0, 0},
  {0, 0},
  {0, 0},
  {cpp_main_keys_key_array_56, 7},
  {0, 0},
  {cpp_main_keys_key_array_58, 6},
  {0, 0},
  {0, 0},
  {cpp_main_keys_key_array_61, 7},
  {cpp_main_keys_key_array_62, 4},
  {0, 0},
  {0, 0},
  {cpp_main_keys_key_array_65, 16},
  {cpp_main_keys_key_array_66, 6},
  {cpp_main_keys_key_array_67, 7},
  {cpp_main_keys_key_array_68, 4},
  {cpp_main_keys_key_array_69, 2},
  {0, 0},
  {0, 0},
  {0, 0},
  {cpp_main_keys_key_array_73, 7},
  {cpp_main_keys_key_array_74, 7},
  {cpp_main_keys_key_array_75, 3},
  {cpp_main_keys_key_array_76, 8},
  {0, 0},
  {0, 0},
  {cpp_main_keys_key_array_79, 8},
  {cpp_main_keys_key_array_80, 4},
  {0, 0},
  {0, 0},
  {0, 0},
  {cpp_main_keys_key_array_84, 7},
  {cpp_main_keys_key_array_85, 7},
  {cpp_main_keys_key_array_86, 7},
  {cpp_main_keys_key_array_87, 7},
  {0, 0},
  {cpp_main_keys_key_array_89, 6},
  {cpp_main_keys_key_array_90, 9},
  {cpp_main_keys_key_array_91, 7},
  {0, 0},
  {cpp_main_keys_key_array_93, 5},
  {cpp_main_keys_key_array_94, 7},
  {cpp_main_keys_key_array_95, 6},
  {cpp_main_keys_key_array_96, 5},
  {0, 0},
  {0, 0},
  {cpp_main_keys_key_array_99, 4},
  {cpp_main_keys_key_array_100, 5},
  {0, 0},
  {cpp_main_keys_key_array_102, 8},
  {0, 0},
  {0, 0},
  {cpp_main_keys_key_array_105, 4},
  {cpp_main_keys_key_array_106, 4},
  {cpp_main_keys_key_array_107, 6},
  {0, 0},
  {cpp_main_keys_key_array_109, 6},
  {cpp_main_keys_key_array_110, 6},
  {cpp_main_keys_key_array_111, 6},
  {0, 0},
  {cpp_main_keys_key_array_113, 6},
  {cpp_main_keys_key_array_114, 6},
  {0, 0},
  {cpp_main_keys_key_array_116, 3},
  {0, 0},
  {cpp_main_keys_key_array_118, 2},
  {cpp_main_keys_key_array_119, 8},
  {0, 0},
  {cpp_main_keys_key_array_121, 4},
  {cpp_main_keys_key_array_122, 12},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {cpp_main_keys_key_array_127, 10},
  {0, 0},
  {0, 0},
  {cpp_main_keys_key_array_130, 5},
  {cpp_main_keys_key_array_131, 9},
  {cpp_main_keys_key_array_132, 8},
  {cpp_main_keys_key_array_133, 8},
  {0, 0},
  {cpp_main_keys_key_array_135, 8},
  {cpp_main_keys_key_array_136, 4},
  {0, 0},
  {cpp_main_keys_key_array_138, 4},
  {0, 0},
  {cpp_main_keys_key_array_140, 12},
  {0, 0},
  {cpp_main_keys_key_array_142, 11},
  {0, 0},
  {0, 0},
  {cpp_main_keys_key_array_145, 5},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {cpp_main_keys_key_array_153, 9},
  {cpp_main_keys_key_array_154, 6},
  {cpp_main_keys_key_array_155, 8},
  {0, 0},
};
Lexeme_Table_Value cpp_main_keys_value_array[157] = {
  {0, 0},
  {0, 0},
  {4, TokenCppKind_Extern},
  {4, TokenCppKind_Volatile},
  {4, TokenCppKind_Template},
  {0, 0},
  {0, 0},
  {0, 0},
  {4, TokenCppKind_NoExcept},
  {16, TokenCppKind_Auto},
  {4, TokenCppKind_Const},
  {16, TokenCppKind_Float},
  {16, TokenCppKind_char16_t},
  {0, 0},
  {4, TokenCppKind_Public},
  {4, TokenCppKind_SizeOf},
  {0, 0},
  {4, TokenCppKind_AlignAs},
  {4, TokenCppKind_Explicit},
  {0, 0},
  {4, TokenCppKind_Typename},
  {0, 0},
  {4, TokenCppKind_StaticAssert},
  {17, TokenCppKind_Try},
  {0, 0},
  {0, 0},
  {0, 0},
  {4, TokenCppKind_AlignOf},
  {0, 0},
  {4, TokenCppKind_New},
  {4, TokenCppKind_Asm},
  {0, 0},
  {17, TokenCppKind_Catch},
  {0, 0},
  {0, 0},
  {0, 0},
  {16, TokenCppKind_Short},
  {16, TokenCppKind_Signed},
  {4, TokenCppKind_Virtual},
  {0, 0},
  {4, TokenCppKind_Register},
  {0, 0},
  {0, 0},
  {0, 0},
  {4, TokenCppKind_Using},
  {16, TokenCppKind_Long},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {16, TokenCppKind_size_t},
  {17, TokenCppKind_Else},
  {4, TokenCppKind_Constexpr},
  {0, 0},
  {0, 0},
  {0, 0},
  {8, TokenCppKind_NullPtr},
  {0, 0},
  {4, TokenCppKind_Inline},
  {0, 0},
  {0, 0},
  {16, TokenCppKind_wchar_t},
  {8, TokenCppKind_Null},
  {0, 0},
  {0, 0},
  {4, TokenCppKind_ReinterpretCast},
  {4, TokenCppKind_Friend},
  {4, TokenCppKind_Private},
  {17, TokenCppKind_Goto},
  {17, TokenCppKind_If},
  {0, 0},
  {0, 0},
  {0, 0},
  {4, TokenCppKind_Typedef},
  {16, TokenCppKind_int64_t},
  {16, TokenCppKind_Int},
  {17, TokenCppKind_Continue},
  {0, 0},
  {0, 0},
  {4, TokenCppKind_DeclType},
  {16, TokenCppKind_Void},
  {0, 0},
  {0, 0},
  {0, 0},
  {16, TokenCppKind_uint8_t},
  {16, TokenCppKind_char8_t},
  {16, TokenCppKind_int32_t},
  {4, TokenCppKind_Mutable},
  {0, 0},
  {4, TokenCppKind_Export},
  {16, TokenCppKind_ptrdiff_t},
  {16, TokenCppKind_int16_t},
  {0, 0},
  {17, TokenCppKind_Break},
  {17, TokenCppKind_Default},
  {17, TokenCppKind_Return},
  {18, TokenCppKind_Union},
  {0, 0},
  {0, 0},
  {4, TokenCppKind_This},
  {8, TokenCppKind_LiteralFalse},
  {0, 0},
  {16, TokenCppKind_uint16_t},
  {0, 0},
  {0, 0},
  {18, TokenCppKind_Enum},
  {17, TokenCppKind_Case},
  {4, TokenCppKind_Delete},
  {0, 0},
  {4, TokenCppKind_TypeID},
  {16, TokenCppKind_Double},
  {18, TokenCppKind_Struct},
  {0, 0},
  {17, TokenCppKind_Switch},
  {4, TokenCppKind_Static},
  {0, 0},
  {17, TokenCppKind_For},
  {0, 0},
  {17, TokenCppKind_Do},
  {16, TokenCppKind_char32_t},
  {0, 0},
  {16, TokenCppKind_Char},
  {4, TokenCppKind_DynamicCast},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {4, TokenCppKind_ConstCast},
  {0, 0},
  {0, 0},
  {18, TokenCppKind_Class},
  {4, TokenCppKind_Protected},
  {16, TokenCppKind_uint32_t},
  {4, TokenCppKind_Operator},
  {0, 0},
  {16, TokenCppKind_Unsigned},
  {8, TokenCppKind_LiteralTrue},
  {0, 0},
  {16, TokenCppKind_Bool},
  {0, 0},
  {4, TokenCppKind_ThreadLocal},
  {0, 0},
  {4, TokenCppKind_StaticCast},
  {0, 0},
  {0, 0},
  {17, TokenCppKind_While},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {4, TokenCppKind_Namespace},
  {16, TokenCppKind_int8_t},
  {16, TokenCppKind_uint64_t},
  {0, 0},
};
i32 cpp_main_keys_slot_count = 157;
u64 cpp_main_keys_seed = 0x31ea1e4b2d9ed322;
u64 cpp_pp_directives_hash_array[25] = {
  0x0000000000000000,0x6cc5428e12e5d5c7,0x6b242d97f7de2c23,0xe38cc4f2e15f6a6b,
  0x0000000000000000,0x0000000000000000,0x6b242d97ef03e3cb,0x0000000000000000,
  0x6b242d97fb573c5b,0x6b242d941aa85d5b,0xa3aa2ae50bca9efb,0x0000000000000000,
  0x6634c071c1137bcb,0x0000000000000000,0x0000000000000000,0xa3aa2afa0b62872f,
  0x0000000000000000,0x0000000000000000,0x6cc55f308b2af207,0xe38cc4f2e15f649f,
  0xa3aa2ae5e6f819dd,0x0000000000000000,0xa3aa2ae5f111e55b,0xe38cc4f2e1a8ad2f,
  0x6b242d941ab5792d,
};
u8 cpp_pp_directives_key_array_1[] = {0x69,0x6e,0x63,0x6c,0x75,0x64,0x65,};
u8 cpp_pp_directives_key_array_2[] = {0x65,0x72,0x72,0x6f,0x72,};
u8 cpp_pp_directives_key_array_3[] = {0x65,0x6c,0x69,0x66,};
u8 cpp_pp_directives_key_array_6[] = {0x65,0x6e,0x64,0x69,0x66,};
u8 cpp_pp_directives_key_array_8[] = {0x69,0x66,0x64,0x65,0x66,};
u8 cpp_pp_directives_key_array_9[] = {0x75,0x6e,0x64,0x65,0x66,};
u8 cpp_pp_directives_key_array_10[] = {0x70,0x72,0x61,0x67,0x6d,0x61,};
u8 cpp_pp_directives_key_array_12[] = {0x69,0x66,};
u8 cpp_pp_directives_key_array_15[] = {0x64,0x65,0x66,0x69,0x6e,0x65,};
u8 cpp_pp_directives_key_array_18[] = {0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,};
u8 cpp_pp_directives_key_array_19[] = {0x65,0x6c,0x73,0x65,};
u8 cpp_pp_directives_key_array_20[] = {0x69,0x6d,0x70,0x6f,0x72,0x74,};
u8 cpp_pp_directives_key_array_22[] = {0x69,0x66,0x6e,0x64,0x65,0x66,};
u8 cpp_pp_directives_key_array_23[] = {0x6c,0x69,0x6e,0x65,};
u8 cpp_pp_directives_key_array_24[] = {0x75,0x73,0x69,0x6e,0x67,};
String_Const_u8 cpp_pp_directives_key_array[25] = {
  {0, 0},
  {cpp_pp_directives_key_array_1, 7},
  {cpp_pp_directives_key_array_2, 5},
  {cpp_pp_directives_key_array_3, 4},
  {0, 0},
  {0, 0},
  {cpp_pp_directives_key_array_6, 5},
  {0, 0},
  {cpp_pp_directives_key_array_8, 5},
  {cpp_pp_directives_key_array_9, 5},
  {cpp_pp_directives_key_array_10, 6},
  {0, 0},
  {cpp_pp_directives_key_array_12, 2},
  {0, 0},
  {0, 0},
  {cpp_pp_directives_key_array_15, 6},
  {0, 0},
  {0, 0},
  {cpp_pp_directives_key_array_18, 7},
  {cpp_pp_directives_key_array_19, 4},
  {cpp_pp_directives_key_array_20, 6},
  {0, 0},
  {cpp_pp_directives_key_array_22, 6},
  {cpp_pp_directives_key_array_23, 4},
  {cpp_pp_directives_key_array_24, 5},
};
Lexeme_Table_Value cpp_pp_directives_value_array[25] = {
  {0, 0},
  {5, TokenCppKind_PPInclude},
  {5, TokenCppKind_PPError},
  {5, TokenCppKind_PPElIf},
  {0, 0},
  {0, 0},
  {5, TokenCppKind_PPEndIf},
  {0, 0},
  {5, TokenCppKind_PPIfDef},
  {5, TokenCppKind_PPUndef},
  {5, TokenCppKind_PPPragma},
  {0, 0},
  {5, TokenCppKind_PPIf},
  {0, 0},
  {0, 0},
  {5, TokenCppKind_PPDefine},
  {0, 0},
  {0, 0},
  {5, TokenCppKind_PPVersion},
  {5, TokenCppKind_PPElse},
  {5, TokenCppKind_PPImport},
  {0, 0},
  {5, TokenCppKind_PPIfNDef},
  {5, TokenCppKind_PPLine},
  {5, TokenCppKind_PPUsing},
};
i32 cpp_pp_directives_slot_count = 25;
u64 cpp_pp_directives_seed = 0x1737856b1d45b639;
u64 cpp_pp_keys_hash_array[9] = {
  0xf9c2719c5da45df5,0x0000000000000000,0x0000000000000000,0x0000000000000000,
  0x0a7fbe1b8b45e1d9,0xc44fc642bc2eab29,0x0000000000000000,0x0a7fb9695da1ab53,
  0x64b7fe7fa0e386b5,
};
u8 cpp_pp_keys_key_array_0[] = {0x70,0x61,0x63,0x6b,};
u8 cpp_pp_keys_key_array_4[] = {0x63,0x6f,0x6d,0x6d,0x65,0x6e,0x74,};
u8 cpp_pp_keys_key_array_5[] = {0x6f,0x70,0x74,0x69,0x6d,0x69,0x7a,0x65,};
u8 cpp_pp_keys_key_array_7[] = {0x64,0x65,0x66,0x69,0x6e,0x65,0x64,};
u8 cpp_pp_keys_key_array_8[] = {0x69,0x6e,0x74,0x72,0x69,0x6e,0x73,0x69,0x63,};
String_Const_u8 cpp_pp_keys_key_array[9] = {
  {cpp_pp_keys_key_array_0, 4},
  {0, 0},
  {0, 0},
  {0, 0},
  {cpp_pp_keys_key_array_4, 7},
  {cpp_pp_keys_key_array_5, 8},
  {0, 0},
  {cpp_pp_keys_key_array_7, 7},
  {cpp_pp_keys_key_array_8, 9},
};
Lexeme_Table_Value cpp_pp_keys_value_array[9] = {
  {4, TokenCppKind_PPPack},
  {0, 0},
  {0, 0},
  {0, 0},
  {4, TokenCppKind_PPComment},
  {4, TokenCppKind_PPOptimize},
  {0, 0},
  {4, TokenCppKind_PPDefined},
  {4, TokenCppKind_PPIntrinsic},
};
i32 cpp_pp_keys_slot_count = 9;
u64 cpp_pp_keys_seed = 0x62c0f8583de00164;
struct Lex_State_Cpp{
  u32 flags_ZF0;
  u32 flags_KF0;
  u16 flags_KB0;
  u8 *base;
  u8 *delim_first;
  u8 *delim_one_past_last;
  u8 *emit_ptr;
  u8 *ptr;
  u8 *opl_ptr;
};
internal void
lex_full_input_cpp_init(Lex_State_Cpp *state_ptr, String_Const_u8 input){
  state_ptr->flags_ZF0 = 0;
  state_ptr->flags_KF0 = 0;
  state_ptr->flags_KB0 = 0;
  state_ptr->base = input.str;
  state_ptr->delim_first = input.str;
  state_ptr->delim_one_past_last = input.str;
  state_ptr->emit_ptr = input.str;
  state_ptr->ptr = input.str;
  state_ptr->opl_ptr = input.str + input.size;
}
internal b32
lex_full_input_cpp_breaks(Arena *arena, Token_List *list, Lex_State_Cpp *state_ptr, u64 max){
  b32 result = false;
  u64 emit_counter = 0;
  Lex_State_Cpp state;
  block_copy_struct(&state, state_ptr);
  {
state_label_1: // root
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_EOF;
            token.kind = 0;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        result = true;
        goto end;
      }
    }
    switch (*state.ptr){
      case 0x00:case 0x01:case 0x02:case 0x03:case 0x04:case 0x05:case 0x06:
      case 0x07:case 0x08:case 0x0e:case 0x0f:case 0x10:case 0x11:case 0x12:
      case 0x13:case 0x14:case 0x15:case 0x16:case 0x17:case 0x18:case 0x19:
      case 0x1a:case 0x1b:case 0x1c:case 0x1d:case 0x1e:case 0x1f:case 0x40:
      case 0x60:case 0x7f:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LexError;
            token.kind = 2;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x09:case 0x0b:case 0x0c:case 0x0d:case 0x20:
      {
        if ((HasFlag(state.flags_KF0, 0x2))){
          state.ptr += 1;
          goto state_label_4; // error_body
        }
        state.ptr += 1;
        goto state_label_3; // whitespace
      }break;
      case 0x0a:
      {
        state.ptr += 1;
        state.flags_KB0 &= ~(0x1);
        state.flags_KF0 &= ~(0x1);
        state.flags_KF0 &= ~(0x2);
        goto state_label_3; // whitespace
      }break;
      case 0x21:
      {
        state.ptr += 1;
        goto state_label_61; // op stage
      }break;
      case 0x22:
      {
        if ((HasFlag(state.flags_KF0, 0x1))){
          state.ptr += 1;
          goto state_label_26; // include_quotes
        }
        state.ptr += 1;
        goto state_label_32; // string
      }break;
      case 0x23:
      {
        if ((!HasFlag(state.flags_KB0, 0x1))){
          state.ptr += 1;
          goto state_label_23; // pp_directive_whitespace
        }
        state.ptr += 1;
        goto state_label_68; // op stage
      }break;
      default:
      {
        state.ptr += 1;
        goto state_label_2; // identifier
      }break;
      case 0x25:
      {
        state.ptr += 1;
        goto state_label_65; // op stage
      }break;
      case 0x26:
      {
        state.ptr += 1;
        goto state_label_62; // op stage
      }break;
      case 0x27:
      {
        state.ptr += 1;
        state.flags_ZF0 |= 0x40;
        goto state_label_32; // string
      }break;
      case 0x28:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_ParenOp;
            token.kind = 13;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x29:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_ParenCl;
            token.kind = 14;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x2a:
      {
        state.ptr += 1;
        goto state_label_64; // op stage
      }break;
      case 0x2b:
      {
        state.ptr += 1;
        goto state_label_54; // op stage
      }break;
      case 0x2c:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Comma;
            token.kind = 15;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x2d:
      {
        state.ptr += 1;
        goto state_label_55; // op stage
      }break;
      case 0x2e:
      {
        state.ptr += 1;
        goto state_label_6; // operator_or_fnumber_dot
      }break;
      case 0x2f:
      {
        state.ptr += 1;
        goto state_label_7; // operator_or_comment_slash
      }break;
      case 0x30:
      {
        state.ptr += 1;
        goto state_label_9; // znumber
      }break;
      case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:case 0x37:
      case 0x38:case 0x39:
      {
        state.ptr += 1;
        goto state_label_8; // number
      }break;
      case 0x3a:
      {
        state.ptr += 1;
        goto state_label_53; // op stage
      }break;
      case 0x3b:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Semicolon;
            token.kind = 15;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x3c:
      {
        if ((!HasFlag(state.flags_KF0, 0x1))){
          state.ptr += 1;
          goto state_label_57; // op stage
        }
        state.ptr += 1;
        goto state_label_25; // include_pointy
      }break;
      case 0x3d:
      {
        state.ptr += 1;
        goto state_label_60; // op stage
      }break;
      case 0x3e:
      {
        state.ptr += 1;
        goto state_label_58; // op stage
      }break;
      case 0x3f:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Ternary;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x4c:
      {
        state.ptr += 1;
        state.flags_ZF0 |= 0x4;
        goto state_label_27; // pre_L
      }break;
      case 0x52:
      {
        state.ptr += 1;
        goto state_label_31; // pre_R
      }break;
      case 0x55:
      {
        state.ptr += 1;
        state.flags_ZF0 |= 0x20;
        goto state_label_29; // pre_U
      }break;
      case 0x5b:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_BrackOp;
            token.kind = 13;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x5c:
      {
        state.ptr += 1;
        goto state_label_5; // backslash
      }break;
      case 0x5d:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_BrackCl;
            token.kind = 14;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x5e:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Xor;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x75:
      {
        state.ptr += 1;
        state.flags_ZF0 |= 0x10;
        goto state_label_28; // pre_u
      }break;
      case 0x7b:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_BraceOp;
            token.kind = 11;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x7c:
      {
        state.ptr += 1;
        goto state_label_63; // op stage
      }break;
      case 0x7d:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_BraceCl;
            token.kind = 12;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x7e:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Tilde;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_2: // identifier
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_KB0, 0x1)){
              Lexeme_Table_Lookup lookup = lexeme_table_lookup(cpp_pp_keys_hash_array, cpp_pp_keys_key_array, cpp_pp_keys_value_array, cpp_pp_keys_slot_count, cpp_pp_keys_seed, state.emit_ptr, token.size);
              if (lookup.found_match){
                token.kind = lookup.base_kind;
                token.sub_kind = lookup.sub_kind;
                break;
              }
            }
            Lexeme_Table_Lookup lookup = lexeme_table_lookup(cpp_main_keys_hash_array, cpp_main_keys_key_array, cpp_main_keys_value_array, cpp_main_keys_slot_count, cpp_main_keys_seed, state.emit_ptr, token.size);
            if (lookup.found_match){
              token.kind = lookup.base_kind;
              token.sub_kind = lookup.sub_kind;
              break;
            }
            token.sub_kind = TokenCppKind_Identifier;
            token.kind = 6;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      case 0x00:case 0x01:case 0x02:case 0x03:case 0x04:case 0x05:case 0x06:
      case 0x07:case 0x08:case 0x09:case 0x0a:case 0x0b:case 0x0c:case 0x0d:
      case 0x0e:case 0x0f:case 0x10:case 0x11:case 0x12:case 0x13:case 0x14:
      case 0x15:case 0x16:case 0x17:case 0x18:case 0x19:case 0x1a:case 0x1b:
      case 0x1c:case 0x1d:case 0x1e:case 0x1f:case 0x20:case 0x21:case 0x22:
      case 0x23:case 0x25:case 0x26:case 0x27:case 0x28:case 0x29:case 0x2a:
      case 0x2b:case 0x2c:case 0x2d:case 0x2e:case 0x2f:case 0x3a:case 0x3b:
      case 0x3c:case 0x3d:case 0x3e:case 0x3f:case 0x40:case 0x5b:case 0x5c:
      case 0x5d:case 0x5e:case 0x60:case 0x7b:case 0x7c:case 0x7d:case 0x7e:
      case 0x7f:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_KB0, 0x1)){
              Lexeme_Table_Lookup lookup = lexeme_table_lookup(cpp_pp_keys_hash_array, cpp_pp_keys_key_array, cpp_pp_keys_value_array, cpp_pp_keys_slot_count, cpp_pp_keys_seed, state.emit_ptr, token.size);
              if (lookup.found_match){
                token.kind = lookup.base_kind;
                token.sub_kind = lookup.sub_kind;
                break;
              }
            }
            Lexeme_Table_Lookup lookup = lexeme_table_lookup(cpp_main_keys_hash_array, cpp_main_keys_key_array, cpp_main_keys_value_array, cpp_main_keys_slot_count, cpp_main_keys_seed, state.emit_ptr, token.size);
            if (lookup.found_match){
              token.kind = lookup.base_kind;
              token.sub_kind = lookup.sub_kind;
              break;
            }
            token.sub_kind = TokenCppKind_Identifier;
            token.kind = 6;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      default:
      {
        state.ptr += 1;
        goto state_label_2; // identifier
      }break;
    }
  }
  {
state_label_3: // whitespace
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Whitespace;
            token.kind = 1;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Whitespace;
            token.kind = 1;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x09:case 0x0b:case 0x0c:case 0x0d:case 0x20:
      {
        state.ptr += 1;
        goto state_label_3; // whitespace
      }break;
      case 0x0a:
      {
        state.ptr += 1;
        state.flags_KB0 &= ~(0x1);
        state.flags_KF0 &= ~(0x1);
        state.flags_KF0 &= ~(0x2);
        goto state_label_3; // whitespace
      }break;
    }
  }
  {
state_label_4: // error_body
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_PPErrorMessage;
            token.kind = 10;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        state.ptr += 1;
        goto state_label_4; // error_body
      }break;
      case 0x0a:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_PPErrorMessage;
            token.kind = 10;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_5: // backslash
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Backslash;
            token.kind = 1;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Backslash;
            token.kind = 1;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x0a:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Backslash;
            token.kind = 1;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x0d:
      {
        state.ptr += 1;
        goto state_label_5; // backslash
      }break;
    }
  }
  {
state_label_6: // operator_or_fnumber_dot
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Dot;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Dot;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x2a:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_DotStar;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x2e:
      {
        state.ptr += 1;
        goto state_label_69; // op stage
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:case 0x38:case 0x39:
      {
        state.ptr += 1;
        goto state_label_10; // fnumber_decimal
      }break;
    }
  }
  {
state_label_7: // operator_or_comment_slash
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Div;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Div;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x2a:
      {
        state.ptr += 1;
        goto state_label_49; // comment_block
      }break;
      case 0x2f:
      {
        state.ptr += 1;
        goto state_label_51; // comment_line
      }break;
      case 0x3d:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_DivEq;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_8: // number
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralInteger;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralInteger;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x27:case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:
      case 0x36:case 0x37:case 0x38:case 0x39:
      {
        state.ptr += 1;
        goto state_label_8; // number
      }break;
      case 0x2e:
      {
        state.ptr += 1;
        goto state_label_10; // fnumber_decimal
      }break;
      case 0x45:case 0x65:
      {
        state.ptr += 1;
        goto state_label_11; // fnumber_exponent
      }break;
      case 0x4c:
      {
        state.ptr += 1;
        goto state_label_18; // L_number
      }break;
      case 0x55:case 0x75:
      {
        state.ptr += 1;
        goto state_label_17; // U_number
      }break;
      case 0x6c:
      {
        state.ptr += 1;
        goto state_label_20; // l_number
      }break;
    }
  }
  {
state_label_9: // znumber
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralInteger;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralInteger;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x2e:
      {
        state.ptr += 1;
        goto state_label_10; // fnumber_decimal
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:
      {
        state.ptr += 1;
        state.flags_ZF0 |= 0x2;
        goto state_label_16; // number_oct
      }break;
      case 0x42:case 0x58:case 0x62:case 0x78:
      {
        state.ptr += 1;
        state.flags_ZF0 |= 0x1;
        goto state_label_14; // number_hex_first
      }break;
      case 0x45:case 0x65:
      {
        state.ptr += 1;
        goto state_label_11; // fnumber_exponent
      }break;
      case 0x4c:
      {
        state.ptr += 1;
        goto state_label_18; // L_number
      }break;
      case 0x55:case 0x75:
      {
        state.ptr += 1;
        goto state_label_17; // U_number
      }break;
      case 0x6c:
      {
        state.ptr += 1;
        goto state_label_20; // l_number
      }break;
    }
  }
  {
state_label_10: // fnumber_decimal
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralFloat64;
            token.kind = 9;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralFloat64;
            token.kind = 9;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x27:case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:
      case 0x36:case 0x37:case 0x38:case 0x39:
      {
        state.ptr += 1;
        goto state_label_10; // fnumber_decimal
      }break;
      case 0x45:case 0x65:
      {
        state.ptr += 1;
        goto state_label_11; // fnumber_exponent
      }break;
      case 0x46:case 0x66:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralFloat32;
            token.kind = 9;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x4c:case 0x6c:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralFloat64;
            token.kind = 9;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_11: // fnumber_exponent
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralFloat64;
            token.kind = 9;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralFloat64;
            token.kind = 9;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x2b:case 0x2d:
      {
        state.ptr += 1;
        goto state_label_12; // fnumber_exponent_sign
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:case 0x38:case 0x39:
      {
        state.ptr += 1;
        goto state_label_13; // fnumber_exponent_digits
      }break;
      case 0x46:case 0x66:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralFloat32;
            token.kind = 9;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x4c:case 0x6c:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralFloat64;
            token.kind = 9;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_12: // fnumber_exponent_sign
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralFloat64;
            token.kind = 9;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralFloat64;
            token.kind = 9;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:case 0x38:case 0x39:
      {
        state.ptr += 1;
        goto state_label_13; // fnumber_exponent_digits
      }break;
      case 0x46:case 0x66:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralFloat32;
            token.kind = 9;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x4c:case 0x6c:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralFloat64;
            token.kind = 9;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_13: // fnumber_exponent_digits
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralFloat64;
            token.kind = 9;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralFloat64;
            token.kind = 9;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:case 0x38:case 0x39:
      {
        state.ptr += 1;
        goto state_label_13; // fnumber_exponent_digits
      }break;
      case 0x46:case 0x66:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralFloat32;
            token.kind = 9;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x4c:case 0x6c:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralFloat64;
            token.kind = 9;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_14: // number_hex_first
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LexError;
            token.kind = 2;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LexError;
            token.kind = 2;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:case 0x38:case 0x39:case 0x41:case 0x42:case 0x43:case 0x44:
      case 0x45:case 0x46:case 0x61:case 0x62:case 0x63:case 0x64:case 0x65:
      case 0x66:
      {
        state.ptr += 1;
        goto state_label_15; // number_hex
      }break;
    }
  }
  {
state_label_15: // number_hex
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralIntegerHex;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralIntegerHex;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x27:case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:
      case 0x36:case 0x37:case 0x38:case 0x39:case 0x41:case 0x42:case 0x43:
      case 0x44:case 0x45:case 0x46:case 0x61:case 0x62:case 0x63:case 0x64:
      case 0x65:case 0x66:
      {
        state.ptr += 1;
        goto state_label_15; // number_hex
      }break;
      case 0x4c:
      {
        state.ptr += 1;
        goto state_label_18; // L_number
      }break;
      case 0x55:case 0x75:
      {
        state.ptr += 1;
        goto state_label_17; // U_number
      }break;
      case 0x6c:
      {
        state.ptr += 1;
        goto state_label_20; // l_number
      }break;
    }
  }
  {
state_label_16: // number_oct
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralIntegerOct;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LiteralIntegerOct;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x27:case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:
      case 0x36:case 0x37:
      {
        state.ptr += 1;
        state.flags_ZF0 |= 0x2;
        goto state_label_16; // number_oct
      }break;
      case 0x4c:
      {
        state.ptr += 1;
        goto state_label_18; // L_number
      }break;
      case 0x55:case 0x75:
      {
        state.ptr += 1;
        goto state_label_17; // U_number
      }break;
      case 0x6c:
      {
        state.ptr += 1;
        goto state_label_20; // l_number
      }break;
    }
  }
  {
state_label_17: // U_number
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_ZF0, 0x1)){
              token.sub_kind = TokenCppKind_LiteralIntegerHexU;
              token.kind = 8;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x2)){
              token.sub_kind = TokenCppKind_LiteralIntegerOctU;
              token.kind = 8;
              break;
            }
            token.sub_kind = TokenCppKind_LiteralIntegerU;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_ZF0, 0x1)){
              token.sub_kind = TokenCppKind_LiteralIntegerHexU;
              token.kind = 8;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x2)){
              token.sub_kind = TokenCppKind_LiteralIntegerOctU;
              token.kind = 8;
              break;
            }
            token.sub_kind = TokenCppKind_LiteralIntegerU;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x4c:
      {
        state.ptr += 1;
        goto state_label_19; // UL_number
      }break;
      case 0x6c:
      {
        state.ptr += 1;
        goto state_label_21; // Ul_number
      }break;
    }
  }
  {
state_label_18: // L_number
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_ZF0, 0x1)){
              token.sub_kind = TokenCppKind_LiteralIntegerHexL;
              token.kind = 8;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x2)){
              token.sub_kind = TokenCppKind_LiteralIntegerOctL;
              token.kind = 8;
              break;
            }
            token.sub_kind = TokenCppKind_LiteralIntegerL;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_ZF0, 0x1)){
              token.sub_kind = TokenCppKind_LiteralIntegerHexL;
              token.kind = 8;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x2)){
              token.sub_kind = TokenCppKind_LiteralIntegerOctL;
              token.kind = 8;
              break;
            }
            token.sub_kind = TokenCppKind_LiteralIntegerL;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x4c:
      {
        state.ptr += 1;
        goto state_label_22; // LL_number
      }break;
      case 0x55:case 0x75:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_ZF0, 0x1)){
              token.sub_kind = TokenCppKind_LiteralIntegerHexUL;
              token.kind = 8;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x2)){
              token.sub_kind = TokenCppKind_LiteralIntegerOctUL;
              token.kind = 8;
              break;
            }
            token.sub_kind = TokenCppKind_LiteralIntegerUL;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_19: // UL_number
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_ZF0, 0x1)){
              token.sub_kind = TokenCppKind_LiteralIntegerHexUL;
              token.kind = 8;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x2)){
              token.sub_kind = TokenCppKind_LiteralIntegerOctUL;
              token.kind = 8;
              break;
            }
            token.sub_kind = TokenCppKind_LiteralIntegerUL;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_ZF0, 0x1)){
              token.sub_kind = TokenCppKind_LiteralIntegerHexUL;
              token.kind = 8;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x2)){
              token.sub_kind = TokenCppKind_LiteralIntegerOctUL;
              token.kind = 8;
              break;
            }
            token.sub_kind = TokenCppKind_LiteralIntegerUL;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x4c:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_ZF0, 0x1)){
              token.sub_kind = TokenCppKind_LiteralIntegerHexULL;
              token.kind = 8;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x2)){
              token.sub_kind = TokenCppKind_LiteralIntegerOctULL;
              token.kind = 8;
              break;
            }
            token.sub_kind = TokenCppKind_LiteralIntegerULL;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_20: // l_number
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_ZF0, 0x1)){
              token.sub_kind = TokenCppKind_LiteralIntegerHexL;
              token.kind = 8;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x2)){
              token.sub_kind = TokenCppKind_LiteralIntegerOctL;
              token.kind = 8;
              break;
            }
            token.sub_kind = TokenCppKind_LiteralIntegerL;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_ZF0, 0x1)){
              token.sub_kind = TokenCppKind_LiteralIntegerHexL;
              token.kind = 8;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x2)){
              token.sub_kind = TokenCppKind_LiteralIntegerOctL;
              token.kind = 8;
              break;
            }
            token.sub_kind = TokenCppKind_LiteralIntegerL;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x55:case 0x75:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_ZF0, 0x1)){
              token.sub_kind = TokenCppKind_LiteralIntegerHexUL;
              token.kind = 8;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x2)){
              token.sub_kind = TokenCppKind_LiteralIntegerOctUL;
              token.kind = 8;
              break;
            }
            token.sub_kind = TokenCppKind_LiteralIntegerUL;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x6c:
      {
        state.ptr += 1;
        goto state_label_22; // LL_number
      }break;
    }
  }
  {
state_label_21: // Ul_number
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_ZF0, 0x1)){
              token.sub_kind = TokenCppKind_LiteralIntegerHexUL;
              token.kind = 8;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x2)){
              token.sub_kind = TokenCppKind_LiteralIntegerOctUL;
              token.kind = 8;
              break;
            }
            token.sub_kind = TokenCppKind_LiteralIntegerUL;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_ZF0, 0x1)){
              token.sub_kind = TokenCppKind_LiteralIntegerHexUL;
              token.kind = 8;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x2)){
              token.sub_kind = TokenCppKind_LiteralIntegerOctUL;
              token.kind = 8;
              break;
            }
            token.sub_kind = TokenCppKind_LiteralIntegerUL;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x6c:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_ZF0, 0x1)){
              token.sub_kind = TokenCppKind_LiteralIntegerHexULL;
              token.kind = 8;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x2)){
              token.sub_kind = TokenCppKind_LiteralIntegerOctULL;
              token.kind = 8;
              break;
            }
            token.sub_kind = TokenCppKind_LiteralIntegerULL;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_22: // LL_number
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_ZF0, 0x1)){
              token.sub_kind = TokenCppKind_LiteralIntegerHexLL;
              token.kind = 8;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x2)){
              token.sub_kind = TokenCppKind_LiteralIntegerOctLL;
              token.kind = 8;
              break;
            }
            token.sub_kind = TokenCppKind_LiteralIntegerLL;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_ZF0, 0x1)){
              token.sub_kind = TokenCppKind_LiteralIntegerHexLL;
              token.kind = 8;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x2)){
              token.sub_kind = TokenCppKind_LiteralIntegerOctLL;
              token.kind = 8;
              break;
            }
            token.sub_kind = TokenCppKind_LiteralIntegerLL;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x55:case 0x75:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_ZF0, 0x1)){
              token.sub_kind = TokenCppKind_LiteralIntegerHexULL;
              token.kind = 8;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x2)){
              token.sub_kind = TokenCppKind_LiteralIntegerOctULL;
              token.kind = 8;
              break;
            }
            token.sub_kind = TokenCppKind_LiteralIntegerULL;
            token.kind = 8;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_23: // pp_directive_whitespace
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LexError;
            token.kind = 2;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LexError;
            token.kind = 2;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x09:case 0x0b:case 0x0c:case 0x20:
      {
        state.ptr += 1;
        goto state_label_23; // pp_directive_whitespace
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:case 0x38:case 0x39:case 0x41:case 0x42:case 0x43:case 0x44:
      case 0x45:case 0x46:case 0x47:case 0x48:case 0x49:case 0x4a:case 0x4b:
      case 0x4c:case 0x4d:case 0x4e:case 0x4f:case 0x50:case 0x51:case 0x52:
      case 0x53:case 0x54:case 0x55:case 0x56:case 0x57:case 0x58:case 0x59:
      case 0x5a:case 0x5f:case 0x61:case 0x62:case 0x63:case 0x64:case 0x65:
      case 0x66:case 0x67:case 0x68:case 0x69:case 0x6a:case 0x6b:case 0x6c:
      case 0x6d:case 0x6e:case 0x6f:case 0x70:case 0x71:case 0x72:case 0x73:
      case 0x74:case 0x75:case 0x76:case 0x77:case 0x78:case 0x79:case 0x7a:
      {
        state.delim_first = state.ptr;
        state.flags_KB0 |= 0x1;
        state.ptr += 1;
        goto state_label_24; // pp_directive
      }break;
    }
  }
  {
state_label_24: // pp_directive
    if (state.ptr == state.opl_ptr){
      if ((true)){
        state.delim_one_past_last = state.ptr;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            Lexeme_Table_Lookup lookup = lexeme_table_lookup(cpp_pp_directives_hash_array, cpp_pp_directives_key_array, cpp_pp_directives_value_array, cpp_pp_directives_slot_count, cpp_pp_directives_seed, state.delim_first, (state.delim_one_past_last - state.delim_first));
            if (lookup.found_match){
              token.kind = lookup.base_kind;
              token.sub_kind = lookup.sub_kind;
              break;
            }
            token.sub_kind = TokenCppKind_PPUnknown;
            token.kind = 2;
          }while(0);
          switch (token.sub_kind){
            case TokenCppKind_PPInclude:
            {
              state.flags_KF0 |= 0x1;
            }break;
            case TokenCppKind_PPError:
            {
              state.flags_KF0 |= 0x2;
            }break;
          }
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        state.delim_one_past_last = state.ptr;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            Lexeme_Table_Lookup lookup = lexeme_table_lookup(cpp_pp_directives_hash_array, cpp_pp_directives_key_array, cpp_pp_directives_value_array, cpp_pp_directives_slot_count, cpp_pp_directives_seed, state.delim_first, (state.delim_one_past_last - state.delim_first));
            if (lookup.found_match){
              token.kind = lookup.base_kind;
              token.sub_kind = lookup.sub_kind;
              break;
            }
            token.sub_kind = TokenCppKind_PPUnknown;
            token.kind = 2;
          }while(0);
          switch (token.sub_kind){
            case TokenCppKind_PPInclude:
            {
              state.flags_KF0 |= 0x1;
            }break;
            case TokenCppKind_PPError:
            {
              state.flags_KF0 |= 0x2;
            }break;
          }
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:case 0x38:case 0x39:case 0x41:case 0x42:case 0x43:case 0x44:
      case 0x45:case 0x46:case 0x47:case 0x48:case 0x49:case 0x4a:case 0x4b:
      case 0x4c:case 0x4d:case 0x4e:case 0x4f:case 0x50:case 0x51:case 0x52:
      case 0x53:case 0x54:case 0x55:case 0x56:case 0x57:case 0x58:case 0x59:
      case 0x5a:case 0x5f:case 0x61:case 0x62:case 0x63:case 0x64:case 0x65:
      case 0x66:case 0x67:case 0x68:case 0x69:case 0x6a:case 0x6b:case 0x6c:
      case 0x6d:case 0x6e:case 0x6f:case 0x70:case 0x71:case 0x72:case 0x73:
      case 0x74:case 0x75:case 0x76:case 0x77:case 0x78:case 0x79:case 0x7a:
      {
        state.ptr += 1;
        goto state_label_24; // pp_directive
      }break;
    }
  }
  {
state_label_25: // include_pointy
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LexError;
            token.kind = 2;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LexError;
            token.kind = 2;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x20:case 0x21:case 0x22:case 0x23:case 0x24:case 0x25:case 0x26:
      case 0x27:case 0x28:case 0x29:case 0x2a:case 0x2b:case 0x2c:case 0x2d:
      case 0x2e:case 0x2f:case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:
      case 0x35:case 0x36:case 0x37:case 0x38:case 0x39:case 0x3a:case 0x3b:
      case 0x3d:case 0x3f:case 0x40:case 0x41:case 0x42:case 0x43:case 0x44:
      case 0x45:case 0x46:case 0x47:case 0x48:case 0x49:case 0x4a:case 0x4b:
      case 0x4c:case 0x4d:case 0x4e:case 0x4f:case 0x50:case 0x51:case 0x52:
      case 0x53:case 0x54:case 0x55:case 0x56:case 0x57:case 0x58:case 0x59:
      case 0x5a:case 0x5b:case 0x5c:case 0x5d:case 0x5e:case 0x5f:case 0x61:
      case 0x62:case 0x63:case 0x64:case 0x65:case 0x66:case 0x67:case 0x68:
      case 0x69:case 0x6a:case 0x6b:case 0x6c:case 0x6d:case 0x6e:case 0x6f:
      case 0x70:case 0x71:case 0x72:case 0x73:case 0x74:case 0x75:case 0x76:
      case 0x77:case 0x78:case 0x79:case 0x7a:case 0x7b:case 0x7c:case 0x7d:
      {
        state.ptr += 1;
        goto state_label_25; // include_pointy
      }break;
      case 0x3e:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_PPIncludeFile;
            token.kind = 10;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_26: // include_quotes
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LexError;
            token.kind = 2;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LexError;
            token.kind = 2;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x20:case 0x21:case 0x23:case 0x24:case 0x25:case 0x26:case 0x27:
      case 0x28:case 0x29:case 0x2a:case 0x2b:case 0x2c:case 0x2d:case 0x2e:
      case 0x2f:case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:
      case 0x36:case 0x37:case 0x38:case 0x39:case 0x3a:case 0x3b:case 0x3c:
      case 0x3d:case 0x3e:case 0x3f:case 0x40:case 0x41:case 0x42:case 0x43:
      case 0x44:case 0x45:case 0x46:case 0x47:case 0x48:case 0x49:case 0x4a:
      case 0x4b:case 0x4c:case 0x4d:case 0x4e:case 0x4f:case 0x50:case 0x51:
      case 0x52:case 0x53:case 0x54:case 0x55:case 0x56:case 0x57:case 0x58:
      case 0x59:case 0x5a:case 0x5b:case 0x5c:case 0x5d:case 0x5e:case 0x5f:
      case 0x61:case 0x62:case 0x63:case 0x64:case 0x65:case 0x66:case 0x67:
      case 0x68:case 0x69:case 0x6a:case 0x6b:case 0x6c:case 0x6d:case 0x6e:
      case 0x6f:case 0x70:case 0x71:case 0x72:case 0x73:case 0x74:case 0x75:
      case 0x76:case 0x77:case 0x78:case 0x79:case 0x7a:case 0x7b:case 0x7c:
      case 0x7d:
      {
        state.ptr += 1;
        goto state_label_26; // include_quotes
      }break;
      case 0x22:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_PPIncludeFile;
            token.kind = 10;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_27: // pre_L
    if (state.ptr == state.opl_ptr){
      if ((true)){
        goto state_label_2; // identifier
      }
    }
    switch (*state.ptr){
      default:
      {
        goto state_label_2; // identifier
      }break;
      case 0x22:
      {
        state.ptr += 1;
        goto state_label_32; // string
      }break;
      case 0x52:
      {
        state.ptr += 1;
        goto state_label_31; // pre_R
      }break;
    }
  }
  {
state_label_28: // pre_u
    if (state.ptr == state.opl_ptr){
      if ((true)){
        goto state_label_2; // identifier
      }
    }
    switch (*state.ptr){
      default:
      {
        goto state_label_2; // identifier
      }break;
      case 0x22:
      {
        state.ptr += 1;
        goto state_label_32; // string
      }break;
      case 0x38:
      {
        state.ptr += 1;
        state.flags_ZF0 |= 0x8;
        goto state_label_30; // pre_u8
      }break;
      case 0x52:
      {
        state.ptr += 1;
        goto state_label_31; // pre_R
      }break;
    }
  }
  {
state_label_29: // pre_U
    if (state.ptr == state.opl_ptr){
      if ((true)){
        goto state_label_2; // identifier
      }
    }
    switch (*state.ptr){
      default:
      {
        goto state_label_2; // identifier
      }break;
      case 0x22:
      {
        state.ptr += 1;
        goto state_label_32; // string
      }break;
      case 0x52:
      {
        state.ptr += 1;
        goto state_label_31; // pre_R
      }break;
    }
  }
  {
state_label_30: // pre_u8
    if (state.ptr == state.opl_ptr){
      if ((true)){
        goto state_label_2; // identifier
      }
    }
    switch (*state.ptr){
      default:
      {
        goto state_label_2; // identifier
      }break;
      case 0x22:
      {
        state.ptr += 1;
        goto state_label_32; // string
      }break;
      case 0x52:
      {
        state.ptr += 1;
        goto state_label_31; // pre_R
      }break;
    }
  }
  {
state_label_31: // pre_R
    if (state.ptr == state.opl_ptr){
      if ((true)){
        goto state_label_2; // identifier
      }
    }
    switch (*state.ptr){
      default:
      {
        goto state_label_2; // identifier
      }break;
      case 0x22:
      {
        state.ptr += 1;
        state.delim_first = state.ptr;
        goto state_label_45; // raw_string_get_delim
      }break;
    }
  }
  {
state_label_32: // string
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LexError;
            token.kind = 2;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        state.ptr += 1;
        goto state_label_32; // string
      }break;
      case 0x0a:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LexError;
            token.kind = 2;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x22:
      {
        if ((!HasFlag(state.flags_ZF0, 0x40))){
          state.ptr += 1;
          {
            Token token = {};
            token.pos = (i64)(state.emit_ptr - state.base);
            token.size = (i64)(state.ptr - state.emit_ptr);
            token.flags = state.flags_KB0;
            do{
              if (HasFlag(state.flags_ZF0, 0x4)){
                token.sub_kind = TokenCppKind_LiteralStringWide;
                token.kind = 10;
                break;
              }
              if (HasFlag(state.flags_ZF0, 0x8)){
                token.sub_kind = TokenCppKind_LiteralStringUTF8;
                token.kind = 10;
                break;
              }
              if (HasFlag(state.flags_ZF0, 0x10)){
                token.sub_kind = TokenCppKind_LiteralStringUTF16;
                token.kind = 10;
                break;
              }
              if (HasFlag(state.flags_ZF0, 0x20)){
                token.sub_kind = TokenCppKind_LiteralStringUTF32;
                token.kind = 10;
                break;
              }
              token.sub_kind = TokenCppKind_LiteralString;
              token.kind = 10;
            }while(0);
            token_list_push(arena, list, &token);
            emit_counter += 1;
            state.emit_ptr = state.ptr;
            if (emit_counter == max){
              goto end;
            }
          }
          state.flags_ZF0 = 0;
          goto state_label_1; // root
        }
        state.ptr += 1;
        goto state_label_32; // string
      }break;
      case 0x27:
      {
        if ((HasFlag(state.flags_ZF0, 0x40))){
          state.ptr += 1;
          {
            Token token = {};
            token.pos = (i64)(state.emit_ptr - state.base);
            token.size = (i64)(state.ptr - state.emit_ptr);
            token.flags = state.flags_KB0;
            do{
              if (HasFlag(state.flags_ZF0, 0x4)){
                token.sub_kind = TokenCppKind_LiteralCharacterWide;
                token.kind = 10;
                break;
              }
              if (HasFlag(state.flags_ZF0, 0x8)){
                token.sub_kind = TokenCppKind_LiteralCharacterUTF8;
                token.kind = 10;
                break;
              }
              if (HasFlag(state.flags_ZF0, 0x10)){
                token.sub_kind = TokenCppKind_LiteralCharacterUTF16;
                token.kind = 10;
                break;
              }
              if (HasFlag(state.flags_ZF0, 0x20)){
                token.sub_kind = TokenCppKind_LiteralCharacterUTF32;
                token.kind = 10;
                break;
              }
              token.sub_kind = TokenCppKind_LiteralCharacter;
              token.kind = 10;
            }while(0);
            token_list_push(arena, list, &token);
            emit_counter += 1;
            state.emit_ptr = state.ptr;
            if (emit_counter == max){
              goto end;
            }
          }
          state.flags_ZF0 = 0;
          goto state_label_1; // root
        }
        state.ptr += 1;
        goto state_label_32; // string
      }break;
      case 0x5c:
      {
        state.ptr += 1;
        goto state_label_33; // string_esc
      }break;
    }
  }
  {
state_label_33: // string_esc
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LexError;
            token.kind = 2;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_EOF;
            token.kind = 0;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        result = true;
        goto end;
      }
    }
    switch (*state.ptr){
      default:
      {
        state.ptr += 1;
        goto state_label_32; // string
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:
      {
        state.ptr += 1;
        goto state_label_34; // string_esc_oct2
      }break;
      case 0x55:
      {
        state.ptr += 1;
        goto state_label_37; // string_esc_universal_8
      }break;
      case 0x75:
      {
        state.ptr += 1;
        goto state_label_41; // string_esc_universal_4
      }break;
      case 0x78:
      {
        state.ptr += 1;
        goto state_label_36; // string_esc_hex
      }break;
    }
  }
  {
state_label_34: // string_esc_oct2
    if (state.ptr == state.opl_ptr){
      if ((true)){
        goto state_label_32; // string
      }
    }
    switch (*state.ptr){
      default:
      {
        goto state_label_32; // string
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:
      {
        state.ptr += 1;
        goto state_label_35; // string_esc_oct1
      }break;
    }
  }
  {
state_label_35: // string_esc_oct1
    if (state.ptr == state.opl_ptr){
      if ((true)){
        goto state_label_32; // string
      }
    }
    switch (*state.ptr){
      default:
      {
        goto state_label_32; // string
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:
      {
        state.ptr += 1;
        goto state_label_32; // string
      }break;
    }
  }
  {
state_label_36: // string_esc_hex
    if (state.ptr == state.opl_ptr){
      if ((true)){
        goto state_label_32; // string
      }
    }
    switch (*state.ptr){
      default:
      {
        goto state_label_32; // string
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:case 0x38:case 0x39:case 0x41:case 0x42:case 0x43:case 0x44:
      case 0x45:case 0x46:case 0x61:case 0x62:case 0x63:case 0x64:case 0x65:
      case 0x66:
      {
        state.ptr += 1;
        goto state_label_36; // string_esc_hex
      }break;
    }
  }
  {
state_label_37: // string_esc_universal_8
    if (state.ptr == state.opl_ptr){
      if ((true)){
        goto state_label_32; // string
      }
    }
    switch (*state.ptr){
      default:
      {
        goto state_label_32; // string
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:case 0x38:case 0x39:case 0x41:case 0x42:case 0x43:case 0x44:
      case 0x45:case 0x46:case 0x61:case 0x62:case 0x63:case 0x64:case 0x65:
      case 0x66:
      {
        state.ptr += 1;
        goto state_label_38; // string_esc_universal_7
      }break;
    }
  }
  {
state_label_38: // string_esc_universal_7
    if (state.ptr == state.opl_ptr){
      if ((true)){
        goto state_label_32; // string
      }
    }
    switch (*state.ptr){
      default:
      {
        goto state_label_32; // string
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:case 0x38:case 0x39:case 0x41:case 0x42:case 0x43:case 0x44:
      case 0x45:case 0x46:case 0x61:case 0x62:case 0x63:case 0x64:case 0x65:
      case 0x66:
      {
        state.ptr += 1;
        goto state_label_39; // string_esc_universal_6
      }break;
    }
  }
  {
state_label_39: // string_esc_universal_6
    if (state.ptr == state.opl_ptr){
      if ((true)){
        goto state_label_32; // string
      }
    }
    switch (*state.ptr){
      default:
      {
        goto state_label_32; // string
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:case 0x38:case 0x39:case 0x41:case 0x42:case 0x43:case 0x44:
      case 0x45:case 0x46:case 0x61:case 0x62:case 0x63:case 0x64:case 0x65:
      case 0x66:
      {
        state.ptr += 1;
        goto state_label_40; // string_esc_universal_5
      }break;
    }
  }
  {
state_label_40: // string_esc_universal_5
    if (state.ptr == state.opl_ptr){
      if ((true)){
        goto state_label_32; // string
      }
    }
    switch (*state.ptr){
      default:
      {
        goto state_label_32; // string
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:case 0x38:case 0x39:case 0x41:case 0x42:case 0x43:case 0x44:
      case 0x45:case 0x46:case 0x61:case 0x62:case 0x63:case 0x64:case 0x65:
      case 0x66:
      {
        state.ptr += 1;
        goto state_label_41; // string_esc_universal_4
      }break;
    }
  }
  {
state_label_41: // string_esc_universal_4
    if (state.ptr == state.opl_ptr){
      if ((true)){
        goto state_label_32; // string
      }
    }
    switch (*state.ptr){
      default:
      {
        goto state_label_32; // string
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:case 0x38:case 0x39:case 0x41:case 0x42:case 0x43:case 0x44:
      case 0x45:case 0x46:case 0x61:case 0x62:case 0x63:case 0x64:case 0x65:
      case 0x66:
      {
        state.ptr += 1;
        goto state_label_42; // string_esc_universal_3
      }break;
    }
  }
  {
state_label_42: // string_esc_universal_3
    if (state.ptr == state.opl_ptr){
      if ((true)){
        goto state_label_32; // string
      }
    }
    switch (*state.ptr){
      default:
      {
        goto state_label_32; // string
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:case 0x38:case 0x39:case 0x41:case 0x42:case 0x43:case 0x44:
      case 0x45:case 0x46:case 0x61:case 0x62:case 0x63:case 0x64:case 0x65:
      case 0x66:
      {
        state.ptr += 1;
        goto state_label_43; // string_esc_universal_2
      }break;
    }
  }
  {
state_label_43: // string_esc_universal_2
    if (state.ptr == state.opl_ptr){
      if ((true)){
        goto state_label_32; // string
      }
    }
    switch (*state.ptr){
      default:
      {
        goto state_label_32; // string
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:case 0x38:case 0x39:case 0x41:case 0x42:case 0x43:case 0x44:
      case 0x45:case 0x46:case 0x61:case 0x62:case 0x63:case 0x64:case 0x65:
      case 0x66:
      {
        state.ptr += 1;
        goto state_label_44; // string_esc_universal_1
      }break;
    }
  }
  {
state_label_44: // string_esc_universal_1
    if (state.ptr == state.opl_ptr){
      if ((true)){
        goto state_label_32; // string
      }
    }
    switch (*state.ptr){
      default:
      {
        goto state_label_32; // string
      }break;
      case 0x30:case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
      case 0x37:case 0x38:case 0x39:case 0x41:case 0x42:case 0x43:case 0x44:
      case 0x45:case 0x46:case 0x61:case 0x62:case 0x63:case 0x64:case 0x65:
      case 0x66:
      {
        state.ptr += 1;
        goto state_label_32; // string
      }break;
    }
  }
  {
state_label_45: // raw_string_get_delim
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LexError;
            token.kind = 2;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_EOF;
            token.kind = 0;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        result = true;
        goto end;
      }
    }
    switch (*state.ptr){
      default:
      {
        state.ptr += 1;
        goto state_label_45; // raw_string_get_delim
      }break;
      case 0x20:case 0x29:case 0x5c:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LexError;
            token.kind = 2;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x28:
      {
        state.delim_one_past_last = state.ptr;
        state.ptr += 1;
        goto state_label_46; // raw_string_find_close
      }break;
    }
  }
  {
state_label_46: // raw_string_find_close
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LexError;
            token.kind = 2;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_EOF;
            token.kind = 0;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        result = true;
        goto end;
      }
    }
    switch (*state.ptr){
      default:
      {
        state.ptr += 1;
        goto state_label_46; // raw_string_find_close
      }break;
      case 0x29:
      {
        state.ptr += 1;
        goto state_label_47; // raw_string_try_delim
      }break;
    }
  }
  {
state_label_47: // raw_string_try_delim
    u64 delim_length = state.delim_one_past_last - state.delim_first;
    u64 parse_length = 0;
    for (;;){
      if (parse_length == delim_length){
        goto state_label_48; // raw_string_try_quote
      }
      if (state.ptr == state.opl_ptr){
        goto state_label_48; // raw_string_try_quote
      }
      if (*state.ptr == state.delim_first[parse_length]){
        state.ptr += 1;
        parse_length += 1;
      }
      else{
        goto state_label_46; // raw_string_find_close
      }
    }
  }
  {
state_label_48: // raw_string_try_quote
    if (state.ptr == state.opl_ptr){
      if ((true)){
        goto state_label_46; // raw_string_find_close
      }
    }
    switch (*state.ptr){
      default:
      {
        goto state_label_46; // raw_string_find_close
      }break;
      case 0x22:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            if (HasFlag(state.flags_ZF0, 0x4)){
              token.sub_kind = TokenCppKind_LiteralStringWideRaw;
              token.kind = 10;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x8)){
              token.sub_kind = TokenCppKind_LiteralStringUTF8Raw;
              token.kind = 10;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x10)){
              token.sub_kind = TokenCppKind_LiteralStringUTF16Raw;
              token.kind = 10;
              break;
            }
            if (HasFlag(state.flags_ZF0, 0x20)){
              token.sub_kind = TokenCppKind_LiteralStringUTF32Raw;
              token.kind = 10;
              break;
            }
            token.sub_kind = TokenCppKind_LiteralStringRaw;
            token.kind = 10;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_49: // comment_block
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_BlockComment;
            token.kind = 3;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_EOF;
            token.kind = 0;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        result = true;
        goto end;
      }
    }
    switch (*state.ptr){
      default:
      {
        state.ptr += 1;
        goto state_label_49; // comment_block
      }break;
      case 0x0a:
      {
        state.ptr += 1;
        state.flags_KB0 &= ~(0x1);
        state.flags_KF0 &= ~(0x1);
        goto state_label_49; // comment_block
      }break;
      case 0x2a:
      {
        state.ptr += 1;
        goto state_label_50; // comment_block_try_close
      }break;
    }
  }
  {
state_label_50: // comment_block_try_close
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_BlockComment;
            token.kind = 3;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_EOF;
            token.kind = 0;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        result = true;
        goto end;
      }
    }
    switch (*state.ptr){
      default:
      {
        state.ptr += 1;
        goto state_label_49; // comment_block
      }break;
      case 0x2a:
      {
        state.ptr += 1;
        goto state_label_50; // comment_block_try_close
      }break;
      case 0x2f:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_BlockComment;
            token.kind = 3;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_51: // comment_line
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LineComment;
            token.kind = 3;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        state.ptr += 1;
        goto state_label_51; // comment_line
      }break;
      case 0x0a:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LineComment;
            token.kind = 3;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x5c:
      {
        state.ptr += 1;
        goto state_label_52; // comment_line_backslashing
      }break;
    }
  }
  {
state_label_52: // comment_line_backslashing
    if (state.ptr == state.opl_ptr){
      if ((true)){
        result = true;
        goto end;
      }
    }
    switch (*state.ptr){
      default:
      {
        state.ptr += 1;
        goto state_label_51; // comment_line
      }break;
      case 0x0d:
      {
        state.ptr += 1;
        goto state_label_52; // comment_line_backslashing
      }break;
    }
  }
  {
state_label_53: // op stage
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Colon;
            token.kind = 15;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Colon;
            token.kind = 15;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x3a:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_ColonColon;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_54: // op stage
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Plus;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Plus;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x2b:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_PlusPlus;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x3d:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_PlusEq;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_55: // op stage
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Minus;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Minus;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x2d:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_MinusMinus;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x3d:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_MinusEq;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x3e:
      {
        state.ptr += 1;
        goto state_label_56; // op stage
      }break;
    }
  }
  {
state_label_56: // op stage
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Arrow;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Arrow;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x2a:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_ArrowStar;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_57: // op stage
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Less;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Less;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x3c:
      {
        state.ptr += 1;
        goto state_label_66; // op stage
      }break;
      case 0x3d:
      {
        state.ptr += 1;
        goto state_label_59; // op stage
      }break;
    }
  }
  {
state_label_58: // op stage
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Grtr;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Grtr;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x3d:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_GrtrEq;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x3e:
      {
        state.ptr += 1;
        goto state_label_67; // op stage
      }break;
    }
  }
  {
state_label_59: // op stage
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LessEq;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LessEq;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x3e:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Compare;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_60: // op stage
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Eq;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Eq;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x3d:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_EqEq;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_61: // op stage
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Not;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Not;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x3d:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_NotEq;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_62: // op stage
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_And;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_And;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x26:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_AndAnd;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_63: // op stage
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Or;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Or;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x7c:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_OrOr;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_64: // op stage
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Star;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Star;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x3d:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_StarEq;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_65: // op stage
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Mod;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_Mod;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x3d:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_ModEq;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_66: // op stage
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LeftLeft;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LeftLeft;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x3d:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LeftLeftEq;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_67: // op stage
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_RightRight;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_RightRight;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x3d:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_RightRightEq;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_68: // op stage
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_PPStringify;
            token.kind = 15;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_PPStringify;
            token.kind = 15;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x23:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_PPConcat;
            token.kind = 15;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
  {
state_label_69: // op stage
    if (state.ptr == state.opl_ptr){
      if ((true)){
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LexError;
            token.kind = 2;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }
    }
    switch (*state.ptr){
      default:
      {
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_LexError;
            token.kind = 2;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
      case 0x2e:
      {
        state.ptr += 1;
        {
          Token token = {};
          token.pos = (i64)(state.emit_ptr - state.base);
          token.size = (i64)(state.ptr - state.emit_ptr);
          token.flags = state.flags_KB0;
          do{
            token.sub_kind = TokenCppKind_DotDotDot;
            token.kind = 7;
          }while(0);
          token_list_push(arena, list, &token);
          emit_counter += 1;
          state.emit_ptr = state.ptr;
          if (emit_counter == max){
            goto end;
          }
        }
        state.flags_ZF0 = 0;
        goto state_label_1; // root
      }break;
    }
  }
end:;
  block_copy_struct(state_ptr, &state);
  return(result);
}
internal Token_List
lex_full_input_cpp(Arena *arena, String_Const_u8 input){
  Lex_State_Cpp state = {};
  lex_full_input_cpp_init(&state, input);
  Token_List list = {};
  lex_full_input_cpp_breaks(arena, &list, &state, max_u64);
  return(list);
}
