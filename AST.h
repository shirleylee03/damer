//
// Created on 2021/11/16.
//

#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<set>

using namespace std;
#define MAX_LENGTH 3000
#define MAX_WORD_NUM 1000
#define MAX_BUFF_SIZE 40
#define tb_sum 20
#define kong -1
#define ERROR1 -1

//enum item
//{
//	关键字 = 1, 标识符, 无符号整数, 赋值号, 算符, 界符, 冒号, 分隔符, 注释号, 左括号, 右括号, 左中括号, 右中括号, 左大括号, 右大括号, 字母, 无符号实数, 结束符, 字符串, 字符,
//	程序, 常量说明, 常量定义, 整数, 实数, 声明头部, 变量说明, 变量定义, 可枚举常量, 类型标识符, 函数定义, 复合语句, 参数表, 主函数, 表达式,
//	项, 因子, 语句, 赋值语句, 条件语句, 条件, 循环语句, 情况语句, 情况表, 情况子语句, 缺省, 函数调用语句, 值参数表, 语句列, 读语句, 写语句, 返回语句, 条件组, 子项, break语句
//};



class gtree
{
public:
    //保留部分，添加属性信息
    int num;//先序节点号
    string place;
    int No_Child;//孩子的个数
    //string code;
    //保留部分，添加属性信息
    gtree *next;//兄弟指针
    gtree *child;//孩子指针
    gtree *parent;//父节点指针
    int type;//节点类型syn
    string contain_call;//存放包含的函数调用语句V_name
    bool contain_call_flag;
    int record_P_num;
    string label_P;//记录前向的label库所，用于goto语句的连接
    vector<pair<string,string>> para;//for compound node:para places' name
    string matched_P;//语法树节点对应语句库所
    string enter_P;//
    bool is_processed;
    int row;

    gtree()
    {
        num = -1;
        place = "";
        next = child = parent = NULL;
        type = -1;
        contain_call = "";
        contain_call_flag = false;
        record_P_num = 0;
        label_P = "";
        matched_P = "";
        is_processed = false;
    }
    void init(int root);//初始化
    void expand(int num, gtree *t, int sum);
    void cnode(int syn, int syn_1, gtree *t);
    void newlable(int a);
    string newlable1(int a, int flag);
    //void gen(string a);
};

gtree *&create_tree(string filename);
gtree *&create_tree_withoutTraverse(string filename);
string get_gen_P();

gtree* find_last_sentence(gtree *node, bool &flag, bool &flag1);

bool judge_assign_statement(gtree *statement1);
bool judge_call_postfix_expression(gtree *postfix_expression);
bool judge_call_statement(gtree *statement1);
bool judge_return(gtree *statement1);
bool judge_goto(gtree *statement1);
bool judge_break(gtree *statement1);
bool judge_expression_statement(gtree *statement1);
bool judge_label_statement(gtree *statement1);
//void pre_process(string &s);
bool judge_statement(gtree *p);
bool judge_compound_statement(gtree *statement1);
bool judge_jump_statement(gtree *statement1);

void cut_tree(gtree *p);
extern string call_statement_suffix;
void intofile_tree(gtree *tree);
void free_tree(gtree *p);
void toFileRowTree(gtree *head);