//
//  B_Plus_Tree.cpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/14.
//

#include "B_Plus_Tree.hpp"
#include <stdlib.h>
#include "Exception.hpp"
#include "Buffer_Manager.hpp"

using namespace std;

/*
 
 BP_tree file struct
 
 block 0:
    int size
    type key_t
    int key_l
 block 1:
    Root
 block 2-n:
    Nodes
 
 */

extern lru_cache db_buffer;

unique_ptr<B_Plus_tree> B_Plus_tree::load(const std::string& filename){
    unique_ptr<B_Plus_tree> bpt;
    bpt->filename = filename;
    unique_ptr<char[]> s = make_unique<char[]>(db_buffer._buf_size);
    db_buffer.read_buf(s.get(), filename, 0, db_buffer._buf_size);
    char* _ptr = s.get();
    memcpy(&bpt->size, _ptr, sizeof(bpt->size));
    _ptr += sizeof(bpt->size);
    memcpy(&bpt->key_t, _ptr, sizeof(bpt->key_t));
    _ptr += sizeof(bpt->key_t);
    memcpy(&bpt->key_l, _ptr, sizeof(bpt->key_l));
    _ptr += sizeof(bpt->key_l);
    return bpt;
}


unique_ptr<B_Plus_tree> B_Plus_tree::create(const std::string& filename, db_item::type T, int key_length){
    unique_ptr<B_Plus_tree> bpt;
    bpt->size = 0;
    bpt->key_t = T;
    bpt->key_l = key_length;
    
    unique_ptr<char[]> s = make_unique<char[]>(db_buffer._buf_size);
    char* _ptr = s.get();
    memcpy(_ptr, &bpt->size, sizeof(bpt->size));
    memcpy(_ptr, &bpt->key_t,sizeof(bpt->key_t));
    _ptr += sizeof(bpt->key_t);
    memcpy(_ptr, &bpt->key_t,sizeof(bpt->key_l));
    _ptr += sizeof(bpt->key_l);
    
    return bpt;
}
