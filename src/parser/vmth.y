%{
	
%}

%union{
	char* sval;
	int   ival;
};

%token ARGUMENT LOCAL STATIC CONSTANT THIS THAT POINTER TEMP
%token ADD SUB NEG
%token EQ GT LT
%token AND OR NOT
%token PUSH POP

%token LABEL GOTO IF_GOTO
%token FUNCTION CALL RETURN

%token COMMA

%token <sval> ID
%token <ival> NUM

%type <sval> expr
%type <ival> segment
%type <ival> index
%type <sval> arithmetic
%type <sval> comparison
%type <sval> logical

%%

start:
	| start expr { }
	;

expr: 
	  PUSH segment index 	{}
	| POP segment index 	{}
	| arithmetic 			{}
	| comparison 			{}
	| logical 				{}
	| LABEL ID 				{}
	| GOTO ID 				{}
	| IF_GOTO ID 			{}
	| CALL ID 				{}
	| FUNCTION ID index 	{}
	| RETURN 				{}
	;

arithmetic:
	  ADD {}
	| SUB {}
	| NEG {}
	;

comparison:
	  EQ {}
	| GT {}
	| LT {}
	;

logical:
	  AND {}
	| OR {}
	| NOT {}
	;

segment:
	  ARGUMENT {}
	| LOCAL {}
	| STATIC {}
	| CONSTANT {}
	| THIS {}
	| THAT {}
	| POINTER {}
	| TEMP {}
	;

index: 
	  NUM { $$ = atoi($1); };