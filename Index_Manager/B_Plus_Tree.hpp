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
#include <memory>
#include "Record_Manager.hpp"

class B_Plus_tree{
public:
    class Node{
    private:
        
    public:
        bool isleaf;
        int pos;
        int parent;
        const B_Plus_tree* bpt;
        std::vector<int> children;
        std::vector<int> f_ptrs;
        std::vector<std::pair<bool,db_item>> keys;
        
        Node(int pos, const B_Plus_tree* bp_tree);
        Node(){};
        std::unique_ptr<Node> static create(int parent, int pos, const B_Plus_tree* bp_tree);
        int static capability(const B_Plus_tree& bp_tree);
        ~Node();
    };
private:
    
    std::unique_ptr<Node> _search(db_item key);
    void _split_node(std::unique_ptr<Node> node);
public:
    std::string filename;
    db_item::type key_t;
    int key_l;
    int size;
    int root;
    B_Plus_tree(const std::string& filename);
    B_Plus_tree(const std::string& filename, db_item::type T, int key_length);
    ~B_Plus_tree();
    void insert(db_item it,int f_ptr);
    void erase(db_item it);
    int search(db_item it);
};

namespace BPT_unit_test {
void test();
}

#endif /* B_Plus_Tree_hpp */
