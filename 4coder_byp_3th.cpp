
#if 0
enum State_3th{
	S_Space,
	S_Ident,
	S_Number,
	S_CoLine,
	S_CoNest,
	S_StrPre,
	S_StrMid,
	S_StrEsc,
	S_COUNT
};

// table fits in 16 cache-lines :)
u32 dfa_3th_table[256] = { 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66040000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66043000, 0x66143111, 0x60643111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66113114, 0x66103111, 0x66143111, 0x66143111, 0x66143115, 0x66143111, 0x66143115, 0x66143111, 0x66143212, 0x66143212, 0x66143212, 0x66143212, 0x66143212, 0x66143212, 0x66143212, 0x66143212, 0x66143212, 0x66143212, 0x66143110, 0x66143110, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143212, 0x66143212, 0x66143212, 0x66143212, 0x66143212, 0x66143212, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x67143113, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143212, 0x66143212, 0x66143212, 0x66143212, 0x66143212, 0x66143212, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, 0x66143111, };

internal Token_List
lex_full_input_3th(Arena *arena, String_Const_u8 input){
	//lex_full_input_cpp
	Token_List list = {};
	Token token = {0, 0, TokenBaseKind_Whitespace};
	u32 a = S_Space, b=0;
	i64 stack_depth = 0;
	u64 i=0;
	for (; i<input.size; i += (a=b, 1)){
		u8 ch = input.str[i];
		u32 row = dfa_3th_table[ch];
		b = ((row >> (a*4)) & 7);

#define T(a,b) ((a<<4) + b)
#define Emit(z,K)                          \
{ token.kind = K;                        \
token.size = i - token.pos + z;        \
token_list_push(arena, &list, &token); \
token.pos = i + z; }

		switch(T(a, b)){
			case T(S_Space, S_Space):{
				Token_Base_Kind tok = (ch == ':' ? TokenBaseKind_Operator :
									   ch == ';' ? TokenBaseKind_StatementClose : 0);
				if(tok != 0){
					Emit(0, TokenBaseKind_Whitespace);
					Emit(1, tok);
				}
			} break;

			case T(S_Space,  S_Ident):
			case T(S_Space,  S_Number):
			case T(S_Space,  S_CoLine):
			case T(S_Space,  S_StrPre): { Emit(0, TokenBaseKind_Whitespace);      } break;
			case T(S_Ident,  S_Space):  { Emit(0, TokenBaseKind_Identifier);      } break;
			case T(S_Number, S_Space):  { Emit(0, TokenBaseKind_LiteralInteger);  } break;
			case T(S_CoLine, S_Space):  { Emit(0, TokenBaseKind_Comment);         } break;
			case T(S_StrPre, S_Space):  { Emit(0, TokenBaseKind_Identifier);      } break;
			case T(S_StrMid, S_Space):  { Emit(1, TokenBaseKind_LiteralString);   } break;

			case T(S_Space,  S_CoNest): { stack_depth=1; Emit(0, TokenBaseKind_Whitespace); } break;
			case T(S_CoNest, S_Ident):  {    ++stack_depth; b = S_CoNest; } break;
			case T(S_CoNest, S_Space):  { if(--stack_depth) b = S_CoNest; else{ Emit(1, TokenBaseKind_Comment); } } break;
		}
	}

	if(false){}
	else if(a == S_Space){ Emit(0, TokenBaseKind_Whitespace); }
	else if(a == S_Ident){ Emit(0, TokenBaseKind_Identifier); }
	else if(a == S_Number){ Emit(0, TokenBaseKind_LiteralInteger); }
	else if(a == S_CoLine){ Emit(0, TokenBaseKind_Comment); }
	else if(a == S_CoNest){ Emit(0, TokenBaseKind_Comment); }
	else if(a == S_StrPre){ Emit(0, TokenBaseKind_Identifier); }
	else if(a == S_StrMid){ Emit(0, TokenBaseKind_LiteralString); }
	else if(a == S_StrEsc){ Emit(0, TokenBaseKind_LiteralString); }
	Emit(0, TokenBaseKind_EOF);

	return(list);
}
#else

enum State_3th{
	S_Space,
	S_Ident,
	S_Number,
	S_CoLine,
	S_CoNest,
	S_StrPre,
	S_StrMid,
	S_StrEsc,
	S_Root,
	S_COUNT
};

const u32 dfa_table_3th[256] = { 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066040000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 066043000, 0166143111, 0160643111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0466113114, 0166103111, 0166143111, 0166143111, 0566143115, 0166143111, 0566143115, 0166143111, 0266143212, 0266143212, 0266143212, 0266143212, 0266143212, 0266143212, 0266143212, 0266143212, 0266143212, 0266143212, 066143110, 066143110, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0266143212, 0266143212, 0266143212, 0266143212, 0266143212, 0266143212, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0367143113, 0166143111, 0166143111, 0166143111, 0166143111, 0266143212, 0266143212, 0266143212, 0266143212, 0266143212, 0266143212, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, 0166143111, };

internal Token_List
lex_full_input_3th(Arena *arena, String_Const_u8 input){
	Token_List list = {};
	Token token = {0, 0};

	u32 a=S_Root, b=S_Root;
	i64 stack_depth = 0;
	u64 i=0;
	for(; i<input.size; i += (a=b,1)){
		u8 ch = input.str[i];
		b = ((dfa_table_3th[ch] >> (a*3)) & 7);

#define Emit(z,K)                          \
{ token.kind = K;                        \
token.size = i - token.pos + z;        \
token_list_push(arena, &list, &token); \
token.pos = i + z; }

		switch((a<<3) + b){
			case 0100:
			case 000:{
				if(ch > 32){
					if(a==0){ Emit(0, TokenBaseKind_Whitespace); }
					Emit(1, Token_Base_Kind(ch == ':' ? TokenBaseKind_Operator : TokenBaseKind_StatementClose));
				}
			} break;
			case 001:{                              Emit(0, TokenBaseKind_Whitespace); } break;
			case 002:{                              Emit(0, TokenBaseKind_Whitespace); } break;
			case 003:{                              Emit(0, TokenBaseKind_Whitespace); } break;
			case 004:{      stack_depth=1;          Emit(0, TokenBaseKind_Whitespace); } break;
			case 005:{                              Emit(0, TokenBaseKind_Whitespace); } break;
			case 010:{                              Emit(0, TokenBaseKind_Identifier); } break;
			case 020:{                              Emit(0, TokenBaseKind_LiteralInteger); } break;
			case 030:{                              Emit(0, TokenBaseKind_Comment); } break;
			case 040:{ if(--stack_depth) b=04; else Emit(1, TokenBaseKind_Comment); } break;
			case 041:{    ++stack_depth; b=04; } break;
			case 050:{                              Emit(0, TokenBaseKind_Identifier); } break;
			case 060:{                              Emit(1, TokenBaseKind_LiteralString); } break;
			case 0104:{ stack_depth=1; } break;
		}
	}

	if(false){}
	else if(a == S_Space) { Emit(0, TokenBaseKind_Whitespace); }
	else if(a == S_Ident) { Emit(0, TokenBaseKind_Identifier); }
	else if(a == S_Number){ Emit(0, TokenBaseKind_LiteralInteger); }
	else if(a == S_CoLine){ Emit(0, TokenBaseKind_Comment); }
	else if(a == S_CoNest){ Emit(0, TokenBaseKind_Comment); }
	else if(a == S_StrPre){ Emit(0, TokenBaseKind_Identifier); }
	else if(a == S_StrMid){ Emit(0, TokenBaseKind_LiteralString); }
	else if(a == S_StrEsc){ Emit(0, TokenBaseKind_LiteralString); }
	Emit(0, TokenBaseKind_EOF);

	return(list);
}
#endif


function void
do_full_lex_async_3th(Async_Context *actx, String_Const_u8 data){
	// do_full_lex_async
	if (data.size != sizeof(Buffer_ID)){ return; }
	Application_Links *app = actx->app;
	Buffer_ID buffer = *(Buffer_ID*)data.str;
	Scratch_Block scratch(app);

	String_Const_u8 contents = {};
	acquire_global_frame_mutex(app);
	contents = push_whole_buffer(app, scratch, buffer);
	release_global_frame_mutex(app);

	Token_List list = lex_full_input_3th(scratch, contents);
	if (async_check_canceled(actx)){ return; }

	acquire_global_frame_mutex(app);
	Managed_Scope scope = buffer_get_managed_scope(app, buffer);
	if (scope != 0){
		Base_Allocator *allocator = managed_scope_allocator(app, scope);
		Token_Array *tokens_ptr = scope_attachment(app, scope, attachment_tokens, Token_Array);
		base_free(allocator, tokens_ptr->tokens);
		Token_Array tokens = {};
		tokens.tokens = base_array(allocator, Token, list.total_count);
		tokens.count = list.total_count;
		tokens.max = list.total_count;
		token_fill_memory_from_list(tokens.tokens, &list);
		block_copy_struct(tokens_ptr, &tokens);
	}
	buffer_mark_as_modified(buffer);
	release_global_frame_mutex(app);
}

function b32
parse_3th(Code_Index_File *index, Generic_Parse_State* state){
	//generic_parse_full_input_breaks
	Token_Iterator_Array it = state->it;

	for (;;){
		if (!token_it_inc(&it)) { break; }
		Token* tok1 = token_it_read(&it);
		if (tok1 == 0 || tok1->kind != TokenBaseKind_Operator){ continue; }

		if (!token_it_inc(&it)) { break; }
		Token *tok2 = token_it_read(&it);
		if (tok2 == 0 || (tok2->kind != TokenBaseKind_Identifier &&
						  tok2->kind != TokenBaseKind_LiteralInteger)){ continue; }
		index_new_note(index, state, Ii64(tok2), CodeIndexNote_Function, 0);
	}

	index->nest_array = code_index_nest_ptr_array_from_list(state->arena, &index->nest_list);
	index->note_array = code_index_note_ptr_array_from_list(state->arena, &index->note_list);
	state->finished = true;

	return true;
}

function void
draw_3th(Application_Links *app, Buffer_ID buffer, Text_Layout_ID text_layout_id){
	Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
	Token_Array token_array = get_token_array_from_buffer(app, buffer);
	if(token_array.tokens == 0){ return; }
	Token_Iterator_Array it = token_iterator_pos(0, &token_array, visible_range.min);

	for (;;){
		if (!token_it_inc(&it)) { break; }
		Token* tok1 = token_it_read(&it);
		if (tok1 == 0 || tok1->kind != TokenBaseKind_Operator){ continue; }
		if (visible_range.max < tok1->pos){ break; }

		if (!token_it_inc(&it)) { break; }
		Token *tok2 = token_it_read(&it);
		if (tok2 == 0 || (tok2->kind != TokenBaseKind_Identifier &&
						  tok2->kind != TokenBaseKind_LiteralInteger)){ continue; }
		paint_text_color(app, text_layout_id, Ii64(tok2), fcolor_resolve(fcolor_id(defcolor_type)));
	}
}