#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <vector>
#include <memory>
#include <algorithm>

template <typename Key, typename Value>
class BPlusTree {
private:
    static const int ORDER = 4; // Branching factor

    struct BNode {
        std::vector<Key> keys;
        std::vector<std::shared_ptr<BNode>> children;
        std::vector<Value> values;
        bool isLeaf;
        std::shared_ptr<BNode> next; // Link to next leaf for range queries

        BNode(bool leaf = true) : isLeaf(leaf), next(nullptr) {}
    };

    std::shared_ptr<BNode> root;

    std::shared_ptr<BNode> searchLeaf(const std::shared_ptr<BNode>& node, const Key& key) {
        if (node->isLeaf) return node;

        size_t i = 0;
        while (i < node->keys.size() && key >= node->keys[i]) {
            i++;
        }
        return searchLeaf(node->children[i], key);
    }

    void insertNonFull(std::shared_ptr<BNode>& node, const Key& key, const Value& value) {
        int i = node->keys.size() - 1;

        if (node->isLeaf) {
            node->keys.push_back(Key());
            node->values.push_back(Value());

            while (i >= 0 && key < node->keys[i]) {
                node->keys[i + 1] = node->keys[i];
                node->values[i + 1] = node->values[i];
                i--;
            }
            node->keys[i + 1] = key;
            node->values[i + 1] = value;
        } else {
            while (i >= 0 && key < node->keys[i]) {
                i--;
            }
            i++;

            if (node->children[i]->keys.size() >= ORDER - 1) {
                splitChild(node, i);
                if (key > node->keys[i]) {
                    i++;
                }
            }
            insertNonFull(node->children[i], key, value);
        }
    }

    void splitChild(std::shared_ptr<BNode>& parent, int i) {
        auto child = parent->children[i];
        auto newNode = std::make_shared<BNode>(child->isLeaf);

        int mid = (ORDER - 1) / 2;

        // Copy keys and values to new node
        for (int j = mid; j < (int)child->keys.size(); j++) {
            newNode->keys.push_back(child->keys[j]);
            newNode->values.push_back(child->values[j]);
        }

        if (!child->isLeaf) {
            for (int j = mid + 1; j < (int)child->children.size(); j++) {
                newNode->children.push_back(child->children[j]);
            }
        }

        // Adjust child
        child->keys.erase(child->keys.begin() + mid, child->keys.end());
        child->values.erase(child->values.begin() + mid, child->values.end());

        if (!child->isLeaf) {
            child->children.erase(child->children.begin() + mid + 1, child->children.end());
        }

        // Move middle key up to parent
        parent->keys.insert(parent->keys.begin() + i, child->keys[mid]);
        parent->children.insert(parent->children.begin() + i + 1, newNode);
    }

    bool findInNode(const std::shared_ptr<BNode>& node, const Key& key, Value& result) const {
        size_t i = 0;
        while (i < node->keys.size() && key > node->keys[i]) {
            i++;
        }

        if (i < node->keys.size() && key == node->keys[i]) {
            result = node->values[i];
            return true;
        }

        if (node->isLeaf) return false;
        return findInNode(node->children[i], key, result);
    }

    void rangeSearch(const std::shared_ptr<BNode>& node, const Key& minKey, const Key& maxKey,
                     std::vector<Value>& results) const {
        if (!node) return;

        if (node->isLeaf) {
            for (size_t i = 0; i < node->keys.size(); i++) {
                if (node->keys[i] >= minKey && node->keys[i] <= maxKey) {
                    results.push_back(node->values[i]);
                }
            }
            if (node->next) {
                rangeSearch(node->next, minKey, maxKey, results);
            }
        } else {
            for (size_t i = 0; i < node->keys.size(); i++) {
                if (minKey <= node->keys[i]) {
                    rangeSearch(node->children[i], minKey, maxKey, results);
                }
            }
            if (node->keys.empty() || maxKey >= node->keys.back()) {
                rangeSearch(node->children.back(), minKey, maxKey, results);
            }
        }
    }

public:
    BPlusTree() : root(std::make_shared<BNode>(true)) {}

    void insert(const Key& key, const Value& value) {
        if (root->keys.size() >= ORDER - 1) {
            auto newRoot = std::make_shared<BNode>(false);
            newRoot->children.push_back(root);
            splitChild(newRoot, 0);
            root = newRoot;
        }
        insertNonFull(root, key, value);
    }

    bool find(const Key& key, Value& result) const {
        return findInNode(root, key, result);
    }

    std::vector<Value> rangeQuery(const Key& minKey, const Key& maxKey) const {
        std::vector<Value> results;
        rangeSearch(root, minKey, maxKey, results);
        return results;
    }

    void clear() {
        root = std::make_shared<BNode>(true);
    }
};

#endif // BPLUSTREE_H
