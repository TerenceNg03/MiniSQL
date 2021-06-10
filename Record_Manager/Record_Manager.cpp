//
//  Record_Manager.cpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/18.
//

#include "Record_Manager.hpp"
#include "Exception.hpp"
#include "Buffer_Manager.hpp"
#include "Catalog_Manager.hpp"

using namespace std;
extern lru_cache db_buffer;

int db_table::record_size() const{
    int s = sizeof(bool);
    for (auto c :columns){
        switch (c.T) {
            case db_item::type::DB_INT:
                s += sizeof(int);
                break;
            case db_item::type::DB_CHAR:
                s += sizeof(char)*c.length;
                break;
            case db_item::type::DB_FLOAT:
                s += sizeof(double);
                break;
            default:
                break;
        }
    }
    return s;
}

db_item::db_item(const std::string& s){
    this->s = s;
    T = type::DB_CHAR;
}

db_item::db_item(const char* s){
    this->s = s;
    T = type::DB_CHAR;
};

db_item::db_item(int i){
    this->i = i;
    T = type::DB_INT;
}
db_item::db_item(double d){
    this->d = d;
    T = type::DB_FLOAT;
}


bool operator ==(const db_item& A, const db_item& B){
    if(A.T!=B.T)return false;
    switch (A.T) {
        case db_item::type::DB_CHAR:
            return A.s==B.s;
            break;
        case db_item::type::DB_INT:
            return A.i==B.i;
        case db_item::type::DB_FLOAT:
            return A.d==B.d;
            break;
    }
}

bool operator <(const db_item& A, const db_item& B){
    if(A.T!=B.T)return false;
    switch (A.T) {
        case db_item::type::DB_CHAR:
            return A.s<B.s;
        case db_item::type::DB_INT:
            return A.i<B.i;
        case db_item::type::DB_FLOAT:
            return A.d<B.d;
    }
}

void db_item::operator=(const db_item&A){
    T = A.T;
    switch (A.T) {
        case db_item::type::DB_CHAR:
            s = A.s;
            break;
        case db_item::type::DB_INT:
            i = A.i;
            break;
        case db_item::type::DB_FLOAT:
            d = A.d;
            break;
    }
}

void Record_Manager::insert(db_table& dbt,const std::vector<db_item>& items){
    int rec_size = dbt.record_size();
    char mem[rec_size];
    char* ptr = mem;
    if(dbt.columns.size()!=items.size())throw FormatException("Value number mismatched.\n");
    bool valid = true;
    memcpy(ptr, &valid, sizeof(valid));
    ptr += sizeof(valid);
    for (int i=0; i<items.size(); i++){
        if(dbt.columns[i].T==items[i].T){
            switch (items[i].T) {
                case db_item::type::DB_INT:
                    memcpy(ptr, &items[i].i, sizeof(items[i].i));
                    ptr += sizeof(items[i].i);
                    break;
                    
                case db_item::type::DB_FLOAT:
                    memcpy(ptr, &items[i].d, sizeof(items[i].d));
                    ptr += sizeof(items[i].d);
                    break;
                case db_item::type::DB_CHAR:
                    char s[dbt.columns[i].length];
                    memset(s, 0, sizeof(s));
                    strcpy(s, items[i].s.c_str());
                    memcpy(ptr, &s, sizeof(s));
                    ptr += sizeof(s);
                    break;
            }
        }else{
            throw FormatException("Value type mismatched.\n");
        }
    }
    
    db_buffer.write_buf((char*)mem, dbt.filename, (dbt.size)*rec_size, rec_size);
    dbt.size++;
}

pair<bool,std::vector<db_item>> Record_Manager::read(const db_table& dbt, int index){
    std::vector<db_item> items;
    int rec_size = dbt.record_size();
    char mem[rec_size];
    db_buffer.read_buf(mem, dbt.filename, index*rec_size, rec_size);
    char* ptr = mem;
    bool valid;
    memcpy(&valid, ptr, sizeof(bool));
    ptr += sizeof(valid);
    for(auto c:dbt.columns){
        db_item it;
        switch (c.T) {
            
            case db_item::type::DB_INT:
                int i;
                memcpy(&i, ptr, sizeof(i));
                ptr += sizeof(i);
                it = db_item(i);
                items.push_back(it);
                break;
                
            case db_item::type::DB_FLOAT:
                double d;
                memcpy(&d, ptr, sizeof(d));
                ptr += sizeof(d);
                it = db_item(d);
                items.push_back(it);
                break;
            case db_item::type::DB_CHAR:
                char s[c.length+1];
                memset(s, 0, sizeof(s));
                memcpy(s, ptr, sizeof(s)-1);
                ptr += sizeof(s)-1;
                it = db_item(s);
                items.push_back(it);
                break;
        }
    }
    return make_pair(valid, items);
}

void Record_Manager::write(db_table& dbt,const pair<bool,std::vector<db_item>>& record, int index){
    int rec_size = dbt.record_size();
    char mem[rec_size];
    char* ptr = mem;
    auto items = record.second;
    if(dbt.columns.size()!=items.size())throw FormatException("Value number mismatched.\n");
    bool valid = record.first;
    memcpy(ptr, &valid, sizeof(valid));
    ptr += sizeof(valid);
    for (int i=0; i<items.size(); i++){
        if(dbt.columns[i].T==items[i].T){
            switch (items[i].T) {
                case db_item::type::DB_INT:
                    memcpy(ptr, &items[i].i, sizeof(items[i].i));
                    ptr += sizeof(items[i].i);
                    break;
                    
                case db_item::type::DB_FLOAT:
                    memcpy(ptr, &items[i].d, sizeof(items[i].d));
                    ptr += sizeof(items[i].d);
                    break;
                case db_item::type::DB_CHAR:
                    char s[dbt.columns[i].length];
                    memset(s, 0, sizeof(s));
                    strcpy(s, items[i].s.c_str());
                    memcpy(ptr, &s, sizeof(s));
                    ptr += sizeof(s);
                    break;
            }
        }else{
            throw FormatException("Value type mismatched.\n");
        }
    }
    
    db_buffer.write_buf((char*)mem, dbt.filename, index*rec_size, rec_size);
    dbt.size++;
}

extern Catalog_Manager ctm;

void Record_Manager_unit_test::test(){
    db_table dbt =
    {
        {
            {db_item::type::DB_FLOAT,"SALARY",false,false},
            {db_item::type::DB_CHAR,"NAME",true,false,20},
            {db_item::type::DB_INT,"ID",true,true}
        },
        "teacher",0,"teacher.dat"
    }
    ;
    vector<db_item> items = {
        10.5,"Hello",1234
    };
    ctm.clear();
    ctm.insert(dbt);
    Record_Manager::insert(ctm.catalogs[0], items);
    Record_Manager::read(ctm.catalogs[0], 0);
}
