//
//  main.cpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/7.
//

#include "Buffer_Manager.hpp"
#include "Catalog_Manager.hpp"
#include "Record_Manager.hpp"
#include "Interpreter.hpp"
#include "B_Plus_Tree.hpp"
#include "y.tab.h"
using namespace std;

lru_cache db_buffer(60,1000);
Catalog_Manager ctm;
void db_driver(){
    yyscan_t scanner;
    yylex_init(&scanner);
    printf("\nMiniSQL shell by Adam Wu\n\nMiniSQL $ ");
    int i=1;
    while(i){
        try{
            filemode = 0;
            i = yyparse(scanner);
        }catch(const exception& ex){
            printf("%s",ex.what());
        }
    }
    yylex_destroy(scanner);
}

int main(int argc, const char * argv[]) {
//    catalog_unit_test::test();
    BPT_unit_test::test();
//    ctm.clear();
//    lru_cache_unit_test::test();
//    Record_Manager_unit_test::test();
    
    db_driver();
//    vector<db_item> items;
//    auto it = Record_Manager::read(ctm.catalogs[0],0);
    return 0;
}
