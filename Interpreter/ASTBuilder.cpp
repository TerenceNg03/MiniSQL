//
//  ASTBuilder.cpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/8.
//

#include "ASTBuilder.h"
#include "Tokenizer.h"
#include "Exception.h"
using namespace std;

statement* statement::create_statement(const std::vector<token>& tokens){
    if(tokens.size()==0){
        auto s = new statement();
        return s;
    }
    
    try{
        switch (tokens.at(0).t) {
            case token::TYPE::CREATE:
                if(tokens.at(1).t==token::TYPE::INDEX){
                    auto s = new c_index(tokens);
                    return s;
                }else if (tokens.at(1).t==token::TYPE::TABLE){
                    
                }else{
                    throw BadGrammmer(tokens.at(1));
                }
                break;
                
            default:
                throw BadGrammmer(tokens.at(0));
        }
    }
    catch(const std::out_of_range& ex){
        throw UnexpectedEOF();
    }
    
    auto s = new statement();
    return s;
}

std::vector<statement*> statement::build(const std::vector<token>& tokens){
    vector<statement*> statements;
    auto splits = split(tokens);
    for(auto i:splits){
        auto s = create_statement(i);
        statements.push_back(s);
    }
    return statements;
}

std::vector<std::vector<token>> statement::split(const std::vector<token>& tokens){
    vector<vector<token>> splits;
    vector<token> cur_sat;
    for(int i=0; i<tokens.size(); i++){
        if(tokens[i].t==token::TYPE::END){
            if(cur_sat.size()>0){
                splits.push_back(cur_sat);
                cur_sat.clear();
            }
        }else{
            cur_sat.push_back(tokens[i]);
        }
    }
    if(cur_sat.size()>0){
        throw IncompleteStatement();
    }
    return splits;
}

c_index::c_index(const std::vector<token>& ts){
    int it = 2;
    
    if (ts.at(it).t==token::TYPE::SYMBOL)index = ts.at(it);
    else throw BadGrammmer(ts.at(it));
    
    it++;
    
    if(!(ts.at(it).t==token::TYPE::ON))throw BadGrammmer(ts.at(it));
    
    it++;
    
    if (ts.at(it).t==token::TYPE::SYMBOL)table = ts.at(it);
    else throw BadGrammmer(ts.at(it));
    
    it++;
    
    if(!(ts.at(it).t==token::TYPE::L_BRACKET))throw BadGrammmer(ts.at(it));
    
    it++;
    
    if (ts.at(it).t==token::TYPE::SYMBOL)column = ts.at(it);
    else throw BadGrammmer(ts.at(it));
    
    it++;
    
    if(!(ts.at(it).t==token::TYPE::R_BRACKET))throw BadGrammmer(ts.at(it));
    it++;
    
    if(it<ts.size())throw BadGrammmer(ts.at(it));
    
}

