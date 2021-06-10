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

struct FormatException : public std::exception{
    std::string s;
    FormatException(const char* format,...);
    const char * what () const throw ();
};
#endif /* Exceptioin_h */
