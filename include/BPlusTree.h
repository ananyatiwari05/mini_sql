#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <vector>
#include <memory>
#include <string>
using namespace std;

// B+ Tree Node for disk-based indexing
class BPlusTreeNode {
public:
    bool isLeaf;
    vector<string> keys;
    vector<int> values;  // Record offsets/pointers
    vector<shared_ptr<BPlusTreeNode>> children;
    shared_ptr<BPlusTreeNode> nextLeaf;  // For range queries
    
    BPlusTreeNode(bool leaf = true) : isLeaf(leaf), nextLeaf(nullptr) {}
};

class BPlusTree {
private:
    shared_ptr<BPlusTreeNode> root;
    const int maxKeys = 4;  // Branching factor
    
    void splitChild(shared_ptr<BPlusTreeNode> parent, int index);
    void insertNonFull(shared_ptr<BPlusTreeNode> node, const string& key, int value);
    void removeFromLeaf(shared_ptr<BPlusTreeNode> node, int index);
    void removeFromNonLeaf(shared_ptr<BPlusTreeNode> node, int index);
    void borrowFromPrev(shared_ptr<BPlusTreeNode> node, int index);
    void borrowFromNext(shared_ptr<BPlusTreeNode> node, int index);
    void merge(shared_ptr<BPlusTreeNode> node, int index);
    
public:
    BPlusTree();
    
    void insert(const string& key, int value);
    int search(const string& key) const;
    vector<int> rangeSearch(const string& start, const string& end) const;
    void remove(const string& key);
    bool exists(const string& key) const;
};

#endif // BPLUSTREE_H
