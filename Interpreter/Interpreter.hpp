//
//  Interpreter.hpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/19.
//

#ifndef Interpreter_hpp
#define Interpreter_hpp
#include <cstdio>
typedef void* yyscan_t;
extern "C" void yyset_in  ( FILE * in_str , yyscan_t scanner );
extern "C" int yyparse(yyscan_t scanner);
extern "C" int yy_scan_string(char* str);
extern "C" int yylex_init ( yyscan_t * ptr_yy_globals ) ;
extern "C" int yylex_destroy ( yyscan_t yyscanner ) ;


#endif /* Interpreter_hpp */
