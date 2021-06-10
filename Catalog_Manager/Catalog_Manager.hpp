//
//  Catalog_Manager.hpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/11.
//

#ifndef Catalog_Manager_hpp
#define Catalog_Manager_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "Buffer_Manager.hpp"
#include "Record_Manager.hpp"


struct Catalog_Manager{
    
    std::vector<db_table> catalogs;
    const static std::string catalog_filename;
    void load_catalog();

    void dump_catalog();
    
    Catalog_Manager();
    ~Catalog_Manager();
    
    void insert(const db_table& tab);
    void erase(const std::string& name);
    void clear();
    
};


namespace catalog_unit_test {
void test();
}

#endif /* Catalog_Manager_hpp */
