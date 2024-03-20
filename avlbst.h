#ifndef AVLBST_H
#define AVLBST_H

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
* An explicit constructor to initialize the elements by calling the base class constructor
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
    virtual void insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node);
    virtual void removeFix(AVLNode<Key,Value>* parent, int diff);

    // add rotateRight and rotateLeft helpers
    virtual void rotateRight(AVLNode<Key, Value>* node);
    virtual void rotateLeft(AVLNode<Key, Value>* node);

};


/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO - there might be some polymorphism issues with this!

    // GET HELP HERE!! issues with root_ and how to use it
    
    // in case of empty tree 
    Node<Key, Value>* current = BinarySearchTree<Key, Value>::root_;

    if(current == NULL){
        BinarySearchTree<Key, Value>::root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        // avlRoot = new AVLNode<Key,Value>(new_item.first, new_item.second, NULL);
        return;
    }
    // if node with key already in the tree, reset its value
    if(BinarySearchTree<Key,Value>::internalFind(new_item.first)){
        Node<Key,Value> *c = BinarySearchTree<Key,Value>::internalFind(new_item.first);
        c->setValue(new_item.second);
        return;
    }

    // // go to root
    // AVLNode<Key, Value> *curr = avlRoot;
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(current);

    // tree traversal
    while(true){
      // if less than go to left
      if(new_item.first < curr->getKey()){
        // if at point to insert, create and add new node
        if(curr->getLeft() == NULL){
            AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, curr);
            curr->setLeft(newNode);
            // if node with right subtree, new balance will be 0
            if(curr->getBalance() == 1){
                curr->updateBalance(-1);
            } 
            // otherwise balance is 0 and call insertFix
            else {
                //update parent balance and call insertFix
                curr->updateBalance(-1);
                insertFix(curr, newNode);
            }
            return;
        }
        // else keep traversing
        curr = curr->getLeft();
       } 
       // if key greater go right
       else {
          // if at point to insert, create and add new node
          if (curr->getRight() == NULL){
            AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, curr);
            curr->setRight(newNode);
            // if node with left subtree, new balance will be 0
            if(curr->getBalance() == -1){
                curr->updateBalance(1);
            } 
            // otherwise balance is 0 and call insertFix
            else {
                // update parent balance and call insertFix
                curr->updateBalance(1);
                insertFix(curr, newNode);
            }
            return;
          }
          // else keep traversing
          curr = curr->getRight();
        }
      }
}

// helper function for insert: 
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node){

    if(parent == NULL || parent->getParent() == NULL){
        return;
    }

    AVLNode<Key, Value>* gparent = parent->getParent();

    // if parent is the left child
    if(gparent->getLeft() == parent){
        // update gparent balance
        gparent->updateBalance(-1);
        // do nothing if new balance is zero
        if(gparent->getBalance() == 0){
            return;
        }
        // recurse if new balance is -1 
        else if (gparent->getBalance() == -1){
            insertFix(gparent, parent);
        } 
        // rotations if new balance -2
        else {
            // if node added is left child of parent, zig-zig
            if(node == parent->getLeft()){
                rotateRight(gparent);

                parent->setBalance(0);
                gparent->setBalance(0);   

            } 
            // if node added is right child, zig-zag
            else {
                rotateLeft(parent);
                rotateRight(gparent);
                // update balances (original node 0, 1, -1)
                // original node balance is 0
                if(node->getBalance() == 0){
                  parent->setBalance(0);
                  gparent->setBalance(0);
                } 
                // original node balance is -1
                else if (node->getBalance() == -1){
                  parent->setBalance(0);
                  gparent->setBalance(1);
                  node->setBalance(0);
                } 
                // original node balance is 1
                else {
                  parent->setBalance(-1);
                  gparent->setBalance(0);
                  node->setBalance(0);
                }
            }
        }
    } 
    // if parent is the right child
    else {
        // update gparent balance
        gparent->updateBalance(1);
        // do nothing if new balance is 0
        if(gparent->getBalance() == 0){
            return;
        } 
        // recurse if new balance is 1
        else if (gparent->getBalance() == 1){
            insertFix(gparent, parent);
        } 
        // rotations if new balance is 2
        else {
            // if node added is right child of parent, zig-zig
            if(node == parent->getRight()){
                rotateLeft(gparent);
                parent->setBalance(0);
                gparent->setBalance(0);
            } 
            // if node added is left-child, zig-zag
            else {
                rotateRight(parent);
                rotateLeft(gparent);
                // add code here to update balances
                if(node->getBalance() == 0){
                  parent->setBalance(0);
                  gparent->setBalance(0);
                } else if (node->getBalance() == -1){
                  parent->setBalance(1);
                  gparent->setBalance(0);
                  node->setBalance(0);
                } else {
                  node->setBalance(0);
                  parent->setBalance(0);
                  gparent->setBalance(-1);
                }
            }
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
 // CHECK THIS !! follow slide pseudocode
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO

    // if the key is not in the BST do nothing
    if(BinarySearchTree<Key, Value>::internalFind(key) == NULL){
      return;
    }

    // traverse the tree to find node to remove
    AVLNode<Key,Value>* toRemove = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::root_);;
    while(true){
        if(toRemove->getKey() == key){
            break;
        }
        if(toRemove->getKey() < key){
            toRemove = toRemove->getRight();
        } else {
            toRemove = toRemove->getLeft();
        }
    }
    // if the node to be removed has two children
    if(toRemove->getLeft() != NULL && toRemove->getRight() != NULL){
      // swap it with its in-order predecessor. polymorphism issues here??
      AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(static_cast<Node<Key, Value>*>(toRemove)));
      nodeSwap(toRemove, pred);
    } 
    // get parent of node to remove
    AVLNode<Key, Value> *parent = toRemove->getParent();
    int diff = 0;
    // check if parent node exists
    if(parent != NULL){
        if(parent->getLeft() == toRemove){
            // do we need to update balance here or should we just set diff??
            // parent->updateBalance(1);
            diff = 1;
        } else {
            // parent->updateBalance(-1);
            diff = -1;
        }
    }

    // CHECK HERE
    // find child if there is one
    AVLNode<Key, Value>* child = NULL;
    if(toRemove->getLeft() != NULL){
      child = toRemove->getLeft();
    } else if (toRemove->getRight() != NULL){
      child = toRemove->getRight();
    }
    // case of no child
    if(child == NULL){
      // in case only node in tree
      if(toRemove->getParent() == NULL){
        BinarySearchTree<Key, Value>::root_ = NULL;
      } 
      // in case left child of parent
      else if (toRemove == toRemove->getParent()->getLeft()){
        toRemove->getParent()->setLeft(NULL);
      }
      // in case right child of parent
       else {
        toRemove->getParent()->setRight(NULL);
      }
    } 
    // in case of one child
    else {
      // only two nodes in tree (toRemove and child)
      if(toRemove->getParent() == NULL){
        BinarySearchTree<Key, Value>::root_ = child;
      } 
      // in case left child of parent
      else if (toRemove == toRemove->getParent()->getLeft()){
        toRemove->getParent()->setLeft(child);
      } 
      // in case right child of parent
      else {
        toRemove->getParent()->setRight(child);
      }
      // set the parent of child
      child->setParent(toRemove->getParent());
    }
    // deallocate node to remove
    delete toRemove;
    //call helper function
    removeFix(parent, diff);
}

// helper function for remove
template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* parent, int diff){
    // if no parent, do nothing
    if(parent == NULL){
        return;
    }
    // get values for next recurisive call
    AVLNode<Key,Value>* gparent = parent->getParent();
    int ndiff = 0;
    if(gparent != NULL){
        if(parent == gparent->getLeft()){
            ndiff = 1;
        } else {
            ndiff = -1;
        }
    }

    // case if a right node was removed
    if(diff == -1){
        // if parent becomes unbalanced
        if(parent->getBalance() + diff == -2){
            AVLNode<Key,Value>* tallChild = parent->getLeft();
            // zig-zig case
            if(tallChild->getBalance() == -1){
                rotateRight(parent);
                parent->setBalance(0);
                tallChild->setBalance(0);
                // b(n) == b(tallChild) == 0?
                removeFix(gparent, ndiff);
            } 
            // another zig-zig case
            else if(tallChild->getBalance() == 0){
                rotateRight(parent);
                // are these adjustments right??
                parent->setBalance(-1);
                tallChild->setBalance(1);
            } 
            // zig-zag case
            else {
                AVLNode<Key,Value>* gChild = tallChild->getRight();
                rotateLeft(tallChild);
                rotateRight(parent);
                // update balances appropriately
                if(gChild->getBalance() == 1){
                    parent->setBalance(0);
                    tallChild->setBalance(-1);
                    gChild->setBalance(0);
                } else if(gChild->getBalance() == 0){
                    parent->setBalance(0);
                    tallChild->setBalance(0);
                } else {
                    parent->setBalance(1);
                    tallChild->setBalance(0);
                    gChild->setBalance(0);
                }
                // recursive call
                removeFix(gparent, ndiff);
            }
        } 
        // simplest case
        else if (parent->getBalance() + diff == -1){
            parent->setBalance(-1);
            return;
        } 
        // if adding diff makes balance 0, recurse
        else {
            parent->setBalance(0);
            removeFix(gparent, ndiff);
        }
    } 
    // mirror case if left node was removed
    else if (diff == 1){
        // if parent becomes unbalanced
        if(parent->getBalance() + diff == 2){
            // add all the subcases here
             AVLNode<Key,Value>* tallChild = parent->getRight();
             // zig-zig case
             if(tallChild->getBalance() == 1){
                rotateLeft(parent);
                parent->setBalance(0);
                tallChild->setBalance(0);
                removeFix(gparent, ndiff);
             } 
             // another zig-zig case
             else if (tallChild->getBalance() == 0){
                rotateLeft(parent);
                parent->setBalance(1);
                tallChild->setBalance(-1);
             } 
             // zig-zag case
             else {
                AVLNode<Key,Value>* gChild = tallChild->getLeft();
                rotateRight(tallChild);
                rotateLeft(parent);
                // update balances
                if(gChild->getBalance() == 1){
                    parent->setBalance(-1);
                    tallChild->setBalance(0);
                    gChild->setBalance(0);
                } else if(gChild->getBalance() == 0){
                    parent->setBalance(0);
                    gChild->setBalance(0);
                    tallChild->setBalance(0);
                } else {
                    parent->setBalance(0);
                    tallChild->setBalance(1);
                    gChild->setBalance(0);
                }
                // recursive call
                removeFix(gparent, ndiff);
             }
        } 
        // simplest case
        else if (parent->getBalance() + diff == 1){
            parent->setBalance(1);
            return;
        }
        // case to recurse 
        else {
            parent->setBalance(0);
            removeFix(gparent, ndiff);
        }
    }
}

// rotate right helper function
template<typename Key, typename Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node){
    // idk if I need this
    if(node == NULL || node->getLeft() == NULL){
        return;
    }
    //make left child new parent
    AVLNode<Key,Value>* newParent = node->getLeft();
    // set the new parent of left child so tree stays connected
    newParent->setParent(node->getParent());

    // if node to be rotated had a parent, adjust pointers
    if(node->getParent() != NULL){
        // if it was the left child
        if(node == node->getParent()->getLeft()){
            node->getParent()->setLeft(newParent);
        } 
        // if it was the right child
        else {
            node->getParent()->setRight(newParent);
        }
    } 
    // if it was the root of the AVL tree
    else {
        BinarySearchTree<Key, Value>::root_ = static_cast<Node<Key, Value>*>(newParent);
    }
    
    // set left child of node to be rotated
    node->setLeft(newParent->getRight());
    // set the parent correctly of right child if it exists
    if(newParent->getRight()!= NULL){
        newParent->getRight()->setParent(node);
    }

    // set the right child of node's left child to be node
    newParent->setRight(node);
    //set the new parent of node
    node->setParent(newParent);
}

// rotate left helper function
template<typename Key, typename Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node){
    // precondition for this to work
    if(node == NULL || node->getRight() == NULL){
        return;
    }
    // make right child the new parent
    AVLNode<Key,Value>* newParent = node->getRight();
    // set the new parent of right child so tree stays connected
    newParent->setParent(node->getParent());
    // connect the tree if parent exists
    if(node->getParent() != NULL){
        if(node == node->getParent()->getLeft()){
            node->getParent()->setLeft(newParent);
        } else {
            node->getParent()->setRight(newParent);
        }
    } 
    // otherwise update root
    else {
        BinarySearchTree<Key, Value>::root_ = static_cast<AVLNode<Key, Value>*>(newParent);
    }
    // set right child of node to be rotated
    node->setRight(newParent->getLeft());
    // update parent pointers if necessary
    if(newParent->getLeft() != NULL){
        newParent->getLeft()->setParent(node);
    }
    // set node to left child of new parent
    newParent->setLeft(node);

    // set parent of node
    node->setParent(newParent);

}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
