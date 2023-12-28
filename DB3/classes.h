#include <bits/stdc++.h>
#include <iostream>
#include <random>
#include <utility>
#include <vector>
#include <tuple>
#include <random>
#include <utility>

using namespace std;

vector<string> splitquery(string line) {
    vector<string> fields;
    stringstream ss(line);
    string substr;

    while (std::getline(ss, substr, ',')) {
        fields.push_back(substr);
    }

    return fields;
}

class Record
{
public:
    int id, manager_id;
    std::string bio, name;

    Record(vector<std::string> fields)
    {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    void print()
    {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }
};

class Node {
 public:
    Node(Node *parent = nullptr, bool isLeaf = false, Node *prev_ = nullptr,
        Node *next_ = nullptr)
        : parent(parent), isLeaf(isLeaf), prev(prev_), next(next_) {
        if (next_) {
        next_->prev = this;
        }

        if (prev_) {
        prev_->next = this;
        }
    }

    vector<int> keys;
    Node *parent;

    vector<Node *> children;
    vector<string> values;
    Node *next;
    Node *prev;

    bool isLeaf;

    int index_C(int key) {
        for (int i = 0; i < keys.size(); i++) {
        if (key < keys[i]) {
            return i;
        }
        }
        return keys.size();
    }

    int index_K(int key) {
        for (int i = 0; i < keys.size(); i++) {
        if (key == keys[i]) {
            return i;
        }
        }
        return -1;
    }

    Node *getChild(int key) { return children[index_C(key)]; }

    void setChild(int key, vector<Node *> value) {
        int i = index_C(key);
        keys.insert(keys.begin() + i, key);
        children.erase(children.begin() + i);
        children.insert(children.begin() + i, value.begin(), value.end());
    }

    tuple<int, Node *, Node *> split_int() {
        Node *left = new Node(parent, false, nullptr, nullptr);
        int mid = keys.size() / 2;

        copy(keys.begin(), keys.begin() + mid, back_inserter(left->keys));
        copy(children.begin(), children.begin() + mid + 1,
            back_inserter(left->children));

        for (Node *child : left->children) {
        child->parent = left;
        }

        int key = keys[mid];
        keys.erase(keys.begin(), keys.begin() + mid + 1);
        children.erase(children.begin(), children.begin() + mid + 1);

        return make_tuple(key, left, this);
    }

    string get(int key) {
        int index = -1;
        for (int i = 0; i < keys.size(); ++i) {
        if (keys[i] == key) {
            index = i;
            break;
        }
        }

        if (index == -1) {
        cout << "key " << key << " not found" << endl;
        }

        return values[index];
    }

    void set(int key, string value) {
        int i = index_C(key);
        if (find(keys.begin(), keys.end(), key) == keys.end()) {
        keys.insert(keys.begin() + i, key);
        values.insert(values.begin() + i, value);
        } else {
        values[i - 1] = value;
        }
    }

    tuple<int, Node *, Node *> split_leaf() {
        Node *left = new Node(parent, true, prev, this);
        int mid = keys.size() / 2;
        left->keys = vector<int>(keys.begin(), keys.begin() + mid);
        left->values = vector<string>(values.begin(), values.begin() + mid);
        keys.erase(keys.begin(), keys.begin() + mid);
        values.erase(values.begin(), values.begin() + mid);
        return make_tuple(keys[0], left, this);
    }
    
    std::string serz() {
        std::stringstream ss;
        ss << isLeaf << "\n";
        ss << keys.size() << "\n";
        for (int i = 0; i < keys.size(); i++) {
            ss << keys[i] << " ";
        }
        ss << "\n";
        ss << children.size() << "\n";
        for (int i = 0; i < children.size(); i++) {
            ss << children[i]->serz();
        }
        ss << values.size() << "\n";
        for (int i = 0; i < values.size(); i++) {
            ss << values[i] << "\n";
        }
        return ss.str();
    }

    void de_ser(std::stringstream& ss) {
        ss >> isLeaf;
        int n_keys;
        ss >> n_keys;
        keys.resize(n_keys);
        for (int i = 0; i < n_keys; i++) {
            ss >> keys[i];
        }
        int n_children;
        ss >> n_children;
        children.resize(n_children);
        for (int i = 0; i < n_children; i++) {
            children[i] = new Node();
            children[i]->parent = this;
            children[i]->de_ser(ss);
        }
        int n_values;
        ss >> n_values;
        values.resize(n_values);
        for (int i = 0; i < n_values; i++) {
            ss >> values[i];
        }
    }
};

class BPlusTree {
public:
    BPlusTree(int _maxCapacity = 4) {
        root = new Node(nullptr, true, nullptr, nullptr);
        maxCapacity = _maxCapacity > 2 ? _maxCapacity : 2;
        minCapacity = maxCapacity / 2;
        depth = 0;
    }
    Node *root;
    int maxCapacity;
    int minCapacity;
    int depth;

    Node *findLeaf(int key) {
        Node *node = root;
        while (!node->isLeaf) {
        node = node->getChild(key);
        }
        return node;
    }
    string get(int key) { 
        return findLeaf(key)->get(key); 
    }

    void set(int key, string value) {
        Node *leaf = findLeaf(key);
        leaf->set(key, value);
        if (leaf->keys.size() > maxCapacity) {
        insert(leaf->split_leaf());
        }
    }
    void insert(tuple<int, Node *, Node *> result) {
        int key = std::get<0>(result);
        Node *left = std::get<1>(result);
        Node *right = std::get<2>(result);
        Node *parent = right->parent;
        if (parent == nullptr) {
        left->parent = right->parent = root =
            new Node(nullptr, false, nullptr, nullptr);
        depth += 1;
        root->keys = {key};
        root->children = {left, right};
        return;
        }
        parent->setChild(key, {left, right});
        if (parent->keys.size() > maxCapacity) {
        insert(parent->split_int());
        }
    }
    void createFromFile(string csvFileName) {
        fstream csvFile(csvFileName);
        string line;

        ofstream dataFile("Data.bin");
        ofstream Index("Index");
        
        char* page = new char[4096];
        int offset = 0;
        int globalOffset = 0;
        int index = 0;

        if (csvFile.is_open()) {
            while (std::getline(csvFile, line)) {
                Record record(splitquery(line));
                int recordSize = calculateRecordSize(record) + 4;
                if (offset + recordSize > 4096) {
                    dataFile.write(page, offset);                    
                    page = new char[4096];
                    offset = 0;
                }

                writeRecordToPage(record, page, offset);

                string address = to_string(globalOffset) + ":" + to_string(globalOffset + recordSize);
                globalOffset += recordSize;
                set(record.id, address);
            }

            this->serz("Index");
            cout << "Write to Index" << endl;
            if (offset > 0) {
                dataFile.write(page, 4096);
            }
        }

        delete[] page;
        csvFile.close();
        dataFile.close();
        Index.close();
    }

    void serz(const std::string& filename) {
        std::stringstream ss;
        std::ofstream outfile(filename);
        ss << root->serz();
        string serzd_tree = ss.str();
        outfile << serzd_tree;
        outfile.close();
    }

    void de_ser(const std::string& filename) {
        std::ifstream infile(filename);
        std::stringstream ss;
        ss << infile.rdbuf();
        root = new Node();
        root->parent = nullptr;
        root->de_ser(ss);
    }

    int calculateRecordSize(Record record) {
        return 8 + record.name.length() + record.bio.length() + 8;
    }

    void writeRecordToPage(Record record, char* page, int& offset) {
        memcpy(page + offset, std::to_string(record.id).c_str(), 8); offset += 8;
        memcpy(page + offset, ",", 1); offset += 1;
        memcpy(page + offset, record.name.c_str(), record.name.length()); offset += record.name.length();
        memcpy(page + offset, ",", 1);  offset += 1;
        memcpy(page + offset, record.bio.c_str(), record.bio.length()); offset += record.bio.length();
        memcpy(page + offset, ",", 1); offset += 1;
        memcpy(page + offset, std::to_string(record.manager_id).c_str(), 8); offset += 8;
        memcpy(page + offset, "\n", 1); offset += 1;
    }
  
};