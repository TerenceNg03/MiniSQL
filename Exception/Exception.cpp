//
//  Exception.cpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/8.
//
#include "Exception.h"

#include <stdio.h>
#include <string>
using namespace std;

BadCharacter::BadCharacter(char c,const string& s = ""){
    this->c = c;
    this->s = s;
}
    
const char * BadCharacter::what () const throw (){
    char* str = new char[100];
    sprintf(str, "Unexpected character '%c' near '%s'",c,s.c_str());
    return str;
}

BadToken::BadToken(const string& s){
    this->s = s;
}
    
const char * BadToken::what () const throw (){
    char* str = new char[100];
    sprintf(str, "Unexpected token '%s'",s.c_str());
    return str;
}

const char * IncompleteStatement::what () const throw (){
    return "Expect ';' at the end of statement";
}

BadGrammmer::BadGrammmer(const token& t){
    this->s = t.s;
};

const char * BadGrammmer::what () const throw (){
    char* str = new char[100];
    sprintf(str, "%s is not allowed here",s.c_str());
    return str;
}

const char * UnexpectedEOF::what () const throw (){
    return "Statement ends unexpectedly";
}

FormatException::FormatException(const char* format,...){
    char str[100];
    va_list args;
    sprintf(str, format, args);
    s = string(str);
}
const char * FormatException::what () const throw (){
    return s.c_str();
}
