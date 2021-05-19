//
//  Catalog_Manager.cpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/11.
//

#include "Catalog_Manager.hpp"

using namespace std;
string Table_def::dump(){
    string s;
    s+=name+' ';
    
    return s;
}
