//
//  B_Plus_Tree.cpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/14.
//

#include "B_Plus_Tree.hpp"

using namespace std;

template <class key> B_Plus_Tree<key>::Node::Node(key x, bool Isleaf){
    values.push_back(x);
    this->Isleaf = Isleaf;
}

template <class key> B_Plus_Tree<key>::Node::~Node(){
}

template <class key> B_Plus_Tree<key>::B_Plus_Tree(int order){
    this->order = order;
    this->root = NULL;
}

template <class key> typename B_Plus_Tree<key>::Node* B_Plus_Tree<key>::getroot(){
    return root;
}

template <class key> void B_Plus_Tree<key>::insert(key x, record r){
    
    Node* target = _search(x);
    
    if(target == NULL){
        root = new Node(x, true);
        root->records.push_back(r);
        return;
    }
    
    for(int i=0; i<target->values.size(); i++){
        if(target->values[i]==x)return;
    }
    
    for(int i=0; i<target->values.size(); i++){
        if(!(x<*(target->values.rbegin()))){
            target->values.push_back(x);
            target->records.push_back(r);
            break;
        }
        if(!(target->values[i]<x||target->values[i]==x)){
            target->values.insert(target->values.begin()+i,x);
            target->records.insert(target->records.begin()+i,r);
            break;
        }
    }

    
    if(target->values.size()>=order){
        _split_node(target);
    }
    
}

template <class key> void B_Plus_Tree<key>:: _split_node(Node* n){
    
    Node* new_node;
    
    if(n->Isleaf){
        int split = (order)/2;
        new_node = new Node(n->values[split]);
        new_node->values = vector<key>(n->values.begin()+split,n->values.end());
        new_node->records = vector<record>(n->records.begin()+split,n->records.end());
        n->values = vector<key>(n->values.begin(),n->values.begin()+split);
        n->records = vector<record>(n->records.begin(),n->records.begin()+split);
        
        if(n->p==NULL){
            Node* new_root = new Node(new_node->values[0],false);
            new_root->child.push_back(n);
            new_root->child.push_back(new_node);
            n->p = new_root;
            new_node->p = new_root;
            root = new_root;
            
        }else{
            int insert_pos = (int)(n->p->values.size()+1);
            Node* p = n->p;
            key end = *(new_node->values.rbegin());
            for(int i=0; i<p->values.size();i++){
                if(!(p->values[i]<end)){
                    insert_pos = i;
                    break;
                }
            }
            if(insert_pos == n->p->values.size()+1){
                p->values.push_back(new_node->values[0]);
                p->child.push_back(new_node);
            }else{
                p->values.insert(p->values.begin()+insert_pos, new_node->values[0]);
                p->child.insert(p->child.begin()+insert_pos+1, new_node);
            }
            new_node->p = n->p;
            if(n->p->values.size()>=order)_split_node(n->p);
        }
    }else{
        int split = (order)/2;
        new_node = new Node(n->values[split+1],false);
        key p_value = n->values[split];
        new_node->values = vector<key>(n->values.begin()+split+1,n->values.end());
        new_node->child = vector<Node*>(n->child.begin()+split+1,n->child.end());
        n->values = vector<key>(n->values.begin(),n->values.begin()+split);
        n->child = vector<Node*>(n->child.begin(),n->child.begin()+split+1);
        
        for(auto i:new_node->child){
            i->p = new_node;
        }
        
        if(n->p==NULL){
            Node* new_root = new Node(p_value,false);
            new_root->child.push_back(n);
            new_root->child.push_back(new_node);
            n->p = new_root;
            new_node->p = new_root;
            root = new_root;
            
        }else{
            int insert_pos = (int)(n->p->values.size()+1);
            Node* p = n->p;
            key end = *(new_node->values.rbegin());
            for(int i=0; i<p->values.size();i++){
                if(!(p->values[i]<end)){
                    insert_pos = i;
                    break;
                }
            }
            if(insert_pos == n->p->values.size()+1){
                p->values.push_back(p_value);
                p->child.push_back(new_node);
            }else{
                p->values.insert(p->values.begin()+insert_pos, p_value);
                p->child.insert(p->child.begin()+insert_pos, new_node);
            }
            new_node->p = n->p;
            if(n->p->values.size()>=order)_split_node(n->p);
        }
    }
    
}

template <class key> typename B_Plus_Tree<key>::Node* B_Plus_Tree<key>::_search(key x){
    
    if (root==NULL)return NULL;
    
    Node* n = root;
    while(!n->Isleaf){
        if(!(x<*(n->values.rbegin()))){
            n = n->child[n->values.size()];
            continue;
        }
        for(int i=0; i<n->values.size(); i++){
            if(x<n->values[i]){
                n = n->child[i];
                break;
            }
        }
    }
    return n;
}

template <class key> typename B_Plus_Tree<key>::record B_Plus_Tree<key>::search(key x){
    Node* target = _search(x);
    
    if(target == NULL){
        return -1;
    }
    
    for(int i=0; i<target->values.size(); i++){
        if(target->values[i]==x){
            return target->records[i];
        }
    }
    
    return -1;
}

template <class key> key B_Plus_Tree<key>::_min(Node* n){
    while (!n->Isleaf) {
        n = n->child[0];
    }
    return n->values[0];
}

template <class key> typename B_Plus_Tree<key>::record B_Plus_Tree<key>::erase(key x){
    Node* target = _search(x);
    record ret = -1;
    
    if(target == NULL){
        return -1;
    }
    
    int pos = 0;
    bool found = false;
    for(int i=0; i<target->values.size(); i++){
        if(target->values[i]==x){
            pos = i;
            found = true;
            break;
        }
    }
    
    if (!found)return NULL;
    
    ret = target->records[pos];
    
    target->values.erase(target->values.begin()+pos);
    target->records.erase(target->records.begin()+pos);
    
    if (target->values.size()<(order/2)){
        underflow_fix(target);
    }
    
    return ret;
}

template <class key> void B_Plus_Tree<key>::underflow_fix(B_Plus_Tree<key>::Node* n){
    
    if (n->values.size()>=(order/2))return;
    if(n->p==NULL){
        if(n->child.size()==0){
            root = NULL;
            delete n;
        }
        if(n->values.size()>=1)return;
        if(n->child.size()==1){
            root = n->child[0];
            delete n;
        }
        return;
    }
    
    auto p = n->p;
    
    int pos = 0;
    
    for(int i=0; i<p->child.size(); i++){
        if(p->child[i]==n){
            pos = i;
            break;
        }
    }
    
    auto left_sib = (pos>0)?p->child[pos-1]:NULL;
    auto right_sib = (pos<p->child.size()-1)?p->child[pos+1]:NULL;
    
    //move an element from sibling
    if(n->Isleaf){
        if(left_sib!=NULL && left_sib->values.size()>(order/2)){
            n->values.insert(n->values.begin(),left_sib->values.back());
            left_sib->values.pop_back();
            n->records.insert(n->records.begin(),left_sib->records.back());
            left_sib->records.pop_back();
            p->values[pos-1] = n->values[0];
            return;
        }
        
        if(right_sib!=NULL && right_sib->values.size()>(order/2)){
            n->values.push_back(right_sib->values[0]);
            n->records.push_back(right_sib->records[0]);
            right_sib->values.erase(right_sib->values.begin());
            right_sib->records.erase(right_sib->records.begin());
            p->values[pos] = right_sib->values[0];
            return;
        }
    }else{
        if(left_sib!=NULL && left_sib->values.size()>(order/2)){
            n->values.insert(n->values.begin(),_min(n));
            left_sib->values.pop_back();
            n->child.insert(n->child.begin(),left_sib->child.back());
            left_sib->child.pop_back();
            n->child[0]->p = n;
            p->values[pos-1] = _min(n);
            return;
        }
        
        if(right_sib!=NULL && right_sib->values.size()>(order/2)){
            n->values.push_back(_min(right_sib->child[0]));
            n->child.push_back(right_sib->child[0]);
            right_sib->values.erase(right_sib->values.begin());
            right_sib->child.erase(right_sib->child.begin());
            n->child[n->child.size()-1]->p = n;
            p->values[pos] = _min(right_sib);
            return;
        }
    }
    
    //merge with sibling
    if (n->Isleaf){
        if(left_sib!=NULL && left_sib->values.size()<=(order/2)){
            left_sib->values.reserve(left_sib->values.size()+n->values.size());
            left_sib->values.insert(left_sib->values.end(),n->values.begin(),n->values.end());
            left_sib->records.reserve(left_sib->records.size()+n->records.size());
            left_sib->records.insert(left_sib->records.end(),n->records.begin(),n->records.end());
            p->values.erase(p->values.begin()+pos-1);
            delete n;
            p->child.erase(p->child.begin()+pos);
        }
        if(right_sib!=NULL && right_sib->values.size()<=(order/2)){
            n->values.reserve(right_sib->values.size()+n->values.size());
            n->values.insert(n->values.end(),right_sib->values.begin(),right_sib->values.end());
            n->records.reserve(right_sib->records.size()+n->records.size());
            n->records.insert(n->records.end(),right_sib->records.begin(),right_sib->records.end());
            p->values.erase(p->values.begin()+pos);
            delete right_sib;
            p->child.erase(p->child.begin()+pos+1);
        }
    }else{
        if(left_sib!=NULL && left_sib->values.size()<=(order/2)){
            left_sib->values.reserve(left_sib->values.size()+n->values.size());
            left_sib->values.insert(left_sib->values.end(),n->values.begin(),n->values.end());
            left_sib->child.reserve(left_sib->child.size()+n->child.size());
            left_sib->child.insert(left_sib->child.end(),n->child.begin(),n->child.end());
            n->values.insert(n->values.begin()+pos-1,n->values[0]);
            delete n;
            p->values.erase(p->values.begin()+pos-1);
            p->child.erase(p->child.begin()+pos);
        }
        if(right_sib!=NULL && right_sib->values.size()<=(order/2)){
            n->values.reserve(right_sib->values.size()+n->values.size());
            n->values.insert(n->values.end(),right_sib->values.begin(),right_sib->values.end());
            n->child.reserve(right_sib->child.size()+n->child.size());
            n->child.insert(n->child.end(),right_sib->child.begin(),right_sib->child.end());
            n->values.insert(n->values.begin()+pos,n->values[0]);
            delete right_sib;
            p->values.erase(p->values.begin()+pos);
            p->child.erase(p->child.begin()+pos+1);
        }
    }
    
    underflow_fix(n->p);
}


void DFS(B_Plus_Tree<db_item>::Node * node,int depth){
    if(node==NULL)return;
    if(node->Isleaf){
        for(int i=0; i<depth; i++){
            putchar('\t');
        }
        for (int i=0; i<node->values.size();i++){
            //printf("%d'%c' ",node->values[i],*node->records[i]);
            printf("%d ",node->values[i].i);
        }
        putchar('\n');
    }else{
        DFS(node->child[0],depth+1);
        for(int i=0; i<node->values.size(); i++){
            for(int i=0; i<depth; i++){
                putchar('\t');
            }
            printf("%d\n",node->values[i].i);
            DFS(node->child[i+1],depth+1);
        }
    }
}

void BP_unit_test::test(){
    B_Plus_Tree<db_item> BPT(3);
    int s[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    int in[]={5,13,6,10,2,0,9,12};
    for(int i=0; i<sizeof(in)/sizeof(int); i++){
        BPT.insert(db_item(in[i]), s[i]);
    }
    auto root = BPT.getroot();
//    DFS(root, 0);
    
//    printf("---------------------------------\n");
    
    BPT.erase(23);
    BPT.erase(19);
    BPT.erase(5);
    BPT.erase(6);
    BPT.erase(9);
//    root = BPT.getroot();
    DFS(root, 0);
    
    
}
