
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<vector>
#include<string>
#include<sstream>
#include<iomanip>
#include<fstream>
#include<set>

#include"AST.h"
#include"y.tab.h"
#include"base.h"

set<string> build_in_type {"void","char","short","int","long","float","double"};
extern string return_suffix;

extern int yyparse(void);
extern FILE *yyin;
extern gtree* head;
void printtree(gtree* t);

#define keywordNum 20
using namespace std;

extern string rg_dirname, rg_sliceOnly_dirname, origin_dirname, newfile_dirname;
int sort_num = 0;
int gen_sel_num, gen_iter_num, gen_jump_num, gen_com_num;


void travel_tree(gtree *tree,ofstream &out,vector<pair<string,int>> &list)
{

    if (tree == NULL)
        return;
    string parent_place, child_place;
    int parent_no, child_no;
    if (tree->parent != NULL)
    {

        parent_place = tree->parent->place;
        if(parent_place[0] == '"')
            parent_place = parent_place.substr(1,parent_place.size()-2);
        parent_no = tree->parent->num;
        child_place = tree->place;
        if(child_place[0] == '"')
            child_place = child_place.substr(1,child_place.size()-2);
        child_no = tree->num;

        out << "node" << to_string(parent_no) << "[label=\"" << parent_place << "\"]" << endl;
        out << "node" << to_string(child_no) << "[label=\"" << child_place << "\"]" << endl;
        out << "node" << to_string(parent_no) << "->" << "node" << to_string(child_no) << ";" << endl;

    }
    travel_tree(tree->child, out, list);
    travel_tree(tree->next, out, list);


    //out.close();
}

void generate_rowTree(gtree *tree,ofstream &out,vector<pair<string,int>> &list)
{

    if (tree == NULL)
        return;
    string parent_place, child_place;
    int parent_no, child_no;
    if (tree->parent != NULL)
    {

        parent_place = tree->parent->place;
        if(parent_place[0] == '"')
            parent_place = parent_place.substr(1,parent_place.size()-2);
        parent_no = tree->parent->num;
        child_place = tree->place;
        if(child_place[0] == '"')
            child_place = child_place.substr(1,child_place.size()-2);
        child_no = tree->num;

        out << "node" << to_string(parent_no) << "[label=\"" << parent_place << "\nrow:"<<tree->parent->row << "\"]" << endl;
        out << "node" << to_string(child_no) << "[label=\"" << child_place << "\nrow:"<<tree->row << "\"]" << endl;
        out << "node" << to_string(parent_no) << "->" << "node" << to_string(child_no) << ";" << endl;

    }
    generate_rowTree(tree->child, out, list);
    generate_rowTree(tree->next, out, list);


    //out.close();
}

void toFileRowTree(gtree *head){
    ofstream out;
    vector<pair<string,int>> list;
    out.open("rowTree.dot", ios::out);

    out << "digraph G{" << endl;
    generate_rowTree(head, out, list);
    out << "}" << endl;
    out.close();

    makeGraph("rowTree.dot", "rowTree.png");
}

//output gtree into 'tree.dot'
void intofile_tree(gtree *tree)
{
    ofstream out;
    vector<pair<string,int>> list;
    out.open("tree.dot", ios::out);

    out << "digraph G{" << endl;
    travel_tree(tree, out, list);
    out << "}" << endl;
    out.close();

}
string gen_sel()
{
    string temp = "";
    return temp + "sel" + to_string(gen_sel_num++);
}

string gen_iter()
{
    string temp = "";
    return temp + "iter" + to_string(gen_iter_num++);
}

string gen_jump()
{
    string temp = "";
    return temp + "jump" + to_string(gen_jump_num++);
}

string gen_com()
{
    string temp = "";
    return temp + "com" + to_string(gen_com_num++);
}

bool judge_assign_statement(gtree *statement1)
{
    if (statement1->type==STATEMENT && statement1->child->type == EXPRESSION_STATEMENT
    && statement1->child->child && statement1->child->child->type == EXPRESSION
    && statement1->child->child->child && statement1->child->child->child->type == ASSIGNMENT_EXPRESSION
    && statement1->child->child->child->child && statement1->child->child->child->child->type != CONDITIONAL_EXPRESSION)
        return true;
    else
        return false;
}

bool judge_call_postfix_expression(gtree *postfix_expression)
{
    if (postfix_expression->type == POSTFIX_EXPRESSION && postfix_expression->child != NULL
        && postfix_expression->child->type == POSTFIX_EXPRESSION && postfix_expression->child->next != NULL
        && postfix_expression->child->next->type == REMAIN && postfix_expression->child->next->place == "(")
        return true;
    else
        return false;
}

bool judge_call_statement(gtree *statement1)
{
    if (statement1->child!=NULL && statement1->child->type == EXPRESSION_STATEMENT && statement1->child->child->type == EXPRESSION
        && statement1->child->child->child->type == ASSIGNMENT_EXPRESSION && statement1->child->child->child->child->type == CONDITIONAL_EXPRESSION)
    {
        gtree *temp = statement1;
        while (temp->type != POSTFIX_EXPRESSION)
            temp = temp->child;
        if (judge_call_postfix_expression(temp))
            return true;
    }
    return false;
}

bool judge_return(gtree *statement1)
{
    if (statement1->type == STATEMENT && statement1->child->type == JUMP_STATEMENT && statement1->child->child->type == RETURN)
        return true;
    return false;
}

bool judge_goto(gtree *statement1)
{
    if (statement1->type == STATEMENT && statement1->child->type == JUMP_STATEMENT && statement1->child->child->type == GOTO)
        return true;
    return false;
}

bool judge_break(gtree *statement1)
{
    if (statement1->type == STATEMENT && statement1->child->type == JUMP_STATEMENT && statement1->child->child->type == BREAK)
        return true;
    return false;
}

bool judge_label_statement(gtree *statement1){
    if (statement1->type == STATEMENT && statement1->child->type == LABELED_STATEMENT)
        return true;
    return false;
}

bool judge_select_statement(gtree *statement1){
    if (statement1->type == STATEMENT && statement1->child->type == SELECTION_STATEMENT)
        return true;
    return false;
}

bool judge_iteration_statement(gtree *statement1){
    if (statement1->type == STATEMENT && statement1->child->type == ITERATION_STATEMENT)
        return true;
    return false;
}

bool judge_expression_statement(gtree *statement1)
{
    if (statement1->type == STATEMENT && statement1->child->type == EXPRESSION_STATEMENT)
        return true;
    return false;
}

bool judge_compound_statement(gtree *statement1)
{
    if(statement1->type == STATEMENT && statement1->child->type == COMPOUND_STATEMENT)
        return true;
    return false;
}

bool judge_jump_statement(gtree *statement1)
{
    if(statement1->type == STATEMENT && statement1->child->type == JUMP_STATEMENT)
        return true;
    return false;
}

bool judge_statement(gtree *p)
{
    //judge the type of statements
    if(judge_compound_statement(p)
       ||judge_select_statement(p)
       ||judge_iteration_statement(p)
       ||judge_label_statement(p)
       ||judge_expression_statement(p)
       ||judge_jump_statement(p)
       )
        return true;
    else
        return false;
}


bool judge_inside_compound_statement(gtree *statement1)//
{
    gtree *temp = statement1;
    while (temp && temp->type != COMPOUND_STATEMENT)
        temp = temp->parent;
    if (temp==NULL || temp->parent->type == FUNCTION_DEFINITION)
        return false;
    return true;
}

void TraverseTree2(gtree *p)
{
    if (p == NULL)return;
    if (p->type == COMPOUND_STATEMENT)
        p->place = gen_com();
    TraverseTree2(p->child);
    TraverseTree2(p->next);

    if (p->type == EXPRESSION || p->type == ASSIGNMENT_EXPRESSION || p->type == UNARY_EXPRESSION || p->type == ASSIGNMENT_EXPRESSION || p->type == CONDITIONAL_EXPRESSION
        || p->type == LOGICAL_OR_EXPRESSION || p->type == LOGICAL_AND_EXPRESSION || p->type == INCLUSIVE_OR_EXPRESSION || p->type == EXCLUSIVE_OR_EXPRESSION
        || p->type == AND_EXPRESSION || p->type == EQUALITY_EXPRESSION || p->type == RELATIONAL_EXPRESSION || p->type == SHIFT_EXPRESSION || p->type == ADDITIVE_EXPRESSION
        || p->type == MULTIPLICATIVE_EXPRESSION ||  (p->type == POSTFIX_EXPRESSION && !judge_call_postfix_expression(p)) || p->type == PRIMARY_EXPRESSION
        || p->type == UNARY_OPERATOR ||  p->type == ARGUMENT_EXPRESSION_LIST || p->type == ASSIGNMENT_OPERATOR || p->type == CONSTANT_EXPRESSION)//内部变量声明place需要带有函数前缀
    {
        if(p->child->next && (p->child->next->type == INC_OP || p->child->next->type == DEC_OP))
            p->place = p->child->place;
        else if(p->child->type == INC_OP)
            p->place = p->child->next->place + "+1";
        else if(p->child->type == DEC_OP)
            p->place = p->child->next->place + "-1";
        else {
            gtree *p1 = p->child;
            string temp_place = "";
            while (p1) {
                temp_place += p1->place;
                p1 = p1->next;
            }
            p->place = temp_place;
        }
    }
    else if(p->type == type_name){
        //just support sizeof(TYPE_SPECIFIER) pattern
        gtree *type_spe = p;
        while(type_spe && type_spe->type!=TYPE_SPECIFIER)
            type_spe = type_spe->child;
        if(type_spe)
            p->place = type_spe->child->place;
    }
    else if(p->type == CAST_EXPRESSION){
        //type conversion now just simplely ignore it
        if(p->child->type == UNARY_EXPRESSION){
            p->place = p->child->place;
        }
        else{
            p->place = p->child->next->next->next->place;
        }
    }
    else if (p->type == SELECTION_STATEMENT)
    {
        p->place = gen_sel();
    }
    else if (p->type == ITERATION_STATEMENT)
    {
        p->place = gen_iter();
    }
    else if (p->type == JUMP_STATEMENT)
    {
        p->place = gen_jump();
    }
    else if (p->type == LABELED_STATEMENT)
    {
        gtree *p1 = p->child;
        string temp_place = " ";
        temp_place += p1->place;
        if (p1->next->place != ":")
            temp_place += p1->next->place;
        p->place = temp_place;
        p->parent->place = temp_place;
    }
    else if (p->type == STORAGE_CLASS_SPECIFIER)
    {
        p->place = p->child->place;
    }
    else if (p->type == TYPE_SPECIFIER)
    {
        gtree *p1 = p->parent;
        string type = "";
        while(p1->type != DECLARATION_SPECIFIERS && p1->type !=SPECIFIER_QUALIFIER_LIST)
            p1=p1->parent;

        if(build_in_type.find(p->child->place)!=build_in_type.end() || p->child->type == TYPE_NAME)
            type = p->child->place;
        else if(p->child->type == STRUCT_OR_UNION_SPECIFIER) {
            if (p->child->child->next->type == IDENTIFIER)
                type = p->child->child->next->place;
            else{
                type = p->child->place;
            }
        }
        if(type != "")
            while(p1->type == DECLARATION_SPECIFIERS) {
                p1->place = type;
                p1=p1->parent;
            }
    }
    else if(p->type == DECLARATION_SPECIFIERS && p->place == "declaration_specifiers"){
        p->place = "int";
        // in case of : unsigned (int) value
        // in case of : typedef struct{...}A;
    }
    else if (p->type == TYPE_QUALIFIER)
    {
        p->place = p->child->place;
    }
    else if (p->type == FUNCTION_DEFINITION)
    {
//        if(p->child->type != DECLARATION_SPECIFIERS){
//            cout << "we don't support function that don't have return type for now!" << endl;
//            exit(-1);
//        }
//        if(p->child->next->child->type == POINTER)
//            p->child->place = "int";

        gtree *temp;
        if (p->child->type == DECLARATION_SPECIFIERS)
            if (p->child->next->child->type == POINTER)
                temp = p->child->next->child->next->child;
            else
                temp = p->child->next->child->child;
        else
        if (p->child->child->type == POINTER)
            temp = p->child->child->next->child;
        else
            temp = p->child->child->child;
        while (temp->type != IDENTIFIER)
            temp = temp->child;
        p->place = temp->place;
    }
    else if (p->type == POSTFIX_EXPRESSION && judge_call_postfix_expression(p))
    {
        if (p->place=="postfix_expression")
            p->place = p->child->child->place + return_suffix;

        gtree *statement = p;
        while (statement->type != STATEMENT)
            statement = statement->parent;
        statement->contain_call_flag = true;
        statement->contain_call = p->child->child->place;

    }
    else if (p->type == STATEMENT)
    {
        if (p->child->type == SELECTION_STATEMENT || p->child->type == ITERATION_STATEMENT)
            p->place = p->child->place;
        else if (p->child->type == EXPRESSION_STATEMENT)
        {
            p->place = p->child->place = p->child->child->place;
//            if (judge_assign_statement(p))
//                p->place = p->child->child->child->place;
//            else if (judge_call_statement(p))
//            {
//                gtree *temp = p, *temp_postfix_expression = NULL;
//                while (temp->type != PRIMARY_EXPRESSION)
//                {
//                    if (temp->type == POSTFIX_EXPRESSION && temp_postfix_expression == NULL)
//                        temp_postfix_expression = temp;
//                    temp = temp->child;
//                }
//                p->place = temp->place + call_statement_suffix;
//                temp_postfix_expression->place = temp->place + call_statement_suffix;
//            }
//            else if(p->child->child->place == ";")// ';' expression
//                p->place = p->child->child->place;
//            else
//                p->place = p->child->child->child->place;
        }
        else if (p->child->type == JUMP_STATEMENT || p->child->type == COMPOUND_STATEMENT)
            p->place = p->child->place;
    }
    else if (p->type == INIT_DECLARATOR)
    {
        if (p->child->child->type == POINTER)
            p->place = p->child->child->next->place;
        else
            p->place = p->child->child->place;
    }
    else if (p->type == DIRECT_DECLARATOR)
    {
        gtree *id = p;
        while (id->type != IDENTIFIER)
            id = id->child;
        p->place = id->place;
    }

    return;
}
void TraverseTree3(gtree *p)
{
    if (p == NULL)
        return;
    p->num = sort_num++;

    TraverseTree3(p->child);
    TraverseTree3(p->next);
}

void Traverse(gtree *p);

void reset_gen_ast()
{
    gen_com_num = 0;
    gen_iter_num = 0;
    gen_jump_num = 0;
    gen_sel_num = 0;
    sort_num = 0;
}

/*function：generate an AST；
 * Input:filename,flag(true if needs preprocess)
 * Output:AST
 * */
gtree *&create_tree(string filename)
{
    reset_gen_ast();
//    if (pre_process_flag == true)
//    {
//        //string filename = "a.txt";
//        string filename1 = filename;
//        string whole_filename1 = filename1.substr(0,filename1.rfind('.')) + ".iii";
//        ifstream fin;
//        ofstream fout;
//        fin.open(filename, ios::in);
//        if (!fin.is_open())
//        {
//            cout << filename << " not exist" << endl;
//            exit(-1);
//        }
//        string temp_s;
//        istreambuf_iterator<char> beg(fin), end;
//        temp_s = string(beg, end);
//        fin.close();
//        pre_process(temp_s);
//        fout.open(whole_filename1, ios::out);
//
//        fout << temp_s;
//        fout.close();
//
//        FILE *fp = fopen(whole_filename1.c_str(), "r");
//        if (fp)
//            yyin = fp;
//        else
//        {
//            cout << whole_filename1 << " not exist" << endl;
//            exit(-1);
//        }
//
//    }
//    else
    {
        FILE *fp = fopen(filename.c_str(), "r");

        if (fp)
            yyin = fp;
        else
        {
            cout << filename << " not exist" << endl;
            exit(-1);
        }
    }
    yyparse();

    //printtree(head);
    //if (flag == true)
    TraverseTree2(head);
    TraverseTree3(head);
    //Traverse(head);

    return head;
}

/*function：generate an AST(相比于create_tree，减少了TraverseTree2和TraverseTree3两个函数，用于程序切片；
 * Input:filename,flag(true if needs preprocess)
 * Output:AST
 * */
gtree *&create_tree_withoutTraverse(string filename)
{
    reset_gen_ast();
//    if (pre_process_flag == true)
//    {
//        //string filename = "a.txt";
//        string filename1 = filename;
//        string whole_filename1 = filename1.substr(0,filename1.rfind('.')) + ".iii";
//        ifstream fin;
//        ofstream fout;
//        fin.open(filename, ios::in);
//        if (!fin.is_open())
//        {
//            cout << filename << " not exist" << endl;
//            exit(-1);
//        }
//        string temp_s;
//        istreambuf_iterator<char> beg(fin), end;
//        temp_s = string(beg, end);
//        fin.close();
//        pre_process(temp_s);
//        fout.open(whole_filename1, ios::out);
//
//        fout << temp_s;
//        fout.close();
//
//        FILE *fp = fopen(whole_filename1.c_str(), "r");
//        if (fp)
//            yyin = fp;
//        else
//        {
//            cout << whole_filename1 << " not exist" << endl;
//            exit(-1);
//        }
//
//    }
//    else
    {
        FILE *fp = fopen(filename.c_str(), "r");

        if (fp)
            yyin = fp;
        else
        {
            cout << filename << " not exist" << endl;
            exit(-1);
        }
    }
    yyparse();

    //printtree(head);
    //if (flag == true)
//    TraverseTree2(head);
//    TraverseTree3(head);
    //Traverse(head);

    return head;
}

void Traverse(gtree *p)
{
    if (p == NULL) return;
    if (p->type == FUNCTION_DEFINITION)
        cout << p->place << endl;
    Traverse(p->child);
    Traverse(p->next);
}

void cut_tree(gtree *p)
{
    if(p==NULL)return;

    cut_tree(p->child);
    cut_tree(p->next);
    if(p && p->parent && p->parent->next == NULL && p->parent->child == p &&p->child &&  p->child->next==NULL && p->next == NULL
    && p->place == p->child->place && p->place == p->parent->place
    && p->parent->type != STATEMENT && p->type != POSTFIX_EXPRESSION && p->parent->type != DIRECT_DECLARATOR)
    {
        gtree *temp = p;
        p->parent->child = temp->child;
        p->child->parent = temp->parent;
//        if(temp->child) {
//            gtree *temp1 = temp->child;
//            while (temp1) {
//                temp1->parent = p;
//                temp1 = temp1->next;
//            }
//        }
        delete temp;
    }
}

void free_tree(gtree *p){
    if(!p)
        return;
    if(p->child)
        free_tree(p->child);
    if(p->next)
        free_tree(p->next);
    delete p;
}