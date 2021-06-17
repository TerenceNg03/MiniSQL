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
    int start_block = index - index%_buf_size;
    char* ptr = _seek_buf(filename, start_block);
    ptr += index%_buf_size;
    if(start_block+_buf_size>=index+size){
        memcpy(__ptr, ptr, size);
    }else{
        int len = _buf_size-index%_buf_size;
        memcpy(__ptr, ptr, len);
        __ptr += len;
        start_block += _buf_size;
        while (index+size>start_block){
            len = min(index+size-start_block, _buf_size);
            ptr = _seek_buf(filename, start_block);
            memcpy(__ptr, ptr, len);
            __ptr += len;
            start_block += _buf_size;
        }
    }
//    int _ptr = index - index%_buf_size;
//    char* ptr = _seek_buf(filename, _ptr);
//    if(index+size<=_ptr+_buf_size){
//        int start = (index>_ptr)?index:_ptr;
//        int len = size;
//        ptr += start%_buf_size;
//        for(int i=0; i<len; i++){
//            *__ptr = *ptr;
//            __ptr++;
//            ptr++;
//        }
//        return;
//    }
//    while(index+size>_ptr+_buf_size){
//        int start = (index>_ptr)?index:_ptr;
//        int len = _buf_size - start%_buf_size;
//        ptr += start%_buf_size;
//        for(int i=0; i<len; i++){
//            *__ptr = *ptr;
//            __ptr++;
//            ptr++;
//        }
//        _ptr += _buf_size;
//        ptr = _seek_buf(filename, _ptr);
//    }
//    int len = index+size-_ptr;
//    for(int i=0; i<len; i++){
//        *__ptr = *ptr;
//        __ptr++;
//        ptr++;
//    }
    
}

void lru_cache::write_buf(char* __ptr, const std::string& filename, int index, int size){
    int start_block = index - index%_buf_size;
    char* ptr = _seek_buf(filename, start_block);
    ptr += index%_buf_size;
    if(start_block+_buf_size>=index+size){
        memcpy(ptr, __ptr, size);
    }else{
        int len = _buf_size-index%_buf_size;
        memcpy(ptr, __ptr, len);
        __ptr += len;
        start_block += _buf_size;
        while (index+size>start_block){
            len = min(index+size-start_block, _buf_size);
            ptr = _seek_buf(filename, start_block);
            memcpy(ptr, __ptr, len);
            __ptr += len;
            start_block += _buf_size;
        }
    }
//    if(index+size<=_ptr+_buf_size){
//        int start = (index>_ptr)?index:_ptr;
//        int len = size;
//        ptr += start%_buf_size;
//        memcpy(ptr, __ptr, len);
//        return;
//    }
//    while(index+size>_ptr+_buf_size){
//        int start = (index>_ptr)?index:_ptr;
//        int len = _buf_size - start%_buf_size;
//        ptr += start%_buf_size;
//        memcpy(ptr, __ptr, len);
//        __ptr += len;
//        _ptr += _buf_size;
//        ptr = _seek_buf(filename, _ptr);
//    }
//    int len = index+size-_ptr;
//    for(int i=0; i<len; i++){
//        *ptr = *__ptr;
//        __ptr++;
//        ptr++;
//    }
    
}

char* lru_cache::_seek_buf(const string& filename, int index){
    pair<string, int> tag = make_pair(filename, index);
    auto it = find_if(cache.begin(), cache.end(), [&](auto const& i)->bool{return i.first==tag;});

    if(it==cache.end()){
        if(cache.size()>=_buf_cap){
            auto lru = prev(cache.end());
            while (lru!=cache.begin()&&(lru->second.pinned)) {
                lru= prev(lru);
            }
            if(lru->second.pinned)throw FormatException("Buffer overflow\n");
            
            FILE* fp = fopen(lru->first.first.c_str(), "r+");
            if(fp!=NULL){
                fseek(fp, lru->first.second, SEEK_SET);
                fwrite(lru->second.content.get(), sizeof(char), _buf_size, fp);
                fclose(fp);
            }
            cache.erase(lru);
            
        }
        block b_ptr(_buf_size);
        //(char*)malloc(_buf_size*sizeof(char));
        FILE* fp = fopen(filename.c_str(), "r");
        if(fp==NULL)throw FormatException("Faild to open file : %s",filename.c_str());
        fseek(fp, index, SEEK_SET);
        fread(b_ptr.content.get(), sizeof(char), _buf_size, fp);
        fclose(fp);
        
        cache.push_front(make_pair(tag,move(b_ptr)));
        return cache.begin()->second.content.get();
    }else{
        cache.splice(cache.begin(), cache,it);
        return cache.begin()->second.content.get();
    }
    
};

lru_cache::~lru_cache(){
    auto it = cache.begin();
    for(int i=0; i<cache.size(); i++){
        FILE* fp = fopen(it->first.first.c_str(), "r+");
        if(fp==NULL)continue;
        fseek(fp, it->first.second, SEEK_SET);
        fwrite(it->second.content.get(), sizeof(char), _buf_size, fp);
        fclose(fp);
        advance(it, 1);
    }
//    hash_table.clear();
//    cache.clear();
}

void lru_cache::pin_buf(const std::string& filename, int index){
    auto tag = make_pair(filename, index);
    auto it = find_if(cache.begin(), cache.end(), [=](auto const& i)->bool{return i.first==tag;});
    if(it!=cache.end())it->second.pinned = true;
}

void lru_cache::unpin_buf(const std::string& filename, int index){
    auto tag = make_pair(filename, index);
    auto it = find_if(cache.begin(), cache.end(), [=](auto const& i)->bool{return i.first==tag;});
    if(it!=cache.end())it->second.pinned = false;
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
    char str[] = "123456789";
    lch.write_buf(str, filename, 7, 9);
    memset(s, 0, sizeof(s));
    lch.read_buf(s, filename, 0, 19);
    printf("%s\n",s);
}
