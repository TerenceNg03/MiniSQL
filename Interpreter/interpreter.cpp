//
//  interpreter.cpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/7.
//

#include <vector>
#include <iostream>
#include <string>
#include "Exception.h"
#include "Tokenizer.h"
#include "Interpreter.h"
#include "ASTBuilder.h"
using namespace std;

string upper(const string& s){
    string str;
    for (auto c:s){
        str += toupper(c);
    }
    return str;
}

Interpreter::Interpreter(const string& source){
    execute(source);
}

void Interpreter::execute(const string& s){
    tokenizer* t;
    try{
        t = new tokenizer(upper(s));
        //t->print();
    }catch(const exception& ex){
        printf("Error : %s\n",ex.what());
        return;
    }
    
    vector<statement*> stat;
    try{
        stat = statement::build(t->tokens);
    }catch(const exception& ex){
        printf("Error : %s\n",ex.what());
        return;
    }
    
    for (auto sat : stat){
        sat->execute();
    }
    
}
