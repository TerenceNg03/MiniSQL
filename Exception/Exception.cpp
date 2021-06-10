//
//  Exception.cpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/8.
//
#include "Exception.hpp"

#include <stdio.h>
#include <string>
using namespace std;

FormatException::FormatException(const char* format,...){
    char str[100];
    memset(str, 0, sizeof(str));
    va_list args;
    va_start(args, format);
    vsprintf(str, format, args);
    va_end(args);
    s = string(str);
}
const char * FormatException::what () const throw (){
    return s.c_str();
}
