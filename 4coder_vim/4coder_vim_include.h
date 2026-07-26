// 4coder_vim
// BYP

// You can add or override entries in vim_request_vtable and vim_text_object_vtable
// just make sure it's declared in the #define before including this file

CUSTOM_ID(colors, defcolor_vim_filebar_pop);
CUSTOM_ID(colors, defcolor_vim_chord_text);
CUSTOM_ID(colors, defcolor_vim_chord_unresolved);
CUSTOM_ID(colors, defcolor_vim_chord_error);

// [0,1]
#ifndef VIM_DO_ANIMATE
#define VIM_DO_ANIMATE 1
#endif

// {unnamed, system}
#ifndef VIM_DEFAULT_REGISTER
#define VIM_DEFAULT_REGISTER unnamed
#endif

// [0,1]
#ifndef VIM_USE_REIGSTER_BUFFER
#define VIM_USE_REIGSTER_BUFFER 1
#endif

// [0,)
#ifndef VIM_ADDITIONAL_REQUESTS
#define VIM_ADDITIONAL_REQUESTS 0
#endif

// [0,)
#ifndef VIM_ADDITIONAL_TEXT_OBJECTS
#define VIM_ADDITIONAL_TEXT_OBJECTS 0
#endif

// [0,)
#ifndef VIM_ADDITIONAL_PEEK
#define VIM_ADDITIONAL_PEEK 0
#endif


/// If you _really_ want to change dynamic register allocation, go for it
#ifndef VIM_GROW_RATE
#define VIM_GROW_RATE(S) (Max(u64(128), u64(1.5*(S))))
#endif

#include "4coder_vim_keycode_lut.h"
#include "4coder_vim_base_types.h"