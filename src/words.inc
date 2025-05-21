;/* vim: set filetype=asm noexpandtab fileencoding=utf-8 nomodified nowrap textwidth=270 foldmethod=marker foldmarker={{{,}}} foldcolumn=4 ruler showcmd lcs=tab\:|- list: */


DEFWORD w_quit,0,"QUIT",f_docol          // 000000 	: QUIT						\ DEFWORD w_quit,0,"QUIT",f_docol ( "quit" )
	.long w_interpret_cw             // 000001 		INTERPRET				\	.long w_interpret_cw
	.long w_branch_cw                // 000002 		BRANCH					\	.long w_branch_cw
	.long -2                         // 000003 		\'-2					\ .long -2
	.long w_exit_cw                  // 000004 		;

DEFWORD w_HIDE,0,"HIDE",f_docol          // 000006 	: HIDE						\ DEFWORD w_hide, 0, "HIDE", f_docol		; HIDE WORD hide/unhide the word
	.long w_word_cw                  // 000007 		WORD FIND HIDDEN ;			\	.long w_word_cw,w_find_cw,w_hidden_cw,w_exit_cw
	.long w_find_cw                 
	.long w_hidden_cw               
	.long w_exit_cw                 

DEFWORD w_colon,0,":",f_docol            // 000009 	: : ( "colon" )					\ DEFWORD w_colon,0,":",f_docol
	.long w_word_cw                  // 000010 		WORD CREATE				\	.long w_word_cw, w_create_cw			; create header
	.long w_create_cw               
	.long w_lit2_cw                  // 000011 		LIT2 \'f_docol ,			\	.long w_lit2_cw, f_docol, w_comma_cw		; fill CW
	.long f_docol                   
	.long w_comma_cw                
	.long var_LAST_cw                // 000012 		LAST D@ HIDDEN				\	.long var_LAST_cw, w_DoubleAt_cw, w_hidden_cw	; hide until done
	.long w_DoubleAt_cw             
	.long w_hidden_cw               
	.long w_right_brac_cw            // 000013 		] ;					\	.long w_right_brac_cw, w_exit_cw		; go to conpile
	.long w_exit_cw                 

DEFWORD w_semicol,FLG_IMMEDIATE,";",f_docol // 000015 	: ; ( "semicol" ) IMMEDIATE			\ DEFWORD w_semicol,FLG_IMMEDIATE,";",f_docol
	.long w_lit2_cw                  // 000016 		LIT2 \'w_exit_cw ,			\	.long w_lit2_cw, w_exit_cw, w_comma_cw		; end word
	.long w_exit_cw                 
	.long w_comma_cw                
	.long var_LAST_cw                // 000017 		LAST D@ HIDDEN				\	.long var_LAST_cw, w_DoubleAt_cw, w_hidden_cw	; unhide it
	.long w_DoubleAt_cw             
	.long w_hidden_cw               
	.long w_left_brac_cw             // 000018 		[ ;					\	.long w_left_brac_cw, w_exit_cw			; back to immediate
	.long w_exit_cw                 

DEFWORD w_FORGET,0,"FORGET",f_docol      // 000020 	: FORGET					\ DEFWORD w_forget,0,"FORGET",f_docol			; forget word (and all after)
	.long w_word_cw                  // 000021 		WORD FIND				\	.long w_word_cw, w_find_cw
	.long w_find_cw                 
	.long w_dup_D_cw                 // 000022 		DUP2 D@ LAST !D				\	.long w_dup_D_cw, w_DoubleAt_cw, var_LAST_cw, w_StoreDouble_cw
	.long w_DoubleAt_cw             
	.long var_LAST_cw               
	.long w_StoreDouble_cw          
	.long var_HERE_cw                // 000023 		HERE D@ !D ;				\	.long var_HERE_cw, w_DoubleAt_cw, w_StoreDouble_cw, w_exit_cw
	.long w_DoubleAt_cw             
	.long w_StoreDouble_cw          
	.long w_exit_cw                 

DEFWORD w_IF,FLG_IMMEDIATE,"IF",f_docol  // 000025 	: IF IMMEDIATE					\ DEFWORD w_if, FLG_IMMEDIATE, "IF", f_docol		; do if not zero
	.long w_tick_cw                  // 000026 		' 0BRANCH ,				\	.long w_tick_cw, w_0branch_cw,w_comma_cw	; if zero, branch
	.long w_0branch_cw              
	.long w_comma_cw                
	.long var_HERE_cw                // 000027 		HERE D@ 0 ,				\	.long var_HERE_cw, w_DoubleAt_cw, w_lit2_cw,0,w_comma_cw	; save this pos, fill 0 for now
	.long w_DoubleAt_cw             
	.long w_lit2_cw, 0              
	.long w_comma_cw                
	.long w_exit_cw                  // 000028 		;					\	.long w_exit_cw

DEFWORD w_FI,FLG_IMMEDIATE,"FI",f_docol  // 000030 	: FI IMMEDIATE					\ DEFWORD w_fi, FLG_IMMEDIATE, "FI", f_docol		; fi - end of IF
	.long w_dup_D_cw                 // 000031 		DUP2 HERE D@ SWAP2 -D /4D SWAP2 !D ;	\	.long w_dup_D_cw, var_HERE_cw, w_DoubleAt_cw, w_swap_D_cw, w_minus_D_cw, w_div4_D_cw, w_swap_D_cw,w_StoreDouble_cw,w_exit_cw
	.long var_HERE_cw               
	.long w_DoubleAt_cw             
	.long w_swap_D_cw               
	.long w_minus_D_cw              
	.long w_div4_D_cw               
	.long w_swap_D_cw               
	.long w_StoreDouble_cw          
	.long w_exit_cw                 

DEFWORD w_THEN,FLG_IMMEDIATE,"THEN",f_docol // 000033 	: THEN IMMEDIATE				\ DEFWORD w_then, FLG_IMMEDIATE, "THEN", f_docol		; fi - end of IF
	.long w_FI_cw                    // 000034 		FI ;					\	.long w_fi_cw, w_exit_cw
	.long w_exit_cw                 

DEFWORD w_ELSE,FLG_IMMEDIATE,"ELSE",f_docol // 000036 	: ELSE IMMEDIATE				\ DEFWORD w_else, FLG_IMMEDIATE, "ELSE", f_docol		; else do this
	.long w_tick_cw                  // 000037 		' BRANCH ,				\	.long w_tick_cw, w_branch_cw,w_comma_cw		; like in IF, but branch everytime
	.long w_branch_cw               
	.long w_comma_cw                
	.long var_HERE_cw                // 000038 		HERE D@ 0 ,				\	.long var_HERE_cw, w_DoubleAt_cw, w_lit2_cw, 0,w_comma_cw	; save this pos, fill 0 for now
	.long w_DoubleAt_cw             
	.long w_lit2_cw, 0              
	.long w_comma_cw                
	.long w_swap_D_cw                // 000039 		SWAP2 FI ;				\	.long w_swap_D_cw, w_fi_cw, w_exit_cw		; swap this and IFs address and fill it like FI
	.long w_FI_cw                   
	.long w_exit_cw                 

DEFWORD w_BEGIN,FLG_IMMEDIATE,"BEGIN",f_docol // 000041 	: BEGIN IMMEDIATE				\	DEFWORD w_begin, FLG_IMMEDIATE, "BEGIN", f_docol	; BEGIN loop-part condition UNTIL
	.long var_HERE_cw                // 000042 		HERE D@ ;				\	.long var_HERE_cw, w_DoubleAt_cw, w_exit_cw
	.long w_DoubleAt_cw             
	.long w_exit_cw                 

DEFWORD w_UNTIL,FLG_IMMEDIATE,"UNTIL",f_docol // 000044 	: UNTIL IMMEDIATE 				\ DEFWORD w_until, FLG_IMMEDIATE, "UNTIL", f_docol	; BEGIN loop-part condition UNTIL
	.long w_tick_cw                  // 000045 		' 0BRANCH ,				\	.long w_tick_cw, w_0branch_cw,w_comma_cw
	.long w_0branch_cw              
	.long w_comma_cw                
	.long var_HERE_cw                // 000046 		HERE D@ -D /4D ,			\	.long var_HERE_cw, w_DoubleAt_cw, w_minus_D_cw, w_div4_D_cw, w_comma_cw
	.long w_DoubleAt_cw             
	.long w_minus_D_cw              
	.long w_div4_D_cw               
	.long w_comma_cw                
	.long w_exit_cw                  // 000047 		;					\	.long w_exit_cw

DEFWORD w_WHILE,FLG_IMMEDIATE,"WHILE",f_docol // 000049 	: WHILE IMMEDIATE				\ DEFWORD w_while, FLG_IMMEDIATE, "WHILE", f_docol	; BEGIN condition WHILE loop-part REPEAT
	.long w_IF_cw                    // 000050 		IF ;					\	.long w_if_cw,w_exit_cw
	.long w_exit_cw                 

DEFWORD w_REPEAT,FLG_IMMEDIATE,"REPEAT",f_docol // 000052 	: REPEAT IMMEDIATE 				\ BEGIN condition WHILE loop-part REPEAT
	.long w_tick_cw                  // 000053 		' BRANCH ,				\ compile BRANCH
	.long w_branch_cw               
	.long w_comma_cw                
	.long w_swap_D_cw                // 000054 		SWAP2					\ get the original offset (from BEGIN)
	.long var_HERE_cw                // 000055 		HERE D@ -D /4D ,			\ and compile it after BRANCH
	.long w_DoubleAt_cw             
	.long w_minus_D_cw              
	.long w_div4_D_cw               
	.long w_comma_cw                
	.long w_swap_D_cw                // 000056 		SWAP2
	.long var_HERE_cw                // 000057 		HERE D@ SWAP2 -D /4D			\ calculate the offset2
	.long w_DoubleAt_cw             
	.long w_swap_D_cw               
	.long w_minus_D_cw              
	.long w_div4_D_cw               
	.long w_swap_D_cw                // 000058 		SWAP2 !D ;				\ and back-fill it in the original location
	.long w_StoreDouble_cw          
	.long w_exit_cw                 
