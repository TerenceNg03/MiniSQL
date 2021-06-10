%{
#include <stdio.h>
#include <string.h>
#include "C_warpper.hpp"
#include "y.tab.h"
#define YYDEBUG 1
extern int yyparse(yyscan_t scanner);

extern int yy_scan_string(char* str);
extern int yylex(YYSTYPE* yylvalp, YYLTYPE* yyllocp, yyscan_t scanner);
extern void yyerror(YYLTYPE* yyllocp, yyscan_t unused, const char* msg);
extern int yylex_init ( yyscan_t * ptr_yy_globals ) ;
extern int yylex_destroy ( yyscan_t yyscanner ) ;

int yywrap(void)
{
    return 1;
}

void yyerror(YYLTYPE* yyllocp, yyscan_t scanner, const char *s)
{
    printf("[error] %s\n", s);
}

%}

%code requires{
    #include "C_warpper.hpp"
    typedef void* yyscan_t;
}

%define parse.error verbose
%define api.pure full
%locations
%param { yyscan_t scanner }

%union
{
int i;
char *s;
double f;
condition* c_con;
conditions* c_cons;
c_column* c_col;
c_columns* c_cols;
c_dbitems* c_its;
c_dbitem* c_it;
int OP;
}

%token CREATE DROP SELECT INSERT QUIT DELETE EXECFILE

%token INDEX TABLE VALUES
%token TYPE_INT TYPE_CHAR TYPE_FLOAT

%token ON UNIQUE PRIMARY KEY FROM WHERE INTO

%token <i> INT
%token <f> FLOAT
%token <s> STR

%token <s> IDENTIFIER
%token <s> FILENAME

%token AND LT GT GE LE EQ NOT

%token L_BRACKET R_BRACKET SEMICOLON COMMA ALL

%type <c_col> table_col table_col_int table_col_char table_col_float table_col_int_uni table_col_char_uni table_col_float_uni;
%type <c_cols> table_cols  ;

%type <c_its> insert_val_pre;
%type <c_it> value;


%type <s> table_col_prim;

%type <c_cons> opt_conditions pre_condition;
%type <c_con> condition;
%type <OP> comparator;

%%
commands: /* empty */| commands command SEMICOLON;

command: | create_index | create_table | drop_table | drop_index | select | insert | delete | execfile | quit;

create_index:
CREATE INDEX IDENTIFIER ON IDENTIFIER L_BRACKET IDENTIFIER R_BRACKET

create_table :
CREATE TABLE IDENTIFIER L_BRACKET table_cols table_col_prim R_BRACKET
{
    cmd_create_table($3,$5,$6);
}

table_cols :
{$$ = create_cols();}| table_cols table_col COMMA
{
    $$ = add_col($1,$2);
}

table_col : table_col_int {$$ = $1;}
| table_col_char {$$ = $1;}
| table_col_float {$$ = $1;}
| table_col_int_uni {$$ = $1;}
| table_col_char_uni {$$ = $1;}
| table_col_float_uni {$$ = $1;}


table_col_int :
IDENTIFIER TYPE_INT
{
    $$ = create_col(DB_INT,$1,0,0);
}

table_col_int_uni :
IDENTIFIER TYPE_INT UNIQUE
{
    $$ = create_col(DB_INT,$1,1,0);
}

table_col_char :
IDENTIFIER TYPE_CHAR L_BRACKET INT R_BRACKET
{
    $$ = create_col(DB_CHAR,$1,0,$4);
}

table_col_char_uni :
IDENTIFIER TYPE_CHAR L_BRACKET INT R_BRACKET UNIQUE
{
    $$ = create_col(DB_CHAR,$1,1,$4);
}

table_col_float :
IDENTIFIER TYPE_FLOAT
{
    $$ = create_col(DB_FLOAT,$1,0,0);
}

table_col_float_uni :
IDENTIFIER TYPE_FLOAT UNIQUE
{
    $$ = create_col(DB_FLOAT,$1,1,0);
}

table_col_prim:
PRIMARY KEY L_BRACKET IDENTIFIER R_BRACKET
{
    $$ = $4;
}

drop_table:
DROP TABLE IDENTIFIER
{
    cmd_drop_table($3);
}

drop_index:
DROP INDEX IDENTIFIER

select:
SELECT ALL FROM IDENTIFIER opt_conditions
{
    cmd_select($4,$5);
}

opt_conditions:
{ $$ = create_conditions();}
|WHERE pre_condition condition
{
   $$ = add_condition($2,$3);
}

pre_condition:
{$$ = create_conditions();}
| pre_condition condition AND
{
   $$ = add_condition($1,$2);
}

condition:
IDENTIFIER comparator value
{
    $$ = create_condition($1,$2,$3);
}

comparator:
LE {$$ = op_LE;}
| GE {$$ = op_GE;}
| LT {$$ = op_LT;}
| GT {$$ = op_GT;}
| EQ {$$ = op_EQ;}
| NOT {$$ = op_NE;}

value:
INT {struct yy_db_item it; it.i=$1; $$=create_dbitem(DB_INT,it);}
| STR {struct yy_db_item it; it.s=$1; $$=create_dbitem(DB_CHAR,it);}
| FLOAT {struct yy_db_item it; it.d=$1; $$=create_dbitem(DB_FLOAT,it);}

insert:
INSERT INTO IDENTIFIER VALUES L_BRACKET insert_val_pre value R_BRACKET
{
    void* items = add_item($6,$7);
    cmd_insert($3, items);
}

insert_val_pre:
{ $$ = create_dbitems();}
| insert_val_pre value COMMA
{
    $$ = add_item($1,$2);
}

delete:
DELETE FROM IDENTIFIER opt_conditions
{
	cmd_delete($3,$4);
}

quit:
QUIT
{
    YYACCEPT;
}

execfile:
EXECFILE IDENTIFIER {cmd_execfile($2);}
| EXECFILE FILENAME {cmd_execfile($2);}
%%
