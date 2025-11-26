#ifndef AVLTREE_H
#define AVLTREE_H

#include <memory>
#include <vector>
#include <string>
using namespace std;

// AVL Tree Node for in-memory sorting and grouping
template<typename T>
class AVLNode {
public:
    T key;
    T value;
    int height;
    shared_ptr<AVLNode<T>> left;
    shared_ptr<AVLNode<T>> right;
    
    AVLNode(T k, T v) : key(k), value(v), height(1), left(nullptr), right(nullptr) {}
};

template<typename T>
class AVLTree {
private:
    shared_ptr<AVLNode<T>> root;
    
    int getHeight(shared_ptr<AVLNode<T>> node) const;
    int getBalance(shared_ptr<AVLNode<T>> node) const;
    shared_ptr<AVLNode<T>> rotateRight(shared_ptr<AVLNode<T>> y);
    shared_ptr<AVLNode<T>> rotateLeft(shared_ptr<AVLNode<T>> x);
    shared_ptr<AVLNode<T>> insertNode(shared_ptr<AVLNode<T>> node, T key, T value);
    void inOrderTraversal(shared_ptr<AVLNode<T>> node, vector<T>& result) const;
    
public:
    AVLTree() : root(nullptr) {}
    
    void insert(T key, T value);
    vector<T> getInOrder() const;
    void clear();
};

#endif // AVLTREE_H
