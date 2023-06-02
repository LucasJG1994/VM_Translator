%{
	#include "vmth.tab.h"
	#include "file_writer.h"

	#include <stdio.h>

	#define YYERROR_VERBOSE

	int line = 0;

	int yyerror(const char* msg){
		printf("%s on line %d\n", msg, line);
		return 0;
	}
%}

%code requires{
	#include "scanner.h"
}

%union{
	char* sval;
	int   ival;
};

%token TK_EOF 0

%token TK_PUSH
%token TK_POP
%token TK_ARG
%token TK_LCL
%token TK_STC
%token TK_CST
%token TK_THS
%token TK_THT
%token TK_PTR
%token TK_TMP
%token TK_ADD
%token TK_SUB
%token TK_NEG
%token TK_EQ
%token TK_GT
%token TK_LT
%token TK_AND
%token TK_OR
%token TK_NOT
%token TK_LABEL
%token TK_GOTO
%token TK_IFGOTO
%token TK_FUNCTION
%token TK_CALL
%token TK_RETURN
%token TK_COMMA

%token <sval> TK_ID
%token <ival> TK_NUM

%type <ival> expr


%%

start:
	| expr
	| start expr
	;

expr:
	  TK_PUSH     TK_ARG    TK_NUM[N]         { fw_push(TK_ARG, $N); }
	| TK_PUSH     TK_LCL    TK_NUM[N]         { fw_push(TK_LCL, $N); }
	| TK_PUSH     TK_STC    TK_NUM[N]         { fw_push(TK_STC, $N); }
	| TK_PUSH     TK_CST    TK_NUM[N]         { fw_push(TK_CST, $N); }
	| TK_PUSH     TK_THS    TK_NUM[N]         { fw_push(TK_THS, $N); }
	| TK_PUSH     TK_THT    TK_NUM[N]         { fw_push(TK_THT, $N); }
	| TK_PUSH     TK_PTR    TK_NUM[N]         { fw_push(TK_PTR, $N); }
	| TK_PUSH     TK_TMP    TK_NUM[N]         { fw_push(TK_TMP, $N); }

	| TK_POP      TK_ARG    TK_NUM[N]         { fw_pop(TK_ARG, $N); }
	| TK_POP      TK_LCL    TK_NUM[N]         { fw_pop(TK_LCL, $N); }
	| TK_POP      TK_STC    TK_NUM[N]         { fw_pop(TK_STC, $N); }
	| TK_POP      TK_THS    TK_NUM[N]         { fw_pop(TK_THS, $N); }
	| TK_POP      TK_THT    TK_NUM[N]         { fw_pop(TK_THT, $N); }
	| TK_POP      TK_PTR    TK_NUM[N]         { fw_pop(TK_PTR, $N); }
	| TK_POP      TK_TMP    TK_NUM[N]         { fw_pop(TK_TMP, $N); }

	| TK_LABEL    TK_ID[ID]                   { fw_label($ID); }
	| TK_GOTO     TK_ID[ID]                   { fw_goto($ID); }
	| TK_IFGOTO   TK_ID[ID]                   { fw_ifgoto($ID); }
	| TK_CALL     TK_ID[ID] TK_NUM[N]         { fw_call($ID, $N); }
	| TK_FUNCTION TK_ID[ID] TK_NUM[N]         { fw_fndef($ID, $N); }
	| TK_RETURN                               { fw_return(); }
	| TK_ADD                                  { fw_add(); }
	| TK_SUB                                  { fw_sub(); }
	| TK_NEG                                  { fw_neg(); }
	| TK_EQ                                   { fw_eq(); }
	| TK_GT                                   { fw_gt(); }
	| TK_LT                                   { fw_lt(); }
	| TK_AND                                  { fw_and(); }
	| TK_OR                                   { fw_or(); }
	| TK_NOT                                  { fw_not(); }
	;
