//
// Created on 2021/10/2.
//

#ifndef PS_3_TREE_H
#define PS_3_TREE_H

#include <string>

#include "AST.h"

struct TreeNode {
    string nodeName;                            // 固定长度的节点名称
    int type;                                   // 节点类型，当前主要用于标识符判定。来自于原语法树

    vector<TreeNode *> children;                // 若children大小为0，则说明这个节点是叶子节点

    TreeNode *parent = nullptr;                 // 父亲节点
    TreeNode *next[2] = {nullptr, nullptr};     // 后继，语法树中每个语句只有1个后继（statement/declaration），但是compound、选择和循环的判断处有2个后继

    //行号转换相关
    int row;
};

// 将gtree转变为TreeNode，更方便使用
TreeNode *gtree2TreeNode(gtree *head);

// 释放TreeNode占用的空间
void deleteTree(TreeNode *r);

/**
 * 预处理-找到每个语句的下一句
 * 传入参数应该为一个语法树根结点
 * 注意：1、并不会每一个语法树节点都有下一个语句，函数定义以及以上级别的节点不会有next，statement以及declaration则是最小粒度
 * @param r_ 语法树根结点
 */
void preProcessor_findNext(TreeNode *r_);


/**
 * 第二版本的预处理-链接树节点（该函数目前不用）
 * 只链接一步，寻找函数也要对应修改
 * @param r_
 */
void preProcessor_linkTree(TreeNode *r_);

#endif //PS_3_TREE_H
