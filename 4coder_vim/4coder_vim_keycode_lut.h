
global char* keycode_lut[2*KeyCode_COUNT];

function void init_keycode_lut(){
  for(i32 i=0; i<2*KeyCode_COUNT; i++){
    keycode_lut[i] = "";
  }
  i32 shift = KeyCode_COUNT;
  char *lowercase_alpha = "a\0b\0c\0d\0e\0f\0g\0h\0i\0j\0k\0l\0m\0n\0o\0p\0q\0r\0s\0t\0u\0v\0w\0x\0y\0z";
  for(i32 i='a'; i<='z'; i++){
    i32 index = i-'a';
    keycode_lut[KeyCode_A+index+shift] = key_code_name[KeyCode_A+index];
    keycode_lut[KeyCode_A+index] = lowercase_alpha + 2*index;
  }
  char *shift_digit = ")\0!\0@\0#\0$\0%\0^\0&\0*\0(";
  for(i32 i=0; i<=9; i++){
    keycode_lut[KeyCode_0+i] = key_code_name[KeyCode_0+i];
    keycode_lut[KeyCode_0+i+shift] = shift_digit + 2*i;;
  }
  char *function_keys = "F1\0F2\0F3\0F4\0F5\0F6\0F7\0F8\0F9\0F10\0F11\0F12";
  for(i32 i=0; i<12; i++){
    keycode_lut[KeyCode_F1+i] = keycode_lut[KeyCode_F1+i+shift] = function_keys + (3*i + (i>9)*(i-9));
  }

  keycode_lut[KeyCode_Space]     = keycode_lut[KeyCode_Space+shift]         = "Sp";
  keycode_lut[KeyCode_Tab]       = keycode_lut[KeyCode_BackwardSlash+shift] = "Tab";
  keycode_lut[KeyCode_Escape]    = keycode_lut[KeyCode_Escape+shift]        = "ESC";
  keycode_lut[KeyCode_Pause]     = keycode_lut[KeyCode_Pause+shift]         = key_code_name[KeyCode_Pause];
  keycode_lut[KeyCode_Up]        = keycode_lut[KeyCode_Up+shift]            = key_code_name[KeyCode_Up];
  keycode_lut[KeyCode_Down]      = keycode_lut[KeyCode_Down+shift]          = key_code_name[KeyCode_Down];
  keycode_lut[KeyCode_Left]      = keycode_lut[KeyCode_Left+shift]          = key_code_name[KeyCode_Left];
  keycode_lut[KeyCode_Right]     = keycode_lut[KeyCode_Right+shift]         = key_code_name[KeyCode_Right];
  keycode_lut[KeyCode_Backspace] = keycode_lut[KeyCode_Backspace+shift]     = "BS";
  keycode_lut[KeyCode_Return]    = keycode_lut[KeyCode_Escape+shift]        = "CR";
  keycode_lut[KeyCode_Delete]    = keycode_lut[KeyCode_Delete+shift]        = "Del";
  keycode_lut[KeyCode_Insert]    = keycode_lut[KeyCode_Insert+shift]        = "Ins";
  keycode_lut[KeyCode_Home]      = keycode_lut[KeyCode_Home+shift]          = "Home";
  keycode_lut[KeyCode_End]       = keycode_lut[KeyCode_End+shift]           = "End";
  keycode_lut[KeyCode_PageUp]    = keycode_lut[KeyCode_PageUp+shift]        = "PageUp";
  keycode_lut[KeyCode_PageDown]  = keycode_lut[KeyCode_PageDown+shift]      = "PageDown";
  keycode_lut[KeyCode_CapsLock]  = keycode_lut[KeyCode_CapsLock+shift]      = "Caps";
  keycode_lut[KeyCode_NumLock]   = keycode_lut[KeyCode_NumLock+shift]       = "NumLock";

  keycode_lut[KeyCode_Tick]          = "`"; keycode_lut[KeyCode_Tick+shift]           = "~";
  keycode_lut[KeyCode_Minus]         = "-"; keycode_lut[KeyCode_Minus+shift]          = "_";
  keycode_lut[KeyCode_Minus]         = "-"; keycode_lut[KeyCode_Minus+shift]          = "_";
  keycode_lut[KeyCode_Equal]         = "="; keycode_lut[KeyCode_Equal+shift]          = "+";
  keycode_lut[KeyCode_LeftBracket]   = "["; keycode_lut[KeyCode_LeftBracket+shift]    = "{";
  keycode_lut[KeyCode_RightBracket]  = "]"; keycode_lut[KeyCode_RightBracket+shift]   = "}";
  keycode_lut[KeyCode_Semicolon]     = ";"; keycode_lut[KeyCode_Semicolon+shift]      = ":";
  keycode_lut[KeyCode_Quote]         = "'"; keycode_lut[KeyCode_Quote+shift]          = "\"";
  keycode_lut[KeyCode_Comma]         = ","; keycode_lut[KeyCode_Comma+shift]          = "<";
  keycode_lut[KeyCode_Period]        = "."; keycode_lut[KeyCode_Period+shift]         = ">";
  keycode_lut[KeyCode_ForwardSlash]  = "/"; keycode_lut[KeyCode_ForwardSlash+shift]   = "?";
  keycode_lut[KeyCode_BackwardSlash] = "\\"; keycode_lut[KeyCode_BackwardSlash+shift] = "|";
}
