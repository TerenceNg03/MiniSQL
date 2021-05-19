//
//  Exceptioin.h
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/8.
//

#ifndef Exceptioin_h
#define Exceptioin_h

#include <string>
#include <exception>
#include "Tokenizer.h"

struct BadCharacter : public std::exception{
    char c;
    std::string s;
    BadCharacter(char c,const std::string& s);
    const char * what () const throw ();
};

struct BadToken : public std::exception{
    std::string s;
    BadToken(const std::string& s);
    
    const char * what () const throw ();
};

struct IncompleteStatement : public std::exception{
    const char * what () const throw ();
};

struct BadGrammmer : public std::exception{
    std::string s;
    BadGrammmer(const token& t);
    const char * what () const throw ();
};

struct UnexpectedEOF : public std::exception{
    const char * what () const throw ();
};

struct FormatException : public std::exception{
    std::string s;
    FormatException(const char* format,...);
    const char * what () const throw ();
};
#endif /* Exceptioin_h */
