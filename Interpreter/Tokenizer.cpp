//
//  TOKEN_TABLE.cpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/8.
//

#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <regex>
#include <vector>
#include <map>
#include "Exception.h"
#include "Tokenizer.h"
using namespace std;
    
struct token::lookup_template{
    //TYPE t;
    regex r;
    string name;
    lookup_template(const string& s,const string& name){
        //this->t = t;
        this->r = regex(s);
        this->name = name;
    }
};

token::token(const string& s){
    for (auto ty : TABLE){
        if (regex_match(s,ty.second.r)){
            this->t = ty.first;
            this->s = s;
            return;
        }
    }
    
    throw BadToken(s);
}

token::token(){
    s = "";
    t = TYPE::NONE;
}


const map<token::TYPE,token::lookup_template> token::TABLE = {
    {token::TYPE::SELECT,{"^SELECT$","SELECT"}},
    {token::TYPE::DROP,{"^DROP$","DROP"}},
    {token::TYPE::INSERT,{"^INSERT$","INSERT"}},
    {token::TYPE::CREATE,{"^CREATE$","CREATE"}},
    {token::TYPE::DELETE,{"^DELETE$","DELETE"}},
    
    {token::TYPE::ON,{"^ON$","ON"}},
    {token::TYPE::FROM,{"^FROM$","FROM"}},
    {token::TYPE::WHERE,{"^WHERE$","WHERE"}},
    {token::TYPE::INTO,{"^INTO$","INTO"}},
    {token::TYPE::PRIMARY,{"^PRIMARY$","PRIMARY"}},
    {token::TYPE::KEY,{"^KEY$","KEY"}},
    {token::TYPE::VALUES,{"^VALUES$","VALUES"}},

    {token::TYPE::TABLE,{"^TABLE$","TABLE"}},
    {token::TYPE::INDEX,{"^INDEX$","INDEX"}},

    {token::TYPE::SYMBOL,{"^([A-Za-z_]{1,}[_A-Za-z0-9\\.]*|\\*)$","SYMBOL"}},
    
    {token::TYPE::INT,{"^-{0,1}[0-9]*$","INT"}},
    {token::TYPE::FLOAT,{"^-{0,1}[0-9]{1,}\\.[0-9]{1,}$","FLOAT"}},
    {token::TYPE::STRING,{"^'.*'$","STRING"}},

    {token::TYPE::EQUAL,{"^=$","EQUAL"}},
    {token::TYPE::NOT,{"^<>$","NOT"}},
    {token::TYPE::GREATER,{"^>$","GREATER"}},
    {token::TYPE::LESS,{"^<$","LESS"}},
    {token::TYPE::GE,{"^>=$","GREATER OR EQUAL"}},
    {token::TYPE::LE,{"^<=$","LE"}},
    {token::TYPE::AND,{"^AND$","AND"}},
    {token::TYPE::OR,{"^OR$","OR"}},
    {token::TYPE::UNIQUE,{"^UNIQUE$","UNIQUE"}},

    {token::TYPE::L_BRACKET,{"^\\($","L BRACKET"}},
    {token::TYPE::R_BRACKET,{"^\\)$","R_BRACKET"}},
        
    {token::TYPE::QUIT,{"^QUIT$","QUIT"}},
    {token::TYPE::EXECFILE,{"^EXECFILE$","EXECFILE"}},
    
    {token::TYPE::COMMA,{"^,$","COMMA"}},
    {token::TYPE::END,{"^;$","END"}}
};

tokenizer::tokenizer(const string& s){
    vector<string> tokenized;
    string cur_token;
    STATE state = NONE;
    
    for (int i=0; i<s.size(); i++){
        char c = s[i];
        if(c=='\''){
            if (state == STRING){
                cur_token += c;
                tokenized.push_back(cur_token);
                cur_token = "";
                state = NONE;
            }else{
                tokenized.push_back(cur_token);
                cur_token = "";
                cur_token += c;
                state = STRING;
            }
        }else if(state == STRING){
            cur_token += c;
        }else if(c==','){
            tokenized.push_back(cur_token);
            cur_token = "";
            cur_token += c;
            tokenized.push_back(cur_token);
            cur_token = "";
            state = NONE;
        }else if(c=='('||c==')'){
            tokenized.push_back(cur_token);
            cur_token = "";
            cur_token += c;
            tokenized.push_back(cur_token);
            cur_token = "";
            state = NONE;
        }else if(isalnum(c)||c=='.'||c=='*'||c=='_'){
            if (state==NAME){
                cur_token += c;
            }else{
                tokenized.push_back(cur_token);
                cur_token = "";
                cur_token += c;
                state = NAME;
            }
        }else if(find(OP_C.begin(), OP_C.end(), c)!=OP_C.end()){
            if(state == OP){
                cur_token += c;
            }else{
                tokenized.push_back(cur_token);
                cur_token = "";
                cur_token += c;
                state = OP;
            }
        }else if(isspace(c)){
            tokenized.push_back(cur_token);
            cur_token = "";
            state = NONE;
        }else if(c==';'){
            tokenized.push_back(cur_token);
            cur_token = "";
            cur_token += c;
            tokenized.push_back(cur_token);
            cur_token = "";
            state = NONE;
        }else{
            throw BadCharacter(c,s.substr((i-5)>=0?(i-5):0,10));
        }
            
    }
    
    //Remove empty string
    tokenized.push_back(cur_token);
    for (int i = 0; i < tokenized.size(); ) {
        if (tokenized[i].size() == 0) {
            tokenized.erase(tokenized.begin() + i);
        } else ++i;
    }
    
    //tokenize
    for(auto i:tokenized){
        tokens.push_back(token(i));
    }
    
};
    
void tokenizer::print(){
    for (auto t:tokens){
        printf("%s %s\n",token::TABLE.at(t.t).name.c_str(),t.s.c_str());
    }
}
    
#endif
