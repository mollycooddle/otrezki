
#ifndef POLINOM_AVL_TREE_H
#define POLINOM_AVL_TREE_H

#include <iostream>
#include <algorithm>
#include <queue>
#include <stack>

template <typename TKey, typename TValue>
class AVLTree {
private:


    struct TTableRec {
        TKey key;
        TValue value;

        TTableRec() = default;
        TTableRec(const TKey& k, const TValue& v) : key(k), value(v) {}

        bool operator==(const TTableRec& other) const {
            return key == other.key && value == other.value;
        }

        bool operator!=(const TTableRec& other) const {
            return !(*this == other);
        }
    };

    struct Node {
        TTableRec data;
        Node* left;
        Node* right;
        int height;

        Node(const TKey& key, const TValue& value)
                : data(key, value), left(nullptr), right(nullptr), height(1) {}
    };
    Node* root = nullptr;
    int treeSize = 0;

    int getHeight(Node* node) const {
        return node ? node->height : 0;
    }

    int getBalanceCoeff(Node* node) const {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    void updateHeight(Node* node) {
        if (node) {
            node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
        }
    }

    Node* rightRotate(Node* y) {
        Node* r1 = y->left;
        Node* r2 = r1->right;

        r1->right = y;
        y->left = r2;

        updateHeight(y);
        updateHeight(r1);

        return r1;
    }

    Node* leftRotate(Node* x) {
        Node* r1 = x->right;
        Node* r2 = r1->left;

        r1->left = x;
        x->right = r2;

        updateHeight(x);
        updateHeight(r1);

        return r1;
    }

    Node* balance(Node* node) {
        if (!node) return nullptr;

        updateHeight(node);
        int balanceFactor = getBalanceCoeff(node);

        if (balanceFactor > 1) {
            if (getBalanceCoeff(node->left) < 0) {
                node->left = leftRotate(node->left);
            }
            return rightRotate(node);
        }
        if (balanceFactor < -1) {
            if (getBalanceCoeff(node->right) > 0) {
                node->right = rightRotate(node->right);
            }
            return leftRotate(node);
        }
        return node;
    }

    Node* insert(Node* node, const TKey& key, const TValue& value) {
        if (!node) {
            treeSize++;
            return new Node(key, value);
        }

        if (key < node->data.key) {
            node->left = insert(node->left, key, value);
        } else if (key > node->data.key) {
            node->right = insert(node->right, key, value);
        } /*else {
            throw std::runtime_error("ID already exists");
        }*/

        return balance(node);
    }

    Node* findMin(Node* node) const {
        while (node && node->left) {
            node = node->left;
        }
        return node;
    }

    Node* erase(Node* node, const TKey& key) {
        if (!node) return nullptr;

        if (key < node->data.key) {
            node->left = erase(node->left, key);
        } else if (key > node->data.key) {
            node->right = erase(node->right, key);
        } else {
            if (!node->left || !node->right) {
                Node* temp = node->left ? node->left : node->right;
                if (!temp) {
                    temp = node;
                    node = nullptr;
                } else {
                    *node = *temp;
                }
                delete temp;
                treeSize--;
            } else {
                Node* temp = findMin(node->right);
                node->data = temp->data;
                node->right = erase(node->right, temp->data.key);
            }
        }

        if (!node) return nullptr;

        return balance(node);
    }

    bool equalTree(Node* node1, Node* node2) const {
        if (!node1 && !node2) return true;
        if (!node1 || !node2) return false;
        return node1->data == node2->data &&
            equalTree(node1->left, node2->left) &&
            equalTree(node1->right, node2->right);
    }

    void print(Node* node, std::ostream& os) const {
        if (node) {
            print(node->left, os);
            os << node->data.key << ": " << node->data.value << "\n";
            print(node->right, os);
        }
    }

    Node* copyTree(Node* node) const {
        if (!node) return nullptr;
        Node* newNode = new Node(node->data.key, node->data.value);
        newNode->left = copyTree(node->left);
        newNode->right = copyTree(node->right);
        newNode->height = node->height;
        return newNode;
    }

    void clearTree(Node* node) {
        if (node) {
            clearTree(node->left);
            clearTree(node->right);
            delete node;
        }
    }

public:

    // Поиск предшественника (наибольший элемент, меньший key)
    bool predecessor(const TKey& key, TKey& pred_key, TValue& pred_value) const {
        Node* current = root;
        Node* pred = nullptr;

        while (current != nullptr) {
            if (current->data.key < key) {
                pred = current;
                current = current->right;
            }
            else {
                current = current->left;
            }
        }

        if (pred != nullptr) {
            pred_key = pred->data.key;
            pred_value = pred->data.value;
            return true;
        }
        return false;
    }

    // Поиск преемника (наименьший элемент, больший key)
    bool successor(const TKey& key, TKey& succ_key, TValue& succ_value) const {
        Node* current = root;
        Node* succ = nullptr;

        while (current != nullptr) {
            if (current->data.key > key) {
                succ = current;
                current = current->left;
            }
            else {
                current = current->right;
            }
        }

        if (succ != nullptr) {
            succ_key = succ->data.key;
            succ_value = succ->data.value;
            return true;
        }
        return false;
    }

    AVLTree() : root(nullptr), treeSize(0) {}
    AVLTree(const AVLTree& other) : root(copyTree(other.root)), treeSize(other.treeSize) {}
    ~AVLTree() { clearTree(root); }

    AVLTree& operator=(const AVLTree& other) {
        if (this != &other) {
            clearTree(root);
            root = copyTree(other.root);
            treeSize = other.treeSize;
        }
        return *this;
    }

    //работает за O(log n)
    void insert(const TKey& key, const TValue& value) {
        root = insert(root, key, value);
    }

    //работает за O(log n)
    void erase(const TKey& key) {
        root = erase(root, key);
    }

    int size() const {
        return treeSize;
    }

    size_t height() const {
        return getHeight(root);
    }

    //работает за O(log n)
    TValue find(const TKey& key) const {
        Node* current = root;
        while (current) {
            if (key < current->data.key) {
                current = current->left;
            } else if (key > current->data.key) {
                current = current->right;
            } else {
                return current->data.value;
            }
        }
        throw std::runtime_error("Id not found");
    }

    bool operator==(const AVLTree& other) const {
        return equalTree(root, other.root);
    }

    bool operator!=(const AVLTree& other) const {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& os, const AVLTree& tree) {
        tree.print(tree.root, os);
        return os;
    }

    class Iterator {
    private:
        std::stack<Node*> stack;
        Node* current;

        void FullStack(Node* node) {
            while (node != nullptr) {
                stack.push(node);
                node = node->left;
            }
        }

    public:
        Iterator(Node* root) : current(nullptr) {
            FullStack(root);
            if (!stack.empty()) {
                current = stack.top();
                stack.pop();
            }
        }

        Iterator() : current(nullptr) {}

        TTableRec& operator*() const {
            if (current == nullptr) {
                throw std::out_of_range("Iterator is out of range");
            }
            return current->data;
        }

        TTableRec* operator->() const {
            if (current == nullptr) {
                throw std::out_of_range("Iterator is out of range");
            }
            return &(current->data);
        }

        Iterator& operator++() {
            if (current == nullptr) {
                return *this;
            }
            FullStack(current->right);
            if (stack.empty()) {
                current = nullptr;
            } else {
                current = stack.top();
                stack.pop();
            }
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }

        bool operator==(const Iterator& other) const {
            return current == other.current;
        }
    };

    Iterator begin() {
        return Iterator(root);
    }

    Iterator end() {
        return Iterator();
    }
};
#endif // POLINOM_AVL_TREE_H
