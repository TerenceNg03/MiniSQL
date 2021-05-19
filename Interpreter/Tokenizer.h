//
//  Token.h
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/8.
//

#ifndef Token_h
#define Token_h

#include <string>
#include <map>
#include <vector>

struct token{
    enum class TYPE{
        NONE,
        
        SELECT,DROP,INSERT,CREATE,DELETE,
        
        ON,FROM,WHERE,INTO,PRIMARY,KEY,VALUES,
        
        TABLE,INDEX,
        
        SYMBOL,
        
        INT,FLOAT,STRING,
        
        EQUAL,NOT,GREATER,LESS,GE,LE,AND,OR,UNIQUE,
        
        L_BRACKET,R_BRACKET,
                
        QUIT,EXECFILE,
        
        COMMA,END
    };
    
    struct lookup_template;

    
    static const std::map<token::TYPE,lookup_template> TABLE;
    
    std::string s;
    TYPE t;
    token(const std::string& s);
    token();
};

class tokenizer{
    const std::vector<char> OP_C = {'>','=','<'};
    enum STATE{NONE,OP,STRING,NAME};
public:
    std::vector<token> tokens;

    tokenizer(const std::string& s);
    
    void print();
    
};
#endif /* Token_h */
