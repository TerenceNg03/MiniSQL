//
//  Record_Manager.hpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/18.
//

#ifndef Record_Manager_hpp
#define Record_Manager_hpp

#include <vector>
#include <cstring>
#include <string>

struct db_item{
    enum class type{DB_CHAR,DB_INT,DB_FLOAT};
    
    std::string s="";
    int i=0;
    double d=0;
    
    type T = type::DB_INT;
    
    db_item(const std::string& s);
    db_item(const char* s);
    db_item(int i);
    db_item(double d);
    db_item(){};
        
    void operator =(const db_item& A);
    
};

struct db_table{
    struct column{
        db_item::type T;
        std::string name;
        bool IsUnique;
        bool IsPrimary;
        unsigned char length = 0;
    };
    std::vector<column> columns;
    std::vector<std::pair<std::string,std::string>> index;
    std::string name;
    int size = 0;
    std::string filename;
    int record_size() const;
};
   

bool operator ==(const db_item& A, const db_item& B);

bool operator <(const db_item& A, const db_item& B);

namespace Record_Manager{

void insert(db_table& dbt,const std::vector<db_item>& items);
std::pair<bool,std::vector<db_item>> read(const db_table& dbt, int index);
void write(db_table& dbt,const std::pair<bool,std::vector<db_item>>& record, int index);

};

namespace Record_Manager_unit_test {
void test();
}
#endif /* Record_Manager_hpp */
