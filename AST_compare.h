//
// Created on 2021/11/16.
//

#ifndef PDNET2_0_AST_COMPARE_H
#define PDNET2_0_AST_COMPARE_H

#include"AST.h"
#include"cpn.h"
#include<vector>
#define minHeight 3

class tree_height
{
public:
    gtree *point;
    int height;
};

class HighIndexList
{
public:
    vector<tree_height> list;

    void order();

    void push(gtree *t);
    vector<gtree *>pop();
    int peekMax();
    void open(gtree *t);
};

class Mapping
{
public:
    gtree *map1;
    gtree *map2;
    int num;
    Mapping(gtree *m1, gtree *m2, int n):map1(m1),map2(m2),num(n){}
    Mapping(){map1 = NULL;map2 = NULL;num=-1;};
};

class AST_change
{
public:
    Mapping m;//map
    vector<pair<int, int>> modify, add, del, move;
};

void top_down(gtree *t1, gtree *t2, vector<Mapping> &M);
vector<Mapping> get_MatchStatement(vector<Mapping> &M);
vector<AST_change> extract_change(gtree *t1, gtree *t2, vector<Mapping> &M, vector<Mapping> &M_statement);
void extract_changeNodes(vector<Mapping> &M_statement,vector<Mapping> &M,vector<gtree *> &vec_t1,vector<gtree *> &vec_t2);
gtree *get_statementNodeFromPos(gtree *firstStatement,int pos);
#endif //PDNET2_0_AST_COMPARE_H
