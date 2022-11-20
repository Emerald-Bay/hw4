#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key, Value>*& node);
    void rotateRight(AVLNode<Key, Value>*& node);
    void insert_Helper(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node);
    void remove_Helper(AVLNode<Key, Value>* node, int height);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    if (this->root_) { //If AVL Tree is not empty
        Node<Key, Value>* node = this->internalFind(new_item.first);

        if (node) { //Node already exists in the tree
            node->setValue(new_item.second);
        }
        else { //Node needs to be inserted
            AVLNode<Key, Value>* buff = (AVLNode<Key, Value>*) this->root_;

            bool cycle = true;
            while (cycle) {
                if (buff->getKey() > new_item.first) {
                    if (buff->getLeft() == nullptr) {
                        cycle = false;

                        AVLNode<Key, Value>* avlNode = new AVLNode<Key, Value>(new_item.first, new_item.second, buff);
                        buff->setLeft(avlNode);
                        avlNode->setBalance(0);
                        
                        if (buff->getBalance() != 0) {
                            buff->setBalance(0);
                        }
                        else {
                            buff->updateBalance(-1);
                            this->insert_Helper(buff, avlNode);
                        }
                    }
                    else {
                        buff = buff->getLeft();
                    }
                }
                else {
                    if (buff->getRight() == nullptr) {
                        cycle = false;

                        AVLNode<Key, Value>* avlNode = new AVLNode<Key, Value>(new_item.first, new_item.second, buff);
                        buff->setRight(avlNode);
                        avlNode->setBalance(0);
                        
                        if (buff->getBalance() != 0) {
                            buff->setBalance(0);
                        }
                        else {
                            buff->updateBalance(1);
                            this->insert_Helper(buff, avlNode);
                        }
                    }
                    else {
                        buff = buff->getRight();
                    }
                }
            }
        }
    }
    else { //AVL Tree is empty
        AVLNode<Key, Value>* buff = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        
        this->root_ = buff;

        buff->setBalance(0);
        buff->setLeft(nullptr);
        buff->setRight(nullptr);
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    Node<Key, Value>* node = this->internalFind(key);
    int height = 0;

    if (node) {
        if (node->getLeft() == nullptr && node->getRight() == nullptr) { //No child nodes
            AVLNode<Key, Value>* parent = (AVLNode<Key, Value>*)(node->getParent());

            if (parent) {
                if ((AVLNode<Key, Value>*)(node) == parent->getLeft()) {
                    height = 1;
                }
                else if ((AVLNode<Key, Value>*)(node) == parent->getRight()) {
                    height = -1;
                }
            }

            if (this->root_ == node) {
                this->root_ = nullptr;
            }
            else if (node->getParent()->getLeft() == node) {
                node->getParent()->setLeft(nullptr);
            }
            else {
                node->getParent()->setRight(nullptr);
            }

            delete node;
            remove_Helper(parent, height);
        }
        else if(node->getLeft() && node->getRight() == nullptr) { //Only left child node
            AVLNode<Key, Value>* parent = (AVLNode<Key, Value>*)(node->getParent());

            if (parent) {
                if ((AVLNode<Key, Value>*)(node) == parent->getLeft()) {
                    height = 1;
                }
                else if ((AVLNode<Key, Value>*)(node) == parent->getRight()) {
                    height = -1;
                }
            }

            if (this->root_ == node) {
                this->root_ = node->getLeft();
                node->getLeft()->setParent(nullptr);
            }
            else if (node->getParent()->getLeft() == node) {
                node->getParent()->setLeft(node->getLeft());
                node->getLeft()->setParent(node->getParent());
            }
            else {
                node->getParent()->setRight(node->getLeft());
                node->getLeft()->setParent(node->getParent());
            }

            delete node;
            remove_Helper(parent, height);
        }
        else if(node->getLeft() == nullptr && node->getRight()) { //Only right child node
            AVLNode<Key, Value>* parent = (AVLNode<Key, Value>*)(node->getParent());

            if (parent) {
                if ((AVLNode<Key, Value>*)(node) == parent->getLeft()) {
                    height = 1;
                }
                else if ((AVLNode<Key, Value>*)(node) == parent->getRight()) {
                    height = -1;
                }
            }

            if (this->root_ == node) {
                this->root_ = node->getRight();
                node->getRight()->setParent(nullptr);
            }
            else if (node->getParent()->getLeft() == node) {
                node->getParent()->setLeft(node->getRight());
                node->getRight()->setParent(node->getParent());
            }
            else {
                node->getParent()->setRight(node->getRight());
                node->getRight()->setParent(node->getParent());
            }

            delete node;
            remove_Helper(parent, height);
        }
        else if (node->getLeft() && node->getRight()) { //Has two child nodes
            AVLNode<Key, Value>* prev = (AVLNode<Key, Value>*)(this->predecessor(node));
            
            nodeSwap((AVLNode<Key, Value>*)(node), prev);

            if (this->root_ == node) {
                this->root_ = prev;
            }

            AVLNode<Key, Value>* parent = (AVLNode<Key, Value>*)(node->getParent());

            if (parent) {
                if ((AVLNode<Key, Value>*)(node) == parent->getLeft()) {
                    height = 1;
                }
                else if ((AVLNode<Key, Value>*)(node) == parent->getRight()) {
                    height = -1;
                }
            }

            if (node->getLeft()) {
                node->getLeft()->setParent(node->getParent());

                if (node->getParent()->getRight() == node) {
                    node->getParent()->setRight(node->getLeft());
                }
                else {
                    node->getParent()->setLeft(node->getLeft());
                }
            }
            else {
                if (node->getParent()->getRight() == node) {
                    node->getParent()->setRight(nullptr);
                }
                else {
                    node->getParent()->setLeft(nullptr);
                }
            }
            
            delete node;
            remove_Helper(parent, height);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>*& node) {
    AVLNode<Key, Value>* child = node->getRight();
    
    if(node->getParent() == nullptr){
        this->root_ = child;
        child->setParent(nullptr);
        node->setParent(child);
        node->setRight(nullptr);
        if(child->getLeft()){
            child->getLeft()->setParent(node);
            node->setRight(child->getLeft());
        }
        node->getParent()->setLeft(node);
    }
    else if(node == node->getParent()->getLeft()){
        child->setParent(node->getParent());
        node->getParent()->setLeft(child);
        node->setParent(child);
        if(child->getLeft()){
            child->getLeft()->setParent(node);
            node->setRight(child->getLeft());
        }
        else{
            node->setRight(nullptr);
        }
        node->getParent()->setLeft(node);
    }
    else if(node == node->getParent()->getRight()){
        child->setParent(node->getParent());
        node->getParent()->setRight(child);
        node->setParent(child);
        if(child->getLeft()){
            child->getLeft()->setParent(node);
            node->setRight(child->getLeft());
        }
        else{
            node->setRight(nullptr);
        }
        node->getParent()->setLeft(node);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>*& node) {
    AVLNode<Key, Value>* child = node->getLeft();

    if(node->getParent() == nullptr){
        this->root_ = child;
        child->setParent(nullptr);
        node->setParent(child);
        node->setLeft(nullptr);
        if(child->getRight()){
            AVLNode<Key, Value>* orphan = child->getRight();
            orphan->setParent(node);
            node->setLeft(orphan);
        }
        node->getParent()->setRight(node);
    }
    else if(node == node->getParent()->getLeft()){
        AVLNode<Key, Value>* child = node->getLeft();
        child->setParent(node->getParent());
        node->getParent()->setLeft(child);
        node->setParent(child);
        if(child->getRight()){
            AVLNode<Key, Value>* orphan = child->getRight();
            orphan->setParent(node);
            node->setLeft(orphan);
        }
        else{
            node->setLeft(nullptr);
        }
        node->getParent()->setRight(node);
    }
    else if(node == node->getParent()->getRight()){
        AVLNode<Key, Value>* child = node->getLeft();
        child->setParent(node->getParent());
        node->getParent()->setRight(child);
        node->setParent(child);
        if(child->getRight()){
            AVLNode<Key, Value>* orphan = child->getRight();
            orphan->setParent(node);
            node->setLeft(orphan);
        }
        else{
            node->setLeft(nullptr);
        }
        node->getParent()->setRight(node);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insert_Helper(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node) {
    if (parent == nullptr || parent->getParent() == nullptr) return;

    AVLNode<Key, Value>* grandpa = parent->getParent();

    if(grandpa->getLeft() == parent) {
        grandpa->updateBalance(-1);

        if (!grandpa->getBalance()) {
            return;
        }
        else if (grandpa->getBalance() == -1) {
            insert_Helper(grandpa, parent);
        }
        else if (grandpa->getBalance() == -2) {
            if (parent->getLeft() == node) {
                rotateRight(grandpa);
                grandpa->setBalance(0);
                parent->setBalance(0);
            }
            else {
                rotateLeft(parent);
                rotateRight(grandpa);

                if (!node->getBalance()) {
                    grandpa->setBalance(0);
                    parent->setBalance(0);
                    node->setBalance(0);
                }
                if (node->getBalance() == -1) {
                    grandpa->setBalance(1);
                    parent->setBalance(0);
                    node->setBalance(0);
                }
                if (node->getBalance() == 1) {
                    grandpa->setBalance(0);
                    parent->setBalance(-1);
                    node->setBalance(0);
                }
            }
        }
    }
    else if (grandpa->getRight() == parent) {
        grandpa->updateBalance(1);

        if (!grandpa->getBalance()) {
            return;
        }
        else if (grandpa->getBalance() == 1) {
            insert_Helper(grandpa, parent);
        }
        else if (grandpa->getBalance() == 2) {
            if (parent->getRight() == node) {
                rotateLeft(grandpa);
                grandpa->setBalance(0);
                parent->setBalance(0);
            }
            else {
                rotateRight(parent);
                rotateLeft(grandpa);

                if (!node->getBalance()) {
                    grandpa->setBalance(0);
                    parent->setBalance(0);
                    node->setBalance(0);
                }
                if (node->getBalance() == -1) {
                    grandpa->setBalance(0);
                    parent->setBalance(1);
                    node->setBalance(0);
                }
                if (node->getBalance() == 1) {
                    grandpa->setBalance(-1);
                    parent->setBalance(0);
                    node->setBalance(0);
                }
            }
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::remove_Helper(AVLNode<Key, Value>* node, int height) {
    if(node == nullptr) {
        return;
    }

    int width = 0;

    AVLNode<Key, Value>* parent = node->getParent();

    if (parent) {
        width = parent->getLeft() == node ? 1 : -1;
    }

    if (height == -1) {
        if (node->getBalance() + height == -2) {
            AVLNode<Key, Value>* pivot = node->getLeft();

            if (!pivot->getBalance()) {
                rotateRight(node);
                pivot->setBalance(1);
                node->setBalance(-1);
            }
            else if (pivot->getBalance() == -1) {
                rotateRight(node);
                pivot->setBalance(0);
                node->setBalance(0);
                remove_Helper(parent, width);
            }
            else if (pivot->getBalance() == 1) {
                AVLNode<Key, Value>* grandpa = pivot->getRight();

                rotateLeft(pivot);
                rotateRight(node);

                if (!grandpa->getBalance()) {
                    grandpa->setBalance(0);
                    pivot->setBalance(0);
                    node->setBalance(0);
                }
                else if (grandpa->getBalance() == -1) {
                    grandpa->setBalance(0);
                    pivot->setBalance(0);
                    node->setBalance(1);
                }
                else if (grandpa->getBalance() == 1) {
                    grandpa->setBalance(0);
                    pivot->setBalance(-1);
                    node->setBalance(0);
                }

                remove_Helper(parent, width);
            }
        }
        else if (node->getBalance() + height == -1) {
            node->setBalance(-1);
        }
        else if (node->getBalance() + height == 0) {
            node->setBalance(0);
            remove_Helper(parent, width);
        }
    }
    else if (height == 1) {
        if (node->getBalance() + height == 2) {
            AVLNode<Key, Value>* pivot = node->getRight();

            if (!pivot->getBalance()) {
                rotateLeft(node);
                pivot->setBalance(1);
                node->setBalance(-1);
            }
            else if (pivot->getBalance() == -1) {
                AVLNode<Key, Value>* grandpa = pivot->getLeft();
                
                rotateRight(pivot);
                rotateLeft(node);

                if (!grandpa->getBalance()) {
                    grandpa->setBalance(0);
                    pivot->setBalance(0);
                    node->setBalance(0);
                }
                else if (grandpa->getBalance() == -1) {
                    grandpa->setBalance(0);
                    pivot->setBalance(1);
                    node->setBalance(0);
                }
                else if (grandpa->getBalance() == 1) {
                    grandpa->setBalance(0);
                    pivot->setBalance(0);
                    node->setBalance(-1);
                }

                remove_Helper(parent, width);
            }
            else if (pivot->getBalance() + height == 1) {
                node->setBalance(1);
            }
        }
        else if (node->getBalance() + height == 1) {
            node->setBalance(1);
        }
        else if (node->getBalance() + height == 0) {
            node->setBalance(0);
            remove_Helper(parent, width);
        }
    }
}

#endif
