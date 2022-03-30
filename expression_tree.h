//
// Created on 2020/12/1.
//

#ifndef CC_TESTING_EXPRESSION_TREE_H
#define CC_TESTING_EXPRESSION_TREE_H

#include<string>
#include<map>
//#define TokenNum_t unsigned short
#define CaseFlag "case$" // case expression
#define CaseFlagLength 5
using namespace std;

enum CTN_type{TokenOperator,Token,Tuple,CaseOperator,Operator,color,variable,null,uninitial};
typedef unsigned short TokenNum_t;

//**  'value' stands for different thing for different node          **//
//**  1.For 'color' and 'variable', it stands for expression value   **//
//**  2.For 'CaseOperator', it stands for one of those case          **//

//**  'node_name' stands for different thing for different node  **//
//**  1.For a 'variable' node,it stands for variable name            **//
//**  2.For a 'color' node,it stands for color name                  **//
//**  3.For 'TokenOperator' and 'Operator',it stands for operation     **//
//**  4.For 'CaseOperator',it stands for variable name               **//

//** 'TokenOperator' stands for only '++' for now **//
//** 'ConditionOperator' stands for only 'if' and 'case' **//
//** 'RelationOperator' and 'Operator' can see in 'condition_tree.cpp' **//

class condition_tree_node
{
public:
    CTN_type node_type;
    string node_name;
    string value;
    TokenNum_t num;
    condition_tree_node *left;
    condition_tree_node *right;
    condition_tree_node *condition;
    condition_tree_node(){left=right=condition=NULL;num=0;node_name=value="uninitialed";node_type=uninitial;}
};

class condition_tree
{
private:
    condition_tree_node *root;
    string exp;
    bool isrefer;//refer condition_tree don't need to free
public:
    condition_tree_node* getroot() const{return root;}
    string getexp() const{return exp;}
    void construct(string s);
//    condition_tree(string s){exp = s;root = construct(s);}
    ~condition_tree(){
        if(!isrefer)
            deconstruct();
    }
    void deconstruct();
    condition_tree(){root = NULL;exp = "";}
    void operator=(const condition_tree &ct);
//    void operator=(condition_tree &&ct);
    void referencetree(const condition_tree &ct);
};

extern int is_operator(string s, int pos);
extern int priority(string s);
extern bool is_unary(string s);

#endif //CC_TESTING_EXPRESSION_TREE_H
