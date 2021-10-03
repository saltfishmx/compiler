%locations
%define parse.error verbose
%{
#include "lex.yy.c"
#include "tree.h"
#include <stdio.h>
int print_test = 0;
extern int error;
%}
/* declared types */
%union {
    struct _Node *node;
}
/* declared tokens */
%token <node> INT FLOAT ID Type
%token <node> ASSIGNOP OR AND RELOP PLUS MINUS STAR DIV NOT LP RP LB RB DOT
%token <node> SEMI COMMA STRUCT LC RC RETURN IF WHILE ELSE

/* operator ,precedence and associativity*/
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT 
%left LP RP LB RB DOT
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

/* non - terminal*/
%type <node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args
%%

Program: ExtDefList {
        if(print_test)printf("Program: ExtDefList\n");
        if($1==NULL)$$ = malloc_node("Program",0,NULL,yylineno);
        else $$ = malloc_node("Program",0,NULL,@1.first_line);
        add($$,$1);
        root = $$;
    }
    ;
ExtDefList: ExtDef ExtDefList{
        if(print_test)printf("ExtDefList: ExtDef ExtDefList\n");
        $$ = malloc_node("ExtDefList",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
    }
    | {
        if(print_test)printf("ExtDefList: empty\n");
        $$ = NULL;
        }
    ;
ExtDef: Specifier ExtDecList SEMI {
        if(print_test)printf("ExtDef: Specifier ExtDecList SEMI\n");
        $$ = malloc_node("ExtDef",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    | Specifier SEMI{
        if(print_test)printf("ExtDef:Specifier SEMI\n");
        $$ = malloc_node("ExtDef",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        }
    | Specifier FunDec CompSt{
        if(print_test)printf("ExtDef:Specifier FunDec CompSt\n");
        $$ = malloc_node("ExtDef",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    ;
ExtDecList: VarDec {
        if(print_test)printf("ExtDecList: VarDec\n");
        $$ = malloc_node("ExtDecList",0,NULL,@1.first_line);
        add($$,$1);
        }
    | VarDec COMMA ExtDecList {
        if(print_test)printf("ExtDecList: VarDec COMMA ExtDecList\n");
        $$ = malloc_node("ExtDecList",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    ;
Specifier:Type {
        if(print_test)printf("Specifier:Type\n");
        $$ = malloc_node("Specifier",0,NULL,@1.first_line);
        add($$,$1);
        }   
    | StructSpecifier {
        if(print_test)printf("Specifier:StructSpecifier\n");
        $$ = malloc_node("Specifier",0,NULL,@1.first_line);
        add($$,$1);
        }
    ;
StructSpecifier: STRUCT OptTag LC DefList RC {
        if(print_test)printf("StructSpecifier: STRUCT OptTag LC DefList RC\n");
        $$ = malloc_node("StructSpecifier",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        add($$,$4);
        add($$,$5);        
        }
    
    | STRUCT Tag {
        if(print_test)printf("StructSpecifier: STRUCT Tag\n");
        $$ = malloc_node("StructSpecifier",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        }
    ; 
OptTag: ID {
        if(print_test)printf("OptTag: ID\n");
        $$ = malloc_node("OptTag",0,NULL,@1.first_line);
        add($$,$1);
        }
    | {
        if(print_test)printf("OptTag: empty\n");
        $$ = NULL;
        }
    ;
Tag: ID {
        if(print_test)printf("Tag: ID\n");
        $$ = malloc_node("Tag",0,NULL,@1.first_line);
        add($$,$1);
        }
    ;
VarDec: ID {
        if(print_test)printf("VarDec: ID\n");
        $$ = malloc_node("VarDec",0,NULL,@1.first_line);
        add($$,$1);
        }
    | VarDec LB INT RB {
        if(print_test)printf("VarDec: VarDec LB INT RB\n");
        $$ = malloc_node("VarDec",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        add($$,$4);        
        }
    | error RB  {}       
    ;
FunDec: ID LP VarList RP {
        if(print_test)printf("FunDec: ID LP VarList RP\n");
        $$ = malloc_node("FunDec",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        add($$,$4);  
        }
    | ID LP RP {
        if(print_test)printf("FunDec: ID LP RP\n");
        $$ = malloc_node("FunDec",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    |  error RP {}
    ;
VarList: ParamDec COMMA VarList {
        if(print_test)printf("VarList: ParamDec COMMA VarList\n");
        $$ = malloc_node("VarList",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    | ParamDec {
        if(print_test)printf("VarList: ParamDec \n");
        $$ = malloc_node("VarList",0,NULL,@1.first_line);
        add($$,$1);
        }
    ;
ParamDec: Specifier VarDec {
        if(print_test)printf("ParamDec: Specifier VarDec\n");
        $$ = malloc_node("ParamDec",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        }
    ;
CompSt: LC DefList StmtList RC {
        if(print_test)printf("CompSt: LC DefList StmtList RC\n");
        $$ = malloc_node("CompSt",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        add($$,$4); 
        }
        | error RC{}     
    ;
StmtList: Stmt StmtList {
        if(print_test)printf("StmtList: Stmt StmtList\n");
        $$ = malloc_node("StmtList",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        }
    | {
        if(print_test)printf("StmtList: empty\n");
        $$ = NULL;
        }
    ;
Stmt: Exp SEMI {
        if(print_test)printf("Stmt: Exp SEMI\n");
        $$ = malloc_node("Stmt",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        }
    | CompSt {
        if(print_test)printf("Stmt: CompSt\n");
        $$ = malloc_node("Stmt",0,NULL,@1.first_line);
        add($$,$1);
        }
    | RETURN Exp SEMI {
        if(print_test)printf("Stmt: RETURN Exp SEMI\n");
        $$ = malloc_node("Stmt",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {
        if(print_test)printf("Stmt: IF LP Exp RP Stmt\n");
        $$ = malloc_node("Stmt",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        add($$,$4);
        add($$,$5);
        }
    | IF LP Exp RP Stmt ELSE Stmt {
        if(print_test)printf("Stmt: IF LP Exp RP Stmt ELSE Stmt\n");
        $$ = malloc_node("Stmt",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        add($$,$4);
        add($$,$5);
        add($$,$6);
        add($$,$7);
        }
    | WHILE LP Exp RP Stmt {
        if(print_test)printf("Stmt: WHILE LP Exp RP Stmt\n");
        $$ = malloc_node("Stmt",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        add($$,$4);
        add($$,$5);
        }

    | error SEMI{}        
    ;
DefList: Def DefList {
        if(print_test)printf("DefList: Def DefList\n");
        $$ = malloc_node("DefList",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        }
    | {
        if(print_test)printf("DefList: empty\n");
        $$ = NULL;
        }
    ;
Def: Specifier DecList SEMI {
        if(print_test)printf("Def: Specifier DecList SEMI\n");
        $$ = malloc_node("Def",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    ;
DecList: Dec {
        if(print_test)printf("DecList: Dec\n");
        $$ = malloc_node("DecList",0,NULL,@1.first_line);
        add($$,$1);
        }
    | Dec COMMA DecList {
        if(print_test)printf("DecList: Dec COMMA DecList\n");
        $$ = malloc_node("DecList",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    ;
Dec: VarDec {
        if(print_test)printf("Dec: VarDec\n");
        $$ = malloc_node("Dec",0,NULL,@1.first_line);
        add($$,$1);
        }
    | VarDec ASSIGNOP Exp {
        if(print_test)printf("Dec: VarDec ASSIGNOP Exp\n");
        $$ = malloc_node("Dec",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    ;
Exp: Exp ASSIGNOP Exp {
        if(print_test)printf("Exp: Exp ASSIGNOP Exp\n");
        $$ = malloc_node("Exp",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    | Exp AND Exp {
        if(print_test)printf("Exp: Exp AND Exp\n");
        $$ = malloc_node("Exp",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    | Exp OR Exp {
        if(print_test)printf("Exp: Exp OR Exp\n");
        $$ = malloc_node("Exp",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    | Exp RELOP Exp {
        if(print_test)printf("Exp: Exp RELOP Exp\n");
        $$ = malloc_node("Exp",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    | Exp PLUS Exp {
        if(print_test)printf("Exp: Exp PLUS Exp\n");
        $$ = malloc_node("Exp",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    | Exp MINUS Exp {
        if(print_test)printf("Exp: Exp MINUS Exp\n");
        $$ = malloc_node("Exp",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    | Exp STAR Exp {
        if(print_test)printf("Exp: Exp STAR Exp\n");
        $$ = malloc_node("Exp",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    | Exp DIV Exp {
        if(print_test)printf("Exp: Exp DIV Exp\n");
        $$ = malloc_node("Exp",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    | LP Exp RP  {
        if(print_test)printf("Exp: LP Exp RP\n");
        $$ = malloc_node("Exp",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    | MINUS Exp {
        if(print_test)printf("Exp: MINUS Exp\n");
        $$ = malloc_node("Exp",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        }
    | NOT Exp  {
        if(print_test)printf("Exp: NOT Exp \n");
        $$ = malloc_node("Exp",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        }
    | ID LP Args RP {
        if(print_test)printf("Exp: ID LP Args RP\n");
        $$ = malloc_node("Exp",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        add($$,$4);
        }
    | ID LP RP {
        if(print_test)printf("Exp: ID LP RP\n");
        $$ = malloc_node("Exp",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    | Exp LB Exp RB {
        if(print_test)printf("Exp: Exp LB Exp RB\n");
        $$ = malloc_node("Exp",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        add($$,$4);
        }
    | Exp DOT ID {
        if(print_test)printf("Exp: Exp DOT ID\n");
        $$ = malloc_node("Exp",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    | ID {
        if(print_test)printf("Exp: ID\n");
        $$ = malloc_node("Exp",0,NULL,@1.first_line);
        add($$,$1);
        }
    | INT {
        if(print_test)printf("Exp: INT \n");
        $$ = malloc_node("Exp",0,NULL,@1.first_line);
        add($$,$1);
        }
    | FLOAT {
        if(print_test)printf("Exp: FLOAT\n");
        $$ = malloc_node("Exp",0,NULL,@1.first_line);
        add($$,$1);
        }        
    ;
Args: Exp COMMA Args {
        if(print_test)printf("Args: Exp COMMA Args\n");
        $$ = malloc_node("Args",0,NULL,@1.first_line);
        add($$,$1);
        add($$,$2);
        add($$,$3);
        }
    | Exp  {
        if(print_test)printf("Args: Exp \n");
        $$ = malloc_node("Args",0,NULL,@1.first_line);
        add($$,$1);
        }
    ; 
%%
yyerror(char* msg) {
    error = 1;
    printf("Error type B at Line %d: %s.\n", yylineno, msg);
}