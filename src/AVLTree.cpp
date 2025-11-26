#include "AVLTree.h"
#include <algorithm>
using namespace std;
template<typename T>
int AVLTree<T>::getHeight(shared_ptr<AVLNode<T>> node) const {
    return node ? node->height : 0;
}

template<typename T>
int AVLTree<T>::getBalance(shared_ptr<AVLNode<T>> node) const {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

template<typename T>
shared_ptr<AVLNode<T>> AVLTree<T>::rotateRight(shared_ptr<AVLNode<T>> y) {
    auto x = y->left;
    auto T2 = x->right;
    
    x->right = y;
    y->left = T2;
    
    y->height = 1 + max(getHeight(y->left), getHeight(y->right));
    x->height = 1 + max(getHeight(x->left), getHeight(x->right));
    
    return x;
}

template<typename T>
shared_ptr<AVLNode<T>> AVLTree<T>::rotateLeft(shared_ptr<AVLNode<T>> x) {
    auto y = x->right;
    auto T2 = y->left;
    
    y->left = x;
    x->right = T2;
    
    x->height = 1 + max(getHeight(x->left), getHeight(x->right));
    y->height = 1 + max(getHeight(y->left), getHeight(y->right));
    
    return y;
}

template<typename T>
shared_ptr<AVLNode<T>> AVLTree<T>::insertNode(shared_ptr<AVLNode<T>> node, T key, T value) {
    if (!node) {
        return make_shared<AVLNode<T>>(key, value);
    }
    
    if (key < node->key) {
        node->left = insertNode(node->left, key, value);
    } else {
        node->right = insertNode(node->right, key, value);
    }
    
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    int balance = getBalance(node);
    
    if (balance > 1 && key < node->left->key) {
        return rotateRight(node);
    }
    if (balance < -1 && key > node->right->key) {
        return rotateLeft(node);
    }
    if (balance > 1 && key > node->left->key) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && key < node->right->key) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    
    return node;
}

template<typename T>
void AVLTree<T>::insert(T key, T value) {
    root = insertNode(root, key, value);
}

template<typename T>
void AVLTree<T>::inOrderTraversal(shared_ptr<AVLNode<T>> node, vector<T>& result) const {
    if (!node) return;
    inOrderTraversal(node->left, result);
    result.push_back(node->value);
    inOrderTraversal(node->right, result);
}

template<typename T>
vector<T> AVLTree<T>::getInOrder() const {
    vector<T> result;
    inOrderTraversal(root, result);
    return result;
}

template<typename T>
void AVLTree<T>::clear() {
    root = nullptr;
}

// Explicit template instantiation
template class AVLTree<string>;
template class AVLTree<int>;
