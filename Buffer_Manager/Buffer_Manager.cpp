//
//  Buffer_Manager.cpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/21.
//

#include "Buffer_Manager.hpp"
#include "Exception.hpp"
#include <memory>
using namespace std;

lru_cache::block::block(int size){
    content = make_unique<char[]>(size);
    pinned = false;
}

void lru_cache::read_buf(char* __ptr, const std::string& filename, int index, int size){
    int _ptr = index - index%_buf_size;
    char* ptr = _seek_buf(filename, _ptr);
    if(index+size<=_ptr+_buf_size){
        int start = (index>_ptr)?index:_ptr;
        int len = size;
        ptr += start%_buf_size;
        for(int i=0; i<len; i++){
            *__ptr = *ptr;
            __ptr++;
            ptr++;
        }
        return;
    }
    while(index+size>_ptr+_buf_size){
        int start = (index>_ptr)?index:_ptr;
        int len = _buf_size - start%_buf_size;
        ptr += start%_buf_size;
        for(int i=0; i<len; i++){
            *__ptr = *ptr;
            __ptr++;
            ptr++;
        }
        _ptr += _buf_size;
        ptr = _seek_buf(filename, _ptr);
    }
    int len = index+size-_ptr;
    for(int i=0; i<len; i++){
        *__ptr = *ptr;
        __ptr++;
        ptr++;
    }
    
}

void lru_cache::write_buf(char* __ptr, const std::string& filename, int index, int size){
    int _ptr = index - index%_buf_size;
    char* ptr = _seek_buf(filename, _ptr);
    if(index+size<=_ptr+_buf_size){
        int start = (index>_ptr)?index:_ptr;
        int len = size;
        ptr += start%_buf_size;
        for(int i=0; i<len; i++){
            *ptr = *__ptr;
            __ptr++;
            ptr++;
        }
    }
    while(index+size>_ptr+_buf_size){
        int start = (index>_ptr)?index:_ptr;
        int len = _buf_size - start%_buf_size;
        ptr += start%_buf_size;
        for(int i=0; i<len; i++){
            *ptr = *__ptr;
            __ptr++;
            ptr++;
        }
        _ptr += _buf_size;
        ptr = _seek_buf(filename, _ptr);
    }
    int len = index+size-_ptr;
    for(int i=0; i<len; i++){
        *ptr = *__ptr;
        __ptr++;
        ptr++;
    }
    
}

char* lru_cache::_seek_buf(const string& filename, int index){
    auto it = hash_table.find(make_pair(filename, index));

    if(it==hash_table.end()){
        if(cache.size()>=_buf_cap){
            auto lru = prev(cache.end());
            while (lru!=cache.begin()&&(lru->get()->pinned)) {
                lru= prev(lru);
            }
            if(lru->get()->pinned)throw FormatException("Buffer overflow\n");
            for(auto i = hash_table.begin(); i!=hash_table.end();i++){
                if(i->second==*lru){
                    FILE* fp = fopen(i->first.first.c_str(), "r+");
                    if(fp!=NULL){
                        fseek(fp, i->first.second, SEEK_SET);
                        fwrite(i->second->content.get(), sizeof(char), _buf_size, fp);
                        fclose(fp);
                    }
                    cache.erase(lru);
                    hash_table.erase(i);
                    break;
                }
            }
            
        }
        shared_ptr<block> b_ptr = shared_ptr<block>(new block(_buf_size));
        //(char*)malloc(_buf_size*sizeof(char));
        FILE* fp = fopen(filename.c_str(), "r");
        if(fp==NULL)throw FormatException("Faild to open file : %s",filename.c_str());
        fseek(fp, index, SEEK_SET);
        fread(b_ptr->content.get(), sizeof(char), _buf_size, fp);
        fclose(fp);
        
        cache.push_front(b_ptr);
        hash_table.insert(make_pair(make_pair(filename, index), b_ptr));
        return b_ptr->content.get();
    }else{
        return it->second->content.get();
    }
    
};

lru_cache::~lru_cache(){
    for(auto it:hash_table){
        FILE* fp = fopen(it.first.first.c_str(), "r+");
        if(fp==NULL)continue;
        fseek(fp, it.first.second, SEEK_SET);
        fwrite(it.second->content.get(), sizeof(char), _buf_size, fp);
        fclose(fp);
    }
    hash_table.clear();
    cache.clear();
}

void lru_cache::pin_buf(const std::string& filename, int index){
    auto it = hash_table.find(make_pair(filename, index));
    it->second->pinned = true;
}

void lru_cache::unpin_buf(const std::string& filename, int index){
    auto it = hash_table.find(make_pair(filename, index));
    it->second->pinned = false;
}

void lru_cache_unit_test::test(){
    lru_cache lch(3,2);
    string filename = "lru_test.txt";
    FILE* fp = fopen(filename.c_str(), "w");
    fprintf(fp, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    fclose(fp);
    char s[20] = {0};
    lch.read_buf(s, filename, 5, 13);
    printf("%s\n",s);
    lch.write_buf("123456789", filename, 7, 9);
    memset(s, 0, sizeof(s));
    lch.read_buf(s, filename, 0, 19);
    printf("%s\n",s);
}
