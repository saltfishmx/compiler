%{
    #include "syntax.tab.h"
    #include "tree.h"
    int yycolumn = 1;
    extern int error;
    #define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
%}
%option yylineno
digit [0-9]
letter [a-zA-Z]
INT 0|[1-9]+[0-9]*
FLOAT {digit}+"."{digit}+
ID (_|{letter})(_|{letter}|{digit}){0,31}
SEMI ;
COMMA ","
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS "+"
MINUS "-"
STAR "*" 
DIV "/"
AND &&
OR "||"
DOT "."
NOT !
TYPE int|float
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
STRUCT struct
RETURN return
IF if
%%
{INT} {
    int val = atoi(yytext);
    yylval.node = malloc_node("INT",INT,&val,yylineno);
    return INT;
    }
{FLOAT} {
    float val = strtof(yytext, NULL);
    yylval.node = malloc_node("FLOAT",FLOAT,&val,yylineno);
    return FLOAT;
    }
{TYPE} {
    char *s = malloc(strlen(yytext) + 1);
    strcpy(s, yytext);
    yylval.node = malloc_node( "TYPE", Type, s,yylineno);
    return Type;
    }    
{SEMI} {
    yylval.node = malloc_node("SEMI",SEMI,NULL,yylineno);
    return SEMI;
    }
{COMMA} {
    yylval.node = malloc_node("COMMA",COMMA,NULL,yylineno);
    return COMMA;
    }
{ASSIGNOP} {
    yylval.node = malloc_node("ASSIGNOP",ASSIGNOP,NULL,yylineno);
    return ASSIGNOP;
    }
{RELOP} {
    yylval.node = malloc_node("RELOP",RELOP,NULL,yylineno);    
    return RELOP;
    }
{PLUS} {
    yylval.node = malloc_node("PLUS",PLUS,NULL,yylineno);
    return PLUS;
    }
{MINUS} {
    yylval.node = malloc_node("MINUS",MINUS,NULL,yylineno);    
    return MINUS;
    }
{STAR} {
    yylval.node = malloc_node("STAR",STAR,NULL,yylineno);
    return STAR;
    }
{DIV} {
    yylval.node = malloc_node("DIV",DIV,NULL,yylineno);
    return DIV;
    }
{AND} {
    yylval.node = malloc_node("AND",AND,NULL,yylineno);
    return AND;
    }
{OR} {
    yylval.node = malloc_node("OR",OR,NULL,yylineno);
    return OR;
    }
{DOT} {
    yylval.node = malloc_node("DOT",DOT,NULL,yylineno);
    return DOT;
    }
{NOT} {
    yylval.node = malloc_node("NOT",NOT,NULL,yylineno);
    return NOT;
    }
{LP} {
    yylval.node = malloc_node("LP",LP,NULL,yylineno);
    return LP;
    }
{RP} {
    yylval.node = malloc_node("RP",RP,NULL,yylineno);
    return RP;
    }
{LB} {
    yylval.node = malloc_node("LB",LB,NULL,yylineno);
    return LB;
    }
{RB} {
    yylval.node = malloc_node("RB",RB,NULL,yylineno);
    return RB;
    }
{LC} {
    yylval.node = malloc_node("LC",LC,NULL,yylineno);
    return LC;
    }
{RC} {
    yylval.node = malloc_node("RC",RC,NULL,yylineno);
    return RC;
    }
{STRUCT} {
    yylval.node = malloc_node("STRUCT",STRUCT,NULL,yylineno);    
    return STRUCT;
    }
{RETURN} {
    yylval.node = malloc_node("RETURN",RETURN,NULL,yylineno);    
    return RETURN;
    }
{IF} {
    yylval.node = malloc_node("IF",IF,NULL,yylineno);    
    return IF;
    }
"else" {
    yylval.node = malloc_node("ELSE",ELSE,NULL,yylineno);    
    return ELSE;
    }
{ID} {
    char *s = malloc(strlen(yytext) + 1);
    strcpy(s, yytext);
    yylval.node = malloc_node( "ID", ID, s,yylineno);
    return ID;
    }
\n {yycolumn = 1;}
[" "|\r|\t] {}
. {
    error = 1;
    printf("Error type A at Line %d: Mysterious characters \'%s\'\n",
    yylineno, yytext);
}
%%
