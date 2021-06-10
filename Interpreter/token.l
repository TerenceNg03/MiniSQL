%{
#include <stdio.h>
#include "y.tab.h"


%}

%option caseless
%option noinput nounput noyywrap 8bit nodefault                                 
%option yylineno
%option reentrant bison-bridge bison-locations

%%
create return CREATE;
drop return DROP;
select return SELECT;
insert return INSERT;
quit return QUIT;
execfile return EXECFILE;
delete return DELETE;

index return INDEX;
table return TABLE;
values return VALUES;

on return ON;
into return INTO;
from return FROM;
where return WHERE;
unique return UNIQUE;

primary return PRIMARY;
key return KEY;

\-{0,1}[0-9]*\.[0-9]* yylval->f = atof(yytext); return FLOAT;
[0-9]* yylval->i = atoi(yytext); return INT;
'.*' yylval->s = strdup(yytext); memmove(yylval->s, yylval->s+1, strlen(yylval->s+1)); yylval->s[strlen(yylval->s)-2] = 0;return STR;

int return TYPE_INT;
float return TYPE_FLOAT;
char return TYPE_CHAR;


and return AND;
"<" return LT;
">" return GT;
"<=" return LE;
">=" return GE;
"=" return EQ;
"<>" return NOT;

[a-zA-Z_]{1,}[0-9a-zA-Z_]* yylval->s = strdup(yytext); return IDENTIFIER;
[a-zA-Z_\-\./]* yylval->s = strdup(yytext); return FILENAME;

\( return L_BRACKET;
\) return R_BRACKET;
\; return SEMICOLON;
\, return COMMA;
\* return ALL;
\n printf("MiniSQL $ ");/* ignore end of line */
[ \t]+ /* ignore whitespace */
%%

