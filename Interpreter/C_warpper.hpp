//
//  C_warpper.hpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/20.
//

#ifndef C_warpper_hpp
#define C_warpper_hpp

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
enum DB_TYPE{DB_INT,DB_FLOAT,DB_CHAR};
enum op{op_EQ,op_LT,op_GT,op_LE,op_GE,op_NE};

struct yy_db_item{
    union{
        int i;
        double d;
        char* s;
    };
};

struct _c_column;
typedef struct _c_column c_column;

c_column* create_col(enum DB_TYPE type, char* name, char unique, int length);

struct _c_columns;
typedef struct _c_columns c_columns;

c_columns* create_cols(void);
c_columns* add_col(c_columns* cols, c_column* col);

void cmd_create_table(char* name, c_columns* cols, char* prim);
void cmd_drop_table(char* name);

struct _c_dbitem;
typedef struct _c_dbitem c_dbitem;

c_dbitem* create_dbitem(enum DB_TYPE type, struct yy_db_item it);

struct _c_dbitems;
typedef struct _c_dbitems c_dbitems;

c_dbitems* create_dbitems(void);
c_dbitems* add_item(c_dbitems* items, c_dbitem* item);
void cmd_insert(char* name, c_dbitems* items);

struct _conditions;
typedef struct _conditions conditions;

struct c_condition;
typedef struct c_condition condition;

conditions* create_conditions(void);
condition* create_condition(char* name, enum op OP, c_dbitem* item);

conditions* add_condition(conditions* _conditions, condition* _condition);
void cmd_select(char* name, conditions* conditions);
void cmd_delete(char* name, conditions* conditions);
void cmd_execfile(char* filename);
void cmd_create_index(char* index, char* table, char* cols);
void cmd_drop_index(char* index);
#ifdef __cplusplus
}
#endif

#endif /* C_warpper_hpp */
