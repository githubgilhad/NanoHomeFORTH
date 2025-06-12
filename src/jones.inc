;/* vim: set filetype=asm noexpandtab fileencoding=utf-8 nomodified nowrap textwidth=270 foldmethod=marker foldmarker={{{,}}} foldcolumn=4 ruler showcmd lcs=tab\:|- list: */


DEFWORD w_CtoD,0,"C>D",f_docol           // 000001 	: C>D LIT \'0x00000000 SWAP ;
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_swap_cw                 
	.long w_exit_cw                 

DEFWORD w_newdiv,0,"/",f_docol           // 000002 	: / /MOD SWAP DROP ;
	.long w_DIVMOD_cw               
	.long w_swap_cw                 
	.long w_drop_cw                 
	.long w_exit_cw                 

DEFWORD w_MOD,0,"MOD",f_docol            // 000003 	: MOD /MOD DROP ;
	.long w_DIVMOD_cw               
	.long w_drop_cw                 
	.long w_exit_cw                 

DEFWORD w_quoted_slash_n,0,"'\\n'",f_docol // 000004 	: '\n' LIT \'0x0000000A ;
	.long w_lit_cw                  
	.long 0x0000000A                
	.long w_exit_cw                 

DEFWORD w_BL,0,"BL",f_docol              // 000005 	: BL LIT \'0x00000020 ;
	.long w_lit_cw                  
	.long 0x00000020                
	.long w_exit_cw                 

DEFWORD w_CR,0,"CR",f_docol              // 000006 	: CR '\n' EMIT ;
	.long w_quoted_slash_n_cw       
	.long w_emit_cw                 
	.long w_exit_cw                 

DEFWORD w_SPACE,0,"SPACE",f_docol        // 000007 	: SPACE BL EMIT ;
	.long w_BL_cw                   
	.long w_emit_cw                 
	.long w_exit_cw                 

DEFWORD w_NEGATE,0,"NEGATE",f_docol      // 000008 	: NEGATE LIT \'0x00000000 SWAP - ;
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_swap_cw                 
	.long w_minus_cw                
	.long w_exit_cw                 

DEFWORD w_TRUE,0,"TRUE",f_docol          // 000009 	: TRUE LIT \'0x00000001 ;
	.long w_lit_cw                  
	.long 0x00000001                
	.long w_exit_cw                 

DEFWORD w_FALSE,0,"FALSE",f_docol        // 000010 	: FALSE LIT \'0x00000000 ;
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_exit_cw                 

DEFWORD w_NOT,0,"NOT",f_docol            // 000011 	: NOT 0= ;
	.long w_zero_cw                 
	.long w_exit_cw                 

DEFWORD w_LITERAL,FLG_IMMEDIATE,"LITERAL",f_docol // 000012 	: LITERAL IMMEDIATE ' LIT , , ;
	.long w_tick_cw                 
	.long w_lit_cw                  
	.long w_comma_cw                
	.long w_comma_cw                
	.long w_exit_cw                 

DEFWORD w_quoted_colon,0,"':'",f_docol   // 000013 	: ':' LIT \'0x0000003A ;
	.long w_lit_cw                  
	.long 0x0000003A                
	.long w_exit_cw                 

DEFWORD w_quoted_semicolon,0,"';'",f_docol // 000014 	: ';' LIT \'0x0000003B ;
	.long w_lit_cw                  
	.long 0x0000003B                
	.long w_exit_cw                 

DEFWORD w_quoted_open,0,"'('",f_docol    // 000015 	: '(' LIT \'0x00000028 ;
	.long w_lit_cw                  
	.long 0x00000028                
	.long w_exit_cw                 

DEFWORD w_quoted_close,0,"')'",f_docol   // 000016 	: ')' LIT \'0x00000029 ;
	.long w_lit_cw                  
	.long 0x00000029                
	.long w_exit_cw                 

DEFWORD w_quoted_quote,0,"'\"'",f_docol  // 000017 	: '"' LIT \'0x00000022 ;
	.long w_lit_cw                  
	.long 0x00000022                
	.long w_exit_cw                 

DEFWORD w_quoted_A,0,"'A'",f_docol       // 000018 	: 'A' LIT \'0x00000041 ;
	.long w_lit_cw                  
	.long 0x00000041                
	.long w_exit_cw                 

DEFWORD w_quoted_0,0,"'0'",f_docol       // 000019 	: '0' LIT \'0x00000030 ;
	.long w_lit_cw                  
	.long 0x00000030                
	.long w_exit_cw                 

DEFWORD w_quoted_minus,0,"'-'",f_docol   // 000020 	: '-' LIT \'0x0000002D ;
	.long w_lit_cw                  
	.long 0x0000002D                
	.long w_exit_cw                 

DEFWORD w_quoted_dot,0,"'.'",f_docol     // 000021 	: '.' LIT \'0x0000002E ;
	.long w_lit_cw                  
	.long 0x0000002E                
	.long w_exit_cw                 

DEFWORD w_quoted_less,0,"'<'",f_docol    // 000022 	: '<' LIT \'0x0000003C ;
	.long w_lit_cw                  
	.long 0x0000003C                
	.long w_exit_cw                 

DEFWORD w_quoted_more,0,"'>'",f_docol    // 000023 	: '>' LIT \'0x0000003E ;
	.long w_lit_cw                  
	.long 0x0000003E                
	.long w_exit_cw                 

DEFWORD w_quoted_pipe,0,"'|'",f_docol    // 000024 	: '|' LIT \'0x0000007C ;
	.long w_lit_cw                  
	.long 0x0000007C                
	.long w_exit_cw                 

DEFWORD w_COMPILE_in_brackets,FLG_IMMEDIATE,"[COMPILE]",f_docol // 000025 	: [COMPILE] IMMEDIATE WORD FIND >CFA , ;
	.long w_word_cw                 
	.long w_find_cw                 
	.long w_ToCFA_cw                
	.long w_comma_cw                
	.long w_exit_cw                 

DEFWORD w_RECURSE,FLG_IMMEDIATE,"RECURSE",f_docol // 000026 	: RECURSE IMMEDIATE LAST D@ >CFA , ;
	.long var_LAST_cw               
	.long w_DoubleAt_cw             
	.long w_ToCFA_cw                
	.long w_comma_cw                
	.long w_exit_cw                 

DEFWORD w_AGAIN,FLG_IMMEDIATE,"AGAIN",f_docol // 000027 	: AGAIN IMMEDIATE ' BRANCH , HERE D@ -D , ;
	.long w_tick_cw                 
	.long w_branch_cw               
	.long w_comma_cw                
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_minus_D_cw              
	.long w_comma_cw                
	.long w_exit_cw                 

DEFWORD w_UNLESS,FLG_IMMEDIATE,"UNLESS",f_docol // 000028 	: UNLESS IMMEDIATE ' NOT , IF ;
	.long w_tick_cw                 
	.long w_NOT_cw                  
	.long w_comma_cw                
	.long w_IF_cw                   
	.long w_exit_cw                 

DEFWORD w_Bracket,FLG_IMMEDIATE,"(",f_docol // 000029 	: ( IMMEDIATE LIT \'0x00000001 KEY DUP '(' = 0BRANCH \'0x00000014 DROP 1+ BRANCH \'0x00000018 ')' = 0BRANCH \'0x00000008 1- DUP 0= 0BRANCH \'0xFFFFFFB8 DROP ;
	.long w_lit_cw                  
	.long 0x00000001                
	.long w_key_cw                  
	.long w_dup_cw                  
	.long w_quoted_open_cw          
	.long w_equal_cw                
	.long w_0branch_cw              
	.long 0x00000014                
	.long w_drop_cw                 
	.long w_1plus_cw                
	.long w_branch_cw               
	.long 0x00000018                
	.long w_quoted_close_cw         
	.long w_equal_cw                
	.long w_0branch_cw              
	.long 0x00000008                
	.long w_1minus_cw               
	.long w_dup_cw                  
	.long w_zero_cw                 
	.long w_0branch_cw              
	.long 0xFFFFFFB8                
	.long w_drop_cw                 
	.long w_exit_cw                 

DEFWORD w_NIP,0,"NIP",f_docol            // 000030 	: NIP SWAP DROP ;
	.long w_swap_cw                 
	.long w_drop_cw                 
	.long w_exit_cw                 

DEFWORD w_NIP2,0,"NIP2",f_docol          // 000031 	: NIP2 SWAP2 DROP2 ;
	.long w_swap_D_cw               
	.long w_drop_D_cw               
	.long w_exit_cw                 

DEFWORD w_TUCK,0,"TUCK",f_docol          // 000032 	: TUCK SWAP OVER ;
	.long w_swap_cw                 
	.long w_OVER_cw                 
	.long w_exit_cw                 

DEFWORD w_PICK,0,"PICK",f_docol          // 000033 	: PICK LIT \'0x00000003 + LIT \'0x00000002 * C>D S? SWAP2 -D @ ;
	.long w_lit_cw                  
	.long 0x00000003                
	.long w_plus_cw                 
	.long w_lit_cw                  
	.long 0x00000002                
	.long w_times_cw                
	.long w_CtoD_cw                 
	.long w_StackAddress_cw         
	.long w_swap_D_cw               
	.long w_minus_D_cw              
	.long w_At_cw                   
	.long w_exit_cw                 

DEFWORD w_SPACES,0,"SPACES",f_docol      // 000034 	: SPACES DUP 0> 0BRANCH \'0x00000014 SPACE 1- BRANCH \'0xFFFFFFE4 DROP ;
	.long w_dup_cw                  
	.long w_positive_cw             
	.long w_0branch_cw              
	.long 0x00000014                
	.long w_SPACE_cw                
	.long w_1minus_cw               
	.long w_branch_cw               
	.long 0xFFFFFFE4                
	.long w_drop_cw                 
	.long w_exit_cw                 

DEFWORD w_newDECIMAL,0,"DECIMAL",f_docol // 000035 	: DECIMAL LIT \'0x0000000A BASE ! ;
	.long w_lit_cw                  
	.long 0x0000000A                
	.long var_BASE_cw               
	.long w_Store_cw                
	.long w_exit_cw                 

DEFWORD w_newHEX,0,"HEX",f_docol         // 000036 	: HEX LIT \'0x00000010 BASE ! ;
	.long w_lit_cw                  
	.long 0x00000010                
	.long var_BASE_cw               
	.long w_Store_cw                
	.long w_exit_cw                 

DEFWORD w_Udotdot,0,"U..",f_docol        // 000037 	: U.. BASE @ /MOD ?DUP 0BRANCH \'0x00000008 U.. DUP LIT \'0x0000000A < 0BRANCH \'0x00000010 '0' BRANCH \'0x00000014 LIT \'0x0000000A - 'A' + EMIT ;
	.long var_BASE_cw               
	.long w_At_cw                   
	.long w_DIVMOD_cw               
	.long w_QDUP_cw                 
	.long w_0branch_cw              
	.long 0x00000008                
	.long w_Udotdot_cw              
	.long w_dup_cw                  
	.long w_lit_cw                  
	.long 0x0000000A                
	.long w_less_cw                 
	.long w_0branch_cw              
	.long 0x00000010                
	.long w_quoted_0_cw             
	.long w_branch_cw               
	.long 0x00000014                
	.long w_lit_cw                  
	.long 0x0000000A                
	.long w_minus_cw                
	.long w_quoted_A_cw             
	.long w_plus_cw                 
	.long w_emit_cw                 
	.long w_exit_cw                 

DEFWORD w_dotS,0,".S",f_docol            // 000038 	: .S S? BASE C@ SWAP21 hex DUP2 S0 >D 0BRANCH \'0x0000003C LIT \'0x00000000 LIT \'0x00000002 -D DUP2 @ '<' EMIT U.. '>' EMIT BRANCH \'0xFFFFFFB8 DROP2 '|' EMIT SPACE BASE C! ;
	.long w_StackAddress_cw         
	.long var_BASE_cw               
	.long w_CharAt_cw               
	.long w_swap_21_cw              
	.long w_hex_cw                  
	.long w_dup_D_cw                
	.long const_S0_cw               
	.long w_greaterD_cw             
	.long w_0branch_cw              
	.long 0x0000003C                
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_lit_cw                  
	.long 0x00000002                
	.long w_minus_D_cw              
	.long w_dup_D_cw                
	.long w_At_cw                   
	.long w_quoted_less_cw          
	.long w_emit_cw                 
	.long w_Udotdot_cw              
	.long w_quoted_more_cw          
	.long w_emit_cw                 
	.long w_branch_cw               
	.long 0xFFFFFFB8                
	.long w_drop_D_cw               
	.long w_quoted_pipe_cw          
	.long w_emit_cw                 
	.long w_SPACE_cw                
	.long var_BASE_cw               
	.long w_CharStore_cw            
	.long w_exit_cw                 

DEFWORD w_UWIDTH,0,"UWIDTH",f_docol      // 000039 	: UWIDTH BASE @ / ?DUP 0BRANCH \'0x00000014 UWIDTH 1+ BRANCH \'0x0000000C LIT \'0x00000001 ;
	.long var_BASE_cw               
	.long w_At_cw                   
	.long w_newdiv_cw               
	.long w_QDUP_cw                 
	.long w_0branch_cw              
	.long 0x00000014                
	.long w_UWIDTH_cw               
	.long w_1plus_cw                
	.long w_branch_cw               
	.long 0x0000000C                
	.long w_lit_cw                  
	.long 0x00000001                
	.long w_exit_cw                 

DEFWORD w_UdotR,0,"U.R",f_docol          // 000040 	: U.R SWAP DUP UWIDTH ROT SWAP - SPACES U.. ;
	.long w_swap_cw                 
	.long w_dup_cw                  
	.long w_UWIDTH_cw               
	.long w_ROT_cw                  
	.long w_swap_cw                 
	.long w_minus_cw                
	.long w_SPACES_cw               
	.long w_Udotdot_cw              
	.long w_exit_cw                 

DEFWORD w_dotR,0,".R",f_docol            // 000041 	: .R SWAP DUP 0< 0BRANCH \'0x00000024 NEGATE LIT \'0x00000001 SWAP ROT 1- BRANCH \'0x00000014 LIT \'0x00000000 SWAP ROT SWAP DUP UWIDTH ROT SWAP - SPACES SWAP 0BRANCH \'0x0000000C '-' EMIT U.. ;
	.long w_swap_cw                 
	.long w_dup_cw                  
	.long w_negative_cw             
	.long w_0branch_cw              
	.long 0x00000024                
	.long w_NEGATE_cw               
	.long w_lit_cw                  
	.long 0x00000001                
	.long w_swap_cw                 
	.long w_ROT_cw                  
	.long w_1minus_cw               
	.long w_branch_cw               
	.long 0x00000014                
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_swap_cw                 
	.long w_ROT_cw                  
	.long w_swap_cw                 
	.long w_dup_cw                  
	.long w_UWIDTH_cw               
	.long w_ROT_cw                  
	.long w_swap_cw                 
	.long w_minus_cw                
	.long w_SPACES_cw               
	.long w_swap_cw                 
	.long w_0branch_cw              
	.long 0x0000000C                
	.long w_quoted_minus_cw         
	.long w_emit_cw                 
	.long w_Udotdot_cw              
	.long w_exit_cw                 

DEFWORD w_dotdot,0,"..",f_docol          // 000042 	: .. LIT \'0x00000000 .R SPACE ;
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_dotR_cw                 
	.long w_SPACE_cw                
	.long w_exit_cw                 

DEFWORD w_Udot,0,"U.",f_docol            // 000043 	: U. U.. SPACE ;
	.long w_Udotdot_cw              
	.long w_SPACE_cw                
	.long w_exit_cw                 

DEFWORD w_question,0,"?",f_docol         // 000044 	: ? @ . ;
	.long w_At_cw                   
	.long w_dot_cw                  
	.long w_exit_cw                 

DEFWORD w_WITHIN,0,"WITHIN",f_docol      // 000045 	: WITHIN -ROT OVER <= 0BRANCH \'0x00000028 > 0BRANCH \'0x00000010 TRUE BRANCH \'0x00000008 FALSE BRANCH \'0x0000000C DROP2 FALSE ;
	.long w_NROT_cw                 
	.long w_OVER_cw                 
	.long w_lessequal_cw            
	.long w_0branch_cw              
	.long 0x00000028                
	.long w_greater_cw              
	.long w_0branch_cw              
	.long 0x00000010                
	.long w_TRUE_cw                 
	.long w_branch_cw               
	.long 0x00000008                
	.long w_FALSE_cw                
	.long w_branch_cw               
	.long 0x0000000C                
	.long w_drop_D_cw               
	.long w_FALSE_cw                
	.long w_exit_cw                 

DEFWORD w_DEPTH,0,"DEPTH",f_docol        // 000046 	: DEPTH S? S0 -D LIT \'0x00000002 C>D -D SWAP DROP /2 ;
	.long w_StackAddress_cw         
	.long const_S0_cw               
	.long w_minus_D_cw              
	.long w_lit_cw                  
	.long 0x00000002                
	.long w_CtoD_cw                 
	.long w_minus_D_cw              
	.long w_swap_cw                 
	.long w_drop_cw                 
	.long w_div2_cw                 
	.long w_exit_cw                 

DEFWORD w_Ccomma,0,"C,",f_docol          // 000047 	: C, HERE D@ C! HERE D@ LIT \'0x00000001 +21 HERE D! ;
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_CharStore_cw            
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_lit_cw                  
	.long 0x00000001                
	.long w_plus21_cw               
	.long var_HERE_cw               
	.long w_DoubleStore_cw          
	.long w_exit_cw                 

DEFWORD w_Squote,FLG_IMMEDIATE,"S\"",f_docol // 000048 	: S" IMMEDIATE STATE C@ 0BRANCH \'0x0000007C ' LITSTRING , HERE D@ LIT \'0x00000000 LIT \'0x00000000 , KEY DUP '"' <> 0BRANCH \'0x00000010 C, BRANCH \'0xFFFFFFE0 DROP DUP2 HERE D@ SWAP2 -D 4D- SWAP2 D! BRANCH \'0x0000005C HERE D@ KEY DUP '"' <> 0BRANCH \'0x00000018 OVER21 C! 1D+ BRANCH \'0xFFFFFFD8 DROP HERE D@ -D HERE D@ SWAP2 SWAP DROP ;
	.long var_STATE_cw              
	.long w_CharAt_cw               
	.long w_0branch_cw              
	.long 0x0000007C                
	.long w_tick_cw                 
	.long w_LITSTRING_cw            
	.long w_comma_cw                
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_comma_cw                
	.long w_key_cw                  
	.long w_dup_cw                  
	.long w_quoted_quote_cw         
	.long w_notequal_cw             
	.long w_0branch_cw              
	.long 0x00000010                
	.long w_Ccomma_cw               
	.long w_branch_cw               
	.long 0xFFFFFFE0                
	.long w_drop_cw                 
	.long w_dup_D_cw                
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_swap_D_cw               
	.long w_minus_D_cw              
	.long w_4Dminus_cw              
	.long w_swap_D_cw               
	.long w_DoubleStore_cw          
	.long w_branch_cw               
	.long 0x0000005C                
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_key_cw                  
	.long w_dup_cw                  
	.long w_quoted_quote_cw         
	.long w_notequal_cw             
	.long w_0branch_cw              
	.long 0x00000018                
	.long w_OVER21_cw               
	.long w_CharStore_cw            
	.long w_1Dplus_cw               
	.long w_branch_cw               
	.long 0xFFFFFFD8                
	.long w_drop_cw                 
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_minus_D_cw              
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_swap_D_cw               
	.long w_swap_cw                 
	.long w_drop_cw                 
	.long w_exit_cw                 

DEFWORD w_dotquote,FLG_IMMEDIATE,".\"",f_docol // 000049 	: ." IMMEDIATE STATE C@ 0BRANCH \'0x0000001C S" ' TELL , BRANCH \'0x00000030 KEY DUP '"' = 0BRANCH \'0x0000000C DROP EXIT EMIT BRANCH \'0xFFFFFFD8 ;
	.long var_STATE_cw              
	.long w_CharAt_cw               
	.long w_0branch_cw              
	.long 0x0000001C                
	.long w_Squote_cw               
	.long w_tick_cw                 
	.long w_TELL_cw                 
	.long w_comma_cw                
	.long w_branch_cw               
	.long 0x00000030                
	.long w_key_cw                  
	.long w_dup_cw                  
	.long w_quoted_quote_cw         
	.long w_equal_cw                
	.long w_0branch_cw              
	.long 0x0000000C                
	.long w_drop_cw                 
	.long w_exit_cw                 
	.long w_emit_cw                 
	.long w_branch_cw               
	.long 0xFFFFFFD8                
	.long w_exit_cw                 

DEFWORD w_CONSTANT,0,"CONSTANT",f_docol  // 000050 	: CONSTANT WORD CREATE DOCOL , ' LIT , C>D , ' EXIT , ;
	.long w_word_cw                 
	.long w_create_cw               
	.long const_DOCOL_cw            
	.long w_comma_cw                
	.long w_tick_cw                 
	.long w_lit_cw                  
	.long w_comma_cw                
	.long w_CtoD_cw                 
	.long w_comma_cw                
	.long w_tick_cw                 
	.long w_exit_cw                 
	.long w_comma_cw                
	.long w_exit_cw                 

DEFWORD w_CONSTANT2,0,"CONSTANT2",f_docol // 000051 	: CONSTANT2 WORD CREATE DOCOL , ' LIT , , ' EXIT , ;
	.long w_word_cw                 
	.long w_create_cw               
	.long const_DOCOL_cw            
	.long w_comma_cw                
	.long w_tick_cw                 
	.long w_lit_cw                  
	.long w_comma_cw                
	.long w_comma_cw                
	.long w_tick_cw                 
	.long w_exit_cw                 
	.long w_comma_cw                
	.long w_exit_cw                 

DEFWORD w_ALLOT,0,"ALLOT",f_docol        // 000052 	: ALLOT HERE D@ SWAP12 HERE +! ;
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_swap_12_cw              
	.long var_HERE_cw               
	.long w_ADDSTORE_cw             
	.long w_exit_cw                 

DEFWORD w_CELLS,0,"CELLS",f_docol        // 000053 	: CELLS LIT \'0x00000002 * ;
	.long w_lit_cw                  
	.long 0x00000002                
	.long w_times_cw                
	.long w_exit_cw                 

DEFWORD w_VARIABLE,0,"VARIABLE",f_docol  // 000054 	: VARIABLE LIT \'0x00000002 CELLS ALLOT WORD CREATE DOCOL , ' LIT2 , , ' EXIT , ;
	.long w_lit_cw                  
	.long 0x00000002                
	.long w_CELLS_cw                
	.long w_ALLOT_cw                
	.long w_word_cw                 
	.long w_create_cw               
	.long const_DOCOL_cw            
	.long w_comma_cw                
	.long w_tick_cw                 
	.long w_lit2_cw                 
	.long w_comma_cw                
	.long w_comma_cw                
	.long w_tick_cw                 
	.long w_exit_cw                 
	.long w_comma_cw                
	.long w_exit_cw                 

DEFWORD w_VALUE,0,"VALUE",f_docol        // 000055 	: VALUE WORD CREATE DOCOL , ' LIT , C>D , ' EXIT , ;
	.long w_word_cw                 
	.long w_create_cw               
	.long const_DOCOL_cw            
	.long w_comma_cw                
	.long w_tick_cw                 
	.long w_lit_cw                  
	.long w_comma_cw                
	.long w_CtoD_cw                 
	.long w_comma_cw                
	.long w_tick_cw                 
	.long w_exit_cw                 
	.long w_comma_cw                
	.long w_exit_cw                 

DEFWORD w_VALUE2,0,"VALUE2",f_docol      // 000056 	: VALUE2 WORD CREATE DOCOL , ' LIT2 , , ' EXIT , ;
	.long w_word_cw                 
	.long w_create_cw               
	.long const_DOCOL_cw            
	.long w_comma_cw                
	.long w_tick_cw                 
	.long w_lit2_cw                 
	.long w_comma_cw                
	.long w_comma_cw                
	.long w_tick_cw                 
	.long w_exit_cw                 
	.long w_comma_cw                
	.long w_exit_cw                 

DEFWORD w_TO,FLG_IMMEDIATE,"TO",f_docol  // 000057 	: TO IMMEDIATE WORD FIND >DFA 4+ STATE C@ 0BRANCH \'0x0000002C ' LIT , C>D , ' D! , BRANCH \'0x00000008 ! ;
	.long w_word_cw                 
	.long w_find_cw                 
	.long w_ToDFA_cw                
	.long w_4plus_cw                
	.long var_STATE_cw              
	.long w_CharAt_cw               
	.long w_0branch_cw              
	.long 0x0000002C                
	.long w_tick_cw                 
	.long w_lit_cw                  
	.long w_comma_cw                
	.long w_CtoD_cw                 
	.long w_comma_cw                
	.long w_tick_cw                 
	.long w_DoubleStore_cw          
	.long w_comma_cw                
	.long w_branch_cw               
	.long 0x00000008                
	.long w_Store_cw                
	.long w_exit_cw                 

DEFWORD w_TO2,FLG_IMMEDIATE,"TO2",f_docol // 000058 	: TO2 IMMEDIATE WORD FIND >DFA 4+ STATE C@ 0BRANCH \'0x00000028 ' LIT2 , , ' D! , BRANCH \'0x00000008 D! ;
	.long w_word_cw                 
	.long w_find_cw                 
	.long w_ToDFA_cw                
	.long w_4plus_cw                
	.long var_STATE_cw              
	.long w_CharAt_cw               
	.long w_0branch_cw              
	.long 0x00000028                
	.long w_tick_cw                 
	.long w_lit2_cw                 
	.long w_comma_cw                
	.long w_comma_cw                
	.long w_tick_cw                 
	.long w_DoubleStore_cw          
	.long w_comma_cw                
	.long w_branch_cw               
	.long 0x00000008                
	.long w_DoubleStore_cw          
	.long w_exit_cw                 

DEFWORD w_plusTO,FLG_IMMEDIATE,"+TO",f_docol // 000059 	: +TO IMMEDIATE WORD FIND >DFA 4+ STATE C@ 0BRANCH \'0x0000002C ' LIT , C>D , ' +! , BRANCH \'0x00000008 +! ;
	.long w_word_cw                 
	.long w_find_cw                 
	.long w_ToDFA_cw                
	.long w_4plus_cw                
	.long var_STATE_cw              
	.long w_CharAt_cw               
	.long w_0branch_cw              
	.long 0x0000002C                
	.long w_tick_cw                 
	.long w_lit_cw                  
	.long w_comma_cw                
	.long w_CtoD_cw                 
	.long w_comma_cw                
	.long w_tick_cw                 
	.long w_ADDSTORE_cw             
	.long w_comma_cw                
	.long w_branch_cw               
	.long 0x00000008                
	.long w_ADDSTORE_cw             
	.long w_exit_cw                 

DEFWORD w_plusTO2,FLG_IMMEDIATE,"+TO2",f_docol // 000060 	: +TO2 IMMEDIATE WORD FIND >DFA 4+ STATE C@ 0BRANCH \'0x00000028 ' LIT , , ' +!D , BRANCH \'0x00000008 +!D ;
	.long w_word_cw                 
	.long w_find_cw                 
	.long w_ToDFA_cw                
	.long w_4plus_cw                
	.long var_STATE_cw              
	.long w_CharAt_cw               
	.long w_0branch_cw              
	.long 0x00000028                
	.long w_tick_cw                 
	.long w_lit_cw                  
	.long w_comma_cw                
	.long w_comma_cw                
	.long w_tick_cw                 
	.long w_ADDSTORED_cw            
	.long w_comma_cw                
	.long w_branch_cw               
	.long 0x00000008                
	.long w_ADDSTORED_cw            
	.long w_exit_cw                 

DEFWORD w_IDdot,0,"ID.",f_docol          // 000061 	: ID. 4D+ 1D+ DUP2 C@ DUP 0> 0BRANCH \'0x00000028 SWAP21 1+ DUP2 C@ EMIT SWAP12 1- BRANCH \'0xFFFFFFD0 DROP DROP2 ;
	.long w_4Dplus_cw               
	.long w_1Dplus_cw               
	.long w_dup_D_cw                
	.long w_CharAt_cw               
	.long w_dup_cw                  
	.long w_positive_cw             
	.long w_0branch_cw              
	.long 0x00000028                
	.long w_swap_21_cw              
	.long w_1plus_cw                
	.long w_dup_D_cw                
	.long w_CharAt_cw               
	.long w_emit_cw                 
	.long w_swap_12_cw              
	.long w_1minus_cw               
	.long w_branch_cw               
	.long 0xFFFFFFD0                
	.long w_drop_cw                 
	.long w_drop_D_cw               
	.long w_exit_cw                 

DEFWORD w_qHIDDEN,0,"?HIDDEN",f_docol    // 000062 	: ?HIDDEN 4+ C@ F_HIDDEN AND ;
	.long w_4plus_cw                
	.long w_CharAt_cw               
	.long const_F_HIDDEN_cw         
	.long w_and_cw                  
	.long w_exit_cw                 

DEFWORD w_qIMMEDIATE,0,"?IMMEDIATE",f_docol // 000063 	: ?IMMEDIATE 4+ C@ F_IMMEDIATE AND ;
	.long w_4plus_cw                
	.long w_CharAt_cw               
	.long const_F_IMMEDIATE_cw      
	.long w_and_cw                  
	.long w_exit_cw                 

DEFWORD w_newWORDS,0,"WORDS",f_docol     // 000064 	: WORDS LAST D@ ?DUP2 NOTNULL 0BRANCH \'0x00000030 DUP2 ?HIDDEN NOT 0BRANCH \'0x00000010 DUP2 ID. SPACE D@ BRANCH \'0xFFFFFFC8 CR ;
	.long var_LAST_cw               
	.long w_DoubleAt_cw             
	.long w_QDUPD_cw                
	.long w_notnull_cw              
	.long w_0branch_cw              
	.long 0x00000030                
	.long w_dup_D_cw                
	.long w_qHIDDEN_cw              
	.long w_NOT_cw                  
	.long w_0branch_cw              
	.long 0x00000010                
	.long w_dup_D_cw                
	.long w_IDdot_cw                
	.long w_SPACE_cw                
	.long w_DoubleAt_cw             
	.long w_branch_cw               
	.long 0xFFFFFFC8                
	.long w_CR_cw                   
	.long w_exit_cw                 

DEFWORD w_newFORGET,0,"FORGET",f_docol   // 000065 	: FORGET WORD FIND DUP2 ISNULL 0BRANCH \'0x0000000C DROP2 RETURN DUP2 D@ LAST D! HERE D! ;
	.long w_word_cw                 
	.long w_find_cw                 
	.long w_dup_D_cw                
	.long w_isnull_cw               
	.long w_0branch_cw              
	.long 0x0000000C                
	.long w_drop_D_cw               
	.long w_return_cw               
	.long w_dup_D_cw                
	.long w_DoubleAt_cw             
	.long var_LAST_cw               
	.long w_DoubleStore_cw          
	.long var_HERE_cw               
	.long w_DoubleStore_cw          
	.long w_exit_cw                 

DEFWORD w_DUMP,0,"DUMP",f_docol          // 000066 	: DUMP BASE C@ -ROT4 HEX ?DUP 0BRANCH \'0x00000138 OVER21 SWAP LIT \'0x00000004 U.R LIT \'0x00000004 U.R SPACE DUP3 1- LIT \'0x0000000F AND 1+ ?DUP 0BRANCH \'0x00000034 SWAP21 DUP2 C@ LIT \'0x00000002 .R SPACE 1+ SWAP12 1- BRANCH \'0xFFFFFFC8 DROP2 DUP3 1- LIT \'0x0000000F AND 1+ ?DUP 0BRANCH \'0x0000005C SWAP21 DUP2 C@ DUP LIT \'0x00000020 LIT \'0x00000080 WITHIN 0BRANCH \'0x00000010 EMIT BRANCH \'0x00000010 DROP '.' EMIT 1+ SWAP12 1- BRANCH \'0xFFFFFFA0 DROP2 CR DUP 1- LIT \'0x0000000F AND 1+ TUCK - >R +21 R> BRANCH \'0xFFFFFEC4 DROP2 BASE C! ;
	.long var_BASE_cw               
	.long w_CharAt_cw               
	.long w_NROT4_cw                
	.long w_newHEX_cw               
	.long w_QDUP_cw                 
	.long w_0branch_cw              
	.long 0x00000138                
	.long w_OVER21_cw               
	.long w_swap_cw                 
	.long w_lit_cw                  
	.long 0x00000004                
	.long w_UdotR_cw                
	.long w_lit_cw                  
	.long 0x00000004                
	.long w_UdotR_cw                
	.long w_SPACE_cw                
	.long w_dup_3_cw                
	.long w_1minus_cw               
	.long w_lit_cw                  
	.long 0x0000000F                
	.long w_and_cw                  
	.long w_1plus_cw                
	.long w_QDUP_cw                 
	.long w_0branch_cw              
	.long 0x00000034                
	.long w_swap_21_cw              
	.long w_dup_D_cw                
	.long w_CharAt_cw               
	.long w_lit_cw                  
	.long 0x00000002                
	.long w_dotR_cw                 
	.long w_SPACE_cw                
	.long w_1plus_cw                
	.long w_swap_12_cw              
	.long w_1minus_cw               
	.long w_branch_cw               
	.long 0xFFFFFFC8                
	.long w_drop_D_cw               
	.long w_dup_3_cw                
	.long w_1minus_cw               
	.long w_lit_cw                  
	.long 0x0000000F                
	.long w_and_cw                  
	.long w_1plus_cw                
	.long w_QDUP_cw                 
	.long w_0branch_cw              
	.long 0x0000005C                
	.long w_swap_21_cw              
	.long w_dup_D_cw                
	.long w_CharAt_cw               
	.long w_dup_cw                  
	.long w_lit_cw                  
	.long 0x00000020                
	.long w_lit_cw                  
	.long 0x00000080                
	.long w_WITHIN_cw               
	.long w_0branch_cw              
	.long 0x00000010                
	.long w_emit_cw                 
	.long w_branch_cw               
	.long 0x00000010                
	.long w_drop_cw                 
	.long w_quoted_dot_cw           
	.long w_emit_cw                 
	.long w_1plus_cw                
	.long w_swap_12_cw              
	.long w_1minus_cw               
	.long w_branch_cw               
	.long 0xFFFFFFA0                
	.long w_drop_D_cw               
	.long w_CR_cw                   
	.long w_dup_cw                  
	.long w_1minus_cw               
	.long w_lit_cw                  
	.long 0x0000000F                
	.long w_and_cw                  
	.long w_1plus_cw                
	.long w_TUCK_cw                 
	.long w_minus_cw                
	.long w_ToR_cw                  
	.long w_plus21_cw               
	.long w_FromR_cw                
	.long w_branch_cw               
	.long 0xFFFFFEC4                
	.long w_drop_D_cw               
	.long var_BASE_cw               
	.long w_CharStore_cw            
	.long w_exit_cw                 

DEFWORD w_CASE,FLG_IMMEDIATE,"CASE",f_docol // 000067 	: CASE IMMEDIATE LIT \'0x00000000 ;
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_exit_cw                 

DEFWORD w_OF,FLG_IMMEDIATE,"OF",f_docol  // 000068 	: OF IMMEDIATE ' OVER , ' = , IF ' DROP , ;
	.long w_tick_cw                 
	.long w_OVER_cw                 
	.long w_comma_cw                
	.long w_tick_cw                 
	.long w_equal_cw                
	.long w_comma_cw                
	.long w_IF_cw                   
	.long w_tick_cw                 
	.long w_drop_cw                 
	.long w_comma_cw                
	.long w_exit_cw                 

DEFWORD w_ENDOF,FLG_IMMEDIATE,"ENDOF",f_docol // 000069 	: ENDOF IMMEDIATE ELSE ;
	.long w_ELSE_cw                 
	.long w_exit_cw                 

DEFWORD w_ENDCASE,FLG_IMMEDIATE,"ENDCASE",f_docol // 000070 	: ENDCASE IMMEDIATE ' DROP , ?DUP 0BRANCH \'0x00000010 THEN BRANCH \'0xFFFFFFEC ;
	.long w_tick_cw                 
	.long w_drop_cw                 
	.long w_comma_cw                
	.long w_QDUP_cw                 
	.long w_0branch_cw              
	.long 0x00000010                
	.long w_THEN_cw                 
	.long w_branch_cw               
	.long 0xFFFFFFEC                
	.long w_exit_cw                 

DEFWORD w_CASE2,FLG_IMMEDIATE,"CASE2",f_docol // 000071 	: CASE2 IMMEDIATE LIT \'0x00000000 LIT \'0x00000000 ;
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_exit_cw                 

DEFWORD w_OF2,FLG_IMMEDIATE,"OF2",f_docol // 000072 	: OF2 IMMEDIATE ' OVER2 , ' =D , IF ' DROP2 , ;
	.long w_tick_cw                 
	.long w_OVER2_cw                
	.long w_comma_cw                
	.long w_tick_cw                 
	.long w_equalD_cw               
	.long w_comma_cw                
	.long w_IF_cw                   
	.long w_tick_cw                 
	.long w_drop_D_cw               
	.long w_comma_cw                
	.long w_exit_cw                 

DEFWORD w_ENDCASE2,FLG_IMMEDIATE,"ENDCASE2",f_docol // 000073 	: ENDCASE2 IMMEDIATE ' DROP2 , ?DUP2 NOTNULL 0BRANCH \'0x00000010 THEN BRANCH \'0xFFFFFFE8 ;
	.long w_tick_cw                 
	.long w_drop_D_cw               
	.long w_comma_cw                
	.long w_QDUPD_cw                
	.long w_notnull_cw              
	.long w_0branch_cw              
	.long 0x00000010                
	.long w_THEN_cw                 
	.long w_branch_cw               
	.long 0xFFFFFFE8                
	.long w_exit_cw                 

DEFWORD w_CFAto,0,"CFA>",f_docol         // 000074 	: CFA> LAST D@ ?DUP2 NOTNULL 0BRANCH \'0x0000002C DUP4 SWAP2 <D 0BRANCH \'0x0000000C NIP2 EXIT D@ BRANCH \'0xFFFFFFCC DROP2 LIT \'0x00000000 LIT \'0x00000000 ;
	.long var_LAST_cw               
	.long w_DoubleAt_cw             
	.long w_QDUPD_cw                
	.long w_notnull_cw              
	.long w_0branch_cw              
	.long 0x0000002C                
	.long w_dup_4_cw                
	.long w_swap_D_cw               
	.long w_lessD_cw                
	.long w_0branch_cw              
	.long 0x0000000C                
	.long w_NIP2_cw                 
	.long w_exit_cw                 
	.long w_DoubleAt_cw             
	.long w_branch_cw               
	.long 0xFFFFFFCC                
	.long w_drop_D_cw               
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_exit_cw                 

DEFWORD w_SEE2,0,"SEE",f_docol           // 000075 	: SEE WORD FIND HERE D@ LAST D@ LIT \'0x00000005 PICK LIT \'0x00000005 PICK OVER2 <>D 0BRANCH \'0x00000018 NIP2 DUP2 D@ BRANCH \'0xFFFFFFC8 DROP2 SWAP2 ':' EMIT SPACE DUP2 ID. SPACE DUP2 ?IMMEDIATE 0BRANCH \'0x0000001A LITSTRING \'0x0000000A \"IMMEDIATE " TELL >DFA OVER2 OVER2 >D 0BRANCH \'0x0000031C DUP2 D@ ' LIT OVER2 =D 0BRANCH \'0x00000034 DROP2 LIT \'0x00000000 LIT \'0x00000004 +D DUP2 @ . SPACE BRANCH \'0x000002B0 ' LIT2 OVER2 =D 0BRANCH \'0x00000040 DROP2 LIT \'0x00000000 LIT \'0x00000004 +D DUP2 D@ SWAP . SPACE . SPACE BRANCH \'0x0000025C ' LITSTRING OVER2 =D 0BRANCH \'0x00000090 DROP2 LIT \'0x00000053 EMIT '"' EMIT SPACE LIT \'0x00000000 LIT \'0x00000004 +D DUP2 C@ SWAP21 LIT \'0x00000000 LIT \'0x00000004 +D SWAP12 OVER21 OVER12 TELL '"' EMIT SPACE +21 LIT \'0x00000000 LIT \'0x00000004 -D BRANCH \'0x000001B8 ' 0BRANCH OVER2 =D 0BRANCH \'0x00000060 DROP2 LITSTRING \'0x0000000A \"0BRANCH ( " TELL LIT \'0x00000000 LIT \'0x00000004 +D DUP2 D@ SWAP . SPACE . LITSTRING \'0x00000002 \") " TELL BRANCH \'0x00000144 ' BRANCH OVER2 =D 0BRANCH \'0x0000005F DROP2 LITSTRING \'0x00000009 \"BRANCH ( " TELL LIT \'0x00000000 LIT \'0x00000004 +D DUP2 D@ SWAP . SPACE . LITSTRING \'0x00000002 \") " TELL BRANCH \'0x000000D1 ' ' OVER2 =D 0BRANCH \'0x00000048 DROP2 LIT \'0x00000027 EMIT SPACE LIT \'0x00000000 LIT \'0x00000004 +D DUP2 D@ CFA> ID. SPACE BRANCH \'0x00000075 ' EXIT OVER2 =D 0BRANCH \'0x00000049 DROP2 OVER2 OVER2 LIT \'0x00000000 LIT \'0x00000004 +D <>D 0BRANCH \'0x00000015 LITSTRING \'0x00000005 \"EXIT " TELL BRANCH \'0x00000018 DUP2 CFA> ID. SPACE DROP2 LIT \'0x00000000 LIT \'0x00000004 +D BRANCH \'0xFFFFFCD8 ';' EMIT CR DROP2 DROP2 ;
	.long w_word_cw                 
	.long w_find_cw                 
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long var_LAST_cw               
	.long w_DoubleAt_cw             
	.long w_lit_cw                  
	.long 0x00000005                
	.long w_PICK_cw                 
	.long w_lit_cw                  
	.long 0x00000005                
	.long w_PICK_cw                 
	.long w_OVER2_cw                
	.long w_notequalD_cw            
	.long w_0branch_cw              
	.long 0x00000018                
	.long w_NIP2_cw                 
	.long w_dup_D_cw                
	.long w_DoubleAt_cw             
	.long w_branch_cw               
	.long 0xFFFFFFC8                
	.long w_drop_D_cw               
	.long w_swap_D_cw               
	.long w_quoted_colon_cw         
	.long w_emit_cw                 
	.long w_SPACE_cw                
	.long w_dup_D_cw                
	.long w_IDdot_cw                
	.long w_SPACE_cw                
	.long w_dup_D_cw                
	.long w_qIMMEDIATE_cw           
	.long w_0branch_cw              
	.long 0x0000001A                
	.long w_LITSTRING_cw            
	.long 0x0000000A                
	.ascii "IMMEDIATE "             
	.long w_TELL_cw                 
	.long w_ToDFA_cw                
	.long w_OVER2_cw                
	.long w_OVER2_cw                
	.long w_greaterD_cw             
	.long w_0branch_cw              
	.long 0x0000031C                
	.long w_dup_D_cw                
	.long w_DoubleAt_cw             
	.long w_tick_cw                 
	.long w_lit_cw                  
	.long w_OVER2_cw                
	.long w_equalD_cw               
	.long w_0branch_cw              
	.long 0x00000034                
	.long w_drop_D_cw               
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_lit_cw                  
	.long 0x00000004                
	.long w_plus_D_cw               
	.long w_dup_D_cw                
	.long w_At_cw                   
	.long w_dot_cw                  
	.long w_SPACE_cw                
	.long w_branch_cw               
	.long 0x000002B0                
	.long w_tick_cw                 
	.long w_lit2_cw                 
	.long w_OVER2_cw                
	.long w_equalD_cw               
	.long w_0branch_cw              
	.long 0x00000040                
	.long w_drop_D_cw               
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_lit_cw                  
	.long 0x00000004                
	.long w_plus_D_cw               
	.long w_dup_D_cw                
	.long w_DoubleAt_cw             
	.long w_swap_cw                 
	.long w_dot_cw                  
	.long w_SPACE_cw                
	.long w_dot_cw                  
	.long w_SPACE_cw                
	.long w_branch_cw               
	.long 0x0000025C                
	.long w_tick_cw                 
	.long w_LITSTRING_cw            
	.long w_OVER2_cw                
	.long w_equalD_cw               
	.long w_0branch_cw              
	.long 0x00000090                
	.long w_drop_D_cw               
	.long w_lit_cw                  
	.long 0x00000053                
	.long w_emit_cw                 
	.long w_quoted_quote_cw         
	.long w_emit_cw                 
	.long w_SPACE_cw                
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_lit_cw                  
	.long 0x00000004                
	.long w_plus_D_cw               
	.long w_dup_D_cw                
	.long w_CharAt_cw               
	.long w_swap_21_cw              
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_lit_cw                  
	.long 0x00000004                
	.long w_plus_D_cw               
	.long w_swap_12_cw              
	.long w_OVER21_cw               
	.long w_OVER12_cw               
	.long w_TELL_cw                 
	.long w_quoted_quote_cw         
	.long w_emit_cw                 
	.long w_SPACE_cw                
	.long w_plus21_cw               
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_lit_cw                  
	.long 0x00000004                
	.long w_minus_D_cw              
	.long w_branch_cw               
	.long 0x000001B8                
	.long w_tick_cw                 
	.long w_0branch_cw              
	.long w_OVER2_cw                
	.long w_equalD_cw               
	.long w_0branch_cw              
	.long 0x00000060                
	.long w_drop_D_cw               
	.long w_LITSTRING_cw            
	.long 0x0000000A                
	.ascii "0BRANCH ( "             
	.long w_TELL_cw                 
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_lit_cw                  
	.long 0x00000004                
	.long w_plus_D_cw               
	.long w_dup_D_cw                
	.long w_DoubleAt_cw             
	.long w_swap_cw                 
	.long w_dot_cw                  
	.long w_SPACE_cw                
	.long w_dot_cw                  
	.long w_LITSTRING_cw            
	.long 0x00000002                
	.ascii ") "                     
	.long w_TELL_cw                 
	.long w_branch_cw               
	.long 0x00000144                
	.long w_tick_cw                 
	.long w_branch_cw               
	.long w_OVER2_cw                
	.long w_equalD_cw               
	.long w_0branch_cw              
	.long 0x0000005F                
	.long w_drop_D_cw               
	.long w_LITSTRING_cw            
	.long 0x00000009                
	.ascii "BRANCH ( "              
	.long w_TELL_cw                 
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_lit_cw                  
	.long 0x00000004                
	.long w_plus_D_cw               
	.long w_dup_D_cw                
	.long w_DoubleAt_cw             
	.long w_swap_cw                 
	.long w_dot_cw                  
	.long w_SPACE_cw                
	.long w_dot_cw                  
	.long w_LITSTRING_cw            
	.long 0x00000002                
	.ascii ") "                     
	.long w_TELL_cw                 
	.long w_branch_cw               
	.long 0x000000D1                
	.long w_tick_cw                 
	.long w_tick_cw                 
	.long w_OVER2_cw                
	.long w_equalD_cw               
	.long w_0branch_cw              
	.long 0x00000048                
	.long w_drop_D_cw               
	.long w_lit_cw                  
	.long 0x00000027                
	.long w_emit_cw                 
	.long w_SPACE_cw                
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_lit_cw                  
	.long 0x00000004                
	.long w_plus_D_cw               
	.long w_dup_D_cw                
	.long w_DoubleAt_cw             
	.long w_CFAto_cw                
	.long w_IDdot_cw                
	.long w_SPACE_cw                
	.long w_branch_cw               
	.long 0x00000075                
	.long w_tick_cw                 
	.long w_exit_cw                 
	.long w_OVER2_cw                
	.long w_equalD_cw               
	.long w_0branch_cw              
	.long 0x00000049                
	.long w_drop_D_cw               
	.long w_OVER2_cw                
	.long w_OVER2_cw                
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_lit_cw                  
	.long 0x00000004                
	.long w_plus_D_cw               
	.long w_notequalD_cw            
	.long w_0branch_cw              
	.long 0x00000015                
	.long w_LITSTRING_cw            
	.long 0x00000005                
	.ascii "EXIT "                  
	.long w_TELL_cw                 
	.long w_branch_cw               
	.long 0x00000018                
	.long w_dup_D_cw                
	.long w_CFAto_cw                
	.long w_IDdot_cw                
	.long w_SPACE_cw                
	.long w_drop_D_cw               
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_lit_cw                  
	.long 0x00000004                
	.long w_plus_D_cw               
	.long w_branch_cw               
	.long 0xFFFFFCD8                
	.long w_quoted_semicolon_cw     
	.long w_emit_cw                 
	.long w_CR_cw                   
	.long w_drop_D_cw               
	.long w_drop_D_cw               
	.long w_exit_cw                 

DEFWORD w_colonNONAME,0,":NONAME",f_docol // 000076 	: :NONAME LIT \'0x00000000 LIT \'0x00000000 CREATE HERE D@ DOCOL , ] ;
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_create_cw               
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long const_DOCOL_cw            
	.long w_comma_cw                
	.long w_right_brac_cw           
	.long w_exit_cw                 

DEFWORD w_bracketedapostrophe,FLG_IMMEDIATE,"[']",f_docol // 000077 	: ['] IMMEDIATE ' LIT , ;
	.long w_tick_cw                 
	.long w_lit_cw                  
	.long w_comma_cw                
	.long w_exit_cw                 

DEFWORD w_EXCEPTION_MARKER,0,"EXCEPTION-MARKER",f_docol // 000078 	: EXCEPTION-MARKER RDROP LIT \'0x00000000 ;
	.long w_rdrop_cw                
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_exit_cw                 

DEFWORD w_CATCH,0,"CATCH",f_docol        // 000079 	: CATCH S? 4+ >R ' EXCEPTION-MARKER 4+ >R EXECUTE ;
	.long w_StackAddress_cw         
	.long w_4plus_cw                
	.long w_ToR_cw                  
	.long w_tick_cw                 
	.long w_EXCEPTION_MARKER_cw     
	.long w_4plus_cw                
	.long w_ToR_cw                  
	.long w_EXECUTE_cw              
	.long w_exit_cw                 

DEFWORD w_THROW,0,"THROW",f_docol        // 000080 	: THROW ?DUP 0BRANCH \'0x000000EE R? DUP R0 4- < 0BRANCH \'0x00000060 DUP @ ' EXCEPTION-MARKER 4+ = 0BRANCH \'0x00000034 4+ R! DUP DUP DUP R> 4- SWAP OVER ! S! EXIT 4+ BRANCH \'0xFFFFFF90 DROP LIT \'0x00000000 1- OVER = 0BRANCH \'0x00000027 DROP LITSTRING \'0x00000007 \"ABORTED" TELL CR BRANCH \'0x0000002F LITSTRING \'0x0000000F \"UNCAUGHT THROW " TELL DUP . CR DROP QUIT ;
	.long w_QDUP_cw                 
	.long w_0branch_cw              
	.long 0x000000EE                
	.long w_RStackAddress_cw        
	.long w_dup_cw                  
	.long const_R0_cw               
	.long w_4minus_cw               
	.long w_less_cw                 
	.long w_0branch_cw              
	.long 0x00000060                
	.long w_dup_cw                  
	.long w_At_cw                   
	.long w_tick_cw                 
	.long w_EXCEPTION_MARKER_cw     
	.long w_4plus_cw                
	.long w_equal_cw                
	.long w_0branch_cw              
	.long 0x00000034                
	.long w_4plus_cw                
	.long w_SetRStack_cw            
	.long w_dup_cw                  
	.long w_dup_cw                  
	.long w_dup_cw                  
	.long w_FromR_cw                
	.long w_4minus_cw               
	.long w_swap_cw                 
	.long w_OVER_cw                 
	.long w_Store_cw                
	.long w_SetStack_cw             
	.long w_exit_cw                 
	.long w_4plus_cw                
	.long w_branch_cw               
	.long 0xFFFFFF90                
	.long w_drop_cw                 
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_1minus_cw               
	.long w_OVER_cw                 
	.long w_equal_cw                
	.long w_0branch_cw              
	.long 0x00000027                
	.long w_drop_cw                 
	.long w_LITSTRING_cw            
	.long 0x00000007                
	.ascii "ABORTED"                
	.long w_TELL_cw                 
	.long w_CR_cw                   
	.long w_branch_cw               
	.long 0x0000002F                
	.long w_LITSTRING_cw            
	.long 0x0000000F                
	.ascii "UNCAUGHT THROW "        
	.long w_TELL_cw                 
	.long w_dup_cw                  
	.long w_dot_cw                  
	.long w_CR_cw                   
	.long w_drop_cw                 
	.long w_quit_cw                 
	.long w_exit_cw                 

DEFWORD w_ABORT,0,"ABORT",f_docol        // 000081 	: ABORT LIT \'0x00000000 1- THROW ;
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_1minus_cw               
	.long w_THROW_cw                
	.long w_exit_cw                 

DEFWORD w_PRINT_STACK_TRACE,0,"PRINT-STACK-TRACE",f_docol // 000082 	: PRINT-STACK-TRACE R? DUP R0 4- < 0BRANCH \'0x000000B6 DUP @ ' EXCEPTION-MARKER 4+ OVER = 0BRANCH \'0x00000046 DROP LITSTRING \'0x0000000C \"CATCH ( DSP=" TELL 4+ DUP @ U. LITSTRING \'0x00000002 \") " TELL BRANCH \'0x00000044 DUP CFA> ?DUP 0BRANCH \'0x0000002C DUP2 ID. LIT \'0x0000002B EMIT SWAP >DFA 4+ - . DROP 4+ BRANCH \'0xFFFFFF3A DROP CR ;
	.long w_RStackAddress_cw        
	.long w_dup_cw                  
	.long const_R0_cw               
	.long w_4minus_cw               
	.long w_less_cw                 
	.long w_0branch_cw              
	.long 0x000000B6                
	.long w_dup_cw                  
	.long w_At_cw                   
	.long w_tick_cw                 
	.long w_EXCEPTION_MARKER_cw     
	.long w_4plus_cw                
	.long w_OVER_cw                 
	.long w_equal_cw                
	.long w_0branch_cw              
	.long 0x00000046                
	.long w_drop_cw                 
	.long w_LITSTRING_cw            
	.long 0x0000000C                
	.ascii "CATCH ( DSP="           
	.long w_TELL_cw                 
	.long w_4plus_cw                
	.long w_dup_cw                  
	.long w_At_cw                   
	.long w_Udot_cw                 
	.long w_LITSTRING_cw            
	.long 0x00000002                
	.ascii ") "                     
	.long w_TELL_cw                 
	.long w_branch_cw               
	.long 0x00000044                
	.long w_dup_cw                  
	.long w_CFAto_cw                
	.long w_QDUP_cw                 
	.long w_0branch_cw              
	.long 0x0000002C                
	.long w_dup_D_cw                
	.long w_IDdot_cw                
	.long w_lit_cw                  
	.long 0x0000002B                
	.long w_emit_cw                 
	.long w_swap_cw                 
	.long w_ToDFA_cw                
	.long w_4plus_cw                
	.long w_minus_cw                
	.long w_dot_cw                  
	.long w_drop_cw                 
	.long w_4plus_cw                
	.long w_branch_cw               
	.long 0xFFFFFF3A                
	.long w_drop_cw                 
	.long w_CR_cw                   
	.long w_exit_cw                 

DEFWORD w_Zquote,FLG_IMMEDIATE,"Z\"",f_docol // 000083 	: Z" IMMEDIATE STATE C@ 0BRANCH \'0x000000BC ' LITSTRING , HERE D@ LIT \'0x00000000 , KEY DUP '"' <> 0BRANCH \'0x00000028 HERE D@ C! LIT \'0x00000001 HERE +! BRANCH \'0xFFFFFFC8 LIT \'0x00000000 HERE D@ C! LIT \'0x00000001 HERE +! DROP DUP HERE D@ SWAP - 4- SWAP ! ' DROP , BRANCH \'0x00000054 HERE D@ KEY DUP '"' <> 0BRANCH \'0x00000018 OVER C! 1+ BRANCH \'0xFFFFFFD8 DROP LIT \'0x00000000 SWAP C! HERE D@ ;
	.long var_STATE_cw              
	.long w_CharAt_cw               
	.long w_0branch_cw              
	.long 0x000000BC                
	.long w_tick_cw                 
	.long w_LITSTRING_cw            
	.long w_comma_cw                
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_comma_cw                
	.long w_key_cw                  
	.long w_dup_cw                  
	.long w_quoted_quote_cw         
	.long w_notequal_cw             
	.long w_0branch_cw              
	.long 0x00000028                
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_CharStore_cw            
	.long w_lit_cw                  
	.long 0x00000001                
	.long var_HERE_cw               
	.long w_ADDSTORE_cw             
	.long w_branch_cw               
	.long 0xFFFFFFC8                
	.long w_lit_cw                  
	.long 0x00000000                
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_CharStore_cw            
	.long w_lit_cw                  
	.long 0x00000001                
	.long var_HERE_cw               
	.long w_ADDSTORE_cw             
	.long w_drop_cw                 
	.long w_dup_cw                  
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_swap_cw                 
	.long w_minus_cw                
	.long w_4minus_cw               
	.long w_swap_cw                 
	.long w_Store_cw                
	.long w_tick_cw                 
	.long w_drop_cw                 
	.long w_comma_cw                
	.long w_branch_cw               
	.long 0x00000054                
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_key_cw                  
	.long w_dup_cw                  
	.long w_quoted_quote_cw         
	.long w_notequal_cw             
	.long w_0branch_cw              
	.long 0x00000018                
	.long w_OVER_cw                 
	.long w_CharStore_cw            
	.long w_1plus_cw                
	.long w_branch_cw               
	.long 0xFFFFFFD8                
	.long w_drop_cw                 
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_swap_cw                 
	.long w_CharStore_cw            
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_exit_cw                 

DEFWORD w_STRLEN,0,"STRLEN",f_docol      // 000084 	: STRLEN DUP DUP C@ 0<> 0BRANCH \'0x00000010 1+ BRANCH \'0xFFFFFFE4 SWAP - ;
	.long w_dup_cw                  
	.long w_dup_cw                  
	.long w_CharAt_cw               
	.long w_notzero_cw              
	.long w_0branch_cw              
	.long 0x00000010                
	.long w_1plus_cw                
	.long w_branch_cw               
	.long 0xFFFFFFE4                
	.long w_swap_cw                 
	.long w_minus_cw                
	.long w_exit_cw                 

DEFWORD w_CSTRING,0,"CSTRING",f_docol    // 000085 	: CSTRING SWAP OVER HERE D@ SWAP CMOVE HERE D@ + LIT \'0x00000000 SWAP C! HERE D@ ;
	.long w_swap_cw                 
	.long w_OVER_cw                 
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_swap_cw                 
	.long w_CMOVE_cw                
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_plus_cw                 
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_swap_cw                 
	.long w_CharStore_cw            
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_exit_cw                 
