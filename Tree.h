//
// Created on 2021/10/2.
//

#ifndef PS_3_TREE_H
#define PS_3_TREE_H

#include <string>

#include "AST.h"

struct TreeNode {
    string nodeName;                            // node name
    int type;                                   // Node type, which is currently mainly used for identifier determination. From primitive tree

    vector<TreeNode *> children;                // If the children size is 0, it indicates that this node is a leaf node

    TreeNode *parent = nullptr;                 // fater node
    TreeNode *next[2] = {nullptr, nullptr};     // There is only one statement / declaration for each statement in the syntax tree, but there are two successors in the judgment of compound, selection and loop

    //Line number conversion correlation
    int row;
};

// Turn gtree into treenode, which is more convenient to use
TreeNode *gtree2TreeNode(gtree *head);

// Free up space occupied by treenode
void deleteTree(TreeNode *r);

/**
 * Preprocessing - find the next sentence of each statement
 * The passed in parameter should be a syntax tree root node
 * Note: 1. Not every syntax tree node will have a next statement. Nodes at function definition and above levels will not have next. Statement and declaration are the minimum granularity
 * @param r_ the root node of ast
 */
void preProcessor_findNext(TreeNode *r_);


/**
 * Preprocessing of the second version - link tree node (this function is not used at present)
 * Only link one step, and the search function should be modified accordingly
 * @param r_
 */
void preProcessor_linkTree(TreeNode *r_);

#endif //PS_3_TREE_H
