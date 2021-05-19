//
//  Record_Manager.hpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/18.
//

#ifndef Record_Manager_hpp
#define Record_Manager_hpp

#include <stdio.h>

class db_object{
protected:
    db_object();
public:
    //virtual char* dump();
    //virtual db_object* load(char* s);
};

class db_int : public db_object{
    int i;
    db_int(int in):i(in){};
};

class db_float : public db_object{
    float i;
    db_float(float in):i(in){};
};

class db_char : public db_object{
    
};


#endif /* Record_Manager_hpp */
