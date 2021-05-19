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

template <class key,class record> class B_Plus_Tree{
    
public:
    
    struct Node{
        Node* p = NULL;
        bool Isleaf;
        std::vector<record*> records;
        std::vector<key> values;
        std::vector<Node*> child;
        Node(key x, bool Isleaf = true);
        ~Node();
    };
    
    B_Plus_Tree(int order = 3);
    
    void insert(key x,record* r);
    
    record* pop(key x);
    
    record search(key x);
    
    Node* getroot();
    
private:
    int order;
    
    Node* root;
    
    Node* _search(key x);
    
    void _split_node(Node* n);
    
    void underflow_fix(Node* n);
    
};

namespace BP_unit_test {

void test();

void DFS(B_Plus_Tree<int, char>::Node * node,int depth);

}


#endif /* B_Plus_Tree_hpp */
