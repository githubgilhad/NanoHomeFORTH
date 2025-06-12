;/* vim: set filetype=asm noexpandtab fileencoding=utf-8 nomodified nowrap textwidth=270 foldmethod=marker foldmarker={{{,}}} foldcolumn=4 ruler showcmd lcs=tab\:|- list: */


DEFWORD w_doubletick,0,"''",f_docol      // 000001 	: '' WORD FIND h2cw ;
	.long w_word_cw                 
	.long w_find_cw                 
	.long w_h2cw_cw                 
	.long w_exit_cw                 

DEFWORD w_HIDE,0,"HIDE",f_docol          // 000002 	: HIDE WORD FIND HIDDEN ;
	.long w_word_cw                 
	.long w_find_cw                 
	.long w_hidden_cw               
	.long w_exit_cw                 

DEFWORD w_IF,FLG_IMMEDIATE,"IF",f_docol  // 000003 	: IF IMMEDIATE ' 0BRANCH , HERE D@ LIT \'0x00000000 LIT \'0x00000000 , ;
	.long w_tick_cw                 
	.long w_0branch_cw              
	.long w_comma_cw                
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_comma_cw                
	.long w_exit_cw                 

DEFWORD w_FI,FLG_IMMEDIATE,"FI",f_docol  // 000004 	: FI IMMEDIATE DUP2 HERE D@ SWAP2 -D SWAP2 !D ;
	.long w_dup_D_cw                
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_swap_D_cw               
	.long w_minus_D_cw              
	.long w_swap_D_cw               
	.long w_StoreDouble_cw          
	.long w_exit_cw                 

DEFWORD w_THEN,FLG_IMMEDIATE,"THEN",f_docol // 000005 	: THEN IMMEDIATE FI ;
	.long w_FI_cw                   
	.long w_exit_cw                 

DEFWORD w_ELSE,FLG_IMMEDIATE,"ELSE",f_docol // 000006 	: ELSE IMMEDIATE ' BRANCH , HERE D@ LIT \'0x00000000 LIT \'0x00000000 , SWAP2 FI ;
	.long w_tick_cw                 
	.long w_branch_cw               
	.long w_comma_cw                
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_comma_cw                
	.long w_swap_D_cw               
	.long w_FI_cw                   
	.long w_exit_cw                 

DEFWORD w_BEGIN,FLG_IMMEDIATE,"BEGIN",f_docol // 000007 	: BEGIN IMMEDIATE HERE D@ ;
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_exit_cw                 

DEFWORD w_UNTIL,FLG_IMMEDIATE,"UNTIL",f_docol // 000008 	: UNTIL IMMEDIATE ' 0BRANCH , HERE D@ -D , ;
	.long w_tick_cw                 
	.long w_0branch_cw              
	.long w_comma_cw                
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_minus_D_cw              
	.long w_comma_cw                
	.long w_exit_cw                 

DEFWORD w_WHILE,FLG_IMMEDIATE,"WHILE",f_docol // 000009 	: WHILE IMMEDIATE IF ;
	.long w_IF_cw                   
	.long w_exit_cw                 

DEFWORD w_REPEAT,FLG_IMMEDIATE,"REPEAT",f_docol // 000010 	: REPEAT IMMEDIATE ' BRANCH , SWAP2 HERE D@ -D , DUP2 HERE D@ SWAP2 -D SWAP2 !D ;
	.long w_tick_cw                 
	.long w_branch_cw               
	.long w_comma_cw                
	.long w_swap_D_cw               
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_minus_D_cw              
	.long w_comma_cw                
	.long w_dup_D_cw                
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_swap_D_cw               
	.long w_minus_D_cw              
	.long w_swap_D_cw               
	.long w_StoreDouble_cw          
	.long w_exit_cw                 

DEFWORD w_FORGET,0,"FORGET",f_docol      // 000011 	: FORGET WORD FIND DUP2 ISNULL 0BRANCH \'0x0000000C DROP2 RETURN DUP2 D@ LAST !D HERE !D ;
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
	.long w_StoreDouble_cw          
	.long var_HERE_cw               
	.long w_StoreDouble_cw          
	.long w_exit_cw                 

DEFWORD w_ascii,0,"ascii",f_docol        // 000012 	: ascii LIT \'0x00000000 DUP EMIT LIT \'0x00000001 + DUP LIT \'0x00000100 - 0= 0BRANCH \'0xFFFFFFD4 DROP ;
	.long w_lit_cw                  
	.long 0x00000000                
	.long w_dup_cw                  
	.long w_emit_cw                 
	.long w_lit_cw                  
	.long 0x00000001                
	.long w_plus_cw                 
	.long w_dup_cw                  
	.long w_lit_cw                  
	.long 0x00000100                
	.long w_minus_cw                
	.long w_zero_cw                 
	.long w_0branch_cw              
	.long 0xFFFFFFD4                
	.long w_drop_cw                 
	.long w_exit_cw                 
