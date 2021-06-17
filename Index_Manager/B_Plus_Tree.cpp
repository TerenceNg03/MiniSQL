//
//  B_Plus_Tree.cpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/14.
//

#include "B_Plus_Tree.hpp"
#include <stdlib.h>
#include <math.h>
#include "Exception.hpp"
#include "Buffer_Manager.hpp"

using namespace std;

/*
 
 BP_tree file structure
 
 block 0:
    int root
    int size
    type key_t
    int key_l
 block 1:
    Root
 block 2-n:
    Nodes
 
 */

/*
 
 Node structure
 
 bool Isleaf
 int parent
 int size
 keys
 
 leaf:
 [
 int f_ptrs
 ]
 
 non-leaf:
 [
 childs
 ]
 */

extern lru_cache db_buffer;

B_Plus_tree::Node::Node(int pos, const B_Plus_tree* bp_tree){
    this->pos = pos;
    this->bpt = bp_tree;
    unique_ptr<char[]> s = make_unique<char[]>(db_buffer._buf_size);
    db_buffer.read_buf(s.get(), bp_tree->filename, pos*db_buffer._buf_size, db_buffer._buf_size);
    char* _ptr = s.get();
    memcpy(&isleaf, _ptr, sizeof(isleaf));
    _ptr += sizeof(isleaf);
    memcpy(&parent, _ptr, sizeof(parent));
    _ptr += sizeof(parent);
    
    int size;
    memcpy(&size, _ptr, sizeof(size));
    _ptr += sizeof(size);
    for(int i=0; i<size; i++){
        int I;double D;char S[256];
        bool valid;
        memcpy(&valid, _ptr, sizeof(valid));
        _ptr += sizeof(valid);
        switch (bpt->key_t) {
            case db_item::type::DB_CHAR:
                memcpy(S, _ptr, bpt->key_l);
                _ptr += bpt->key_l;
                keys.push_back(make_pair(valid,string(S)));
                break;
            case db_item::type::DB_INT:
                memcpy(&I, _ptr, sizeof(I));
                _ptr += sizeof(I);
                keys.push_back(make_pair(valid, I));
                break;
                
            case db_item::type::DB_FLOAT:
                memcpy(&D, _ptr, sizeof(D));
                _ptr += sizeof(D);
                keys.push_back(make_pair(valid, D));
                break;
        }
    }
    
    if(isleaf){
        int f_ptr;
        for(int i=0; i<size; i++){
            memcpy(&f_ptr, _ptr, sizeof(f_ptr));
            _ptr += sizeof(f_ptr);
            f_ptrs.push_back(f_ptr);
        }
    }else{
        int child;
        for(int i=0; i<size+1; i++){
            memcpy(&child, _ptr, sizeof(child));
            _ptr += sizeof(child);
            children.push_back(child);
        }
    }
}

std::unique_ptr<B_Plus_tree::Node> B_Plus_tree::Node::create(int parent, int pos, const B_Plus_tree* bp_tree){
    unique_ptr<Node> n_ptr = make_unique<Node>();
    n_ptr->pos = pos;
    n_ptr->parent = parent;
    n_ptr->bpt = bp_tree;
    n_ptr->isleaf = true;
    return n_ptr;
}

B_Plus_tree::Node::~Node(){
    unique_ptr<char[]> s = make_unique<char[]>(db_buffer._buf_size);
    char* _ptr = s.get();
    memcpy(_ptr, &isleaf, sizeof(isleaf));
    _ptr += sizeof(isleaf);
    memcpy(_ptr, &parent, sizeof(size));
    _ptr += sizeof(parent);
    int size = (int)keys.size();
    memcpy(_ptr, &size, sizeof(size));
    _ptr += sizeof(size);
    for(auto it:keys){
        memcpy(_ptr, &it.first, sizeof(it.first));
        _ptr += sizeof(it.first);
        switch (bpt->key_t) {
            case db_item::type::DB_CHAR:
                memcpy(_ptr, it.second.s.c_str(), bpt->key_l);
                _ptr += bpt->key_l;
                break;
            case db_item::type::DB_INT:
                memcpy(_ptr, &it.second.i, sizeof(it.second.i));
                _ptr += sizeof(it.second.i);
                break;
                
            case db_item::type::DB_FLOAT:
                memcpy(_ptr, &it.second.d, sizeof(it.second.d));
                _ptr += sizeof(it.second.d);
                break;
        }
    }
    
    if(isleaf){
        for(int i:f_ptrs){
            memcpy(_ptr, &i, sizeof(i));
            _ptr += sizeof(i);
        }
    }else{
        for(int i:children){
            memcpy(_ptr, &i, sizeof(i));
            _ptr += sizeof(i);
        }
    }
    db_buffer.write_buf(s.get(), bpt->filename, db_buffer._buf_size*pos, db_buffer._buf_size);
}

int B_Plus_tree::Node::capability(const B_Plus_tree& bp_tree){
    int total = db_buffer._buf_size;
    total -= sizeof(int);
    total -= sizeof(bool);
    
    int key_length = 0;
    switch (bp_tree.key_t) {
        case db_item::type::DB_CHAR:
            key_length = bp_tree.key_l;
            break;
        case db_item::type::DB_INT:
            key_length = sizeof(int);
            break;
            
        case db_item::type::DB_FLOAT:
            key_length = sizeof(double);
            break;
    }
    int ret = total/(sizeof(int)+key_length)-2;
    if(ret<=2) throw FormatException("Buffer too small for b+ tree\n");
    return ret;
}


B_Plus_tree::B_Plus_tree(const std::string& filename){
    this->filename = filename;
    unique_ptr<char[]> s = make_unique<char[]>(db_buffer._buf_size);
    db_buffer.read_buf(s.get(), filename, 0, db_buffer._buf_size);
    char* _ptr = s.get();
    memcpy(&root, _ptr, sizeof(root));
    _ptr += sizeof(root);
    memcpy(&size, _ptr, sizeof(size));
    _ptr += sizeof(size);
    memcpy(&key_t, _ptr, sizeof(key_t));
    _ptr += sizeof(key_t);
    memcpy(&key_l, _ptr, sizeof(key_l));
    _ptr += sizeof(key_l);
}


B_Plus_tree::B_Plus_tree(const std::string& filename, db_item::type T, int key_length){
    size = 0;
    key_t = T;
    key_l = key_length;
    root = -1;
    this->filename = filename;
    FILE* fp = fopen(filename.c_str(), "w");
    fclose(fp);
    unique_ptr<char[]> s = make_unique<char[]>(db_buffer._buf_size);
    char* _ptr = s.get();
    memcpy(_ptr, &root, sizeof(root));
    _ptr += sizeof(root);
    memcpy(_ptr, &size, sizeof(size));
    _ptr += sizeof(size);
    memcpy(_ptr, &key_t,sizeof(key_t));
    _ptr += sizeof(key_t);
    memcpy(_ptr, &key_t,sizeof(key_l));
    _ptr += sizeof(key_l);
    
    db_buffer.write_buf(s.get(), filename, 0, db_buffer._buf_size);
}

B_Plus_tree::~B_Plus_tree(){
    unique_ptr<char[]> s = make_unique<char[]>(db_buffer._buf_size);
    char* _ptr = s.get();
    memcpy(_ptr, &root, sizeof(root));
    _ptr += sizeof(root);
    memcpy(_ptr, &size, sizeof(size));
    _ptr += sizeof(size);
    memcpy(_ptr, &key_t,sizeof(key_t));
    _ptr += sizeof(key_t);
    memcpy(_ptr, &key_t,sizeof(key_l));
    _ptr += sizeof(key_l);
    
    db_buffer.write_buf(s.get(), filename, 0, db_buffer._buf_size);
}

unique_ptr<B_Plus_tree::Node> B_Plus_tree::_search(db_item key){
    unique_ptr<B_Plus_tree::Node> n_ptr;
    
    if(size==0)return n_ptr;
    
    n_ptr = make_unique<B_Plus_tree::Node>(root,this);
    while(!n_ptr->isleaf){
    
        int i;
        for(i=0;i<n_ptr->keys.size();i++){
            if(key<n_ptr->keys[i].second)break;
        }
        
        n_ptr = make_unique<Node>(n_ptr->children.at(i), this) ;
    }
    
    return n_ptr;
}


void B_Plus_tree::insert(db_item it,int f_ptr){
    unique_ptr<Node> tar = _search(it);
    if(it.T!=this->key_t)throw FormatException("Internal error : failed index type check\n");
    if(tar==nullptr){
        unique_ptr<Node> root = Node::create(-1,1,this);
        this->size++;
        root->isleaf = true;
        root->keys.push_back(make_pair(true, it));
        root->f_ptrs.push_back(f_ptr);
        this->root = root->pos;
        return;
    }
    
    for(auto k:tar->keys){
        if(k.second==it){
            if(!k.first)k.first=true;
            else throw FormatException("Duplicated item find while update index\n");
            return;
        }
    }
    
    bool found = false;
    for(int i=0; i<tar->keys.size(); i++){
        if(it<tar->keys.at(i).second){
            tar->keys.insert(tar->keys.begin()+i,make_pair(true, it));
            tar->f_ptrs.insert(tar->f_ptrs.begin()+i,f_ptr);
            found = true;
            break;
        }
    }
    if(!found){
        tar->keys.push_back(make_pair(true, it));
        tar->f_ptrs.push_back(f_ptr);
    }
    
    if(tar->keys.size()>Node::capability(*this)){
        _split_node(move(tar));
    }
    
}

void B_Plus_tree::_split_node(std::unique_ptr<Node> node){
    
    unique_ptr<Node> new_node = Node::create(node->parent,this->size+1, this);
    this->size++;
    
    int split = Node::capability(*this)/2;
    
    if(node->isleaf){
        new_node->keys = vector<pair<bool,db_item>>(node->keys.begin()+split,node->keys.end());
        new_node->f_ptrs = vector<int>(node->f_ptrs.begin()+split,node->f_ptrs.end());
        node->keys = vector<pair<bool,db_item>>(node->keys.begin(),node->keys.begin()+split);
        node->f_ptrs = vector<int>(node->f_ptrs.begin(),node->f_ptrs.begin()+split);
        new_node->isleaf = true;
        if(node->parent<=0){
            unique_ptr<Node> new_root = Node::create(-1, this->size+1, this);
            this->size++;
            new_root->isleaf = false;
            new_root->children.push_back(node->pos);
            new_root->children.push_back(new_node->pos);
            new_root->keys.push_back(new_node->keys.at(0));
            root = new_root->pos;
            new_node->parent = root;
            node->parent = root;
        }else{
            unique_ptr<Node> p = make_unique<Node>(node->parent,this);
            db_item new_key = new_node->keys.at(0).second;
            int i=0;
            for(i=0; i<p->keys.size(); i++){
                if(new_key<p->keys[i].second)break;
            }
            p->keys.insert(p->keys.begin()+i ,make_pair(true, new_key));
            p->children.insert(p->children.begin()+i+1, new_node->pos);
            new_node->parent = p->pos;
            if(p->keys.size()>=Node::capability(*this))_split_node(move(p));
        }
    }else{
        new_node->isleaf = false;
        db_item p_value = node->keys[split].second;
        new_node->keys = vector<pair<bool,db_item>>(node->keys.begin()+split+1,node->keys.end());
        new_node->children = vector<int>(node->children.begin()+split+1,node->children.end());
        node->keys = vector<pair<bool,db_item>>(node->keys.begin(),node->keys.begin()+split);
        node->children = vector<int>(node->children.begin(),node->children.begin()+split+1);
        
        for(int i:new_node->children){
            unique_ptr<Node> c = make_unique<Node>(i,this);
            c->parent = new_node->pos;
        }
        
        if(node->parent<=0){
            unique_ptr<Node> new_root = Node::create(-1, this->size+1, this);
            this->size++;
            new_root->children.push_back(node->pos);
            new_root->children.push_back(new_node->pos);
            new_root->keys.push_back(new_node->keys.at(0));
            new_root->isleaf = false;
            root = new_root->pos;
            new_node->parent = root;
            node->parent = root;
        }else{
            unique_ptr<Node> p = make_unique<Node>(node->parent,this);
            db_item new_key = new_node->keys.at(0).second;
            int i=0;
            for(i=0; i<p->keys.size(); i++){
                if(new_key<p->keys[i].second)break;
            }
            p->keys.insert(p->keys.begin()+i ,make_pair(true, new_key));
            p->children.insert(p->children.begin()+i+1, new_node->pos);
            new_node->parent = p->pos;
            if(p->keys.size()>=Node::capability(*this))_split_node(move(p));
        }
        
    }
}

void B_Plus_tree::erase(db_item it){
    auto n_ptr = _search(it);
    if(n_ptr==nullptr)return;
    for(int i=0; i<n_ptr->keys.size(); i++){
        if(n_ptr->keys[i].first&&(n_ptr->keys[i].second==it)){
            n_ptr->keys[i].first = false;
        }
    }
}

int B_Plus_tree::search(db_item it){
    auto n_ptr = _search(it);
    for(int i=0; i<n_ptr->keys.size(); i++){
        if(n_ptr->keys[i].first&&(n_ptr->keys[i].second==it)){
            return n_ptr->f_ptrs[i];
        }
    }
    return -1;
}


void dfs(unique_ptr<B_Plus_tree::Node> node,int depth,const B_Plus_tree* bpt){
    if(node->isleaf){
            for(int i=0; i<depth; i++){
                putchar('\t');
            }
            for (int i=0; i<node->keys.size();i++){
                //printf("%d'%c' ",node->values[i],*node->records[i]);
                printf("%d-%d ",node->keys[i].first,node->keys[i].second.i);
            }
            putchar('\n');
        }else{
            auto n = make_unique<B_Plus_tree::Node>(node->children[0],bpt);
            dfs(move(n),depth+1,bpt);
            for(int i=0; i<node->keys.size(); i++){
                for(int i=0; i<depth; i++){
                    putchar('\t');
                }
                printf("%d\n",node->keys[i].second.i);
                n = make_unique<B_Plus_tree::Node>(node->children[i+1],bpt);
                dfs(move(n),depth+1,bpt);
            }
        }
}

void BPT_unit_test::test(){
    auto bpt = make_unique<B_Plus_tree>("test.bpt", db_item::type::DB_INT, 0);
    
//    auto old_n = B_Plus_tree::Node::create(-1, 2, bpt.get());
//    old_n.reset();
//    auto n = B_Plus_tree::Node(2, bpt.get());
    bpt->insert(5, 1);
    bpt->insert(3, 2);
    bpt->insert(4, 6);
    bpt->insert(1, 2);
    bpt->insert(10, 7);
    bpt->insert(2, -7);
    bpt->insert(9, 10);
    bpt->insert(0, 2);
    bpt->insert(18, 6);
    
    
    auto root = make_unique<B_Plus_tree::Node>(bpt->root,bpt.get());

    dfs(move(root), 0, bpt.get());
    
    bpt->erase(5);
    
    root = make_unique<B_Plus_tree::Node>(bpt->root,bpt.get());

    dfs(move(root), 0, bpt.get());
}
