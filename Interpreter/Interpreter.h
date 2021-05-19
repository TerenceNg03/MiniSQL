//
//  Interpreter.h
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/8.
//

#ifndef Interpreter_h
#define Interpreter_h

#include "Tokenizer.h"
#include "ASTBuilder.h"
class Interpreter{
        
public:
    Interpreter(const std::string& source);
    
    void execute(const std::string& s);
};



#endif /* Interpreter_h */
