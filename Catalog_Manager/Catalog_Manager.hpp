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

const char catalog_filename[] = {"catalog.cat"} ;

class Table_def{
public:
    enum TYPE{INT,FLOAT,CHAR};
    struct column{
        TYPE type;
        unsigned char length;
        std::string name;
    };
    std::vector<column> columns;
    std::string name;
    Table_def();
    std::string dump();
    void load(FILE*);
};


#endif /* Catalog_Manager_hpp */
