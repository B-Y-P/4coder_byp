
function b32
vim_do_buffer_close_user_check(Application_Links *app, Buffer_ID buffer, View_ID view){
  Scratch_Block scratch(app);
  Lister_Choice_List list = {};
  lister_choice(scratch, &list, "(N)o"  , "", KeyCode_N, SureToKill_No);
  lister_choice(scratch, &list, "(Y)es" , "", KeyCode_Y, SureToKill_Yes);
  lister_choice(scratch, &list, "(S)ave", "", KeyCode_S, SureToKill_Save);

  Lister_Choice *choice = get_choice_from_user(app, string_u8_litexpr("There are unsaved changes, close anyway?"), list);

  b32 do_kill = false;
  if (choice != 0){
    switch (choice->user_data){
      case SureToKill_No:{} break;

      case SureToKill_Yes:{ do_kill = true; } break;

      case SureToKill_Save:{
        String_Const_u8 file_name = push_buffer_file_name(app, scratch, buffer);
        if(buffer_save(app, buffer, file_name, BufferSave_IgnoreDirtyFlag)){
          do_kill = true;
        }else{
#define M "Did not close '%.*s' because it did not successfully save."
          String_Const_u8 str = push_u8_stringf(scratch, M, string_expand(file_name));
#undef M
          print_message(app, str);
        }
      } break;
    }
  }

  return do_kill;
}

function Buffer_Kill_Result
vim_try_buffer_kill(Application_Links *app){
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  Buffer_Kill_Flag flags = 0;
  Buffer_Kill_Result result = buffer_kill(app, buffer, flags);
  if(result == BufferKillResult_Dirty){
    if(vim_do_buffer_close_user_check(app, buffer, view)){
      result = buffer_kill(app, buffer, BufferKill_AlwaysKill);
    }
  }
  return result;
}

function b32
vim_do_4coder_close_user_check(Application_Links *app, View_ID view){
  Scratch_Block scratch(app);
  Lister_Choice_List list = {};
  lister_choice(scratch, &list, "(N)o"  , "", KeyCode_N, SureToKill_No);
  lister_choice(scratch, &list, "(Y)es" , "", KeyCode_Y, SureToKill_Yes);
  lister_choice(scratch, &list, "(S)ave all and close", "", KeyCode_S, SureToKill_Save);

#define M "There are one or more buffers with unsave changes, close anyway?"
  Lister_Choice *choice = get_choice_from_user(app, string_u8_litexpr(M), list);
#undef M

  b32 do_exit = false;
  if(choice != 0){
    switch(choice->user_data){
      case SureToKill_No:{} break;

      case SureToKill_Yes:{
        allow_immediate_close_without_checking_for_changes = true;
        do_exit = true;
      } break;

      case SureToKill_Save:{
        save_all_dirty_buffers(app);
        allow_immediate_close_without_checking_for_changes = true;
        do_exit = true;
      } break;
    }
  }

  return do_exit;
}