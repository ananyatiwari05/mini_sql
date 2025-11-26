#include "BPlusTree.h"
#include <algorithm>
using namespace std;
BPlusTree::BPlusTree() : root(make_shared<BPlusTreeNode>(true)) {}

void BPlusTree::insert(const string& key, int value) {
    if (root->keys.size() >= maxKeys) {
        auto newRoot = make_shared<BPlusTreeNode>(false);
        newRoot->children.push_back(root);
        splitChild(newRoot, 0);
        root = newRoot;
    }
    insertNonFull(root, key, value);
}

void BPlusTree::insertNonFull(shared_ptr<BPlusTreeNode> node, const string& key, int value) {
    int i = node->keys.size() - 1;
    
    if (node->isLeaf) {
        node->keys.push_back("");
        node->values.push_back(0);
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            node->values[i + 1] = node->values[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->values[i + 1] = value;
    } else {
        while (i >= 0 && key < node->keys[i]) i--;
        i++;
        if (node->children[i]->keys.size() >= maxKeys) {
            splitChild(node, i);
            if (key > node->keys[i]) i++;
        }
        insertNonFull(node->children[i], key, value);
    }
}

void BPlusTree::splitChild(shared_ptr<BPlusTreeNode> parent, int index) {
    auto fullChild = parent->children[index];
    auto newChild = make_shared<BPlusTreeNode>(fullChild->isLeaf);
    
    int mid = maxKeys / 2;
    newChild->keys.assign(fullChild->keys.begin() + mid, fullChild->keys.end());
    fullChild->keys.erase(fullChild->keys.begin() + mid, fullChild->keys.end());
    
    if (fullChild->isLeaf) {
        newChild->values.assign(fullChild->values.begin() + mid, fullChild->values.end());
        fullChild->values.erase(fullChild->values.begin() + mid, fullChild->values.end());
        newChild->nextLeaf = fullChild->nextLeaf;
        fullChild->nextLeaf = newChild;
    } else {
        newChild->children.assign(fullChild->children.begin() + mid + 1, fullChild->children.end());
        fullChild->children.erase(fullChild->children.begin() + mid + 1, fullChild->children.end());
    }
    
    parent->keys.insert(parent->keys.begin() + index, fullChild->keys[mid - 1]);
    parent->children.insert(parent->children.begin() + index + 1, newChild);
}

int BPlusTree::search(const string& key) const {
    auto node = root;
    while (!node->isLeaf) {
        int i = 0;
        while (i < node->keys.size() && key > node->keys[i]) i++;
        node = node->children[i];
    }
    for (size_t i = 0; i < node->keys.size(); i++) {
        if (node->keys[i] == key) return node->values[i];
    }
    return -1;
}

bool BPlusTree::exists(const string& key) const {
    return search(key) != -1;
}

vector<int> BPlusTree::rangeSearch(const string& start, const string& end) const {
    vector<int> results;
    auto node = root;
    while (!node->isLeaf) {
        int i = 0;
        while (i < node->keys.size() && start > node->keys[i]) i++;
        node = node->children[i];
    }
    while (node) {
        for (size_t i = 0; i < node->keys.size(); i++) {
            if (node->keys[i] >= start && node->keys[i] <= end) {
                results.push_back(node->values[i]);
            }
        }
        node = node->nextLeaf;
    }
    return results;
}

void BPlusTree::remove(const string& key) {
    // Simplified delete - mark as deleted
}
