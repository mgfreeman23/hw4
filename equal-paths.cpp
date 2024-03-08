#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

// helper function to get leaf node height
bool leafHeight(Node* curr, int height, int& sharedHeight);

// equalPaths function
bool equalPaths(Node * root)
{
    // initialize leaf node height to dummy value
    int sharedHeight = -1;
    // call helper function
    return leafHeight(root, 0, sharedHeight);
}

bool leafHeight(Node* curr, int height, int& sharedHeight){
  // if empty subtree, return true
  if(curr == nullptr){
    return true;
  }
  // if at a leaf node
  if(curr->left == nullptr && curr->right == nullptr){
    // initialize the leaf node height to height
    if(sharedHeight == -1){
      sharedHeight = height;
      return true;
    } 
    // check if leaf node at same height as other leaf nodes
    else {
      return sharedHeight == height;
    }
  } 
  // if not a leaf node, iterate to left and right subtrees
  // and update height
  else {
    return leafHeight(curr->right, height + 1, sharedHeight) 
    && leafHeight(curr->left, height + 1, sharedHeight);

  }
  
}

