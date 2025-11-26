#ifndef AVLTREE_H
#define AVLTREE_H

#include <memory>
#include <vector>
#include <functional>

template <typename Key, typename Value>
class AVLTree {
private:
    struct AVLNode {
        Key key;
        Value value;
        std::shared_ptr<AVLNode> left;
        std::shared_ptr<AVLNode> right;
        int height;

        AVLNode(const Key& k, const Value& v)
            : key(k), value(v), left(nullptr), right(nullptr), height(1) {}
    };

    std::shared_ptr<AVLNode> root;

    int getHeight(const std::shared_ptr<AVLNode>& node) {
        return node ? node->height : 0;
    }

    int getBalance(const std::shared_ptr<AVLNode>& node) {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    void updateHeight(std::shared_ptr<AVLNode>& node) {
        if (node) {
            node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
        }
    }

    std::shared_ptr<AVLNode> rotateRight(std::shared_ptr<AVLNode> y) {
        auto x = y->left;
        y->left = x->right;
        x->right = y;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    std::shared_ptr<AVLNode> rotateLeft(std::shared_ptr<AVLNode> x) {
        auto y = x->right;
        x->right = y->left;
        y->left = x;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    std::shared_ptr<AVLNode> insertNode(std::shared_ptr<AVLNode> node, const Key& key, const Value& value) {
        if (!node) {
            return std::make_shared<AVLNode>(key, value);
        }

        if (key < node->key) {
            node->left = insertNode(node->left, key, value);
        } else if (key > node->key) {
            node->right = insertNode(node->right, key, value);
        } else {
            node->value = value; // Update if key already exists
            return node;
        }

        updateHeight(node);
        int balance = getBalance(node);

        // Left-Left case
        if (balance > 1 && key < node->left->key) {
            return rotateRight(node);
        }

        // Right-Right case
        if (balance < -1 && key > node->right->key) {
            return rotateLeft(node);
        }

        // Left-Right case
        if (balance > 1 && key > node->left->key) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        // Right-Left case
        if (balance < -1 && key < node->right->key) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    bool findNode(const std::shared_ptr<AVLNode>& node, const Key& key, Value& result) const {
        if (!node) return false;

        if (key < node->key) {
            return findNode(node->left, key, result);
        } else if (key > node->key) {
            return findNode(node->right, key, result);
        } else {
            result = node->value;
            return true;
        }
    }

    void inorderTraversal(const std::shared_ptr<AVLNode>& node, std::vector<std::pair<Key, Value>>& result) const {
        if (!node) return;
        inorderTraversal(node->left, result);
        result.push_back({node->key, node->value});
        inorderTraversal(node->right, result);
    }

public:
    AVLTree() : root(nullptr) {}

    void insert(const Key& key, const Value& value) {
        root = insertNode(root, key, value);
    }

    bool find(const Key& key, Value& result) const {
        return findNode(root, key, result);
    }

    std::vector<std::pair<Key, Value>> getAllInOrder() const {
        std::vector<std::pair<Key, Value>> result;
        inorderTraversal(root, result);
        return result;
    }

    bool isEmpty() const {
        return root == nullptr;
    }

    void clear() {
        root = nullptr;
    }
};

#endif // AVLTREE_H
