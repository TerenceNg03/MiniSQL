//
//  B_Plus_Tree.hpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/14.
//

#ifndef B_Plus_Tree_hpp
#define B_Plus_Tree_hpp

#include <vector>
#include <cstdio>
#include "Record_Manager.hpp"

template <class T> class buf_ptr{
    T* val;
    buf_ptr(std::string filename, T& t);
    T* operator ->();
    
};

template <class key> class B_Plus_Tree{
    
public:
    typedef int record;
    
    struct Node{
        Node* p = NULL;
        bool Isleaf;
        std::vector<int> records;
        std::vector<key> values;
        std::vector<Node*> child;
        Node(key x, bool Isleaf = true);
        ~Node();
    };
    
    B_Plus_Tree(int order = 3);
    
    void insert(key x,record r);
    
    record erase(key x);
    
    record search(key x);
    
    Node* getroot();
    
private:
    int order;
    
    Node* root;
    
    Node* _search(key x);
    
    void _split_node(Node* n);
    
    void underflow_fix(Node* n);
    
    key _min(Node* n);
    
};

namespace BP_unit_test {
void test();
}


#endif /* B_Plus_Tree_hpp */
