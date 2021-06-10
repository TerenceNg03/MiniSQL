//
//  Buffer_Manager.hpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/21.
//

#ifndef Buffer_Manager_hpp
#define Buffer_Manager_hpp

#include "Record_Manager.hpp"
#include <vector>
#include <list>
#include <stdio.h>
#include <utility>
#include <map>
#include <memory>

class lru_cache{
    
    struct block{
        std::unique_ptr<char[]> content;
        bool pinned = false;
        block(int size);
    };
    const int _buf_size;
    const int _buf_cap;
    std::list<std::shared_ptr<block>> cache;
    std::map<std::pair<std::string, int>,std::shared_ptr<block>> hash_table;
    char* _seek_buf(const std::string& filename, int index);
public:
    lru_cache(int buf_size = 4096, int buf_cap = 512):_buf_size(buf_size),_buf_cap(buf_cap) {};
    
    void read_buf(char* __ptr, const std::string& filename, int index, int size);
    
    void write_buf(char* __ptr, const std::string& filename, int index, int size);
    
    
    void pin_buf(const std::string& filename, int index);
    
    void unpin_buf(const std::string& filename, int index);
    
    ~lru_cache();
    

};

namespace lru_cache_unit_test {
    void test();
}
#endif /* Buffer_Manager_hpp */
