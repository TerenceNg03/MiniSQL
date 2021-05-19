//
//  Executer.cpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/14.
//

#include "Exception.h"
#include "Tokenizer.h"
#include "ASTBuilder.h"

bool statement::execute() {
    throw FormatException("None type statement is not allowed to execute");
    return true;
}

bool c_index::execute(){
    throw FormatException("Not support yet");
    return true;
}
