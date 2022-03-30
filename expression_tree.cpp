//
// Created on 2020/12/1.
//

#include "expression_tree.h"
#include<stack>
#include<iostream>

using namespace std;

bool judge_logic_operator(string s)
{
    if (s == "&&" || s == "||") // "and" "or"
        return true;
    return false;
}

bool judge_relation_operator(string s)
{
    if (s == ">" || s == "<" || s == ">=" || s == "<=" || s == "==" || s == "!=")
        return true;
    return false;
}

int priority(string s)
{
    if (s == "#")
        return -2;
    else
    {
        if (s == "(")
            return -1;
//        else if(s == "++")
//            return 0;
        else if (judge_logic_operator(s))
            return 1;
        else if (judge_relation_operator(s))
            return 2;
        else if (s == "+" || s == "-")
            return 3;
        else if (s == "*" || s == "/" || s == "%")
            return 4;
        else if (s == "&" || s == "|")
            return 5;
        else if (s == ",")
            return 6;
        else if (s == ")" || s == "!" || s == "@" || s == "^")
            return 7;

    }
    cout << "priority error!" << endl;
    return -2;
}

bool is_operator(string s)
{
    if (s == "+" || s == "-" || s == "*"
        || s == "/" || s == "(" || s == ")"
        || s == "#" || s == "&" || s == "|"
        || s == "^" || s == "%" || s == "!"
        || s == ","
        //|| s == "++"
        ||judge_logic_operator(s)||judge_relation_operator(s))
    {
        return true;
    }
    return false;
}

int is_operator(string s, int pos)
{
    if (s[pos] == '#')
        return 1;
    if(s[pos] == '>' && pos>=1 && s[pos-1] == '=')
        return 0;
    if(s[pos] == '*' && pos>=1 && (isdigit(s[pos-1]) || isalpha(s[pos-1]) || s[pos-1] == '_' || s[pos-1] == ';'))//';' stands for case expression
        ;
    else if(s[pos] == '*')
        return 0;
    else if(s[pos] == '-' && s.size()>=pos+1 && s[pos+1] == '>')
        return 0;
    else
        ;
    string temp_s;
    temp_s = s[pos];
    temp_s.append(1,s[pos + 1]);
    if (is_operator(temp_s))
        return 2;
    temp_s = s[pos];
    if (is_operator(temp_s))
        return 1;
    return 0;
}

bool is_unary(string s)
{
    if (s == "!" || s == "@" )
        return true;
    else
        return false;
}

void process_stack(stack<condition_tree_node *> &Stack_n,stack<string> &Stack_o)
{
    string top = Stack_o.top();
    Stack_o.pop();

    condition_tree_node *node = new condition_tree_node;
    condition_tree_node *top1,*top2;
//    if(top == "," || top == "++")
//        node->node_type = TokenOperator;
//    else if(judge_logic_operator(top) || judge_relation_operator(top) )
//        node->node_type = RelationOperator;
//    else
    node->node_type = Operator;
    node->node_name = top;
    if(!is_unary(top))
    {
        top1 = Stack_n.top();
        Stack_n.pop();
        top2 = Stack_n.top();
        Stack_n.pop();

        node->right = top1;
        node->left = top2;
        Stack_n.push(node);
    }
    else
    {
        top1 = Stack_n.top();
        Stack_n.pop();
        node->left = top1;
        Stack_n.push(node);
    }
}

bool judge_TokenOperator(string s){

    if(s.size() >= 2 &&
       (s=="++"||s=="--"))
        return true;
    else
        return false;
}

bool judge_case(string s){
    if(s.length()>CaseFlagLength && s.substr(0,5) == CaseFlag)
        return true;
    else
        return false;
}

bool judge_tuple(string s){
    if(judge_case(s)){
        auto pos = s.find("=>");
        if(judge_tuple(s.substr(pos+2)))
            return true;
        else
            return false;
    }
    else{
        if(s[0]=='{')
            return true;
        else
            return false;
    }
}

condition_tree_node* color_construct(string s);
condition_tree_node* process_case(string s,condition_tree_node* func(string s1)){
    if(s[s.size()-1]!=';')
        throw "Case should end with ';'!";
    condition_tree_node *casenode,*rnode,*lnode;
    stack<string> st;
    string tmp,condition,color,token,dft;

    int pos,casecount=0;
    for(unsigned int i=CaseFlagLength-1;i<s.size()-CaseFlagLength;i++){
        if(s.substr(i,CaseFlagLength) == CaseFlag)
            casecount++;
        if(s[i]==';')
            casecount--;
        if(casecount == 0 && s[i] == ':'){
            pos = i;
            break;
        }
    }
    condition = s.substr(CaseFlagLength,pos-CaseFlagLength);//extract variable

    casecount = 0;
    for(int i=pos+1;i<s.size();i++){
        if(s.substr(i,CaseFlagLength) == CaseFlag)
            casecount++;
        if(s[i]==';')
            casecount--;
        if(casecount == 0 && s[i]==':') {
            st.push(tmp);
            tmp.clear();
        }
        else if(casecount == -1 && s[i]==';'){
            dft = tmp;
            tmp.clear();
            break;
        }
        else
            tmp = tmp + s[i];
    }
    if(dft == "default")
        rnode = NULL;
    else {
        pos = dft.find("=>");
        dft = dft.substr(pos+2);
        rnode = func(dft);
    }
    while(!st.empty()){
        casenode = new condition_tree_node;
        casenode->node_type = CaseOperator;
        casenode->right = rnode;
        tmp = st.top();
        st.pop();
        pos = tmp.find("=>");
        color = tmp.substr(0,pos);
        casenode->node_name = condition;
        casenode->condition = color_construct(condition + "==" + color);
        casenode->value = color;
        token = tmp.substr(pos+2);
        lnode = func(token);
        casenode->left = lnode;
        rnode = casenode;
    }
    return rnode;
}

condition_tree_node* Singlecolor_construct(string s){
    if(judge_case(s)){
        return process_case(s,color_construct);
    }
    else{
        condition_tree_node *newnode = new condition_tree_node;
        if(s[0]=='_'||isalpha(s[0]))
            newnode->node_type = variable;
        else
            newnode->node_type = color;
        newnode->value = newnode->node_name = s;
        return newnode;
    }
}

condition_tree_node* color_construct(string s){
    int count;
    string tmp;

    condition_tree_node *newnode;
    stack<string> Stack_o;
    stack<condition_tree_node*> Stack_n;

    s = s + "#";
    Stack_o.push("#");

    int caseflag = false;
    unsigned short casecount=0;
    for(unsigned int i=0;i<s.size();i++) {
        if(s.substr(i,CaseFlagLength)== CaseFlag) {
            casecount++;
            if(casecount == 1)
                caseflag = true;
        }
        if(s[i]==';') {
            casecount--;
            if(casecount == 0)
                caseflag = false;
        }
        count = is_operator(s, i);
        if (count != 0 && !caseflag) {
            string op = s.substr(i,count);
            if(tmp.size()==0 && s[i] == '-'){
                Stack_o.push("@");
                continue;
            }
            else if(tmp.size() != 0){
                newnode = Singlecolor_construct(tmp);
                Stack_n.push(newnode);
                tmp.clear();
            }
            if(op == "("){
                Stack_o.push(op);
            }
            else if(op == ")"){
                while(Stack_o.top() != "("){
                    process_stack(Stack_n,Stack_o);
                }
                Stack_o.pop();
            }
            else{
                while(priority(op) <= priority(Stack_o.top())){
                    if (s[i] == '#' && Stack_o.top() == "#")
                    {
                        condition_tree_node *root = Stack_n.top();
                        Stack_n.pop();
                        return root;
                    }
                    process_stack(Stack_n, Stack_o);
                }
                Stack_o.push(op);
            }

            i += count - 1;
        }
        else if (s[i] == '=' && (s[i+1] == '_' || isalnum(s[i+1]) || isdigit(s[i+1]))
        && (s[i+1] == '_' || isalnum(s[i+1]) || isdigit(s[i+1])) )//deal with assignment expression 'a=1' imply '1'
            tmp.clear();
        else
            tmp = tmp + s[i];
    }


}

condition_tree_node* tuple_construct(string s){
    if(judge_case(s)){
        return process_case(s,tuple_construct);
    }
    else{
        if(s[0]!='{'||s[s.size()-1]!='}')
            throw "Error tuple when tuple_construct!";
        stack<string> st;
        string s1 = s.substr(1,s.size()-2),tmp;
        condition_tree_node *newnode,*lnode,*rnode;
        for(unsigned int i=0;i<s1.size();i++){
            if(s1[i]==','){
                st.push(tmp);
                tmp.clear();
            }
            else
                tmp = tmp + s1[i];
        }
        st.push(tmp);
        rnode = color_construct(st.top());
        st.pop();
        while(!st.empty()){
            newnode = new condition_tree_node;
            newnode->node_type = Tuple;
            newnode->node_name = "Tuple";
            newnode->right = rnode;
            lnode = color_construct(st.top());
            st.pop();
            newnode->left = lnode;
            rnode = newnode;
        }
        return rnode;
    }
}

condition_tree_node* token_construct(string s){
    if(judge_case(s)){
        return process_case(s,token_construct);
    }
    else{
        string tmp;
        for(unsigned int i=0;i<s.size();i++){
            if(s[i]=='`')
            {
                condition_tree_node *lnode,*newnode = new condition_tree_node;
                newnode->right = NULL;
                newnode->node_type = Token;
                newnode->node_name = "Token";
                newnode->num = atoi(tmp.c_str());

                if(judge_tuple(s.substr(i+1))){
                    lnode = tuple_construct(s.substr(i+1));
                    newnode->left = lnode;
                }
                else{
                    lnode = color_construct(s.substr(i+1));
                    newnode->left = lnode;
                }

                return newnode;
            }
            tmp = tmp + s[i];
        }
    }
}

condition_tree_node* manytoken_construct(string s){
    if(s.length()==0)
        throw "Error!Empty arc expression!";
    string tmp;
    stack<string> st;
    stack<string> st_op;
    unsigned int last=0;
    int casecount = 0;
    bool tupleflag = false;
    for(int i=0;i<s.size()-2;i++){
        if(s[i] == '{')
            tupleflag = true;
        if(s[i] == '}')
            tupleflag = false;
        if(!tupleflag && s.size()>CaseFlagLength){
            if(s.substr(i,CaseFlagLength) == CaseFlag)
                casecount++;
        }
        if(!tupleflag && s[i] == ';')
            casecount--;
        tmp = s.substr(i,2);

        if(casecount == 0 && judge_TokenOperator(tmp)) {
            st.push(s.substr(last, i-last));
            st_op.push(tmp);
            last = i + 2;
        }
    }
    st.push(s.substr(last,s.size()));

    condition_tree_node *lnode,*rnode,*newnode;

    if(st.top().length() > CaseFlagLength && st.top().substr(0,CaseFlagLength) == CaseFlag)
        rnode = process_case(st.top(),manytoken_construct);
    else
        rnode = token_construct(st.top());
    st.pop();
    while(!st.empty()) {
        if(st.top().length() > CaseFlagLength && st.top().substr(0,CaseFlagLength) == CaseFlag)
            lnode = process_case(st.top(),manytoken_construct);
        else
            lnode = token_construct(st.top());
        st.pop();
        newnode = new condition_tree_node;
        newnode->node_type = TokenOperator;
        newnode->node_name = st_op.top();
        st_op.pop();
        newnode->right = rnode;
        newnode->left = lnode;
        rnode = newnode;

    }
    return rnode;
}

void condition_tree::construct(string s) {

//    if(root != NULL) {
//        deconstruct();
//        root = NULL;
//    }
    exp = s;
    if(s == "NULL" || s == "")
        root = NULL;
    else
        root = manytoken_construct(s);
}

void deconstruct_node(condition_tree_node *node)
{
    if(node == NULL)
        return;
    deconstruct_node(node->left);
    deconstruct_node(node->right);
    delete node;
}

void condition_tree::deconstruct() {
    exp = "";
    deconstruct_node(root);
}

void copy_condition_tree(condition_tree_node *&des,condition_tree_node const *src)
{
    if(src)
    {
        des = new condition_tree_node;
        des->node_name = src->node_name;
        des->node_type = src->node_type;
        des->value = src->value;
        des->left = NULL;
        des->right = NULL;
        des->num = src->num;
        copy_condition_tree(des->condition,src->condition);
//        des->condition = src->condition;
        if(src->left) {
            des->left = new condition_tree_node;
            copy_condition_tree(des->left,src->left);
        }
        if(src->right) {
            des->right = new condition_tree_node;
            copy_condition_tree(des->right,src->right);
        }
    }
}

void condition_tree::operator=(const condition_tree &ct) {
    if(ct.root!=NULL) {
//        copy_condition_tree(root, ct.root);
        construct(ct.exp);
    }
    else
        root = NULL;

    exp = ct.exp;
}

void condition_tree::referencetree(const condition_tree &ct){
    root = ct.root;
    exp = ct.exp;
    isrefer = true;
}

//void condition_tree::operator=(condition_tree &&ct) {
//    root = ct.root;
//    exp = ct.exp;
//}
