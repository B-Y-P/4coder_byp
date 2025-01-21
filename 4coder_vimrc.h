
#define VIM_DO_ANIMATE                 1
#define VIM_USE_TRADITIONAL_CHORDS     0
#define VIM_USE_REIGSTER_BUFFER        1

enum BYP_Vim_Request{
  BYP_REQUEST_Title,
  BYP_REQUEST_Comment,
  BYP_REQUEST_UnComment,

  BYP_REQUEST_COUNT
};

enum BYP_Vim_Objects{
  BYP_OBJECT_param0,
  BYP_OBJECT_param1,
  BYP_OBJECT_camel0,
  BYP_OBJECT_camel1,

  BYP_OBJECT_COUNT
};
#define VIM_ADDITIONAL_REQUESTS        BYP_REQUEST_COUNT
#define VIM_ADDITIONAL_TEXT_OBJECTS    BYP_OBJECT_COUNT

#define VIM_ADDITIONAL_PEEK            2

#define VIM_DEFAULT_REGISTER           unnamed

#define VIM_USE_BOTTOM_LISTER          1
#define VIM_LISTER_RANGE               3,5
#define VIM_LISTER_MAX_RATIO           0.35f
