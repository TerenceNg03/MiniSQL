//
//  C_warpper.cpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/20.
//

#include "C_warpper.hpp"
#include <vector>
#include "Catalog_Manager.hpp"
#include "Record_Manager.hpp"
#include "Exception.hpp"
#include "Interpreter.hpp"
#include <regex>
#include <memory>
#include "B_Plus_Tree.hpp"
using namespace std;

struct _c_column{
    db_table::column val;
};

c_column* create_col(DB_TYPE type, char* name, char unique, int length){
    auto c_col = new c_column();
    c_col->val.name = name;
    c_col->val.IsUnique = unique;
    switch (type) {
        case DB_INT:
            c_col->val.T = db_item::type::DB_INT;
            break;
        case DB_FLOAT:
            c_col->val.T = db_item::type::DB_FLOAT;
            break;
        case DB_CHAR:
            c_col->val.T = db_item::type::DB_CHAR;
            c_col->val.length = length;
            break;
    }
    return c_col;
}

struct _c_columns{
    vector<db_table::column> val;
};

c_columns* create_cols(){
    auto c_cols = new c_columns();
    return c_cols;
}
c_columns* add_col(c_columns* c_cols, c_column* c_col){
    c_cols->val.push_back(c_col->val);
    //printf("%s\n",c_cols->val.back().name.c_str());
    delete c_col;
    return c_cols;
}

extern Catalog_Manager ctm;

void cmd_create_table(char* name, c_columns* cols, char* prim){
    bool found = false;
    for(int i=0; i<cols->val.size(); i++){
        //printf("%s %s\n",c.name.c_str(),prim);
        if(!strcmp(cols->val[i].name.c_str(),prim)){
            cols->val[i].IsPrimary = true;
            cols->val[i].IsUnique = true;
            found = true;
        }
    }
    if(!found){
        throw FormatException("Invalid primary key %s.\n",name);
    }
    
    db_table dbt;
    dbt.columns = cols->val;
    delete cols;
    free(prim);
    dbt.name = name;
    dbt.filename = "DB_Data/"+dbt.name + ".db";
    dbt.size = 0;
    
    ctm.insert(dbt);
    free(name);
}

void cmd_drop_table(char* name){
    ctm.erase(name);
    free(name);
}

struct _c_dbitem{
    db_item val;
};

c_dbitem* create_dbitem(enum DB_TYPE type, yy_db_item it){
    auto item = new c_dbitem();
    switch (type) {
        case DB_INT:
            item->val.T = db_item::type::DB_INT;
            item->val.i = it.i;
            break;
        case DB_FLOAT:
            item->val.T = db_item::type::DB_FLOAT;
            item->val.d = it.d;
            break;
        case DB_CHAR:
            item->val.T = db_item::type::DB_CHAR;
            item->val.s = it.s;
            break;
    }
    return item;
}

struct _c_dbitems{
    vector<db_item> val;
};

c_dbitems* create_dbitems(){
    auto its = new c_dbitems();
    return its;
}
c_dbitems* add_item(c_dbitems* items, c_dbitem* item){
    
    items->val.push_back(item->val);
    delete item;
    return items;
}

void cmd_insert(char* _name, c_dbitems* items){
    string name = _name;
    free(_name);
    auto it = find_if(ctm.catalogs.begin(), ctm.catalogs.end(), [=](auto dbt)->bool{return (dbt.name==name);});
    if(it==ctm.catalogs.end()){
        throw FormatException("Can not insert into non-exist table %s.\n");
    }
    for(int i=0; i<it->size; i++){
        auto entry = Record_Manager::read(*it, i);
        if(entry.first){
            for(int i=0; i<it->columns.size(); i++){
                if(it->columns[i].IsUnique){
                    if(entry.second[i]==items->val[i])
                        throw FormatException("Unique constraint check failed\n");
                }
            }
        }
    }
    
    
    Record_Manager::insert(ctm.catalogs[it-ctm.catalogs.begin()], items->val);
    delete items;
}

struct _conditions{

    vector<tuple<string,db_item,op>> compares;
    
    bool compare(const vector<db_item>& entry, const db_table& dbt){
        for(auto item:compares){
            string name = get<0>(item);
            db_item des = get<1>(item);
            auto _it = find_if(dbt.columns.begin(), dbt.columns.end(), [=](auto i)->bool{return i.name==name;});
            db_item src;
            if(_it!=dbt.columns.end()){
                src = entry.at(_it-dbt.columns.begin());
            }else{
                throw FormatException("Unknown condition '%s'",name.c_str());
            }
            switch (get<2>(item)) {
                case op::op_EQ:
                    if(!(src==des))return false;
                    break;
                case op::op_LT:
                    if(!(src<des))return false;
                    break;
                case op::op_GT:
                    if(src<des||src==des)return false;
                    break;
                case op::op_LE:
                    if(!(src<des||src==des))return false;
                    break;
                case op::op_GE:
                    if(src<des)return false;
                    break;
                case op::op_NE:
                    if(!(src==des))return false;
                    break;
            }
        }
        return true;
    }
};

conditions* create_conditions(void){
    auto res = new conditions();
    return res;
}

struct c_condition{
    tuple<string,db_item,op> val;
};

condition* create_condition(char* name, enum op OP, c_dbitem* item){
    
    auto res = new condition();
    res->val = {name,item->val,OP};
    delete item;
    return res;
}

conditions* add_condition(conditions* conditions, condition* condition){
    conditions->compares.push_back(condition->val);
    delete condition;
    return conditions;
}

void cmd_select(char* _name, conditions* cds){
    string name(_name);
    free(_name);
    auto dbt = find_if(ctm.catalogs.begin(), ctm.catalogs.end(), [=](auto i)->bool{return i.name==name;});
    if(dbt==ctm.catalogs.end()){
        throw FormatException("Unknown table name %s\n",name.c_str());
    }
    printf("size : %d\n",dbt->size);
//    auto result = vector<vector<db_item>>();
    for(auto c:dbt->columns){
        printf("\t%s",c.name.c_str());
    }
    putchar('\n');
    for(int i=0; i<dbt->size; i++){
        auto entry = Record_Manager::read(*dbt, i);
        if(entry.first&&cds->compare(entry.second, *dbt)){
            for(auto c:entry.second){
                switch (c.T) {
                    case db_item::type::DB_CHAR:
                        printf("\t%s",c.s.c_str());
                        break;
                    case db_item::type::DB_INT:
                        printf("\t%d",c.i);
                        break;
                        
                    case db_item::type::DB_FLOAT:
                        printf("\t%.2lf",c.d);
                        break;
                }
            }
            putchar('\n');
        }
    }
}

void cmd_delete(char* _name, conditions* cds){
    string name(_name);
    free(_name);
    auto dbt = find_if(ctm.catalogs.begin(), ctm.catalogs.end(), [=](auto i)->bool{return i.name==name;});
    if(dbt==ctm.catalogs.end()){
        throw FormatException("Unknown table name %s\n",name.c_str());
    }
    for(int i=0; i<dbt->size; i++){
        auto entry = Record_Manager::read(*dbt, i);
        if(entry.first&&cds->compare(entry.second, *dbt)){
            Record_Manager::write(*dbt, make_pair(false, entry.second), i);
        }
    }
}

void cmd_execfile(char* filename){
    FILE* fp = fopen(filename, "r");
    if(fp==NULL)throw FormatException("Fail to open file %s\n",filename);
    yyscan_t scanner;
    yylex_init(&scanner);
    yyset_in(fp, scanner);
    yyparse(scanner);
    yylex_destroy(scanner);
    free(filename);
}

void cmd_create_index(char* _index, char* _name, char*_col){
    string name(_name);
    string index(_index);
    string col(_col);
    free(_index);
    free(_name);
    free(_col);
    auto dbt = find_if(ctm.catalogs.begin(), ctm.catalogs.end(), [=](auto i)->bool{return i.name==name;});
    if(dbt==ctm.catalogs.end()){
        throw FormatException("Unknown table name %s\n",name.c_str());
    }
    auto dbt_col = find_if(dbt->columns.begin(), dbt->columns.end(), [=](auto i)->bool{return i.name==col;});
    if(dbt_col==dbt->columns.end()){
        throw FormatException("Unknown coloumn name %s\n",col.c_str());
    }
    if(!dbt_col->IsUnique)throw FormatException("Cannot create index on non-unique coloumn\n");
    auto bpt = B_Plus_tree("DB_Data/"+index+".idx",dbt_col->T,dbt_col->length);
    ctm.indexs.push_back(make_pair(index, make_pair(name, col)));
    for(int i=0; i<dbt->size; i++){
        auto entry = Record_Manager::read(*dbt, i);
        if(entry.first){
            bpt.insert(entry.second[dbt_col-dbt->columns.begin()], i);
        }
    }
}
void cmd_drop_index(char* _index){
    string index(_index);
    free(_index);
    auto idx = find_if(ctm.indexs.begin(), ctm.indexs.end(), [=](auto i)->bool{return i.first==index;});
    if(idx==ctm.indexs.end()){
        throw FormatException("Unknown index %s\n",index.c_str());
    }
    
    ctm.indexs.erase(idx);
}
