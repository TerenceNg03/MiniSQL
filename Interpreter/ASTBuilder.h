//
//  ASTBuilder.h
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/8.
//

#ifndef ASTBuilder_h
#define ASTBuilder_h

#include <vector>
#include <map>
#include "Tokenizer.h"

class statement{
    
public:
    enum class TYPE {
        NONE,
        CREATE_TABLE,
        CREATE_INDEX,
        DROP_TABLE,
        DROP_INDEX,
        SELECT,
        DELETE,
        QUIT,
        EXECFILE
    };
    
    const TYPE type = TYPE::NONE;
    
    statement(){};
    
    virtual bool execute();
    
    static statement* create_statement(const std::vector<token>& tokens);
    
    static std::vector<statement*> build(const std::vector<token>& tokens);
    
    static std::vector<std::vector<token>> split(const std::vector<token>& tokens);
    
};

class c_index : public statement{
    token index;
    token table;
    token column;
public:
    const TYPE type = TYPE::CREATE_INDEX;
    c_index(const std::vector<token>& tokens);
    bool execute();
};


#endif /* ASTBuilder_h */
