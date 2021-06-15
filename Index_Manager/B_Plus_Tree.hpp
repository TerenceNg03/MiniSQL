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
    class Node{
    private:
//        Node();
    public:
        bool isleaf;
        bool vaild;
        int pos;
        std::vector<int> child;
        std::vector<int> record;
        std::vector<db_item> key;
        
        Node(int pos, const B_Plus_tree& bp_tree);
        ~Node();
    };
    
    B_Plus_tree();
    
public:
    std::string filename;
    db_item::type key_t;
    int key_l;
    int size;
    static std::unique_ptr<B_Plus_tree> load(const std::string& filename);
    static std::unique_ptr<B_Plus_tree> create(const std::string& filename, db_item::type T, int key_length);
    
    std::unique_ptr<Node> _search(db_item key);
};

#endif /* B_Plus_Tree_hpp */
