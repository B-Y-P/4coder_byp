#if 0
b8 Table::lookup_i(u64 key, i64 *oi);
b8 Table::lookup(u64 key);
b8 Table::lookup(u64 key, u64* ov);
b8 Table::lookup(u64 key, u64 val);
b8 Table::update(u64 key, u64 val);
b8 Table::insert(u64 key, u64 val);
b8 Table::upsert(u64 key, u64 val);
b8 Table::remove(u64 key);
b8 Table::grow(u64 new_cap);
#endif

struct Table{
  u64 *keys;   // nil-slot at [-1], probe slots at [0,cap)
  u64 *vals;   //
  i64 len;     // len < 3/4*cap  gurantees probing finds nil for termination
  i64 cap;     // pow-2
  b64 has_nil; // manually track/intercept nil-keys to not interfere with probing

  b8 lookup_i(u64 key, i64 *oi){
    if(key == 0){ return has_nil ? (*oi=-1, true) : false; }
    for(u64 i=table_hash(key) & (cap-1);;){
      if(keys[i] ==   0){ *oi=i; return false; }
      if(keys[i] == key){ *oi=i; return  true; }
      i = (i+1) & (cap-1);
    }
  }

  b8 place(u64 key, u64 val, b32 no_overwrite){
    if(3*cap <= 4*len && !grow(cap==0 ? 32 : 2*cap)){ return false; }
    i64 i; if(!lookup_i(key, &i)){ len += (key != 0); } else if(no_overwrite){ return true; }
    keys[i] = key;
    vals[i] = val;
    return true;
  }

  b8 lookup(u64 key)         { i64 i; return lookup_i(key, &i); }
  b8 lookup(u64 key, u64* ov){ i64 i; return lookup_i(key, &i) ? (*ov=vals[i], true) : false; }
  b8 lookup(u64 key, u64 val){ i64 i; return lookup_i(key, &i) ? (vals[i]==val)      : false; }
  b8 update(u64 key, u64 val){ i64 i; return lookup_i(key, &i) ? (vals[i]=val, true) : false; }
  b8 insert(u64 key, u64 val){ return place(key, val,  true); }
  b8 upsert(u64 key, u64 val){ return place(key, val, false); }

  b8 remove(u64 key){
    if(key == 0){ return has_nil ? (has_nil=false, true) : false; }
    i64 i,j; if(!lookup_i(key, &i)){ return false; }
    len--;
    keys[j=i] = 0;
    for(;;){
      j = (j+1) & (cap-1);
      if(keys[i] == 0){ return true; }
      i64 h = (i64)table_hash(keys[j]);
      if(h==j){ continue; }
      if(h<j){ if(i<h || j<i) continue; }
      if(j<h){ if(j<i && i<h) continue; }
      keys[i] = keys[j];
      vals[i] = vals[j];
      keys[i=j] = 0;
    }
  }

  b8 grow(i64 new_cap){
    Base_Allocator* alloc = get_base_allocator_system();
    u64* ptr = base_array(alloc, u64, 2*(1+new_cap));
    if(ptr == NULL){ return false; }
    ptr += 1;
    Table t = Table{ptr, ptr+new_cap, 0, new_cap, has_nil};
    if(has_nil){ t.keys[-1]=keys[-1]; t.vals[-1]=vals[-1]; }
    block_zero_dynamic_array(ptr, new_cap);
    for(i64 i=0; i<cap; i++){ if(keys[i] != 0) t.insert(keys[i], vals[i]); }
    if(keys){ base_free(alloc, keys-1); }
    *this = t;
    return true;
  }
};

internal void
table_clear(Table *table){
  if(table->keys == NULL){ return; }
  table->len = 0;
  table->has_nil = false;
  block_zero_dynamic_array(table->keys, table->cap);
}

internal void
table_free(Table *table){
  if(table->keys == NULL){ return; }
  Base_Allocator *alloc = get_base_allocator_system();
  base_free(alloc, table->keys-1);
  block_zero_struct(table);
}

//-

global Table vim_table;

typedef u8 KeyMod;
enum{ Ctl=1, Alt=2, Sft=4, Mnu=8, Cmd=16, };

union Vim_Bind_Key{
  struct{ u8 mode, sub_mode, _a_, _b_, _c_, mods, event_kind, event_code; };
  u64 bits;
};

function KeyMod mods_from_code(Key_Code k){
  return (k == KeyCode_Control ? Ctl :
          k == KeyCode_Alt     ? Alt :
          k == KeyCode_Shift   ? Sft :
          k == KeyCode_Command ? Cmd :
          k == KeyCode_Menu    ? Mnu : 0);
}

function Vim_Bind_Key key_from_event(Input_Event *e){
  Vim_Bind_Key key = {};
  key.mode       = (u8)vim_state.mode;
  key.sub_mode   = (u8)vim_state.sub_mode;
  key.event_kind = (u8)e->kind;
  key.event_code = (u8)(e->kind == InputEventKind_KeyStroke          ? e->key.code   :
                        e->kind == InputEventKind_KeyRelease         ? e->key.code   :
                        e->kind == InputEventKind_MouseButton        ? e->mouse.code :
                        e->kind == InputEventKind_MouseButtonRelease ? e->mouse.code :
                        e->kind == InputEventKind_Core               ? e->core.code  : 0);
  Input_Modifier_Set mod_set = (e->kind == InputEventKind_KeyStroke          ? e->key.modifiers         :
                                e->kind == InputEventKind_KeyRelease         ? e->key.modifiers         :
                                e->kind == InputEventKind_MouseButton        ? e->mouse.modifiers       :
                                e->kind == InputEventKind_MouseButtonRelease ? e->mouse.modifiers       :
                                e->kind == InputEventKind_MouseWheel         ? e->mouse_wheel.modifiers :
                                e->kind == InputEventKind_MouseMove          ? e->mouse_move.modifiers  : Input_Modifier_Set{});
  foreach(i, mod_set.count){ key.mods |= (u8)mods_from_code(mod_set.mods[i]); }
  return key;
}

global bool vim_was_in_sub_mode;

function void vim_pre_command(Vim_Bind_Key key, Implicit_Map_Result bind){
  vim_was_in_sub_mode = (vim_state.sub_mode != SUB_None);
  if (key.event_kind == InputEventKind_KeyStroke){
    vim_cursor_blink = 0;
    vim_state.active_command = bind.command;
    if (mods_from_code(key.event_code) == 0){
      vim_state.chord_state = (bind.command == no_op || bind.command == 0) ? VIM_CHORD_Error : VIM_CHORD_Resolved;
    }
  }
}

function void vim_post_command(){
  if (vim_was_in_sub_mode){ vim_state.sub_mode = SUB_None; }
  if (vim_keystroke_text.size >= vim_keystroke_text.cap){ vim_keystroke_text.size = 0; }
}

function Implicit_Map_Result
vim_input_map(Application_Links *app, String_ID lang, String_ID global_mode, Input_Event *event){
  Implicit_Map_Result bind = {};
  Vim_Bind_Key key = key_from_event(event);

  vim_pre_keystroke_size = vim_keystroke_text.size;
  if (key.event_kind == InputEventKind_KeyStroke && mods_from_code(key.event_code) == 0){
    if (vim_state.chord_state != VIM_CHORD_Pending){ vim_keystroke_text.size=0; }
    if (key.mods & Ctl){ string_append_character(&vim_keystroke_text, '^'); }
    if (key.mods & Alt){ string_append_character(&vim_keystroke_text, '~'); }
    i32 index = key.event_code + KeyCode_COUNT*HasFlag(key.mods, Sft);
    string_append(&vim_keystroke_text, SCu8(keycode_lut[index]));
  }

  if (!vim_table.lookup(key.bits, (u64*)&bind.command)){
    bind = default_implicit_map(app, 0, 0, event);
  }

  return MC_implicit_map_inner(app, event, bind);
}

//-

function void vim_map_bind_key(Table *t, u32 mode, u32 sub_mode, Custom_Command_Function *func, u64 mods, u64 key){
  Vim_Bind_Key k = { (u8)mode, (u8)sub_mode, 0,0,0, (u8)mods, (u8)InputEventKind_KeyStroke, (u8)key };
  t->upsert(k.bits, PtrAsInt(func));
}

function void vim_map_bind_text(Table* t, u32 mode, u32 sub_mode, Custom_Command_Function *func){
  Vim_Bind_Key k = { (u8)mode, (u8)sub_mode, 0,0,0,0, (u8)InputEventKind_TextInsert, 0 };
  t->upsert(k.bits, PtrAsInt(func));
}

function b32 string_has_postfix(String_Const_u8 a, String_Const_u8 b){
  return string_match(string_postfix(a, b.size), b);
}

function b32 string_has_prefix(String_Const_u8 a, String_Const_u8 b){
  return string_match(string_prefix(a, b.size), b);
}

function void vim_table_builtin(Table *t);

function b32 vim_parse_vim_keys(Application_Links *app, Arena *arena, Config *parsed, Config_Assignment *node){
  Table t = {};
  vim_table_builtin(&t);

  if (!string_match(node->l->identifier, string_u8_litexpr("keys_vim")) || node->r->type != ConfigRValueType_Compound){ return false; }
  for (Config_Compound_Element *e=node->r->compound->first; e; e=e->next){
    if (e->r->type != ConfigRValueType_Compound){ continue; }
    Config_Compound *src = e->r->compound;

    String_Const_u8 str_modes   = {};
    String_Const_u8 str_command = {};
    String_Const_u8 str_keybind = {};
    if (!config_compound_string_member(parsed, src, "modes",   0, &str_modes))  { continue; }
    if (!config_compound_string_member(parsed, src, "cmd",     1, &str_command)){ continue; }
    if (!config_compound_string_member(parsed, src, "keybind", 2, &str_keybind)){ continue; }

    u64 f = PtrAsInt(no_op);
    String_Const_u8 cmd_name = str_command.size==0 ? string_u8_litexpr("no_op") : str_command;
    if (auto cmd = get_command_metadata_from_name(cmd_name)){
      f = PtrAsInt(cmd->proc);
    } else {
      Config_Get_Result member = config_compound_member(parsed, src, string_u8_litexpr("cmd"), 1);
      String_Const_u8 str_error = push_stringf(arena, "Invalid command: '%S'", str_command);
      def_config_push_error(arena, parsed, member.pos, (char*)str_error.str);
    }

    i64 idx = string_find_last(str_keybind, '|') + 1;
    String_Const_u8 bind_end = string_postfix(str_keybind, str_keybind.size - idx);
    i64 idx_split = string_find_first(bind_end, '-');
    String_Const_u8 key1 = string_skip_chop_whitespace(string_prefix(bind_end, idx_split));
    String_Const_u8 key2 = string_skip_chop_whitespace(string_postfix(bind_end, bind_end.size-idx_split-1));
    u8 k1=0, k2=0;
    for (i64 i=1; i<KeyCode_COUNT; i += 1){
      if (string_match(key1, SCu8(key_code_name[i]))){ k1 = (u8)i; }
      if (string_match(key2, SCu8(key_code_name[i]))){ k2 = (u8)i; }
    }

    if (k1 == 0 && k2 == 0){
      Config_Get_Result member = config_compound_member(parsed, src, string_u8_litexpr("keybind"), 2);
      String_Const_u8 str_error = push_stringf(arena, "Invalid keybind: '%S'", str_keybind);
      def_config_push_error(arena, parsed, member.pos, (char*)str_error.str);
      continue;
    }
    else if (k1 == 0){ k1 = k2; }
    else if (k2 == 0){ k2 = k1; }

    Vim_Bind_Key k = {};
    bool did_add = false;
    k.event_kind = InputEventKind_KeyStroke;
    k.mods     |= (u8)string_has_substr(str_keybind, string_u8_litexpr("Ctl"))*Ctl;
    k.mods     |= (u8)string_has_substr(str_keybind, string_u8_litexpr("Sft"))*Sft;
    k.mods     |= (u8)string_has_substr(str_keybind, string_u8_litexpr("Alt"))*Alt;
    k.mods     |= (u8)string_has_substr(str_keybind, string_u8_litexpr("Mnu"))*Mnu;
    k.mods     |= (u8)string_has_substr(str_keybind, string_u8_litexpr("Cmd"))*Cmd;
    k.sub_mode += (u8)string_has_prefix(str_keybind, string_u8_litexpr("SUB_G"))*SUB_G;
    k.sub_mode += (u8)string_has_prefix(str_keybind, string_u8_litexpr("SUB_Z"))*SUB_Z;
    k.sub_mode += (u8)string_has_prefix(str_keybind, string_u8_litexpr("SUB_Leader"))*SUB_Leader;
    k.sub_mode += (u8)string_has_prefix(str_keybind, string_u8_litexpr("SUB_Window"))*SUB_Window;
    if (string_match(str_modes, string_u8_litexpr("MAP"))){ str_modes = string_u8_litexpr("NIV"); }
    for(u8 i=Min(k1,k2); i<=Max(k1,k2); i++){
      k.event_code = i;
      if (string_has_substr(str_modes, string_u8_litexpr("N"))){ t.upsert((u64)VIM_Normal|k.bits, f); did_add=true; }
      if (string_has_substr(str_modes, string_u8_litexpr("I"))){ t.upsert((u64)VIM_Insert|k.bits, f); did_add=true; }
      if (string_has_substr(str_modes, string_u8_litexpr("V"))){ t.upsert((u64)VIM_Visual|k.bits, f); did_add=true; }
    }

    if (!did_add){
      Config_Get_Result member = config_compound_member(parsed, src, string_u8_litexpr("modes"), 0);
      String_Const_u8 str_error = push_stringf(arena, "Invalid modes: '%S'", str_modes);
      def_config_push_error(arena, parsed, member.pos, (char*)str_error.str);
    }
  }

  if (parsed->errors.first != 0){
    String_Const_u8 error_text = config_stringize_errors(app, arena, parsed);
    comp_error(app, error_text);
    if (error_text.size > 0){
      print_message(app, error_text);
    }
    table_free(&t);
  }else{
    table_free(&vim_table);
    vim_table = t;
  }

  return true;
}