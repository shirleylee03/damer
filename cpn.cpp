//
// Created on 2020/10/10.
//
#include "cpn.h"
#include "v_table.h"
#include<cmath>
#include<stack>
#include <utility>

SortTable sorttable;
//SORTID SortTable::psptr = 0;

//vector<Triple> readpointer;
//vector<Triple> writepointer;
//vector<Triple> strcpypointer;
//vector<Triple> threadcreatearr;

map<string,unsigned short> place_size;//first is place,second is size. place(1)——address(n)
map<string,bool> place_isglobal;//first is place,second is isglobal. isglobal and size should be together(can be improved)

int gen_P_num=0,gen_T_num=0;
ID_t id_ptr = 1000;

const TID_t init_tid = "\'main\'"; // initial function
const TID_t pthread_P = "pthread";
type TID_colorset = String;

string alloc_store_P;
const string alloc_store_type = "alloc_type";
const string malloc_str = "malloc";
const string calloc_str = "calloc";
const string index_sortname = "___index";
const string id_sortname = "___id";
const string tid_sortname = "___tid";
ID_t init_alloc = 10000;
unsigned int malloc_maxsize = 100;
unsigned short thread_max = 51;

//map<string,string> map_address;//first is address(string),second is variable. address(1)——variable(1)


map<string,type> map_build_in_type {
        {"void",dot},
        {"char",String},
        {"short",Integer},
        {"int",Integer},
        {"long",Integer},
        {"float",Real},
        {"double",Real},
        {"pthread_mutex_t",dot},
        {"pthread_cond_t",dot}
};
//map<string,MSI> usrdf_type;
string controltypename = "control";
string arr_suffix = "_arr",var_suffix = "_var",begin_suffix = " begin",
        id_suffix = "_id",tid_str = "__tid",
        end_suffix = " end",return_suffix = "_v",
        call_suffix = "()",global_suffix = "_global", local_suffix = "_local";
string executed_P_name = "executed_P";
vector<string> pthread_type = {"pthread_t","pthread_mutex_t","pthread_cond_t"};
vector<string> pthread_func_type = {"pthread_create","pthread_join","pthread_exit",
                                    "pthread_mutex_init","pthread_mutex_lock","pthread_mutex_unlock",
                                    "pthread_cond_init","pthread_cond_signal","pthread_cond_wait","pthread_mutex_destroy"};
vector<string> lib_func_type = {"printf","malloc","calloc","strcpy"};



string removeTypeNamePostfix(string s){
    string res;
    if(endswith(s,global_suffix)){
        res = s.substr(0,s.length()-global_suffix.length());
        if(endswith(res,var_suffix)){
            res = res.substr(0,res.length()-var_suffix.length());
        }
        else if(endswith(res,arr_suffix)){
            res = res.substr(0,res.length()-arr_suffix.length());
        }
        else
            throw "TypeName shou marked array or var!";
    }
    else if(endswith(s,local_suffix)){
        res = s.substr(0,s.length()-local_suffix.length());
        if(endswith(res,var_suffix)){
            res = res.substr(0,res.length()-var_suffix.length());
        }
        else if(endswith(res,arr_suffix)){
            res = res.substr(0,res.length()-arr_suffix.length());
        }
        else
            throw "TypeName shou marked array or var!";
    }
    else
        throw "TypeName is already program type. No need for removing postfix!";
    return res;
}

string add_idPostfix(string s){
    if(s[s.length()-1] ==';'){
//        vector<string> child_list;
        int caseCount = 0;
        int lastPos = s.length() - 2;
        for(int i=s.length()-2;i>=0;i--){
            if(s[i] == ';')
                caseCount++;
            else if(s[i] == '$')
                caseCount--;
            if(s[i] == '$' && caseCount == 0)
                break;
            if(i>0 && s[i] == '>' && s[i-1] == '=' && caseCount == 0){
                string tmp_str;
                tmp_str = s.substr(i+1,lastPos-i);
                if(tmp_str != "default") {
                    tmp_str = add_idPostfix(tmp_str);
                    s = s.replace(i+1, lastPos - i , tmp_str);
                }
            }
            if(s[i] == ':' && caseCount == 0)
                lastPos = i-1;
        }
        return s;
    }
    else{
        s = s + id_suffix;
        return s;
    }
}

string addQuote(string s){
    if(s[s.length()-1] ==';'){
//        vector<string> child_list;
        int caseCount = 0;
        int lastPos = s.length() - 2;
        for(int i=s.length()-2;i>=0;i--){
            if(s[i] == ';')
                caseCount++;
            else if(s[i] == '$')
                caseCount--;
            if(s[i] == '$' && caseCount == 0)
                break;
            if(i>0 && s[i] == '>' && s[i-1] == '=' && caseCount == 0){
                string tmp_str;
                tmp_str = s.substr(i+1,lastPos-i);
                if(tmp_str != "default") {
                    tmp_str = addQuote(tmp_str);
                    s = s.replace(i+1, lastPos - i, tmp_str);
                }
            }
            if(s[i] == ':' && caseCount == 0)
                lastPos = i-1;
        }
        return s;
    }
    else{
        return "'"+ s + "'";
    }
}

string controlflowArcExp(string arcexp){
    return "1`" + arcexp;
}

//index array to str
//example: [1,2,3] -> _(1*arrsize(0)+2*arrsize(1)+3*arrsize(2))
int Multiindex2index(vector<short> indexs,SizeList sl){
    int idx = 0;
    if(indexs.size()!=sl.getdim())
        throw "index and sizelist don't match!";
    for(int i=0;i<indexs.size();i++)
        idx += indexs[i] * sl.getarrsize(i);
    return idx;
}

int conststr2const(string conststr){
    ///!!!
    return atoi(conststr.c_str());
}

bool judge_isidentifier(string s){
    if(s.length() == 0)
        return false;
    if(s[0] != '_' && !isalpha(s[0]))
        return false;
    for(int i=1;i<s.length();i++){
        if(s[i]!='_' && !isalpha(s[i]) && !isdigit(s[i]))
            return false;
    }
    return true;
}

//index array to offset
//example int a[5][10], a[1][2]->12
int transindexs2number(vector<short> indexs,vector<short> sizes){
    int res;
    for(int i=0;i<indexs.size();i++){
        int weight = 1;
        for(int j=i+1;j<sizes.size();j++)
            weight *= sizes[j];
        res += indexs[i] * weight;
    }
    return res;
}

//find P_id through variable name
string CPN::find_P_id(string v_name,string base)
{
    for(unsigned int i=0;i<v_tables.size();i++)
    {
        if(v_tables[i]->get_name() == base)
        {
            return v_tables[i]->get_place(v_name);
        }
    }
    throw "can't find P name";
}

SizeList CPN::find_v_size(string v_name,string base)
{
    for(unsigned int i=0;i<v_tables.size();i++)
    {
        if(v_tables[i]->get_name() == base)
        {
            return v_tables[i]->get_size(v_name);
        }
    }
    throw "can't find v size";
}

bool CPN::find_v_ispointer(string v_name,string base)
{
    for(unsigned int i=0;i<v_tables.size();i++)
    {
        if(v_tables[i]->get_name() == base)
        {
            return v_tables[i]->get_ispointer(v_name);
        }
    }
    throw "can't find v ispointer";
}
bool CPN::find_v_isglobal(string v_name,string base){

    for(unsigned int i=0;i<v_tables.size();i++)
    {
        if(v_tables[i]->get_name() == base)
        {
            return v_tables[i]->get_isglobal(v_name);
        }
    }
    throw "can't find v isglobal";
}
string CPN::find_v_tag(string v_name,string base)
{
    for(unsigned int i=0;i<v_tables.size();i++)
    {
        if(v_tables[i]->get_name() == base)
        {
            return v_tables[i]->get_tag(v_name);
        }
    }
    throw "can't find v tag";
}
string CPN::find_v_basetag(string v_name,string base)
{
    for(unsigned int i=0;i<v_tables.size();i++)
    {
        if(v_tables[i]->get_name() == base)
        {
            return v_tables[i]->get_basetag(v_name);
        }
    }
    throw "can't find v basetag";
}

void init_pthread_type()
{
    aka temp;
    for(unsigned int i=0;i<pthread_type.size();i++) {
        temp.origin_name = pthread_type[i];
        temp.aka_name = pthread_type[i];
        temp.level = 0;
        aka_type_array.push_back(temp);
    }
}

void uninit_pthread_type(){
    aka_type_array.clear();
    now_level = 1;
}

string uniqueexp(string exp,CPN *cpn){

    string tmp_exp = exp;
    bool found = false;
    while(1) {
        found = cpn->findPlaceExp(tmp_exp);
        if (!found)
            break;
        tmp_exp = "_" + tmp_exp;
    }
    return tmp_exp;
}

//string antiuniqueexp(string exp){
//    int i;
//    for(i=0;i<exp.length();i++){
//        if(exp[i]!='$')
//            break;
//    }
//    if(i==0)
//        return exp;
//    else
//        return exp.substr(i);
//}

string construct_normaltoken(string value,string index,string id,string tid){
    string result;

    if(index == "" && id == "" && tid == "") {
        result = "1`" + value;
        return result;
    }

    result.append("1`{");
    result.append(value);
    if(!index.empty()){
        result.append(",");
        result.append(index);
    }
    if(!id.empty()) {
        result.append(",");
        result.append(id);
    }
    if(!tid.empty()) {
        result.append(",");
        result.append(tid);
    }
    result.append("}");

    return result;
}


void process_stack(stack<int> &Stack_n,stack<string> &Stack_o)
{
    string top = Stack_o.top();
    int top1,top2;
    string op = Stack_o.top();
    Stack_o.pop();
    if(!is_unary(top))
    {
        top1 = Stack_n.top();
        Stack_n.pop();
        top2 = Stack_n.top();
        Stack_n.pop();

        Stack_n.push(operate(top1,top2,op));
    }
    else
    {
        top1 = Stack_n.top();
        Stack_n.pop();

        Stack_n.push(operate(top1,0,op));
    }
}

int cal_constant(string s){
    int count;
    string tmp;

    stack<string> Stack_o;
    stack<int> Stack_n;

    s = s + "#";
    Stack_o.push("#");

    for(unsigned int i=0;i<s.size();i++) {
        count = is_operator(s, i);
        if (count != 0) {
            string op = s.substr(i,count);
            if(tmp.size()==0 && s[i] == '-'){
                Stack_o.push("@");
                continue;
            }
            else if(tmp.size() != 0){
                auto tmp_i = atoi(tmp.c_str());
                if(tmp_i == 0 && tmp.c_str() != "0")
                    throw tmp + " is not a constant!";
                Stack_n.push(tmp_i);
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
                        auto res = Stack_n.top();
                        Stack_n.pop();
                        return res;
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

bool judge_constantindex(string s){
    for(int i=0;i<s.length();i++){
        if(!isdigit(s[i]))
            return false;
    }
    return true;
}

bool judge_constantindex(vector<string> indexs){
    bool constantindex = true;
    for(int i=0;i<indexs.size();i++)
        if(!judge_constantindex(indexs[i])){
            constantindex = false;
            break;
        }
    return constantindex;
}

string arrindexmark(string arrindex){
    return "_" + arrindex;
}


void Add_series_productsort(const string& productsort_name,vector<string> base_sortname,vector<MemberInfo> base_memberinfo,vector<MSI> base_msi){
    string id;
    vector<string> sortname;
    vector<MemberInfo> memberinfo;
    vector<MSI> sortid;

    MSI m_int,m_str;
    m_int.tid = Integer;
    m_int.sid = 0;
    m_str.tid = String;
    m_str.sid = 0;

    id = productsort_name;
    sortname = base_sortname;
    memberinfo = base_memberinfo;
    sortid = base_msi;
    sorttable.Add_productsort(id,sortname,memberinfo,sortid,false,false);

    //build_in type array

    id = productsort_name + arr_suffix + local_suffix;
    sortname = base_sortname;
    memberinfo = base_memberinfo;
    sortid = base_msi;

    sortname.emplace_back(index_sortname);
    sortname.emplace_back(id_sortname);
    sortname.emplace_back(tid_sortname);
    sortid.emplace_back(m_int);
    sortid.emplace_back(m_int);
    sortid.emplace_back(m_str);
//    memberinfo.clear();
    sorttable.Add_productsort(id,sortname,memberinfo,sortid,true,true);

    //build_in type var
    id = productsort_name + var_suffix + local_suffix;
    sortname = base_sortname;
    memberinfo = base_memberinfo;
    sortid = base_msi;
    sortname.emplace_back(id_sortname);
    sortname.emplace_back(tid_sortname);
    sortid.emplace_back(m_int);
    sortid.emplace_back(m_str);
//    memberinfo.clear();
    sorttable.Add_productsort(id,sortname,memberinfo,sortid,false,true);

    //build_in type array global
    id = productsort_name + arr_suffix + global_suffix;
    sortname = base_sortname;
    memberinfo = base_memberinfo;
    sortid = base_msi;
    sortname.emplace_back(index_sortname);
    sortname.emplace_back(id_sortname);
    sortid.emplace_back(m_int);
    sortid.emplace_back(m_int);
//    memberinfo.clear();
    sorttable.Add_productsort(id,sortname,memberinfo,sortid,true,false);

    //build_in type var global
    id = productsort_name + var_suffix + global_suffix;
    sortname = base_sortname;
    memberinfo = base_memberinfo;
    sortid = base_msi;
    sortname.emplace_back(id_sortname);
    sortid.emplace_back(m_int);
//    memberinfo.clear();
    sorttable.Add_productsort(id,sortname,memberinfo,sortid,false,false);
}

vector<gtree *> extract_paras(gtree *p)
{
    //p must be a postfix_expression
    vector<gtree *> res;
    gtree *tmp = p;
    while(tmp && tmp->type!=PRIMARY_EXPRESSION)
        tmp = tmp->child;
    if(!tmp->parent->next || tmp->parent->next->place!="(")
    {
        cout<<"extract_paras's para must be a call_postfix!"<<endl;
        exit(-1);
    }
    gtree *argument_list = tmp->parent->next->next,*assignment;
    if(argument_list->place == ")")
        return res;
    while(argument_list->type!=ASSIGNMENT_EXPRESSION)
        argument_list = argument_list->child;
    assignment = argument_list;
    while(assignment && assignment->type==ASSIGNMENT_EXPRESSION)
    {
        res.push_back(assignment);
        assignment = assignment->parent->next->next;
    }
    return res;
}

void CPN::initDecl() {

    string id;
    vector<string> sortname;
    vector<MemberInfo> memberinfo;
    vector<MSI> sortid;

//    id = controltypename;
//    sortname.emplace_back("string");
//    memberinfo.clear();
//    sortid.emplace_back(String,0);
//
//    sorttable.Add_productsort(id,sortname,memberinfo,sortid);

    for(auto miter=map_build_in_type.begin();miter!=map_build_in_type.end();miter++)
    {
        sortname.clear(),memberinfo.clear(),sortid.clear();
        id = miter->first;
        sortname.emplace_back(miter->first);
        memberinfo.clear();
        sortid.emplace_back(miter->second,0);
        Add_series_productsort(id,sortname,memberinfo,sortid);
    }
}

void getinfofromdeclarator(gtree *declarator,bool &pointer_flag,SizeList &arr_size,string &id,bool &isfuncdec,bool &ispara,bool &isglobal){
    if(declarator->type != DECLARATOR)
        throw "ERROR!getinfofromdeclarator should be a declarator node!";
    ispara = PARAMETER_DECLARATION == declarator->parent->type;
    isfuncdec = false;
    isglobal = true;

    gtree *com = declarator->parent;
    while(com){
        if(com->type == COMPOUND_STATEMENT)
            isglobal = false;
        com = com->parent;
    }

    gtree *direct_declarator;
    pointer_flag = false;
    arr_size.nonsize();
    gtree* identifier;
    if(declarator->child->type == POINTER){
        direct_declarator = declarator->child->next;
        pointer_flag = true;
    }
    else
        direct_declarator = declarator->child;
    if(direct_declarator->child && direct_declarator->child->type == DIRECT_DECLARATOR
    && direct_declarator->child->next && direct_declarator->child->next->place == "("){
        isfuncdec = true;
    }
    else if(direct_declarator->child && direct_declarator->child->type == DIRECT_DECLARATOR){
        gtree *identifier = direct_declarator;
        while(identifier->child)
            identifier = identifier->child;
        id = identifier->place;

        while(identifier->parent->next && identifier->parent->next->place == "["
        && identifier->parent->next->next){
            if(identifier->parent->next->next->type == CONSTANT_EXPRESSION) {
                string size_str = identifier->parent->next->next->place;
                int tmp_size = cal_constant(size_str);
                arr_size.Add_size(tmp_size);
                identifier = identifier->parent;
            }
            else
                throw "we don't support array declaration like: int a[] for now!";
        }
    }
    else if(direct_declarator){
        // due to cut_tree, there may just have one IDENTIFIER below DIRECT_DECLARATOR
        id = direct_declarator->place;
    }
    if(ispara && !arr_size.empty()) {
        pointer_flag = true;
        arr_size.nonsize();
    }
    if(ispara)
        isglobal = false;
}

string CPN::Add_executed_P(vector<string> source_T,vector<string> target_T){
    if(source_T.size()==0)
        return "";
    string executed_P = gen_P();
    Add_Place(executed_P,controltypename,true,SizeList(),executed_P_name,true,true,NoneRow);
    for (unsigned int i = 0; i < source_T.size(); i++) {
//        auto tier = mapTransition.find(source_T[i]);
//        if(tier == mapTransition.end()){
//            cerr << "ERROR!can't find trans!"<<endl;
//            exit(-1);
//        }
//        if(!transition[tier->second].isreturn)
        Add_Arc(source_T[i], executed_P, controlflowArcExp(tid_str), false, executed,normal);
    }
    for (unsigned int i = 0; i < target_T.size(); i++)
        Add_Arc(executed_P, target_T[i], controlflowArcExp(tid_str), true, control,normal);
    return executed_P;
}

void CPN::iter_extra(string newP,string _P){
    vector<string> false_exit = get_falseexit_T(_P);
    auto call_P = get_call_P(_P);
    for (unsigned int i = 0; i < false_exit.size(); i++) {
        if(newP != "")
            Add_Arc(false_exit[i], newP, controlflowArcExp(tid_str), false, executed,normal);
        Add_Arc(false_exit[i], _P, controlflowArcExp(tid_str), false, executed,normal);
        if(call_P.size()!=0){
            Add_Arc(false_exit[i], call_P[0], controlflowArcExp(tid_str), false, executed,normal);
        }
    }
}

void CPN::iter_extra_withoutdependence(string _P){
    vector<string> false_exit = get_falseexit_T(_P);
    auto call_P = get_call_P(_P);
    for (unsigned int i = 0; i < false_exit.size(); i++) {
        // if(newP != "")
            // Add_Arc(false_exit[i], newP, controlflowArcExp(tid_str), false, executed,normal);
        if(call_P.size()!=0)
            Add_Arc(false_exit[i], call_P[0], controlflowArcExp(tid_str), false, executed,normal);
        else
            Add_Arc(false_exit[i], _P, controlflowArcExp(tid_str), false, executed,normal);
        
    }
}

//tree1 indicates a compound_statement node
void CPN::inside_block(gtree *tree1, string T){
    gtree *tr;
    int sum = 0;
    //bool flag = false;
    if(tree1->child->type == COMPOUND_STATEMENT)
        tree1 = tree1->child;
    if (tree1->child->next->type == STATEMENT_LIST)
    {
        tr = tree1->child->next;
    }
    else if(tree1->child->next->next == NULL)
    {
        return;
    }
    else if (tree1->child->next->next->type == STATEMENT_LIST)
    {
        tr = tree1->child->next->next;
    }
    else//只有定义
    {
        cout << "only have declaration!" << endl;
        return;
        //exit(-1);
    }
    while (tr->type != STATEMENT)
        tr = tr->child;

    vector<string> now;
    vector<string> last;
    bool mutex_flag=false;

    string mutex_P;
    string mutex_T;
//    string create_P;
    map<string,string> create_P_list;

    //init
    last.push_back(T);
    while (tr) {
        now.clear();
        /*if (tr->child->type == SELECTION_STATEMENT || tr->child->type == ITERATION_STATEMENT
            || judge_assign_statement(tr) || judge_call_statement(tr) || judge_return_statement(tr))*/
        if (!judge_statement(tr))
            break;


        bool control_P, t;
        int n1 = 0;
        double d = 0.0;
        string tag;
        string _P = tr->matched_P;
        vector<string> call_P = get_call_P(_P);

        if (tr->place == "pthread_mutex_unlock" + return_suffix) {
            mutex_flag = false;
            vector<string> cor_P;
//            cor_P.push_back(_P);
//            petri->set_correspond_P(mutex_P, cor_P);
//            cor_P.clear();
//            cor_P.push_back(mutex_P);
//            petri->set_correspond_P(_P, cor_P);
        }

        if(last.size() != 0) {
            if (call_P.size() != 0) {
                //call_P[0]代表调用到的第一个函数
//            for (unsigned int i = 0; i < call_P.size(); i++) {
                if (mutex_flag == false)
                    Add_Arc(T, call_P[0], controlflowArcExp(tid_str), false, control,beoverrided);
                else
                    Add_Arc(mutex_T, call_P[0], controlflowArcExp(tid_str), false, control,beoverrided);
//            }
            } else {
                if (mutex_flag == false)
                    Add_Arc(T, _P, controlflowArcExp(tid_str), false, control,beoverrided);
                else
                    Add_Arc(mutex_T, _P, controlflowArcExp(tid_str), false, control,beoverrided);
            }
        }
        //int flag = petri.get_call_flag(_P);


        if (call_P.size() == 0)
            now = get_enter_T(_P);
        else
            now = get_enter_T(call_P[0]);


//        if (sum == 0) {
//            sum++;
        string newP;


        newP = Add_executed_P(last,now);
        if (tr->child->type == ITERATION_STATEMENT)//while statement
            iter_extra(newP,_P);


        last = get_exit_T(_P);

        if (tr->place == "pthread_mutex_lock" + return_suffix) {
            mutex_flag = true;
            mutex_P = tr->matched_P;
            vector<string> lock_T = get_enter_T(mutex_P);
            mutex_T = lock_T[0];
        }

        if (tr->parent->next->type == STATEMENT)
            tr = tr->parent->next;
        else
            break;

    }
}

void CPN::inside_block_withoutdependence(gtree *tree1, string T){
    gtree *tr;
    int sum = 0;
    //bool flag = false;
    if(tree1->child->type == COMPOUND_STATEMENT)
        tree1 = tree1->child;
    if (tree1->child->next->type == STATEMENT_LIST)
    {
        tr = tree1->child->next;
    }
    else if(tree1->child->next->next == NULL)
    {
        return;
    }
    else if (tree1->child->next->next->type == STATEMENT_LIST)
    {
        tr = tree1->child->next->next;
    }
    else//只有定义
    {
        cout << "only have declaration!" << endl;
        return;
        //exit(-1);
    }
    while (tr->type != STATEMENT)
        tr = tr->child;

    string now;
    vector<string> last;
    bool mutex_flag=false;

    // string mutex_P;
    // string mutex_T;
//    string create_P;
    // map<string,string> create_P_list;

    //init
    last.push_back(T);
    while (tr) {
        now.clear();
        /*if (tr->child->type == SELECTION_STATEMENT || tr->child->type == ITERATION_STATEMENT
            || judge_assign_statement(tr) || judge_call_statement(tr) || judge_return_statement(tr))*/
        if (!judge_statement(tr))
            break;


        bool control_P, t;
        int n1 = 0;
        double d = 0.0;
        string tag;
        string _P = tr->matched_P;
        vector<string> call_P = get_call_P(_P);

//         if (tr->place == "pthread_mutex_unlock" + return_suffix) {
//             mutex_flag = false;
//             vector<string> cor_P;
// //            cor_P.push_back(_P);
// //            petri->set_correspond_P(mutex_P, cor_P);
// //            cor_P.clear();
// //            cor_P.push_back(mutex_P);
// //            petri->set_correspond_P(_P, cor_P);
//         }

//         if(last.size() != 0) {
//             if (call_P.size() != 0) {
//                 //call_P[0]代表调用到的第一个函数
// //            for (unsigned int i = 0; i < call_P.size(); i++) {
//                 // if (mutex_flag == false)
//                     Add_Arc(T, call_P[0], controlflowArcExp(tid_str), false, control,beoverrided);
//                 // else
//                 //     Add_Arc(mutex_T, call_P[0], controlflowArcExp(tid_str), false, control,beoverrided);
// //            }
//             } else {
//                 // if (mutex_flag == false)
//                     Add_Arc(T, _P, controlflowArcExp(tid_str), false, control,beoverrided);
//                 // else
//                 //     Add_Arc(mutex_T, _P, controlflowArcExp(tid_str), false, control,beoverrided);
//             }
//         }
        //int flag = petri.get_call_flag(_P);


        if (call_P.size() == 0)
            now = _P;
        else
            now = call_P[0];


//        if (sum == 0) {
//            sum++;
        // string newP;
        for(int i=0;i<last.size();i++){
            Add_Arc(last[i],now,controlflowArcExp(tid_str),false,control,beoverrided);
        }

        // newP = Add_executed_P(last,now);
        if (tr->child->type == ITERATION_STATEMENT)//while statement
            iter_extra_withoutdependence(_P);


        last = get_exit_T(_P);

        // if (tr->place == "pthread_mutex_lock" + return_suffix) {
        //     mutex_flag = true;
        //     mutex_P = tr->matched_P;
        //     vector<string> lock_T = get_enter_T(mutex_P);
        //     mutex_T = lock_T[0];
        // }

        if (tr->parent->next->type == STATEMENT)
            tr = tr->parent->next;
        else
            break;

    }
}


vector<var_type*> process_struct_declaration_list(gtree *p)
{
    vector<var_type*> var_list;
    gtree *struct_declaration_list = p;
    while(struct_declaration_list->type==STRUCT_DECLARATION_LIST)
        struct_declaration_list = struct_declaration_list->child;
    gtree *struct_declaration = struct_declaration_list;
    while(struct_declaration->type==STRUCT_DECLARATION)
    {
        gtree *specifier_qualifier_list = struct_declaration->child;
        gtree *struct_declarator_list = struct_declaration->child->next;
        string specifier = "";
        vector<var_type> declarators;

        while(specifier_qualifier_list != NULL)
        {
            if(specifier_qualifier_list->child->type == TYPE_SPECIFIER)
            {
                specifier = specifier_qualifier_list->child->child->place;
                break;
            }
            specifier_qualifier_list = specifier_qualifier_list->child->next;
        }

        if(specifier == "")
        {
            cout<<"error! declarator doesn't have a type!"<<endl;
            exit(-1);
        }

        while(struct_declarator_list->type == STRUCT_DECLARATOR_LIST)
            struct_declarator_list = struct_declarator_list->child;

        gtree *struct_declarator = struct_declarator_list;
        while(struct_declarator != NULL)
        {
            if(struct_declarator->child->type == DECLARATOR)
            {
                bool pointer_flag;
                SizeList arr_size;
                string id;
                bool isfuncdec,ispara,isglobal;
                getinfofromdeclarator(struct_declarator->child,pointer_flag,arr_size,id,isfuncdec,ispara,isglobal);
                if(isfuncdec)
                    throw "we don't support structure who has member function for now!";
                if(ispara)
                    throw "structure can't have para declarator!";
                var_type *v_type;
                v_type = new var_type;
                //now we just support dimension 1 or 0
                if(!arr_size.empty())
                    v_type->init(specifier,id,pointer_flag,arr_size);
                else
                    v_type->init(specifier,id,pointer_flag,arr_size);
                var_list.push_back(v_type);
//                if(struct_declarator->child->child->type == POINTER){
//                    cout<<"error! pointer doesn't not support for now."<<endl;
//                    exit(-1);
//                }
//                else{
//                    declarators.push_back(struct_declarator->child->child->child->place);
//                }
            }
            else
                throw "ERROR!struct_declarator should have declarator for now!";
            struct_declarator = struct_declarator->parent->next->next;
        }
//        for(unsigned int i=0;i<declarators.size();i++) {
//            if(declarators[i].get_pointer_flag())
//                sortname.push_back(make_pair(specifier, declarators[i]));
//        }
        struct_declaration = struct_declaration->parent->next;
    }
    return var_list;
}

vector<string> CPN::get_compound_statement_exit(gtree *statement1)
{
    if(statement1->child->next->next == NULL){
        //if there is no stmt in compound_statement, exit will be its exit
        return get_exit_T(statement1->parent->matched_P);
    }
    else if(statement1->child->next->type == STATEMENT_LIST){
        auto last_stmt = statement1->child->next->child;
        if(last_stmt->type == STATEMENT_LIST)
            last_stmt = last_stmt->next;
        return get_exit_T(last_stmt->matched_P);
    }
    else if(statement1->child->next->next->type == STATEMENT_LIST){
        auto last_stmt = statement1->child->next->next->child;
        if(last_stmt->type == STATEMENT_LIST)
            last_stmt = last_stmt->next;
        return get_exit_T(last_stmt->matched_P);
    }
    else
        return get_exit_T(statement1->parent->matched_P);

}

void CPN::getDecl(gtree *tree){
    if (tree == NULL)return;
    else if(tree->type == STRUCT_OR_UNION_SPECIFIER)
    {
        if(tree->child->next->type == REMAIN || (tree->child->next->next != NULL &&
                                                 tree->child->next->next->type == REMAIN))
        {
            MSI m,m1,m2,m_int;
            m_int.tid = Integer;
            m_int.sid = 0;

            ProductSort pp,pp1,pp2,pp3,pp_base;
            gtree *struct_declare_list;
            string usrdefine_typename;
            vector<string> base_sortname;
            vector<MemberInfo> base_memberInfos;
            vector<MSI> base_sortid;

            if(tree->parent->parent->type == DECLARATION_SPECIFIERS)
                usrdefine_typename = tree->parent->parent->place;
            else
                throw "structure type error! " + tree->place;
            if(tree->child->next->type == REMAIN)
            {
//                usrdefine_typename = "";
                struct_declare_list = tree->child->next->next->child;
            }
            else {
//                usrdefine_typename = tree->child->next->place;
                struct_declare_list = tree->child->next->next->next->child;
            }

            vector<var_type*> var_list = process_struct_declaration_list(struct_declare_list);


            //var_list.size();
            for(SORTID i=0;i<var_list.size();i++) {
                if(var_list[i]->get_pointer_flag()) {
                    base_sortid.push_back(m_int);
                    base_memberInfos.emplace_back(var_list[i]->get_name(), var_list[i]->get_size_table());
                    base_sortname.push_back(var_list[i]->get_tag());
                }
                else {

                    auto iter = map_build_in_type.find(var_list[i]->get_tag());
                    MSI tmp_msi;
                    if (iter == map_build_in_type.end()) {
                        auto msi = sorttable.find_typename(var_list[i]->get_tag());
                        tmp_msi = msi;
                    } else {
                        tmp_msi.tid = iter->second;
                        tmp_msi.sid = 0;
                    }

                    if (var_list[i]->get_dimension() == 0) {
                        base_sortid.push_back(tmp_msi);
                        base_memberInfos.emplace_back(var_list[i]->get_name(),var_list[i]->get_size_table());
                        base_sortname.push_back(var_list[i]->get_tag());
                    }
                    else {
                        auto totalsize = var_list[i]->get_size_table().totalsize();
                        for(int j=0;j<totalsize;j++) {
                            base_sortid.push_back(tmp_msi);

                            base_sortname.push_back(var_list[i]->get_tag());
                        }
                        base_memberInfos.emplace_back(var_list[i]->get_name() ,var_list[i]->get_size_table());
                    }
                }
            }

            Add_series_productsort(usrdefine_typename,base_sortname,base_memberInfos,base_sortid);

            for(unsigned int i=0;i<var_list.size();i++){
                delete var_list[i];
            }
        }
    }
    getDecl(tree->child);
    getDecl(tree->next);

}

CPN::CPN() {
    threadDomainInit = 2000;
    placecount = 0;
    varPlacecount = 0;
    ctlPlacecount = 0;
    exePlacecount = 0;
    transitioncount = 0;
    arccount = 0;
    varcount = 0;
//    sorttable.psptr = 0;
    gen_P_num = gen_T_num = 0;
    placecount = transitioncount = arccount = varcount = 0;

    place = new CPlace[400];
    transition = new CTransition[400];
    arc = new CArc[2000];

    vartable = new Variable[500];

    init_V_tables();
}

CPN::~CPN() {
    delete [] place;
    delete [] transition;
    delete [] arc;
    delete [] vartable;

    release_V_tables();
}

bool is_BoolOperator(string s)
{
    if (s == ">" || s == "<" || s == ">=" || s == "<=" || s == "==" || s == "!="
    ||s == "&&" || s == "||")
        return true;
    return false;
}

bool booloperate(const Bucket &lbk,const Bucket &rbk,string Operator){
    if(lbk.tid != rbk.tid)
        throw "ERROR in Bucket booloperate!";
    switch(lbk.tid){
        case Integer:
            return booloperate(lbk.integer,rbk.integer,Operator);
        case Real:
            return booloperate(lbk.real,rbk.real,Operator);
        case String:
            return booloperate(lbk.str,rbk.str,Operator);
        case dot:
            throw "'booloperate' can't operate Dotsort!";
        case productsort:
            throw "'booloperate' can't operate productsort!";
    }
}

Bucket operate(const Bucket &lbk,const Bucket &rbk,string Operator){
    if(lbk.tid != rbk.tid)
        throw "ERROR in Bucket booloperate!";
    Bucket bkt;
    switch(lbk.tid){
        case Integer:
            bkt.tid = Integer;
            bkt.integer = operate(lbk.integer,rbk.integer,Operator);
            break;
        case Real:
            bkt.tid = Real;
            bkt.real = operate(lbk.real,rbk.real,Operator);
            break;
        case String:
            bkt.tid = String;
            bkt.str = operate(lbk.str,rbk.str,Operator);
            break;
        case dot:
            throw "'operate' can't operate dotsort!";
        case productsort:
            throw "'operate' can't operate productsort!";
    }
    return bkt;
}

Bucket operate(const Bucket &lbk,string Operator){
    Bucket bkt;
    string tmpstr;
    if(!is_unary(Operator))
        throw "unary operate must deal with unary!";
    switch(lbk.tid){
        case Integer:
            bkt.tid = Integer;
            bkt.integer = operate(lbk.integer,0,Operator);
            break;
        case Real:
            bkt.tid = Real;
            bkt.real = operate(lbk.real,0.0,Operator);
            break;
        case String:
            bkt.tid = String;
            bkt.str = operate(lbk.str,tmpstr,Operator);
            break;
        case dot:
            throw "'operate' can't operate dotsort!";
        case productsort:
            throw "'operate' can't operate productsort!";
    }
    return bkt;
}

//judge const type of a string value
//return value:
// 0--Integer
// 1--Real
// 2--Char
type JudgeConstType(string value){
    if(value[0] == '\'')
        return String;
    else if(value.find(".") != string::npos)
        return Real;
    else
        return Integer;
}


void CPN::CTN2SingleColor(condition_tree_node *root, token &tk) {
    if(root == NULL)
        return ;

    if(root->node_type == Operator){
        string Op = root->node_name;
        if(is_BoolOperator(Op)){
            token ltk,rtk;
            CTN2SingleColor(root->left,ltk);
            CTN2SingleColor(root->right,rtk);
            Bucket lbk,rbk,finalbkt;
            ltk->getcolor(lbk);
            rtk->getcolor(rbk);
            tk = (token)(new IntegerSortValue);

            finalbkt.tid = Integer;
            if(booloperate(lbk,rbk,Op))
                finalbkt.integer = 1;
            else
                finalbkt.integer = 0;
            tk->setcolor(finalbkt);
        }
        else{
            if(is_unary(Op)){
                token ltk;
                CTN2SingleColor(root->left,ltk);
                Bucket lbk,finalbkt;
                ltk->getcolor(lbk);
                finalbkt = operate(lbk,Op);
                switch(lbk.tid){
                    case Integer:
                        tk = (token)(new IntegerSortValue);
                        break;
                    case Real:
                        tk = (token)(new RealSortValue);
                        break;
                    case String:
                        tk = (token)(new StringSortValue);
                        break;
                }
                tk->setcolor(finalbkt);
            }
            else{
                token ltk,rtk;
                CTN2SingleColor(root->left,ltk);
                CTN2SingleColor(root->right,rtk);
                Bucket lbk,rbk,finalbkt;
                ltk->getcolor(lbk);
                rtk->getcolor(rbk);
                finalbkt = operate(lbk,rbk,Op);
                switch(lbk.tid){
                    case Integer:
                        tk = (token)(new IntegerSortValue);
                        break;
                    case Real:
                        tk = (token)(new RealSortValue);
                        break;
                    case String:
                        tk = (token)(new StringSortValue);
                        break;
                }
                tk->setcolor(finalbkt);
            }
        }
    }
    else if(root->node_type == color){
        string value = root->value;
        auto consttype = JudgeConstType(value);
        Bucket bkt;
        bkt.tid = consttype;
        switch(consttype){
            case Integer:
                tk = (token)(new IntegerSortValue);
                bkt.integer = atoi(value.c_str());
                break;
            case Real:
                tk = (token)(new RealSortValue);
                bkt.real = atof(value.c_str());
                break;
            case String:
                tk = (token)(new StringSortValue);
                bkt.str = value;
                break;
        }
        tk->setcolor(bkt);
    }
    else if(root->node_type == variable){
        string vname = root->node_name;
        if(vname == "pthread_create_v" || vname == "pthread_join_v"){
            //特殊处理pthread_create_v pthread_join_v
            Bucket bkt;
            bkt.tid = Integer;
            bkt.integer = 0;
            tk = (token)(new IntegerSortValue);
            tk->setcolor(bkt);
            return;
        }
        Bucket bkt;
        auto iter = mapVariable.find(vname);
        if(iter == mapVariable.end())
            throw "invalid CPN variable!";
        else{
            vartable[iter->second].getvcolor(bkt);
            switch(vartable[iter->second].gettid()){
                case Integer:
                    tk = (token)(new IntegerSortValue);
//                    bkt.integer = atoi(value.c_str());
                    break;
                case Real:
                    tk = (token)(new RealSortValue);
//                    bkt.real = atof(value.c_str());
                    break;
                case String:
                    tk = (token)(new StringSortValue);
//                    bkt.str = value;
                    break;
            }
            tk->setcolor(bkt);
        }
    }
    else if(root->node_type == CaseOperator){
        token condtk;
        CTN2SingleColor(root->condition,condtk);
        Bucket bkt;
        condtk->getcolor(bkt);
        if(bkt.integer != 0)
            CTN2SingleColor(root->left,tk);
        else {
            if(root->right)
                CTN2SingleColor(root->right, tk);
            else
                throw "ERROR in CaseOperator! don't match any condition!";
        }
    }
}

void CPN::CTN2Token(condition_tree_node *root, vector<token> &tks) {
    if(root == NULL)
        return;

    if(root->node_type == Tuple){
        token singletk;
        CTN2SingleColor(root->left,singletk);
        tks.emplace_back(singletk);
        if(root->right) {
            if(root->right->node_type == Tuple)
                CTN2Token(root->right,tks);
            else {
                token singletk1;
                CTN2SingleColor(root->right,singletk1);
                tks.emplace_back(singletk1);
            }
        }
        else
            throw "Tuple must have right child!";
    }
    else if(root->node_type == CaseOperator){
        token condtk;
        CTN2SingleColor(root->condition,condtk);
        Bucket bkt;
        condtk->getcolor(bkt);
        if(bkt.integer != 0)
            CTN2Token(root->left,tks);
        else {
            if(root->right)
                CTN2Token(root->right, tks);
            else
                ;
//                throw "ERROR in CaseOperator! don't match any condition!";
        }
    }
    else {
        token singletk;
        CTN2SingleColor(root, singletk);
        tks.emplace_back(singletk);
    }
}

void CPN::CTN2MS(condition_tree_node *root,MultiSet &ms,const type &tid,const SORTID &sid) {
    if(root == NULL)
        return;
    MultiSet lms(tid,sid),rms(tid,sid);
    ms.Init_MS(tid,sid);
    if(root->node_type == TokenOperator){

        CTN2MS(root->left,lms,tid,sid);
        CTN2MS(root->right,rms,tid,sid);

        if(root->node_name == "++") {
            ms.PLUS(lms);
            ms.PLUS(rms);
        }
        else if(root->node_name == "--"){
            ms.PLUS(lms);
            if(!ms.MINUS(rms))
                throw "CTN2MS error in --";
        }
        else
            throw "ERROR!We just support '++' and '--' for now!";
    }
    else if(root->node_type == Token){
        vector<token> tokens;
        CTN2Token(root->left,tokens);

        if(tid == productsort) {
            auto ps = sorttable.find_productsort(sid);
            if(ps.get_sortnum() != tokens.size())
                throw "CTN2MS ERROR! productsort type don't match!";
            token finaltoken = (token)(new ProductSortValue(sid));//fpro.generateSortValue(sid);

            Bucket bkt;
            finaltoken->getcolor(bkt);
            for (int i = 0; i < tokens.size(); i++) {
                Bucket tmpbkt;
                tokens[i]->getcolor(tmpbkt);
                bkt.pro[i]->setcolor(tmpbkt);
//                finaltoken.get()[i].setcolor(bkt);
            }
            finaltoken->setcolor(bkt);
            ms.generateFromToken(finaltoken);
//            ms.printMS();
            ms.setTokenCount(root->num);
        }
        else{
            if(tokens.size() != 1)
                throw "CTN2MS ERROR! basic type don't match!";
            ms.generateFromToken(tokens[0]);
            ms.setTokenCount(root->num);
        }
    }
    else if(root->node_type == CaseOperator){
        token condtk;
        CTN2SingleColor(root->condition,condtk);
        Bucket bkt;
        condtk->getcolor(bkt);
        if(bkt.integer != 0) {
            CTN2MS(root->left,lms,tid,sid);
            ms.PLUS(lms);
        }
        else {
            if(root->right) {
                CTN2MS(root->right, rms, tid, sid);
                ms.PLUS(rms);
            }
            else
                ;
//                throw "ERROR in CaseOperator! don't match any condition!";
        }
    }


}

void CPN::CT2MS(const condition_tree &ct,MultiSet &ms,const type &tid,const SORTID &sid) {
    ms.Init_MS(tid,sid);
    if(ct.getexp() == "NULL") {
        ms.settype(tid,sid);
        return;
    }
//    auto ps = sorttable.productsort[sid];

    CTN2MS(ct.getroot(),ms,tid,sid);
}



void CPN::setVariableValue(string id, const Bucket &bkt) {
    auto iter = mapVariable.find(id);
    if(iter == mapVariable.end())
        throw "can't find Variable:" + id;
    auto v = &vartable[iter->second];
    v->setvcolor(bkt);
}

void CPN::create_v_table(gtree *p){
    V_Table *table = new V_Table(p->place);
    v_tables.push_back(table);
    gtree *up = p->parent;

    while (up != NULL && up->type != COMPOUND_STATEMENT)
        up = up->parent;
    if (up == NULL)
        v_tables[v_tables.size()-1]->connect(v_tables[0]);
    else{
        for(int i=0;i<v_tables.size();i++)
            if (v_tables[i]->get_name() == up->place){
                v_tables[v_tables.size()-1]->connect(v_tables[i]);
                break;
            }
    }
}

string get_real_Type_name(const string &Typename,const SizeList &sl,bool isglobal){
    string Type_name = Typename;
    if(!sl.empty())
        Type_name += arr_suffix;
    else
        Type_name += var_suffix;
    if(isglobal)
        Type_name += global_suffix;
    else
        Type_name += local_suffix;
    return Type_name;
}

//if func is not ""
//means the declarator is parameter
void CPN::process_initdeclarator(gtree *initdeclarator, string tag, string func)
{
    gtree *declarator;
    if(initdeclarator->type == INIT_DECLARATOR)
        declarator = initdeclarator->child;
    else
        declarator = initdeclarator;
    bool init_flag = false;//true when has init
    SizeList array_size;
    bool point_flag = false;
    string init_str;
//    string exp;
//    MultiSet init_ms;
    string id;
    bool isfuncdec,ispara,isglobal;
    string basetag = "";

    getinfofromdeclarator(declarator,point_flag,array_size,id,isfuncdec,ispara,isglobal);

    if(declarator->next) {
        init_flag = true;
        if(declarator->next->next->child->type != ASSIGNMENT_EXPRESSION)
            throw "we can't support array initialization for now!";
        init_str = declarator->next->next->child->place;
    }

    if (!isfuncdec) {

        //Add variable place

        if (point_flag) {
            basetag = tag;
            tag = "int";//ID is a type of int
        }
        else {
            // pthread_t don't do any other things, return directly
            if(tag == "pthread_t")
                return;
            for (unsigned int i = 0; i < aka_type_array.size(); i++) {
                if (tag == aka_type_array[i].aka_name) {
                    tag = aka_type_array[i].origin_name;
                    break;
                }
            }
        }
        string _P;
        _P = gen_P();

        string Type_name = get_real_Type_name(tag, array_size, isglobal);
        Add_Place(_P, Type_name, false, array_size,id, isglobal,false,initdeclarator->row);

        if (!init_flag)
            init_str = "";
        if(!exist_in(pthread_type,tag))
            init_place_MS(_P,array_size,init_str);

        if(init_flag && exist_in(pthread_type,tag)){
            if(init_str == "PTHREAD_MUTEX_INITIALIZER"){
                auto pp = findP_byid(_P);
                Bucket bkt;
                bkt.tid = Integer;
                bkt.integer = 0;
                token tk;
                tk = (token)new IntegerSortValue;
                tk->setcolor(bkt);
                MultiSet ms;
                ms.generateFromToken(tk);
                pp->AddMultiSet(ms);
            }
            else
                throw "pthread variable init error!";
        }

        //it is for v_tables
        gtree *compound = declarator;
        string table_name;
        if (declarator->parent->type == PARAMETER_DECLARATION) {
            while (compound->parent->type != FUNCTION_DEFINITION)
                compound = compound->parent;
            while (compound->type != COMPOUND_STATEMENT)
                compound = compound->next;
            table_name = compound->place;

        } else {

            while (compound != NULL && compound->type != COMPOUND_STATEMENT)
                compound = compound->parent;
            if (compound == NULL)
                table_name = global_table_name;
            else
                table_name = compound->place;
        }
        for (unsigned int i = 0; i < v_tables.size(); i++) {
            if (table_name == v_tables[i]->get_name()) {
                if(basetag == "")
                    v_tables[i]->insert(id, _P, tag,tag,array_size, point_flag);
                else
                    v_tables[i]->insert(id, _P, tag,basetag,array_size, point_flag);
                break;
            }
        }

        if(func != "")
            f_table.insert_para(func,id,_P);
    }

}

void CPN::process_declaration(gtree *declaration) {

    gtree *init_declarator = declaration->child->next;
    if(init_declarator->place == ";")
        return;
    string tag = declaration->child->place;
    while (init_declarator->type != INIT_DECLARATOR)
        init_declarator = init_declarator->child;

    process_initdeclarator(init_declarator->child, tag, "");
    gtree *temp;
    while (init_declarator->parent->next != NULL && init_declarator->parent->next->type == REMAIN &&
           init_declarator->parent->next->place == ",") {
        init_declarator = init_declarator->parent;
        temp = init_declarator->next->next;
        process_initdeclarator(temp->child, tag, "");
    }

}

void CPN::process_para_type_list(gtree *para_type_list, string func)
{
    gtree *para_list;
    if (para_type_list->child->next != NULL)//ELLIPSIS暂不考虑
    {
        cout << "ELLIPSIS暂不考虑！" << endl;
        exit(-1);
    }
    else
        para_list = para_type_list->child;

    gtree *para_declaration = para_list->child;
    while (para_declaration->type != PARAMETER_DECLARATION)
        para_declaration = para_declaration->child;

    string tag = para_declaration->child->place;
    if (para_declaration->child->next == NULL || para_declaration->child->next->type == ABSTRACT_DECLARATOR)
    {
        if (para_declaration->child->place == "void")
            return;
        throw "abstract_declarator doesn't consider!";
    }
    process_initdeclarator(para_declaration->child->next,  tag,  func);

    gtree *temp;
    while (para_declaration->parent->next != NULL && para_declaration->parent->next->type == REMAIN && para_declaration->parent->next->place == ",")
    {
        para_declaration = para_declaration->parent;
        temp = para_declaration->next->next;
        tag = temp->child->place;
        if (temp->child->next == NULL || temp->child->next->type == ABSTRACT_DECLARATOR)
        {
            if (temp->child->place == "void")
                return;
            cout << "abstract_declarator doesn't consider!" << endl;
            exit(-1);
        }
        process_initdeclarator(temp->child->next, tag,  func);
    }
}

void CPN::Traverse_ST0(gtree *tree) {
    if(tree == NULL)return;
    if(tree->type == COMPOUND_STATEMENT)
        create_v_table(tree);

    Traverse_ST0(tree->child);
    Traverse_ST0(tree->next);
}

void CPN::Traverse_ST1(gtree *tree) {
    if(tree == NULL)return;

    //inherited attribute
    if(tree->type == DECLARATION)
        visit_declaration(tree);
    else if(tree->type == FUNCTION_DEFINITION)
        visit_function(tree);
    else if(judge_statement(tree))
        visit_statement(tree);
//    else if(judge_call_postfix_expression(tree))
//        visit_call(tree);
//    else if(tree->type == REMAIN && tree->place == "?")
//        visit_condition(tree->parent);//handle(condition?exp1:exp2) , because of 'cut_tree' cannot judge condition_exp simplely

    Traverse_ST1(tree->child);
    Traverse_ST1(tree->next);
}

void CPN::Traverse_ST2(gtree *tree) {
    if(tree == NULL)return;

    Traverse_ST2(tree->child);
    Traverse_ST2(tree->next);

    if(tree->type == COMPOUND_STATEMENT)
        handle_compound(tree);
    else if(tree->type == EXPRESSION)
        handle_expression(tree);
    else if(tree->type == ITERATION_STATEMENT || tree->type == SELECTION_STATEMENT)
        handle_iter_sel(tree);
    else if(tree->type == FUNCTION_DEFINITION)
        handle_func(tree);
    else if(judge_jump_statement(tree))//equal to jump_statement,the same level with SELECTION\ITERATION statement above
        handle_jump_statement(tree);
    else if(judge_call_postfix_expression(tree))
        handle_call(tree);
    else if(tree->child
            && tree->child->next && (tree->child->next->type == INC_OP || tree->child->next->type == DEC_OP))
        handle_INC_DEC_OP(tree->child->next->type,true,tree);
    else if(tree->child
            && (tree->child->type == INC_OP || tree->child->type == DEC_OP))
        handle_INC_DEC_OP(tree->child->type,false,tree);
    else if(tree->type == LABELED_STATEMENT)
        handle_label(tree);
//
    if(judge_statement(tree))
        organize_call(tree);

}

void CPN::Traverse_ST1_withoutdependence(gtree *tree) {
    if(tree == NULL)return;

    //inherited attribute
    if(tree->type == DECLARATION)
        visit_declaration(tree);
    else if(tree->type == FUNCTION_DEFINITION)
        visit_function(tree);
    else if(judge_statement(tree))
        visit_statement(tree);
//    else if(judge_call_postfix_expression(tree))
//        visit_call(tree);
//    else if(tree->type == REMAIN && tree->place == "?")
//        visit_condition(tree->parent);//handle(condition?exp1:exp2) , because of 'cut_tree' cannot judge condition_exp simplely

    Traverse_ST1_withoutdependence(tree->child);
    Traverse_ST1_withoutdependence(tree->next);
}

void CPN::Traverse_ST2_withoutdependence(gtree *tree) {
   if(tree == NULL)return;

    Traverse_ST2_withoutdependence(tree->child);
    Traverse_ST2_withoutdependence(tree->next);

    if(tree->type == COMPOUND_STATEMENT)
        handle_compound_withoutdependence(tree);
    else if(tree->type == EXPRESSION)
        handle_expression(tree);
    else if(tree->type == ITERATION_STATEMENT || tree->type == SELECTION_STATEMENT)
        handle_iter_sel_withoutdependence(tree);
    else if(tree->type == FUNCTION_DEFINITION)
        handle_func(tree);
    else if(judge_jump_statement(tree))//equal to jump_statement,the same level with SELECTION\ITERATION statement above
        handle_jump_statement_withoutdependence(tree);
    else if(judge_call_postfix_expression(tree))
        handle_call_withoutdependence(tree);
    else if(tree->child
            && tree->child->next && (tree->child->next->type == INC_OP || tree->child->next->type == DEC_OP))
        handle_INC_DEC_OP(tree->child->next->type,true,tree);
    else if(tree->child
            && (tree->child->type == INC_OP || tree->child->type == DEC_OP))
        handle_INC_DEC_OP(tree->child->type,false,tree);
    else if(tree->type == LABELED_STATEMENT)
        handle_label(tree);
//
    if(judge_statement(tree))
        organize_call_withoutdependence(tree); 
}

void CPN::visit_declaration(gtree *p) {
//    gtree *p1 = p->parent;
//    string func = "";
//    while (p1 != NULL && p1->type != COMPOUND_STATEMENT)
//        p1 = p1->parent;
//    if (p1 != NULL)
//        func = p1->place;
//    else
//        func = global_table_name;
    if(p->child->type == DECLARATION_SPECIFIERS && p->child->child->type == STORAGE_CLASS_SPECIFIER && p->child->child->child->type == TYPEDEF)
        return;
    process_declaration(p);
}

void CPN::visit_function(gtree *p) {
    string ret_tag;
    if (p->child->type == DECLARATION_SPECIFIERS) {
        if(p->child->next->child->type == POINTER)
            ret_tag = "int";
        else
            ret_tag = p->child->place;
    }
    else
    {
        cout << "we don't support function that don't have return type for now!" << endl;
        exit(-1);
    }
    string func = p->place;
////    bool return_pointer_flag = false;
//    if (p->child->next->child->type == POINTER) {
//        func = p->child->next->child->next->child->child->place;
////        return_pointer_flag = true;
//        //ret_tag = "*int";//unsolve pointer
//    }
//    else
//        func = p->child->next->child->child->child->place;

    //construct begin_P,end_P,begin_T

    string begin_P = gen_P();
    p->matched_P = begin_P;
    Add_Place(begin_P,controltypename,true,SizeList(),func + begin_suffix,true,false,p->row);
//    mapFunction.insert(make_pair(func + begin_suffix,begin_P));

    string begin_T = gen_T();
    Add_Transition(begin_T,"",func + begin_suffix,p->row);
    vector<string> enter;
    enter.push_back(begin_T);

    Add_enter_T(begin_P,enter);

    Add_Arc(begin_P,begin_T,controlflowArcExp(tid_str),true,control,normal);

    string end_P = gen_P();
    Add_Place(end_P,controltypename,true,SizeList(),func+ end_suffix,true,false,p->row);
//    mapFunction.insert(make_pair(func + end_suffix,end_P));
//    mapFunction.insert(make_pair(begin_P,ret_tag));

    f_table.insert(func,begin_P,end_P,ret_tag);
    //construct parameter_P


    gtree *direct_declarator;
    if(p->child->type == DECLARATION_SPECIFIERS && p->child->next->type == DECLARATOR
       && p->child->next->child->type == POINTER)
        direct_declarator = p->child->next->child->next;
    else if(p->child->type == DECLARATION_SPECIFIERS && p->child->next->type == DECLARATOR
            && p->child->next->child->type == DIRECT_DECLARATOR)
        direct_declarator = p->child->next->child;
    else
        throw "ERROR!There is something wrong with function:"+p->place;
    if (direct_declarator->child->next->type == REMAIN && direct_declarator->child->next->place == "(")
    {
        if (direct_declarator->child->next->next->type == PARAMETER_TYPE_LIST)
        {
            gtree *para_type_list = direct_declarator->child->next->next;

            process_para_type_list(para_type_list,func);
        }
        else//no para
        ;
    }
    else
    {
        cout << "there is no '('!" << endl;
        exit(-1);
    }
}

void CPN::visit_function_withoutdependence(gtree *p) {
    string ret_tag;
    if (p->child->type == DECLARATION_SPECIFIERS) {
        if(p->child->next->child->type == POINTER)
            ret_tag = "int";
        else
            ret_tag = p->child->place;
    }
    else
    {
        cout << "we don't support function that don't have return type for now!" << endl;
        exit(-1);
    }
    string func = p->place;
////    bool return_pointer_flag = false;
//    if (p->child->next->child->type == POINTER) {
//        func = p->child->next->child->next->child->child->place;
////        return_pointer_flag = true;
//        //ret_tag = "*int";//unsolve pointer
//    }
//    else
//        func = p->child->next->child->child->child->place;

    //construct begin_P,end_P,begin_T

    string begin_P = gen_P();
    p->matched_P = begin_P;
    Add_Place(begin_P,controltypename,true,SizeList(),func + begin_suffix,true,false,p->row);
//    mapFunction.insert(make_pair(func + begin_suffix,begin_P));

    string begin_T = gen_T();
    Add_Transition(begin_T,"",func + begin_suffix,p->row);
    vector<string> enter;
    enter.push_back(begin_T);

    Add_enter_T(begin_P,enter);

    Add_Arc(begin_P,begin_T,controlflowArcExp(tid_str),true,control,normal);

    string end_P = gen_P();
    Add_Place(end_P,controltypename,true,SizeList(),func+ end_suffix,true,false,p->row);

    string end_T = gen_T();
    Add_Transition(end_T,"",func + end_suffix,p->row);

    Add_enter_T(end_P,end_T);

    Add_Arc(end_P,end_T,controlflowArcExp(tid_str),true,control,normal);
//    mapFunction.insert(make_pair(func + end_suffix,end_P));
//    mapFunction.insert(make_pair(begin_P,ret_tag));

    f_table.insert(func,begin_P,end_P,ret_tag);
    //construct parameter_P


    gtree *direct_declarator;
    if(p->child->type == DECLARATION_SPECIFIERS && p->child->next->type == DECLARATOR
       && p->child->next->child->type == POINTER)
        direct_declarator = p->child->next->child->next;
    else if(p->child->type == DECLARATION_SPECIFIERS && p->child->next->type == DECLARATOR
            && p->child->next->child->type == DIRECT_DECLARATOR)
        direct_declarator = p->child->next->child;
    else
        throw "ERROR!There is something wrong with function:"+p->place;
    if (direct_declarator->child->next->type == REMAIN && direct_declarator->child->next->place == "(")
    {
        if (direct_declarator->child->next->next->type == PARAMETER_TYPE_LIST)
        {
            gtree *para_type_list = direct_declarator->child->next->next;

            process_para_type_list(para_type_list,func);
        }
        else//no para
        ;
    }
    else
    {
        cout << "there is no '('!" << endl;
        exit(-1);
    }
}

void CPN::visit_statement(gtree *p) {
    if(p->child->type == SELECTION_STATEMENT || p->child->type == ITERATION_STATEMENT)
    {
        //construct  P、T
        string control_P = gen_P();
        gtree *statement = p;
//        while(statement->type != STATEMENT)
//            statement = statement->parent;
        gtree *com = p;
        while(com->type!=COMPOUND_STATEMENT)
            com = com->parent;
        string base = com->place;
        statement->matched_P = control_P;
        Add_Place(control_P,controltypename,true,SizeList(),p->place,true,false,p->row);
        string control_T1 = gen_T(),control_T2 = gen_T();

        //guard is added in backward traverse
        Add_Transition(control_T1,"",base,p->row);
        Add_Transition(control_T2,"",base,p->row);
        Add_Arc(control_P,control_T1,controlflowArcExp(tid_str),true,control,normal);
        Add_Arc(control_P,control_T2,controlflowArcExp(tid_str),true,control,normal);

        //set enter,enter_P
        vector<string> enter,enter_P;
        enter.push_back(control_T1);
        enter.push_back(control_T2);
        enter_P.push_back(control_P);
        Add_enter_T(control_P,enter);
    }
    else
    {
        //construct  P、T
        string control_P = gen_P();
        gtree *statement = p;
//        while(statement->type != STATEMENT)
//            statement = statement->parent;
        statement->matched_P = control_P;
        Row_Type currentRow;

        ///COMPOUND_STATEMENT不记录行号
        if(p->child->type == COMPOUND_STATEMENT)
            currentRow = NoneRow;
        else
            currentRow = p->row;
        Add_Place(control_P,controltypename,true,SizeList(),p->place,true,false,currentRow);
        string control_T = gen_T();
        Add_Transition(control_T,"",p->place,currentRow);
        Add_Arc(control_P,control_T,controlflowArcExp(tid_str),true,control,normal);

        //set exit,enter
        vector<string> enter,exit_T;
        enter.push_back(control_T);
        exit_T.push_back(control_T);//compound_statement's exit_T will clear this
        //enter_P.push_back(control_P);
        Add_enter_T(control_P,enter);
        Add_exit_T(control_P,exit_T);

//        if(judge_return(statement)){
//            auto tier = mapTransition.find(control_T);
//            if(tier == mapTransition.end()){
//                throw "ERROR!can't find transition!";
//            transition[tier->second].isreturn = true;
//        }
    }
}

void CPN::visit_condition(gtree *p) {
    string logic_or_exp,exp1,exp2;
//    logic_or_exp = p->child->place;
    exp1 = p->child->next->next->place;
    exp2 = p->child->next->next->next->next->place;

//    gtree *com = p;
//    while(com->type != COMPOUND_STATEMENT)
//        com = com->parent;
//    string base = com->place;

    p->place = CaseFlag + translate_exp2arcexp(p) + ":" + to_string(0) + "=>" + exp2 + ":" + exp1 + ";";

    gtree *tmp_pointer = p->parent,*tmp_child;
    while(tmp_pointer->type != EXPRESSION){
        tmp_child = tmp_pointer->child;
        string tmp_str = "";
        while(tmp_child){
            tmp_str.append(tmp_child->place);
            tmp_child = tmp_child->next;
        }
        tmp_pointer->place = tmp_str;
        tmp_pointer = tmp_pointer->parent;
    }
    tmp_child = tmp_pointer->child;
    string tmp_str = "";
    while(tmp_child){
        tmp_str.append(tmp_child->place);
        tmp_child = tmp_child->next;
    }
    tmp_pointer->place = tmp_str;
}

void CPN::handle_compound(gtree *p) {
    string statement_P = p->parent->matched_P;
    string statement_T = get_enter_T(statement_P)[0];//compound_statement just have one enter_T

    inside_block(p,statement_T);

    //set exit
    vector<string> exit_T;

    exit_T = get_compound_statement_exit(p);
//    if(exit_T.size()!=0)
    reset_exit_T(statement_P,exit_T);
}

void CPN::handle_compound_withoutdependence(gtree *p) {
    string statement_P = p->parent->matched_P;
    string statement_T = get_enter_T(statement_P)[0];//compound_statement just have one enter_T

    inside_block_withoutdependence(p,statement_T);

    //set exit
    vector<string> exit_T;

    exit_T = get_compound_statement_exit(p);
//    if(exit_T.size()!=0)
    reset_exit_T(statement_P,exit_T);
}


void CPN::handle_unary_expression(gtree *p,string T,string base,string content){
    writeOperation(T,p,content);
}

void CPN::handle_conditional_expression(gtree *p,string T,string base){
    if(p->type != CONDITIONAL_EXPRESSION)
        create_connect(T, p->parent->place ,base);
    else
        create_connect(T, p->place ,base);
}

void CPN::handle_assignment_expression(gtree *p,string T){
//    if(p->child->type == CONDITIONAL_EXPRESSION)

    gtree *unary_expression = p->child,*conditional_expression,*com = p;
    while(com->type!=COMPOUND_STATEMENT)
        com = com->parent;
    string base = com->place;
    while(unary_expression->type == UNARY_EXPRESSION && unary_expression->next && unary_expression->next->type == ASSIGNMENT_OPERATOR)
//        {
//            handle_unary_expression(cpn,unary_expression,T);
        unary_expression = unary_expression->next->next->child;
//        }
    conditional_expression = unary_expression;
    string content = translate_exp2arcexp(conditional_expression);
    handle_conditional_expression(conditional_expression,T,base);
    unary_expression = p->child;
    while(unary_expression->type == UNARY_EXPRESSION
    && unary_expression->next && unary_expression->next->type == ASSIGNMENT_OPERATOR)
    {
        handle_unary_expression(unary_expression,T,base,content);
        unary_expression = unary_expression->next->next->child;
    }
}

void CPN::handle_expression(gtree *p) {
    gtree *expression=p,*assignment_expression,*statement=p;
    while(statement && statement->type != STATEMENT)
        statement = statement->parent;
    if(!statement || statement->is_processed)
        return;
    string statement_P = statement->matched_P;
    vector<string> statement_T = get_enter_T(statement_P);
//    while(expression->type == EXPRESSION)
//        expression = expression->child;
    if(expression->child->type == ASSIGNMENT_EXPRESSION)
        assignment_expression = expression->child;
    else
        assignment_expression = expression->child->next->next;
//    while(assignment_expression && assignment_expression->parent->next) {
    if(statement_T.size()==0){cout<<"error in handle_expression!"<<endl;exit(-1);}
    else if(statement_T.size()==1)
        handle_assignment_expression(assignment_expression,statement_T[0]);
    else if(statement_T.size()==2){
        handle_assignment_expression(assignment_expression,statement_T[0]);
        handle_assignment_expression(assignment_expression,statement_T[1]);
    }
//        assignment_expression = assignment_expression->parent->next->next;
//    }
}

void CPN::handle_func(gtree *p) {
    string func;
    func = p->place;

    string begin_P = f_table.findbeginplace(func);;
    string end_P = f_table.findendplace(func);

    vector<string> enter = get_enter_T(begin_P);
    string begin_T = enter[0];

    vector<string> v;//store the exit_Ts

    gtree *compound_statement = p->child;
    while (compound_statement->type != COMPOUND_STATEMENT)
        compound_statement = compound_statement->next;



    gtree *statement_list = compound_statement->child;
    while (statement_list != NULL && statement_list->type != STATEMENT_LIST)
        statement_list = statement_list->next;
    if (statement_list == NULL) {

//        string identifier = p->place;
//        auto iter1 = mapFunction.find(identifier + begin_suffix);
        vector<string> enter_T = get_enter_T(begin_P);
//        auto iter2 = mapFunction.find(identifier + end_suffix);

        Add_Arc(begin_T, end_P, controlflowArcExp(tid_str), false, executed,beoverrided);
        return;
    }
    gtree *statement = statement_list;
    while (statement->type != STATEMENT)
        statement = statement->child;

    bool already_return = false;

    while (statement->parent != statement_list)
    {
        if (judge_return(statement))
        {
            already_return = true;
            break;
        }
        statement = statement->parent->next;
    }
    if (already_return == false)
    {
        if (judge_return(statement))
            already_return = true;
        else
        {
            vector<string> v1 = get_exit_T(statement->matched_P);
            v.insert(v.end(), v1.begin(), v1.end());
        }
    }

    if (already_return == false)
    {
//        string func_end = iter1->second;
        string func_v = "";
        if (p->child->type == DECLARATION_SPECIFIERS && p->child->child->type == TYPE_SPECIFIER && p->child->child->place == "void")
            ;
        else//no return
            ;

        for (unsigned int i = 0; i < v.size(); i++)
            Add_Arc(v[i], end_P, controlflowArcExp(tid_str), false,call_exit,beoverrided);
    }

    // func_T to para
    string Exp;
    Exp = "NULL";

    auto paras = f_table.findfuncparas(func);

//    string base = compound_statement->place;
    for(unsigned int i=0;i<paras.size();i++) {

        Add_Arc(begin_T, paras[i].second, Exp, false, writeArc,normal);
        Add_Arc(paras[i].second,begin_T,  Exp, true, writeArc,normal);
    }
}

bool judge_deadloop(gtree *p){
    if(p->type == ITERATION_STATEMENT){
        auto statement = p->child->next->next->next->next;
        if(statement->child->type == EXPRESSION_STATEMENT && statement->child->child->place == ";"
        || statement->child->type == COMPOUND_STATEMENT && statement->child->child->next->place == "}")
            return true;
        auto condition = p->child->next->next;
        if(condition->place == "1")
            return true;
    }
    return false;
}

void CPN::handle_iter_sel(gtree *p) {
    gtree *statement = p->parent;
    string control_P = statement->matched_P;
    string control_T1 = get_enter_T(control_P)[0];
    string control_T2 = get_enter_T(control_P)[1];

    //set guard , must be set in backward traverse, because the expression might change(in case of return_v)

    gtree *com = p;
    while(com->type != COMPOUND_STATEMENT)
        com = com->parent;
    string base = com->place;
    string condition = p->child->next->next->place;

    auto tt1 = findT_byid(control_T1),tt2 = findT_byid(control_T2);
    condition = translate_exp2arcexp(p->child->next->next);
//    Set_Guard(control_T1,condition,base);
    tt1->reset_Guard(condition);
    string condition_op = "!("+condition +")";//opposite_all(condition);
    tt2->reset_Guard(condition_op);
//    Set_Guard(control_T2,condition_op,base);
//    create_connect(control_T1,condition,base);
//    create_connect(control_T2,condition,base);

    //set exit
    vector<string> exit_T, falseexit;

    vector<string> temp1, temp2;
    gtree *statement1 = p->child->next->next->next->next;
    gtree *statement2 = NULL;
//    temp1 = get_statement_exit(statement1, this);
    temp1 = get_exit_T(statement1->matched_P);
    if (p->child->next->next->next->next->next) {
        statement2 = p->child->next->next->next->next->next->next;
//        temp2 = get_statement_exit(statement2, this);
        temp2 = get_exit_T(statement2->matched_P);
    }
    else
        temp2.push_back(control_T2);
    if (p->type == SELECTION_STATEMENT) {
        if(temp1.size()!=0)
            exit_T.insert(exit_T.end(),temp1.begin(),temp1.end());
//        else
//            exit_T.push_back(control_T1);
        if(temp2.size()!=0)
            exit_T.insert(exit_T.end(),temp2.begin(),temp2.end());
//        else
//            exit_T.push_back(control_T2);
    }
    else if(p->type == ITERATION_STATEMENT && p->child->type == DO)
        throw "Do while problem!";
    else
    {
        ///deadloop
        if(judge_deadloop(p))
            deadloops.push_back(control_P);

        if(temp1.size()!=0)
            falseexit.insert(exit_T.end(),temp1.begin(),temp1.end());
//        else
//            falseexit.push_back(control_T1);
        exit_T.push_back(control_T2);
    }
    Add_exit_T(control_P,exit_T);
    if(falseexit.size()!=0)
        Add_falseexit_T(control_P,falseexit);

    //connect with child statement
    string child_P1,child_P2;

    child_P1 = statement1->matched_P;
    vector<string> call_P1 = get_call_P(child_P1);
    if(!call_P1.empty())
        child_P1 = call_P1[0];
    vector<string> temp;
    temp.push_back(control_T1);
    Add_Arc(control_T1,child_P1,controlflowArcExp(tid_str),false,control,normal);
    Add_executed_P(temp,get_enter_T(child_P1));
    if(statement2) {
        child_P2 = statement2->matched_P;
        vector<string> call_P2 = get_call_P(child_P2);
        if(!call_P2.empty())
            child_P2 = call_P2[0];
        Add_Arc(control_T2,child_P2,controlflowArcExp(tid_str),false,control,normal);
        temp.clear();
        temp.push_back(control_T2);
        Add_executed_P(temp,get_enter_T(child_P2));
    }
}

void CPN::handle_iter_sel_withoutdependence(gtree *p) {
    gtree *statement = p->parent;
    string control_P = statement->matched_P;
    string control_T1 = get_enter_T(control_P)[0];
    string control_T2 = get_enter_T(control_P)[1];

    //set guard , must be set in backward traverse, because the expression might change(in case of return_v)

    gtree *com = p;
    while(com->type != COMPOUND_STATEMENT)
        com = com->parent;
    string base = com->place;
    string condition = p->child->next->next->place;

    auto tt1 = findT_byid(control_T1),tt2 = findT_byid(control_T2);
    condition = translate_exp2arcexp(p->child->next->next);
//    Set_Guard(control_T1,condition,base);
    tt1->reset_Guard(condition);
    string condition_op = "!("+condition +")";//opposite_all(condition);
    tt2->reset_Guard(condition_op);
//    Set_Guard(control_T2,condition_op,base);
//    create_connect(control_T1,condition,base);
//    create_connect(control_T2,condition,base);

    //set exit
    vector<string> exit_T, falseexit;

    vector<string> temp1, temp2;
    gtree *statement1 = p->child->next->next->next->next;
    gtree *statement2 = NULL;
//    temp1 = get_statement_exit(statement1, this);
    temp1 = get_exit_T(statement1->matched_P);
    if (p->child->next->next->next->next->next) {
        statement2 = p->child->next->next->next->next->next->next;
//        temp2 = get_statement_exit(statement2, this);
        temp2 = get_exit_T(statement2->matched_P);
    }
    else
        temp2.push_back(control_T2);
    if (p->type == SELECTION_STATEMENT) {
        if(temp1.size()!=0)
            exit_T.insert(exit_T.end(),temp1.begin(),temp1.end());
//        else
//            exit_T.push_back(control_T1);
        if(temp2.size()!=0)
            exit_T.insert(exit_T.end(),temp2.begin(),temp2.end());
//        else
//            exit_T.push_back(control_T2);
    }
    else if(p->type == ITERATION_STATEMENT && p->child->type == DO)
        throw "Do while problem!";
    else
    {
        ///deadloop
        if(judge_deadloop(p))
            deadloops.push_back(control_P);

        if(temp1.size()!=0)
            falseexit.insert(exit_T.end(),temp1.begin(),temp1.end());
//        else
//            falseexit.push_back(control_T1);
        exit_T.push_back(control_T2);
    }
    Add_exit_T(control_P,exit_T);
    if(falseexit.size()!=0)
        Add_falseexit_T(control_P,falseexit);

    //connect with child statement
    string child_P1,child_P2;

    child_P1 = statement1->matched_P;
    vector<string> call_P1 = get_call_P(child_P1);
    if(!call_P1.empty())
        child_P1 = call_P1[0];
    // vector<string> temp;
    // temp.push_back(control_T1);
    Add_Arc(control_T1,child_P1,controlflowArcExp(tid_str),false,control,normal);
    // Add_executed_P(temp,get_enter_T(child_P1));
    if(statement2) {
        child_P2 = statement2->matched_P;
        vector<string> call_P2 = get_call_P(child_P2);
        if(!call_P2.empty())
            child_P2 = call_P2[0];
        Add_Arc(control_T2,child_P2,controlflowArcExp(tid_str),false,control,normal);
        // temp.clear();
        // temp.push_back(control_T2);
        // Add_executed_P(temp,get_enter_T(child_P2));
    }
}

void CPN::handle_jump_statement(gtree *p) {
    gtree *statement = p;
    while (statement->type != STATEMENT)
        statement = statement->parent;
    string control_P = statement->matched_P;
    string control_T = get_enter_T(control_P)[0];
    if(p->child->child->type == RETURN) {
        gtree *find_func = p;

        while (find_func->type != FUNCTION_DEFINITION)
            find_func = find_func->parent;

        string identifier = find_func->place;

        gtree *exp_tree;
        string expression;
        if (p->child->child->next->type == EXPRESSION) {
            expression = p->child->child->next->place;
            exp_tree = p->child->child->next;
        }
        else {
            expression = "";
            exp_tree = NULL;
        }


//        auto iter_end = mapFunction.find(identifier + end_suffix);
//        if (iter_end == mapFunction.end()) {
//            cout << "can't find end_P in create_CPN" << endl;
//            exit(-1);
//        }

        string last_func_end = f_table.findendplace(identifier);
        Add_Arc(control_T, last_func_end, controlflowArcExp(tid_str), false, call_exit,normal);

//        auto iter_begin = mapFunction.find(identifier + begin_suffix);
//        if (iter_begin == mapFunction.end()) {
//            cout << "can't find end_P in create_CPN" << endl;
//            exit(-1);
//        }
        string last_func_begin = f_table.findbeginplace(identifier);
        gtree *com = p;
        while(com->type!=COMPOUND_STATEMENT)
            com = com->parent;
        string base = com->place;
        f_table.insert_returns(identifier,control_T,exp_tree);
//        Add_returns(last_func_begin,control_T);

        create_connect(control_T, expression, base);

    }
    else if(p->child->child->type == CONTINUE){
        gtree *last_while = p;
        while(last_while && last_while->type != ITERATION_STATEMENT)
            last_while = last_while->parent;
        string last_while_P = last_while->parent->matched_P;
        Add_falseexit_T(last_while_P,control_T);

        //continue和while关联
        auto cor_P = get_correspond_P(last_while_P);
        cor_P.push_back(control_P);
        set_correspond_P(last_while_P,cor_P);
    }
    else if(p->child->child->type == BREAK){
        gtree *last_while = p;
        while(last_while && last_while->type != ITERATION_STATEMENT)
            last_while = last_while->parent;
        string last_while_P = last_while->parent->matched_P;
        Add_exit_T(last_while_P,control_T);

        //break和while关联
        auto cor_P = get_correspond_P(last_while_P);
        cor_P.push_back(control_P);
        set_correspond_P(last_while_P,cor_P);
    }
    else if(p->child->child->type == GOTO){
//        Add_Goto(control_T,p->child->child->next->place);
        throw "we can not handle goto for now!";
    }
    else
        throw "error in jump_statement!";
    reset_exit_T(control_P,vector<string>());//clear exit
}

void CPN::handle_jump_statement_withoutdependence(gtree *p) {
    gtree *statement = p;
    while (statement->type != STATEMENT)
        statement = statement->parent;
    string control_P = statement->matched_P;
    string control_T = get_enter_T(control_P)[0];
    if(p->child->child->type == RETURN) {
        gtree *find_func = p;

        while (find_func->type != FUNCTION_DEFINITION)
            find_func = find_func->parent;

        string identifier = find_func->place;

        gtree *exp_tree;
        string expression;
        if (p->child->child->next->type == EXPRESSION) {
            expression = p->child->child->next->place;
            exp_tree = p->child->child->next;
        }
        else {
            expression = "";
            exp_tree = NULL;
        }


//        auto iter_end = mapFunction.find(identifier + end_suffix);
//        if (iter_end == mapFunction.end()) {
//            cout << "can't find end_P in create_CPN" << endl;
//            exit(-1);
//        }

        string last_func_end = f_table.findendplace(identifier);
        Add_Arc(control_T, last_func_end, controlflowArcExp(tid_str), false, call_exit,normal);

//        auto iter_begin = mapFunction.find(identifier + begin_suffix);
//        if (iter_begin == mapFunction.end()) {
//            cout << "can't find end_P in create_CPN" << endl;
//            exit(-1);
//        }
        string last_func_begin = f_table.findbeginplace(identifier);
        gtree *com = p;
        while(com->type!=COMPOUND_STATEMENT)
            com = com->parent;
        string base = com->place;
        f_table.insert_returns(identifier,control_T,exp_tree);
//        Add_returns(last_func_begin,control_T);

        create_connect(control_T, expression, base);

    }
    else if(p->child->child->type == CONTINUE){
        gtree *last_while = p;
        while(last_while && last_while->type != ITERATION_STATEMENT)
            last_while = last_while->parent;
        string last_while_P = last_while->parent->matched_P;
        Add_falseexit_T(last_while_P,control_T);

        //continue和while关联
        auto cor_P = get_correspond_P(last_while_P);
        cor_P.push_back(control_P);
        set_correspond_P(last_while_P,cor_P);
    }
    else if(p->child->child->type == BREAK){
        gtree *last_while = p;
        while(last_while && last_while->type != ITERATION_STATEMENT)
            last_while = last_while->parent;
        string last_while_P = last_while->parent->matched_P;
        Add_exit_T(last_while_P,control_T);

        //break和while关联
        auto cor_P = get_correspond_P(last_while_P);
        cor_P.push_back(control_P);
        set_correspond_P(last_while_P,cor_P);
    }
    else if(p->child->child->type == GOTO){
//        Add_Goto(control_T,p->child->child->next->place);
        cout<< "we can not handle goto for now!"<<endl;
        exit(-1);
    }
    else{
        cout<< "error in jump_statement!"<<endl;
        exit(-1);
    }
    reset_exit_T(control_P,vector<string>());//clear exit
}

void CPN::handle_call(gtree *p) {
    gtree *com = p;
    while (com->type != COMPOUND_STATEMENT)
        com = com->parent;
    string base = com->place;



    if(exist_in(pthread_func_type,p->child->place))
    {
        // get statement_P and paras
        gtree *statement = p;
        while (statement->type != STATEMENT)
            statement = statement->parent;
        string statement_P = statement->matched_P;

        vector<gtree *> paras;
        paras = extract_paras(p);
        if(p->child->place == "pthread_create") {
            gtree* thread_v = paras[0], *thread_func = paras[2],*para_pass = paras[3];
//            if (thread_v[0] == '&')
//                thread_v = thread_v.substr(1);
//            if(para_pass[0] == '&')
//                para_pass = para_pass.substr(1);

            string func_begin_P = f_table.findbeginplace(thread_func->place);

            string func_end_P = f_table.findendplace(thread_func->place);

            //add thread map
            string newtid = translate_exp2tid(thread_v);

            newtid = addQuote(newtid);
            mapPthread.insert(make_pair(newtid + begin_suffix, func_begin_P));
            mapPthread.insert(make_pair(newtid + end_suffix, func_end_P));


            //add thread arc
            string newtidstr = newtid ;
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0], func_begin_P, controlflowArcExp(newtidstr), false, control,normal);
            if(enter_T.size() > 1)
                Add_Arc(enter_T[1], func_begin_P, controlflowArcExp(newtidstr), false, control,normal);

            //add copy

//
//            string statement_T = get_enter_T(statement_P)[0];
            ///!!!
            auto paras = f_table.findfuncparas(thread_func->place);
            if(paras.size()!=1)
                throw "pthread_create's target function just have one parameter!";

            Add_pthreadCopys(enter_T[0],newtidstr,paras[0].second, translate_exp2arcexp(para_pass));
            if(enter_T.size() > 1){
                Add_pthreadCopys(enter_T[1],newtidstr,paras[0].second, translate_exp2arcexp(para_pass));
            }

            if(join_create.find(newtid) == join_create.end())
                join_create.insert(make_pair(newtid,statement_P));

        }
        else if(p->child->place == "pthread_join")
        {
            gtree *thread_v = paras[0];
//            if (thread_v[0] == '&')
//                thread_v = thread_v.substr(1);

//            string jointid = translate_exp2tid(thread_v) + id_suffix;// + id_suffix because the parameter is different with pthread_create
            // + id_suffix because the parameter is different with pthread_create
//            string jointid =  add_idPostfix(translate_exp2tid(thread_v));
            string jointidstr = add_idPostfix(translate_exp2tid(thread_v));

            jointidstr = addQuote(jointidstr);
            auto iter = mapPthread.find(jointidstr + end_suffix);
            if(iter == mapPthread.end())
                throw "can't find pthread_end in mapPthread";
            string func_end_P = iter->second;

            //add map
            auto biter = mapPthread.find(jointidstr + begin_suffix);
            if(biter == mapPthread.end())
                throw "can't find pthread_begin when pthread_join";
//            mapJoin.insert(make_pair(biter->second,statement_P));

            //add thread arc
            vector<string> enter_T = get_enter_T(statement_P);


            Add_Arc(func_end_P,enter_T[0], controlflowArcExp(jointidstr), true, control,normal);
            if(enter_T.size() > 1)
                Add_Arc(func_end_P,enter_T[1], controlflowArcExp(jointidstr), true, control,normal);


            //cor_P
//            string thread_idexp;
//            auto pos = thread_v.find("[");
//            if(pos != string::npos)
//                thread_idexp = thread_v.substr(0,pos);
//            else
//                thread_idexp = thread_v;
            auto mapiter = join_create.find(jointidstr);
            if(mapiter == join_create.end())
                throw "ERROR!pthread_join doesn't have correspond create!";
            vector<string> cor_P = get_correspond_P(mapiter->second);
            cor_P.push_back(statement_P);
            set_correspond_P(mapiter->second,cor_P);
        }
        else if(p->child->place == "pthread_exit")
        {
            gtree *find_func = p;

            while (find_func->type != FUNCTION_DEFINITION)
                find_func = find_func->parent;
            string identifier = find_func->place;

            string last_func_end = f_table.findendplace(identifier);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0], last_func_end, controlflowArcExp(tid_str), false,executed,normal);
        }
        else if(p->child->place == "pthread_mutex_init")
        {
            string mutex_v = paras[0]->place;
            if(mutex_v[0]=='&')
                mutex_v = mutex_v.substr(1);
            string Exp;
            auto pos = mutex_v.find("[");
            if(pos != string::npos){
                Exp = "1`" + mutex_v.substr(pos+1,mutex_v.length()-pos-2);
                mutex_v = mutex_v.substr(0,pos);
            }
            else{
                Exp = "1`0";
            }
            gtree *com = p;
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string mutex_P = find_P_id(mutex_v,base);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0],mutex_P,Exp,false,control,normal);
        }
        else if(p->child->place == "pthread_mutex_lock")
        {
            string mutex_v = paras[0]->place;
            if(mutex_v[0]=='&')
                mutex_v = mutex_v.substr(1);
            string Exp;
            auto pos = mutex_v.find("[");
            if(pos != string::npos){
                Exp = "1`" + mutex_v.substr(pos+1,mutex_v.length()-pos-2);
                mutex_v = mutex_v.substr(0,pos);
            }
            else{
                Exp = "1`0";
            }
            gtree *com = p;
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string mutex_P = find_P_id(mutex_v,base);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(mutex_P,enter_T[0],Exp,true,control,normal);

            gtree *tr = statement->parent->next;

        }
        else if(p->child->place == "pthread_mutex_unlock")
        {
            string mutex_v = paras[0]->place;
            if(mutex_v[0]=='&')
                mutex_v = mutex_v.substr(1);
            string Exp;
            auto pos = mutex_v.find("[");
            if(pos != string::npos){
                Exp = "1`" + mutex_v.substr(pos+1,mutex_v.length()-pos-2);
                mutex_v = mutex_v.substr(0,pos);
            }
            else{
                Exp = "1`0";
            }
            gtree *com = p;
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string mutex_P = find_P_id(mutex_v,base);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0],mutex_P,Exp,false,control,normal);
        }
        else if(p->child->place == "pthread_cond_signal")
        {
            string cond_v = paras[0]->place;
            if (cond_v[0] == '&')
                cond_v = cond_v.substr(1);
            gtree *com = p;
            string Exp;
            auto pos = cond_v.find("[");
            if(pos != string::npos){
                Exp = "1`" + cond_v.substr(pos+1,cond_v.length()-pos-2);
                cond_v = cond_v.substr(0,pos);
            }
            else{
                Exp = "1`0";
            }
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string cond_P = find_P_id(cond_v,base);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0],cond_P,Exp,false,control,normal);
        }
        else if(p->child->place == "pthread_cond_wait")
        {
            string cond_v = paras[0]->place,mutex_v = paras[1]->place;
            if (cond_v[0] == '&')
                cond_v = cond_v.substr(1);
            if(mutex_v[0] == '&')
                mutex_v = mutex_v.substr(1);
            string Exp_mutex,Exp_cond;
            auto pos = cond_v.find("[");
            if(pos != string::npos){
                Exp_cond = "1`" +cond_v.substr(pos+1,cond_v.length()-pos-2);
                cond_v = cond_v.substr(0,pos);
            }
            else{
                Exp_cond = "1`0";
            }
            pos = mutex_v.find("[");
            if(pos != string::npos){
                Exp_mutex = "1`"+mutex_v.substr(pos+1,mutex_v.length()-pos-2);
                mutex_v = mutex_v.substr(0,pos);
            }
            else{
                Exp_mutex = "1`0";
            }
            gtree *com = p;
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string cond_P = find_P_id(cond_v,base);
            string mutex_P = find_P_id(mutex_v,base);
            vector<string> enter_T = get_enter_T(statement_P);


            //construct P、T
            string P1,P2,P3,T1,T2;
            P1 = gen_P();
            P2 = gen_P();
            P3 = gen_P();
            T1 = gen_T();
            T2 = gen_T();

            Add_Place(P1,controltypename,true,SizeList(),"signalP1",true,false,NoneRow);
            Add_Place(P2,controltypename,true,SizeList(),"signalP2",true,false,NoneRow);
            Add_Place(P3,controltypename,true,SizeList(),"signalP3",true,false,NoneRow);
            Add_Transition(T1,"","signalT1",NoneRow);
            Add_Transition(T2,"","signalT2",NoneRow);

            //construct arcs
            Add_Arc(enter_T[0],P1,controlflowArcExp(tid_str),false,control,normal);
            Add_Arc(enter_T[0],P3,controlflowArcExp(tid_str),false,executed,normal);
            Add_Arc(enter_T[0],mutex_P,Exp_mutex,false,control,normal);
            Add_Arc(cond_P,T1,Exp_cond,true,control,normal);
            Add_Arc(P1,T1,controlflowArcExp(tid_str),true,control,normal);
            Add_Arc(T1,P2,controlflowArcExp(tid_str),false,control,normal);
            Add_Arc(mutex_P,T2,Exp_mutex,true,control,normal);
            Add_Arc(P2,T2,controlflowArcExp(tid_str),true,control,normal);
            Add_Arc(P3,T2,controlflowArcExp(tid_str),true,control,normal);

            //set exit
            vector<string> exit_T;
            exit_T.push_back(T2);
            reset_exit_T(statement_P,exit_T);

//            vector<string> cor_P;
//            cor_P.push_back(P1);
//            cor_P.push_back(P2);
//            cor_P.push_back(P3);
//            set_correspond_P(statement_P,cor_P);
        }
        else if(p->child->place == "pthread_mutex_destroy")
            ;
    }
    else if(exist_in(lib_func_type,p->child->place)){
        // get statement_P and paras
        gtree *statement = p;
        while (statement->type != STATEMENT)
            statement = statement->parent;
        string statement_P = statement->matched_P;

        statement->is_processed = true;//have been processed

        vector<gtree *> paras;
        paras = extract_paras(p);
        if(p->child->place == "printf")
            ;
        else if(p->child->place == "malloc"){

            string tag,tmp_size;
            int size;
            string para = paras[0]->place;
            auto pos_start = para.find("sizeof(");
            if(pos_start == string::npos)
                throw "malloc should be the standard form:malloc(sizeof(type)*num)!";
            auto pos_end = para.find(")",pos_start);
            tag = para.substr(pos_start + 7,pos_end - pos_start - 7);

            if(tag == "pthread_t")
                return;
            auto mul_pos = para.find("*");
            if(mul_pos == string::npos)
                size = 1;
            else {
                if (mul_pos < pos_start)
                    tmp_size = para.substr(0, mul_pos);
                else
                    tmp_size = para.substr(mul_pos + 1);
                if(isdigit(tmp_size[0]))
                    size = atoi(tmp_size.c_str());
                else
                    size = malloc_maxsize;
            }

            string malloc_P = gen_P();
            SizeList sl;
            sl.Add_size(size);
            string Type_name = get_real_Type_name(tag, sl, true);
            Add_Place(malloc_P,Type_name,false,sl,"malloc",true,false,NoneRow);


//            for(int i=0;i<v_tables.size();i++){
//                if(v_tables[i]->get_name() == base){
                    v_tables[0]->insert("malloc",malloc_P,tag,tag,sl,false);//global
//                    break;
//                }
//            }

//            string malloc_v_P = gen_P();
//            Add_Place(malloc_v_P,"int",1,false,"malloc_v",)

            string statement_T = get_enter_T(statement_P)[0];
//            create_assignment(this,alloc_store_P,statement_T,"","",)

            //Here arcexp is solid, remain update
            string allocid_str = "allocid";
            string allocid_Exp1 = controlflowArcExp(allocid_str);
            string allocid_Exp2 = controlflowArcExp(allocid_str + "+" + to_string(size));
            Add_Arc(alloc_store_P,statement_T,allocid_Exp1,true,writeArc,normal);
            Add_Arc(statement_T,alloc_store_P,allocid_Exp2,false,writeArc,normal);
            string Exp;
            string init_value;
            if(tag == "char")
                init_value = "\' \'";
            else if (tag == "int" || tag == "short" || tag == "long")
                init_value = "0";
            else if(tag == "float" || tag == "double")
                init_value = "0.0";
            else
                throw "dynamic structure haven't been solved yet!";
            if(size > 1) {
                for (unsigned int i = 0; i < size; i++) {
                    if (i != 0)
                        Exp += "++";
                    Exp += "1`{" + init_value +"," + to_string(i) + "," + allocid_str + "+" + to_string(i) + "}";//construct_normaltoken("0", to_string(i), allocid_str + "+" + to_string(i), "");
                }
            }
            else
                Exp += "1`{" + init_value + "," + "0," + allocid_str + "}";// construct_normaltoken("0", "", allocid_str, "");
            Add_Arc(statement_T, malloc_P, Exp, false, allocwrite,normal);
            gtree *assignment = statement;
            string left_id;
            while(assignment->type != ASSIGNMENT_EXPRESSION)
                assignment = assignment->child;
            if(assignment->child->type == UNARY_EXPRESSION)
                left_id = assignment->child->place;
            else
                throw "malloc should be the standard form:malloc(sizeof(type)*num)!";
            string left_P = find_P_id(left_id,base);
            writeOperation(left_P,statement_T,allocid_str,"",SizeList(),base);
//            create_assignment(statement_T,left_P,NULL,allocid_str);
        }
        else if(p->child->place == "calloc"){
            string tag,tmp_size;
            int size;
            string para1 = paras[0]->place,para2 = paras[1]->place;
            auto pos_start = para2.find("sizeof(");
            if(pos_start == string::npos)
                throw "calloc should be the standard form:calloc(num,sizeof(type))!";
            auto pos_end = para2.find(")",pos_start);
            tag = para2.substr(pos_start + 7,pos_end - pos_start - 7);
            if(tag == "pthread_t")
                return;
            size = atoi(para1.c_str());
            string calloc_P = gen_P();
            SizeList sl;
            sl.Add_size(size);
            string Type_name = get_real_Type_name(tag, sl, true);
            Add_Place(calloc_P,Type_name,false,sl,"calloc",true,false,NoneRow);



//            for(int i=0;i<v_tables.size();i++){
//                if(v_tables[i]->get_name() == base){
                    v_tables[0]->insert("calloc",calloc_P,tag,tag,sl,false);//global
//                    break;
//                }
//            }
//            string malloc_v_P = gen_P();
//            Add_Place(malloc_v_P,"int",1,false,"malloc_v",)

            string statement_T = get_enter_T(statement_P)[0];
//            create_assignment(this,alloc_store_P,statement_T,"","",)

            //Here arcexp is solid, remain update
            string allocid_str = "allocid";
            string allocid_Exp1 = controlflowArcExp(allocid_str);
            string allocid_Exp2 = controlflowArcExp(allocid_str + "+" + to_string(size));
            Add_Arc(alloc_store_P,statement_T,allocid_Exp1,true,writeArc,normal);
            Add_Arc(statement_T,alloc_store_P,allocid_Exp2,false,writeArc,normal);
            string Exp;
            string init_value;
            if(tag == "char")
                init_value = "\' \'";
            else if (tag == "int" || tag == "short" || tag == "long")
                init_value = "0";
            else if(tag == "float" || tag == "double")
                init_value = "0.0";
            else
                throw "dynamic structure haven't been solved yet!";

            if(size > 1) {
                for (unsigned int i = 0; i < size; i++) {
                    if (i != 0)
                        Exp += "++";
                    Exp += "1`{" + init_value + "," + to_string(i) + "," + allocid_str + "+" + to_string(i) + "}";//construct_normaltoken("0", to_string(i), allocid_str + "+" + to_string(i), "");
                }
            }
            else
                Exp += "1`{" + init_value + "," + allocid_str + "}";// construct_normaltoken("0", "", allocid_str, "");
            Add_Arc(statement_T, calloc_P, Exp, false, allocwrite,normal);
            gtree *assignment = statement;
            string left_id;
            while(assignment->type != ASSIGNMENT_EXPRESSION)
                assignment = assignment->child;
            if(assignment->child->type == UNARY_EXPRESSION)
                left_id = assignment->child->place;
            else
                throw "calloc should be the standard form:calloc(num,sizeof(type))!";
            string left_P = find_P_id(left_id,base);
            writeOperation(left_P,statement_T,allocid_str,"",SizeList(),base);
//            create_assignment(statement_T,left_P,NULL,allocid_str);
        }
        else if(p->child->place == "strcpy"){
            string para1 = paras[0]->place,para2 = paras[1]->place;
            if(para2[0] != '"' || para2[para2.size()-1] != '"')
                throw "ERROR!strcpy must match the standard form:strcpy(variable,\"str\")";
            para2 = para2.substr(1,para2.length()-2);

            string id = para1;
            string left_P = find_P_id(id,base);


            string statement_T = get_enter_T(statement_P)[0];
            string tmp_value;
            string tid_flag;

            bool isglobal = find_v_isglobal(id,base);
//            if(isglobal)
//                tid_flag = "";
//            else
//                tid_flag = tid_str;

            bool ispointer = find_v_ispointer(id,base);

//            auto piter = mapPlace.find(left_P);
//            if(piter == mapPlace.end()){
//                cerr << "ERROR!can't find place!"<<endl;
//                exit(-1);
//            }
            auto pp = findP_byid(left_P);
            string real_id = pp->getExp();
            string Exp_read,Exp_write;

            if(ispointer){
                ///!!!
                writestrcpy(statement_T,para2,real_id);
//                Triple triple;
//                triple.first = statement_T;
//                triple.second = para2;
//                triple.third = real_id;
//                strcpypointer.push_back(triple);
//
                string Exp = "1`{"+ real_id + "," + real_id + id_suffix;
                if(isglobal)
                    Exp += "}";
                else
                    Exp += "," + tid_str + "}";
                Add_Arc(statement_T,left_P,Exp,false,writeArc,override);
                Add_Arc(left_P,statement_T,Exp,true,writeArc,override);
            }
            else{
                throw "strcpy error! first parameter must be char *!";
//                for(unsigned int i=0;i<para2.length();i++){
//                    tmp_value = "'" + to_string(para2[i]) + "'";
//                    if(i>0){
//                        Exp_read += "++";
//                        Exp_write += "++";
//                    }
//                    Exp_write += construct_normaltoken(tmp_value,to_string(i),real_id + id_suffix,tid_flag);;
//                    Exp_read += construct_normaltoken(real_id,to_string(i),real_id + id_suffix,tid_flag);
//                }
//                Add_Arc(statement_T,left_P,Exp_write,false,write,false);
//                Add_Arc(left_P,statement_T,Exp_read,true,write,false);
            }
        }
    }
    else
    {
//process all the variable as local variable
//        bool isglobal = find_v_isglobal(base);
        string tid_flag;
//        if(isglobal)
//            tid_flag = "";
//        else
        tid_flag = tid_str;

        string call_func_id = p->child->place, call_func_P_begin, call_func_P_end;

        //construct call structure
        string call_P = gen_P();
        string call_T = gen_T();
        p->matched_P = call_P;
        Add_Place(call_P, controltypename, true,SizeList(), p->child->place + call_suffix,true,false,NoneRow);
        Add_Transition(call_T, "", p->child->place + call_suffix,NoneRow);
        Add_Arc(call_P, call_T, controlflowArcExp(tid_str), true, control,normal);

        //将abort()函数调用加入main begin库所的相关库所
        if(p->child->place == "abort"){
            string main_begin_P = f_table.findbeginplace("main");
            auto cor_P = get_correspond_P(main_begin_P);
            cor_P.push_back(call_P);
            set_correspond_P(main_begin_P,cor_P);
        }

        vector<string> enter;
        enter.push_back(call_T);
        reset_enter_T(call_P, enter);

        auto pp = findP_byid(call_P);
        string unique = pp->getExp();
        mapCall2Func.emplace(unique,p->child->place);

        // set call_P
        gtree *statement = p;
        while (statement->type != STATEMENT)
            statement = statement->parent;
        string statement_P = statement->matched_P;
        vector<string> statement_call_P = get_call_P(statement_P);
        statement_call_P.push_back(call_P);
        set_call_P(statement_P, statement_call_P);

        // vector<string> statement_correspond_P = get_correspond_P(statement_P);
        // statement_correspond_P.push_back(call_P);
        // set_correspond_P(statement_P,statement_correspond_P);
        vector<string> cor_P;
        cor_P.push_back(statement_P);
        set_correspond_P(call_P,cor_P);

//        auto iter_begin = mapFunction.find(call_func_id + begin_suffix);
//        if(iter_begin == mapFunction.end())
//        {
//            cout<<"can't find call_begin_P in handle_call"<<endl;
//            exit(-1);
//        }
        call_func_P_begin = f_table.findbeginplace(call_func_id);


        //passing parameter
        gtree *temp_tree = p->child->next->next;
        if (temp_tree->type == ARGUMENT_EXPRESSION_LIST) {
            vector<gtree*> v;
            gtree *temp_assignment_expression = temp_tree;
            while (temp_assignment_expression->type != ASSIGNMENT_EXPRESSION)
                temp_assignment_expression = temp_assignment_expression->child;

            while (1) {
                string value = temp_assignment_expression->place;
                v.push_back(temp_assignment_expression);
                if (temp_assignment_expression->parent->next->next != NULL
                    && temp_assignment_expression->parent->next->next->type == ASSIGNMENT_EXPRESSION)
                    temp_assignment_expression = temp_assignment_expression->parent->next->next;
                else
                    break;
            }

//            auto iter1 = mapPlace.find(call_func_P_begin);
//            if(iter1 == mapPlace.end())
//            {
//                cout<<"can't find call_func_P_begin"<<endl;
//                exit(-1);
//            }
//            CPlace *begin_place = findP_byid(call_func_P_begin);
            auto paras = f_table.findfuncparas(call_func_id);
            for (unsigned int i = 0; i < paras.size(); i++) {
                auto para = paras[i];

                create_connect(call_T, v[i]->place, base);
                string exp_tmp = translate_exp2arcexp(v[i]);

                writeOperation(para.second,call_T,exp_tmp,"",SizeList(),base);
//                create_assignment(call_T,para.first,NULL,exp_tmp);
            }
        }

        //construct return places
//        auto iter = mapFunction.find(call_func_P_begin);
//        if(iter == mapFunction.end()){
//            cerr << "can't find call_func_P_begin!"<<endl;
//            exit(-1);
//        }
        string ret_tag = f_table.findfuncreturntag(call_func_id);
        bool ret_ispointer = false;
        if(ret_tag[0]=='*') {
            ret_ispointer = true;
            ret_tag = ret_tag.substr(1);
        }
        if(ret_tag!="void") {
            string return_P = gen_P();
            string return_v;

            return_v = p->place;

            // return place is a local place
            string Type_name = get_real_Type_name(ret_tag, SizeList() , false);
            Add_Place(return_P, Type_name, false,SizeList() ,return_v,false,false,NoneRow);
            init_place_MS(return_P,SizeList(),"");
//            auto piter = mapPlace.find(return_P);
//            if(piter == mapPlace.end()){
//                cerr << "ERROR!<can't find place!"<<endl;
//                exit(-1);
//            }
            auto pp = findP_byid(return_P);
            return_v = pp->getExp();

//            for(int i=0;i<v_tables.size();i++){
//                if(v_tables[i]->get_name() == base){
//                    v_tables[i]->insert(p->place,return_P,ret_tag,ret_tag,SizeList(),ret_ispointer);
//                    break;
//                }
//            }

            // return place is a little unique, because it may appear in one statement with two place
            // so we should identify them
            p->place = return_v;
            for(int i=0;i<v_tables.size();i++) {
                if(v_tables[i]->get_name() == base) {
                    v_tables[i]->insert(return_v, return_P, ret_tag, ret_tag, SizeList(), ret_ispointer);
                    break;
                }
            }
            gtree *tmp_pointer = p->parent,*tmp_child;

            while(tmp_pointer->type != EXPRESSION_STATEMENT
                  && tmp_pointer->type != ITERATION_STATEMENT
                  && tmp_pointer->type != SELECTION_STATEMENT
                  && tmp_pointer->type != JUMP_STATEMENT){
                tmp_child = tmp_pointer->child;
                string tmp_str = "";
                while(tmp_child){
                    tmp_str.append(tmp_child->place);
                    tmp_child = tmp_child->next;
                }
                tmp_pointer->place = tmp_str;
                tmp_pointer = tmp_pointer->parent;
            }

            // connect return_P with all return statements
//            vector<Triple> returns = get_returns(call_func_P_begin);
            auto returns = f_table.findfuncreturns(call_func_id);
            for (unsigned int i = 0; i < returns.size(); i++) {
                string Exp1,Exp2,exp_tmp;

                exp_tmp = translate_exp2arcexp(returns[i].second);

                writeOperation(return_P,returns[i].first,exp_tmp,"",SizeList(),base);
//                create_assignment(returns[i].first,return_P,NULL,exp_tmp);

            }


            string last_P;
            vector<string> last_T;
            gtree *last_call = p->parent;
            while (!judge_call_postfix_expression(last_call) && last_call->type != STATEMENT) {

                last_call = last_call->parent;
            }
            if(!last_call->matched_P.empty()) {
                last_P = last_call->matched_P;
                last_T = get_enter_T(last_P);
                for (unsigned int j = 0; j < last_T.size(); j++) {
                    string Exp;

                    //Here cannot create_connection, because create_connection will not connect return_v
//                    create_connect(this, last_T[j], return_v, base);
                    Exp = "1`{" + return_v + "," + return_v + id_suffix + "," + tid_flag + "}";

                    Add_Arc(last_T[j], return_P, Exp, false, readArc,normal);
                    Add_Arc(return_P, last_T[j], Exp, true, readArc,normal);

                }
            }
            else
                throw "ERROR!can't find last_call!";
        }

        //construct enter (not return) arcs
        vector<string> enter_T = get_enter_T(statement_P);
        string called_identifier = p->child->place;

        Add_Arc(call_T, call_func_P_begin, controlflowArcExp(tid_str), false, call_enter,normal);
    }
}

void CPN::handle_call_withoutdependence(gtree *p) {
    gtree *com = p;
    while (com->type != COMPOUND_STATEMENT)
        com = com->parent;
    string base = com->place;
    if(exist_in(pthread_func_type,p->child->place))
    {
        // get statement_P and paras
        gtree *statement = p;
        while (statement->type != STATEMENT)
            statement = statement->parent;
        string statement_P = statement->matched_P;

        vector<gtree *> paras;
        paras = extract_paras(p);
        if(p->child->place == "pthread_create") {
            gtree* thread_v = paras[0], *thread_func = paras[2],*para_pass = paras[3];
//            if (thread_v[0] == '&')
//                thread_v = thread_v.substr(1);
//            if(para_pass[0] == '&')
//                para_pass = para_pass.substr(1);

            string func_begin_P = f_table.findbeginplace(thread_func->place);

            string func_end_P = f_table.findendplace(thread_func->place);

            //add thread map
            string newtid = translate_exp2tid(thread_v);

            newtid = addQuote(newtid);
            mapPthread.insert(make_pair(newtid + begin_suffix, func_begin_P));
            mapPthread.insert(make_pair(newtid + end_suffix, func_end_P));


            //add thread arc
            string newtidstr = newtid ;
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0], func_begin_P, controlflowArcExp(newtidstr), false, control,normal);
            if(enter_T.size() > 1)
                Add_Arc(enter_T[1], func_begin_P, controlflowArcExp(newtidstr), false, control,normal);

            //add copy

//
//            string statement_T = get_enter_T(statement_P)[0];
            ///!!!
            auto paras = f_table.findfuncparas(thread_func->place);
            if(paras.size()!=1)
                throw "pthread_create's target function just have one parameter!";

            Add_pthreadCopys(enter_T[0],newtidstr,paras[0].second, translate_exp2arcexp(para_pass));
            if(enter_T.size() > 1){
                Add_pthreadCopys(enter_T[1],newtidstr,paras[0].second, translate_exp2arcexp(para_pass));
            }

            if(join_create.find(newtid) == join_create.end())
                join_create.insert(make_pair(newtid,statement_P));

        }
        else if(p->child->place == "pthread_join")
        {
            gtree *thread_v = paras[0];
//            if (thread_v[0] == '&')
//                thread_v = thread_v.substr(1);

//            string jointid = translate_exp2tid(thread_v) + id_suffix;// + id_suffix because the parameter is different with pthread_create
            // + id_suffix because the parameter is different with pthread_create
//            string jointid =  add_idPostfix(translate_exp2tid(thread_v));
            string jointidstr = add_idPostfix(translate_exp2tid(thread_v));

            jointidstr = addQuote(jointidstr);
            auto iter = mapPthread.find(jointidstr + end_suffix);
            if(iter == mapPthread.end())
                throw "can't find pthread_end in mapPthread";
            string func_end_P = iter->second;

            //add map
            auto biter = mapPthread.find(jointidstr + begin_suffix);
            if(biter == mapPthread.end())
                throw "can't find pthread_begin when pthread_join";
//            mapJoin.insert(make_pair(biter->second,statement_P));

            //add thread arc
            vector<string> enter_T = get_enter_T(statement_P);


            Add_Arc(func_end_P,enter_T[0], controlflowArcExp(jointidstr), true, control,normal);
            if(enter_T.size() > 1)
                Add_Arc(func_end_P,enter_T[1], controlflowArcExp(jointidstr), true, control,normal);


            //cor_P
//            string thread_idexp;
//            auto pos = thread_v.find("[");
//            if(pos != string::npos)
//                thread_idexp = thread_v.substr(0,pos);
//            else
//                thread_idexp = thread_v;
            auto mapiter = join_create.find(jointidstr);
            if(mapiter == join_create.end())
                throw "ERROR!pthread_join doesn't have correspond create!";
            vector<string> cor_P = get_correspond_P(mapiter->second);
            cor_P.push_back(statement_P);
            set_correspond_P(mapiter->second,cor_P);
        }
        else if(p->child->place == "pthread_exit")
        {
            gtree *find_func = p;

            while (find_func->type != FUNCTION_DEFINITION)
                find_func = find_func->parent;
            string identifier = find_func->place;

            string last_func_end = f_table.findendplace(identifier);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0], last_func_end, controlflowArcExp(tid_str), false,executed,normal);
        }
        else if(p->child->place == "pthread_mutex_init")
        {
            string mutex_v = paras[0]->place;
            if(mutex_v[0]=='&')
                mutex_v = mutex_v.substr(1);
            string Exp;
            auto pos = mutex_v.find("[");
            if(pos != string::npos){
                Exp = "1`" + mutex_v.substr(pos+1,mutex_v.length()-pos-2);
                mutex_v = mutex_v.substr(0,pos);
            }
            else{
                Exp = "1`0";
            }
            gtree *com = p;
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string mutex_P = find_P_id(mutex_v,base);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0],mutex_P,Exp,false,control,normal);
        }
        else if(p->child->place == "pthread_mutex_lock")
        {
            string mutex_v = paras[0]->place;
            if(mutex_v[0]=='&')
                mutex_v = mutex_v.substr(1);
            string Exp;
            auto pos = mutex_v.find("[");
            if(pos != string::npos){
                Exp = "1`" + mutex_v.substr(pos+1,mutex_v.length()-pos-2);
                mutex_v = mutex_v.substr(0,pos);
            }
            else{
                Exp = "1`0";
            }
            gtree *com = p;
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string mutex_P = find_P_id(mutex_v,base);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(mutex_P,enter_T[0],Exp,true,control,normal);

            gtree *tr = statement->parent->next;

        }
        else if(p->child->place == "pthread_mutex_unlock")
        {
            string mutex_v = paras[0]->place;
            if(mutex_v[0]=='&')
                mutex_v = mutex_v.substr(1);
            string Exp;
            auto pos = mutex_v.find("[");
            if(pos != string::npos){
                Exp = "1`" + mutex_v.substr(pos+1,mutex_v.length()-pos-2);
                mutex_v = mutex_v.substr(0,pos);
            }
            else{
                Exp = "1`0";
            }
            gtree *com = p;
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string mutex_P = find_P_id(mutex_v,base);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0],mutex_P,Exp,false,control,normal);
        }
        else if(p->child->place == "pthread_cond_signal")
        {
            string cond_v = paras[0]->place;
            if (cond_v[0] == '&')
                cond_v = cond_v.substr(1);
            gtree *com = p;
            string Exp;
            auto pos = cond_v.find("[");
            if(pos != string::npos){
                Exp = "1`" + cond_v.substr(pos+1,cond_v.length()-pos-2);
                cond_v = cond_v.substr(0,pos);
            }
            else{
                Exp = "1`0";
            }
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string cond_P = find_P_id(cond_v,base);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0],cond_P,Exp,false,control,normal);
        }
        else if(p->child->place == "pthread_cond_wait")
        {
            string cond_v = paras[0]->place,mutex_v = paras[1]->place;
            if (cond_v[0] == '&')
                cond_v = cond_v.substr(1);
            if(mutex_v[0] == '&')
                mutex_v = mutex_v.substr(1);
            string Exp_mutex,Exp_cond;
            auto pos = cond_v.find("[");
            if(pos != string::npos){
                Exp_cond = "1`" +cond_v.substr(pos+1,cond_v.length()-pos-2);
                cond_v = cond_v.substr(0,pos);
            }
            else{
                Exp_cond = "1`0";
            }
            pos = mutex_v.find("[");
            if(pos != string::npos){
                Exp_mutex = "1`"+mutex_v.substr(pos+1,mutex_v.length()-pos-2);
                mutex_v = mutex_v.substr(0,pos);
            }
            else{
                Exp_mutex = "1`0";
            }
            gtree *com = p;
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string cond_P = find_P_id(cond_v,base);
            string mutex_P = find_P_id(mutex_v,base);
            vector<string> enter_T = get_enter_T(statement_P);


            //construct P、T
            string P1,P2,P3,T1,T2;
            P1 = gen_P();
            P2 = gen_P();
            P3 = gen_P();
            T1 = gen_T();
            T2 = gen_T();

            Add_Place(P1,controltypename,true,SizeList(),"signalP1",true,false,NoneRow);
            Add_Place(P2,controltypename,true,SizeList(),"signalP2",true,false,NoneRow);
            Add_Place(P3,controltypename,true,SizeList(),"signalP3",true,false,NoneRow);
            Add_Transition(T1,"","signalT1",NoneRow);
            Add_Transition(T2,"","signalT2",NoneRow);

            //construct arcs
            Add_Arc(enter_T[0],P1,controlflowArcExp(tid_str),false,control,normal);
            Add_Arc(enter_T[0],P3,controlflowArcExp(tid_str),false,executed,normal);
            Add_Arc(enter_T[0],mutex_P,Exp_mutex,false,control,normal);
            Add_Arc(cond_P,T1,Exp_cond,true,control,normal);
            Add_Arc(P1,T1,controlflowArcExp(tid_str),true,control,normal);
            Add_Arc(T1,P2,controlflowArcExp(tid_str),false,control,normal);
            Add_Arc(mutex_P,T2,Exp_mutex,true,control,normal);
            Add_Arc(P2,T2,controlflowArcExp(tid_str),true,control,normal);
            Add_Arc(P3,T2,controlflowArcExp(tid_str),true,control,normal);

            //set exit
            vector<string> exit_T;
            exit_T.push_back(T2);
            reset_exit_T(statement_P,exit_T);

//            vector<string> cor_P;
//            cor_P.push_back(P1);
//            cor_P.push_back(P2);
//            cor_P.push_back(P3);
//            set_correspond_P(statement_P,cor_P);
        }
        else if(p->child->place == "pthread_mutex_destroy")
            ;
    }
    else if(exist_in(lib_func_type,p->child->place)){
        // get statement_P and paras
        gtree *statement = p;
        while (statement->type != STATEMENT)
            statement = statement->parent;
        string statement_P = statement->matched_P;

        statement->is_processed = true;//have been processed

        vector<gtree *> paras;
        paras = extract_paras(p);
        if(p->child->place == "printf")
            ;
        else if(p->child->place == "malloc"){

            string tag,tmp_size;
            int size;
            string para = paras[0]->place;
            auto pos_start = para.find("sizeof(");
            if(pos_start == string::npos)
                throw "malloc should be the standard form:malloc(sizeof(type)*num)!";
            auto pos_end = para.find(")",pos_start);
            tag = para.substr(pos_start + 7,pos_end - pos_start - 7);

            if(tag == "pthread_t")
                return;
            auto mul_pos = para.find("*");
            if(mul_pos == string::npos)
                size = 1;
            else {
                if (mul_pos < pos_start)
                    tmp_size = para.substr(0, mul_pos);
                else
                    tmp_size = para.substr(mul_pos + 1);
                if(isdigit(tmp_size[0]))
                    size = atoi(tmp_size.c_str());
                else
                    size = malloc_maxsize;
            }

            string malloc_P = gen_P();
            SizeList sl;
            sl.Add_size(size);
            string Type_name = get_real_Type_name(tag, sl, true);
            Add_Place(malloc_P,Type_name,false,sl,"malloc",true,false,NoneRow);


//            for(int i=0;i<v_tables.size();i++){
//                if(v_tables[i]->get_name() == base){
                    v_tables[0]->insert("malloc",malloc_P,tag,tag,sl,false);//global
//                    break;
//                }
//            }

//            string malloc_v_P = gen_P();
//            Add_Place(malloc_v_P,"int",1,false,"malloc_v",)

            string statement_T = get_enter_T(statement_P)[0];
//            create_assignment(this,alloc_store_P,statement_T,"","",)

            //Here arcexp is solid, remain update
            string allocid_str = "allocid";
            string allocid_Exp1 = controlflowArcExp(allocid_str);
            string allocid_Exp2 = controlflowArcExp(allocid_str + "+" + to_string(size));
            Add_Arc(alloc_store_P,statement_T,allocid_Exp1,true,writeArc,normal);
            Add_Arc(statement_T,alloc_store_P,allocid_Exp2,false,writeArc,normal);
            string Exp;
            string init_value;
            if(tag == "char")
                init_value = "\' \'";
            else if (tag == "int" || tag == "short" || tag == "long")
                init_value = "0";
            else if(tag == "float" || tag == "double")
                init_value = "0.0";
            else
                throw "dynamic structure haven't been solved yet!";
            if(size > 1) {
                for (unsigned int i = 0; i < size; i++) {
                    if (i != 0)
                        Exp += "++";
                    Exp += "1`{" + init_value +"," + to_string(i) + "," + allocid_str + "+" + to_string(i) + "}";//construct_normaltoken("0", to_string(i), allocid_str + "+" + to_string(i), "");
                }
            }
            else
                Exp += "1`{" + init_value + "," + "0," + allocid_str + "}";// construct_normaltoken("0", "", allocid_str, "");
            Add_Arc(statement_T, malloc_P, Exp, false, allocwrite,normal);
            gtree *assignment = statement;
            string left_id;
            while(assignment->type != ASSIGNMENT_EXPRESSION)
                assignment = assignment->child;
            if(assignment->child->type == UNARY_EXPRESSION)
                left_id = assignment->child->place;
            else
                throw "malloc should be the standard form:malloc(sizeof(type)*num)!";
            string left_P = find_P_id(left_id,base);
            writeOperation(left_P,statement_T,allocid_str,"",SizeList(),base);
//            create_assignment(statement_T,left_P,NULL,allocid_str);
        }
        else if(p->child->place == "calloc"){
            string tag,tmp_size;
            int size;
            string para1 = paras[0]->place,para2 = paras[1]->place;
            auto pos_start = para2.find("sizeof(");
            if(pos_start == string::npos)
                throw "calloc should be the standard form:calloc(num,sizeof(type))!";
            auto pos_end = para2.find(")",pos_start);
            tag = para2.substr(pos_start + 7,pos_end - pos_start - 7);
            if(tag == "pthread_t")
                return;
            size = atoi(para1.c_str());
            string calloc_P = gen_P();
            SizeList sl;
            sl.Add_size(size);
            string Type_name = get_real_Type_name(tag, sl, true);
            Add_Place(calloc_P,Type_name,false,sl,"calloc",true,false,NoneRow);



//            for(int i=0;i<v_tables.size();i++){
//                if(v_tables[i]->get_name() == base){
                    v_tables[0]->insert("calloc",calloc_P,tag,tag,sl,false);//global
//                    break;
//                }
//            }
//            string malloc_v_P = gen_P();
//            Add_Place(malloc_v_P,"int",1,false,"malloc_v",)

            string statement_T = get_enter_T(statement_P)[0];
//            create_assignment(this,alloc_store_P,statement_T,"","",)

            //Here arcexp is solid, remain update
            string allocid_str = "allocid";
            string allocid_Exp1 = controlflowArcExp(allocid_str);
            string allocid_Exp2 = controlflowArcExp(allocid_str + "+" + to_string(size));
            Add_Arc(alloc_store_P,statement_T,allocid_Exp1,true,writeArc,normal);
            Add_Arc(statement_T,alloc_store_P,allocid_Exp2,false,writeArc,normal);
            string Exp;
            string init_value;
            if(tag == "char")
                init_value = "\' \'";
            else if (tag == "int" || tag == "short" || tag == "long")
                init_value = "0";
            else if(tag == "float" || tag == "double")
                init_value = "0.0";
            else
                throw "dynamic structure haven't been solved yet!";

            if(size > 1) {
                for (unsigned int i = 0; i < size; i++) {
                    if (i != 0)
                        Exp += "++";
                    Exp += "1`{" + init_value + "," + to_string(i) + "," + allocid_str + "+" + to_string(i) + "}";//construct_normaltoken("0", to_string(i), allocid_str + "+" + to_string(i), "");
                }
            }
            else
                Exp += "1`{" + init_value + "," + allocid_str + "}";// construct_normaltoken("0", "", allocid_str, "");
            Add_Arc(statement_T, calloc_P, Exp, false, allocwrite,normal);
            gtree *assignment = statement;
            string left_id;
            while(assignment->type != ASSIGNMENT_EXPRESSION)
                assignment = assignment->child;
            if(assignment->child->type == UNARY_EXPRESSION)
                left_id = assignment->child->place;
            else
                throw "calloc should be the standard form:calloc(num,sizeof(type))!";
            string left_P = find_P_id(left_id,base);
            writeOperation(left_P,statement_T,allocid_str,"",SizeList(),base);
//            create_assignment(statement_T,left_P,NULL,allocid_str);
        }
        else if(p->child->place == "strcpy"){
            string para1 = paras[0]->place,para2 = paras[1]->place;
            if(para2[0] != '"' || para2[para2.size()-1] != '"')
                throw "ERROR!strcpy must match the standard form:strcpy(variable,\"str\")";
            para2 = para2.substr(1,para2.length()-2);

            string id = para1;
            string left_P = find_P_id(id,base);


            string statement_T = get_enter_T(statement_P)[0];
            string tmp_value;
            string tid_flag;

            bool isglobal = find_v_isglobal(id,base);
//            if(isglobal)
//                tid_flag = "";
//            else
//                tid_flag = tid_str;

            bool ispointer = find_v_ispointer(id,base);

//            auto piter = mapPlace.find(left_P);
//            if(piter == mapPlace.end()){
//                cerr << "ERROR!can't find place!"<<endl;
//                exit(-1);
//            }
            auto pp = findP_byid(left_P);
            string real_id = pp->getExp();
            string Exp_read,Exp_write;

            if(ispointer){
                ///!!!
                writestrcpy(statement_T,para2,real_id);
//                Triple triple;
//                triple.first = statement_T;
//                triple.second = para2;
//                triple.third = real_id;
//                strcpypointer.push_back(triple);
//
                string Exp = "1`{"+ real_id + "," + real_id + id_suffix;
                if(isglobal)
                    Exp += "}";
                else
                    Exp += "," + tid_str + "}";
                Add_Arc(statement_T,left_P,Exp,false,writeArc,override);
                Add_Arc(left_P,statement_T,Exp,true,writeArc,override);
            }
            else{
                throw "strcpy error! first parameter must be char *!";
//                for(unsigned int i=0;i<para2.length();i++){
//                    tmp_value = "'" + to_string(para2[i]) + "'";
//                    if(i>0){
//                        Exp_read += "++";
//                        Exp_write += "++";
//                    }
//                    Exp_write += construct_normaltoken(tmp_value,to_string(i),real_id + id_suffix,tid_flag);;
//                    Exp_read += construct_normaltoken(real_id,to_string(i),real_id + id_suffix,tid_flag);
//                }
//                Add_Arc(statement_T,left_P,Exp_write,false,write,false);
//                Add_Arc(left_P,statement_T,Exp_read,true,write,false);
            }
        }
    }
    else
    {
//process all the variable as local variable
//        bool isglobal = find_v_isglobal(base);
        string tid_flag;
//        if(isglobal)
//            tid_flag = "";
//        else
        tid_flag = tid_str;

        string call_func_id = p->child->place, call_func_P_begin, call_func_P_end;

        //construct call structure
        string call_P = gen_P();
        string call_T = gen_T();
        p->matched_P = call_P;
        Add_Place(call_P, controltypename, true,SizeList(), p->child->place + call_suffix,true,false,NoneRow);
        Add_Transition(call_T, "", p->child->place + call_suffix,NoneRow);
        Add_Arc(call_P, call_T, controlflowArcExp(tid_str), true, control,normal);

        vector<string> enter;
        enter.push_back(call_T);
        reset_enter_T(call_P, enter);

        auto pp = findP_byid(call_P);
        string unique = pp->getExp();
        mapCall2Func.emplace(unique,p->child->place);

        // set call_P
        gtree *statement = p;
        while (statement->type != STATEMENT)
            statement = statement->parent;
        string statement_P = statement->matched_P;
        vector<string> statement_call_P = get_call_P(statement_P);
        statement_call_P.push_back(call_P);
        set_call_P(statement_P, statement_call_P);

        // vector<string> statement_correspond_P = get_correspond_P(statement_P);
        // statement_correspond_P.push_back(call_P);
        // set_correspond_P(statement_P,statement_correspond_P);
        vector<string> cor_P;
        cor_P.push_back(statement_P);
        set_correspond_P(call_P,cor_P);

//        auto iter_begin = mapFunction.find(call_func_id + begin_suffix);
//        if(iter_begin == mapFunction.end())
//        {
//            cout<<"can't find call_begin_P in handle_call"<<endl;
//            exit(-1);
//        }
        call_func_P_begin = f_table.findbeginplace(call_func_id);


        //passing parameter
        gtree *temp_tree = p->child->next->next;
        if (temp_tree->type == ARGUMENT_EXPRESSION_LIST) {
            vector<gtree*> v;
            gtree *temp_assignment_expression = temp_tree;
            while (temp_assignment_expression->type != ASSIGNMENT_EXPRESSION)
                temp_assignment_expression = temp_assignment_expression->child;

            while (1) {
                string value = temp_assignment_expression->place;
                v.push_back(temp_assignment_expression);
                if (temp_assignment_expression->parent->next->next != NULL
                    && temp_assignment_expression->parent->next->next->type == ASSIGNMENT_EXPRESSION)
                    temp_assignment_expression = temp_assignment_expression->parent->next->next;
                else
                    break;
            }

//            auto iter1 = mapPlace.find(call_func_P_begin);
//            if(iter1 == mapPlace.end())
//            {
//                cout<<"can't find call_func_P_begin"<<endl;
//                exit(-1);
//            }
//            CPlace *begin_place = findP_byid(call_func_P_begin);
            auto paras = f_table.findfuncparas(call_func_id);
            for (unsigned int i = 0; i < paras.size(); i++) {
                auto para = paras[i];

                create_connect(call_T, v[i]->place, base);
                string exp_tmp = translate_exp2arcexp(v[i]);

                writeOperation(para.second,call_T,exp_tmp,"",SizeList(),base);
//                create_assignment(call_T,para.first,NULL,exp_tmp);
            }
        }

        //construct return places
//        auto iter = mapFunction.find(call_func_P_begin);
//        if(iter == mapFunction.end()){
//            cerr << "can't find call_func_P_begin!"<<endl;
//            exit(-1);
//        }
        string ret_tag = f_table.findfuncreturntag(call_func_id);
        bool ret_ispointer = false;
        if(ret_tag[0]=='*') {
            ret_ispointer = true;
            ret_tag = ret_tag.substr(1);
        }
        if(ret_tag!="void") {
            string return_P = gen_P();
            string return_v;

            return_v = p->place;

            // return place is a local place
            string Type_name = get_real_Type_name(ret_tag, SizeList() , false);
            Add_Place(return_P, Type_name, false,SizeList() ,return_v,false,false,NoneRow);
            init_place_MS(return_P,SizeList(),"");
//            auto piter = mapPlace.find(return_P);
//            if(piter == mapPlace.end()){
//                cerr << "ERROR!<can't find place!"<<endl;
//                exit(-1);
//            }
            auto pp = findP_byid(return_P);
            return_v = pp->getExp();

//            for(int i=0;i<v_tables.size();i++){
//                if(v_tables[i]->get_name() == base){
//                    v_tables[i]->insert(p->place,return_P,ret_tag,ret_tag,SizeList(),ret_ispointer);
//                    break;
//                }
//            }

            // return place is a little unique, because it may appear in one statement with two place
            // so we should identify them
            p->place = return_v;
            for(int i=0;i<v_tables.size();i++) {
                if(v_tables[i]->get_name() == base) {
                    v_tables[i]->insert(return_v, return_P, ret_tag, ret_tag, SizeList(), ret_ispointer);
                    break;
                }
            }
            gtree *tmp_pointer = p->parent,*tmp_child;

            while(tmp_pointer->type != EXPRESSION_STATEMENT
                  && tmp_pointer->type != ITERATION_STATEMENT
                  && tmp_pointer->type != SELECTION_STATEMENT
                  && tmp_pointer->type != JUMP_STATEMENT){
                tmp_child = tmp_pointer->child;
                string tmp_str = "";
                while(tmp_child){
                    tmp_str.append(tmp_child->place);
                    tmp_child = tmp_child->next;
                }
                tmp_pointer->place = tmp_str;
                tmp_pointer = tmp_pointer->parent;
            }

            // connect return_P with all return statements
//            vector<Triple> returns = get_returns(call_func_P_begin);
            auto returns = f_table.findfuncreturns(call_func_id);
            for (unsigned int i = 0; i < returns.size(); i++) {
                string Exp1,Exp2,exp_tmp;

                exp_tmp = translate_exp2arcexp(returns[i].second);

                writeOperation(return_P,returns[i].first,exp_tmp,"",SizeList(),base);
//                create_assignment(returns[i].first,return_P,NULL,exp_tmp);

            }


            string last_P;
            vector<string> last_T;
            gtree *last_call = p->parent;
            while (!judge_call_postfix_expression(last_call) && last_call->type != STATEMENT) {

                last_call = last_call->parent;
            }
            if(!last_call->matched_P.empty()) {
                last_P = last_call->matched_P;
                last_T = get_enter_T(last_P);
                for (unsigned int j = 0; j < last_T.size(); j++) {
                    string Exp;

                    //Here cannot create_connection, because create_connection will not connect return_v
//                    create_connect(this, last_T[j], return_v, base);
                    Exp = "1`{" + return_v + "," + return_v + id_suffix + "," + tid_flag + "}";

                    Add_Arc(last_T[j], return_P, Exp, false, readArc,normal);
                    Add_Arc(return_P, last_T[j], Exp, true, readArc,normal);

                }
            }
            else{
                cout<< "ERROR!can't find last_call!"<<endl;
                exit(-1);
            }
        }

        //construct enter (not return) arcs
        vector<string> enter_T = get_enter_T(statement_P);
        string called_identifier = p->child->place;

        Add_Arc(call_T, call_func_P_begin, controlflowArcExp(tid_str), false, call_enter,normal);
    }
}

//organize_call decide the order of many calls
//also, it will connect arcs between calls structure and statement structure
void CPN::organize_call(gtree *p){
    string statement_P = p->matched_P;
    vector<string> statement_T = get_enter_T(statement_P);
    vector<string> call_P = get_call_P(statement_P);
    if(call_P.size()==0)
        return;
    unsigned int i=0;
    string last_P,next_P,last_T,next_T;//call just has one control_T
    string last_end_P;
    for(;i<call_P.size()-1;i++)
    {
        last_P = call_P[i];
        next_P = call_P[i+1];
        last_T = get_enter_T(last_P)[0];
        next_T = get_enter_T(next_P)[0];
        Add_Arc(last_T,next_P,controlflowArcExp(tid_str),false,call_connect,normal);

//        auto iter = cpn->mapPlace.find(last_P);
        auto pp_begin = findP_byid(last_P);
        string call_exp = pp_begin->getExp();//cpn->place[iter->second].expression;
        string func = mapCall2Func.find(call_exp)->second;

//        string_replace(call_exp,call_suffix,end_suffix);
//        string end_exp = call_exp;
//        auto iter_end = cpn->mapFunction.find(end_exp);
        last_end_P = f_table.findendplace(func);
        Add_Arc(last_end_P,next_T,controlflowArcExp(tid_str),true,control,normal);
    }
    last_P = call_P[i];
    next_P = statement_P;
    last_T = get_enter_T(last_P)[0];
    Add_Arc(last_T,next_P,controlflowArcExp(tid_str),false,call_connect,normal);
    for(unsigned int j=0;j<statement_T.size();j++)
    {
        next_T = statement_T[j];
//        auto iter = cpn->mapPlace.find(last_P);
        auto pp_begin = findP_byid(last_P);
        string call_exp = pp_begin->getExp();//cpn->place[iter->second].expression;
        string func = mapCall2Func.find(call_exp)->second;
//        string_replace(call_exp,call_suffix,end_suffix);
//        string end_exp = call_exp;
//        auto iter_end = cpn->mapFunction.find(end_exp);
        last_end_P = f_table.findendplace(func);
        Add_Arc(last_end_P,next_T,controlflowArcExp(tid_str),true,control,normal);
    }
}

void CPN::organize_call_withoutdependence(gtree *p){
    string statement_P = p->matched_P;
    vector<string> statement_T = get_enter_T(statement_P);
    vector<string> call_P = get_call_P(statement_P);
    if(call_P.size()==0)
        return;
    unsigned int i=0;
    string last_P,next_P,last_T,next_T;//call just has one control_T
    string last_end_P;

    for(;i<call_P.size()-1;i++)
    {
        last_P = call_P[i];
        next_P = call_P[i+1];
        last_T = get_enter_T(last_P)[0];
        next_T = get_enter_T(next_P)[0];
        Add_Arc(last_T,next_P,controlflowArcExp(tid_str),false,call_connect,normal);

//        auto iter = cpn->mapPlace.find(last_P);
        auto pp_begin = findP_byid(last_P);
        string call_exp = pp_begin->getExp();//cpn->place[iter->second].expression;
        string func = mapCall2Func.find(call_exp)->second;

//        string_replace(call_exp,call_suffix,end_suffix);
//        string end_exp = call_exp;
//        auto iter_end = cpn->mapFunction.find(end_exp);
        last_end_P = f_table.findendplace(func);
        Add_Arc(last_end_P,next_T,controlflowArcExp(tid_str),true,control,normal);
        // auto last_end_T = get_enter_T(last_end_P);//end_P对应的end_T只有唯一的一个
        // Add_Arc(last_end_T[0],next_P,controlflowArcExp(tid_str),false,executed,normal);
    }

    last_P = call_P[i];
    // next_P = statement_P;
    last_T = get_enter_T(last_P)[0];
    Add_Arc(last_T,statement_P,controlflowArcExp(tid_str),false,call_connect,normal);
    for(unsigned int j=0;j<statement_T.size();j++)
    {
        next_T = statement_T[j];
//        auto iter = cpn->mapPlace.find(last_P);
        auto pp_begin = findP_byid(last_P);
        string call_exp = pp_begin->getExp();//cpn->place[iter->second].expression;
        string func = mapCall2Func.find(call_exp)->second;
//        string_replace(call_exp,call_suffix,end_suffix);
//        string end_exp = call_exp;
//        auto iter_end = cpn->mapFunction.find(end_exp);
        last_end_P = f_table.findendplace(func);
        auto last_end_T = get_enter_T(last_end_P);//end_P对应的end_T只有唯一的一个
        Add_Arc(last_end_P,next_T,controlflowArcExp(tid_str),true,control,normal);
        // Add_Arc(last_end_T[0],statement_P,controlflowArcExp(tid_str),false,executed,normal);
    }
}

void CPN::handle_INC_DEC_OP(int nodetype, bool isfollow, gtree *p) {
    gtree *com = p;
    while(com->type != COMPOUND_STATEMENT)
        com = com->parent;
    string base = com->place;
    gtree *ptr;
    if(isfollow)
        ptr = p->child;
    else
        ptr = p->child->next;
    gtree *statement = p;
    while(statement->type != STATEMENT)
        statement = statement->parent;

    string statement_P = statement->matched_P;
    string statement_T = get_enter_T(statement_P)[0];

    string content = translate_exp2arcexp(ptr);
    if(nodetype == INC_OP)
        content += "+1";
    else
        content += "-1";
    writeOperation(statement_T,ptr,content);
}

void CPN::handle_label(gtree *p) {
    gtree *statement = p->parent;
    gtree *tmp = p->child;
    string label;
    if(!tmp){
        cerr<<"error in handle_label!"<<endl;
        exit(-1);
    }
    while(tmp && tmp->place != ":"){
        label += tmp->place;
        tmp = tmp->next;
    }
    if(!tmp){
        cerr<<"error in handle_label!"<<endl;
        exit(-1);
    }
//    Add_Label(label,statement->matched_P);
//    mapLabel.insert(make_pair(label,place));

    gtree *child_statement = p->child->next->next;

    //connect with child_statement
    string control_T = get_enter_T(statement->matched_P)[0];
    Add_Arc(control_T,child_statement->matched_P,controlflowArcExp(tid_str),false,control,normal);

    //Add execution place
    Add_executed_P(get_exit_T(statement->matched_P),get_enter_T(child_statement->matched_P));

    //reset exit
    reset_exit_T(statement->matched_P,get_exit_T(child_statement->matched_P));
}

void Variable::getvcolor(Bucket &bkt) {
    value->getcolor(bkt);
}

void Variable::setvcolor(const Bucket &bkt) {
    value->setcolor(bkt);
}

void Variable::Init_Variable(string _id, type _tid) {
    id = _id;
    tid = _tid;
    switch(tid){
        case Integer:
            value = (token)(new IntegerSortValue);
            break;
        case Real:
            value = (token)(new RealSortValue);
            break;
        case String:
            value = (token)(new StringSortValue);
            break;
        case dot:
            value = (token)(new DotSortValue);
            break;
        default:
            throw "ERROR in Init_Variable!";
    }

}

void CPN_Place::Init_Place(string _id, type tid,SORTID sid,string exp,bool iscontrolP, bool isglobal, bool isexecuted,CPN *cpn, Row_Type _row) {
    if(exp != executed_P_name)
        expression = uniqueexp(exp,cpn);
    else
        expression = exp;
    control_P = iscontrolP;
    is_global = isglobal;
    is_executed = isexecuted;
    initMarking.settype(tid,sid);
    id = _id;
    row = _row;
}

void CPN::InitDistance(vector<string> &criteria) {
    for(auto item :criteria){
        auto pp = findP_byid(item);
        pp->setDistance(0);
    }
}

void CPN::Add_Place_MS(string id, const MultiSet &ms) {
    auto pp = findP_byid(id);
    pp->AddMultiSet(ms);
}

void CPN::Sub_Place_MS(string id, const MultiSet &ms) {

    auto pp = findP_byid(id);
    pp->SubMultiSet(ms);
}

string CPN::construct_initnormaltokenstr(string _data, string _index, string _id, string _tid) {
    string result;

    if (_index == "" && _id == "" && _tid == "") {
        result = "1`" + _data;
        return result;
    }

    result.append("1`{");
    result.append(_data);
    if (!_index.empty()) {
        result.append(",");
        result.append(_index);
    }
    if (!_id.empty()) {
        result.append(",");
        result.append(_id);
    }
    if (!_tid.empty()) {
        result.append(",");
        result.append(_tid);
    }
    result.append("}");

    return result;
}

void CPN::Add_place_token(CPlace *pp,int index,string _tid){
    auto ps = sorttable.find_productsort(pp->getsid());
    token tk = (token)(new ProductSortValue(pp->getsid()));//fpro.generateSortValue(pp->getsid());
    Bucket bkt;
    tk->getcolor(bkt);
    auto sortname = ps.get_sortname();
    auto sortid = ps.get_sortid();
    if(bkt.pro.size() != sortname.size())
        throw "ERROR! bucket and sortname don't match!";
    for(int i=0;i<sortname.size();i++){
        Bucket tmp_bkt;
        if(sortname[i] == index_sortname){
            tmp_bkt.tid = Integer;
            tmp_bkt.integer = index;
            bkt.pro[i]->setcolor(tmp_bkt);
        }
        else if(sortname[i] == id_sortname) {
            tmp_bkt.tid = Integer;
            int id_offset = getSize(pp->gettid(),pp->getsid());
            tmp_bkt.integer = gen_id(id_offset);
            bkt.pro[i]->setcolor(tmp_bkt);
        }
        else if(sortname[i] == tid_sortname) {
            tmp_bkt.tid = String;
            tmp_bkt.str = _tid;
            bkt.pro[i]->setcolor(tmp_bkt);
        }
    }
    tk->setcolor(bkt);
    MultiSet ms(pp->gettid(),pp->getsid());
    ms.generateFromToken(tk);
    pp->AddMultiSet(ms);
}

//generate initial token for place,
//if init_str == "", integer will be 0, real will be 0.0,string will be ''
void CPN::init_place_MS(string P_id, const SizeList &sl, string init_str) {
    auto pp = findP_byid(P_id);
    if(pp->gettid() != productsort)
        throw "ERROR! init_place_MS should have been productsort!";
    auto isglobal = pp->getisglobal();
    string _tid = isglobal ? "" : init_tid;
    if(init_str != "") {
        if (!sl.empty())
            throw "we don't support array initialization for now!";
        int id_offset = getSize(pp->gettid(),pp->getsid());
        string ms_str = construct_initnormaltokenstr(init_str, "", to_string(gen_id(id_offset)), _tid);
        condition_tree ct;
        ct.construct(ms_str);
        MultiSet ms(pp->gettid(),pp->getsid());
        CT2MS(ct, ms, pp->gettid(),pp->getsid());
        pp->AddMultiSet(ms);
    }
    else{
        if(!sl.empty()) {
            auto totalsize = sl.totalsize();
            for (int i = 0; i < totalsize; i++) {
                Add_place_token(pp,i,_tid);
            }
        }
        else{
            Add_place_token(pp,0,_tid);
        }
    }
}

int CPN::getSize(type tid,SORTID sid){
    if(tid != productsort)
        return 1;
    auto ps = sorttable.find_productsort(sid);
    auto memberinfos = ps.get_memberinfo();
    if(memberinfos.empty())
        return 1;
    int size = 0;
    for(int i=0;i < memberinfos.size();i++){
        //now we assume that all structures' members are not structure
        size += memberinfos[i].msl.totalsize();
    }
    return size;
}

void CPN::Add_Place(string id, string Type_name, bool controlP, SizeList sl,string exp, bool isglobal, bool isexecuted, Row_Type row) {
    CPlace *pp = &place[placecount++];

    if(Type_name == alloc_store_type){
        pp->Init_Place(id,Integer,0,exp,controlP,isglobal,isexecuted,this,NoneRow);
        mapPlace.insert(make_pair(id,placecount-1));
        ctlPlacecount++;
        return;
    }

    if(controlP) {
        if(isexecuted)
            exePlacecount++;
        else
            ctlPlacecount++;
        pp->Init_Place(id,String,0,exp,controlP,isglobal,isexecuted,this,row);
        if(exp == "abort begin"){
//            otherLocks.push_back(id);
        }
    }
    else {
        varPlacecount++;
        string base_type = removeTypeNamePostfix(Type_name);
        if(base_type == "pthread_t") {
            placecount--;
            gen_P_num--;
            return;
        }
        else if(exist_in(pthread_type,base_type)){
            if(!isglobal)
                throw "we just support global pthread_mutex_t and pthread_cond_t!";
            pp->Init_Place(id,Integer,0,exp,true,isglobal,isexecuted,this,row);
        }
        else {
//            Type_name = get_real_Type_name(Type_name, sl, isglobal);
//        if(!sl.empty())
//            Type_name += arr_suffix;
//        else
//            Type_name += var_suffix;
//        if(isglobal)
//            Type_name += global_suffix;

//        auto siter = sorttable.mapSort.find(Type_name);
//        if(siter == sorttable.mapSort.end())
//            throw "can't find Type_name:" + Type_name;
            auto msi = sorttable.find_typename(Type_name);
            pp->Init_Place(id, msi.tid, msi.sid, exp, controlP, isglobal, isexecuted, this, row);
        }
    }

    mapPlace.insert(make_pair(id,placecount-1));
}

string Multiindexs2index(vector<string> indexs,SizeList sl){
    string indexstr;
    for(int i=0;i<indexs.size();i++){
//        indexstr += indexs[i] + "*" + to_string(sl.getarrsize(i));
        if(i!=0)
            indexstr += "+";
        indexstr += indexs[i] + "*" + to_string(sl.getarrsize(sl.getdim()-1-i));
    }
    return indexstr;
}



string CPN::translate_exp2arcexp(gtree *exp_tree) {
    ///!!!
    if(!exp_tree)
        return "";

    if(exp_tree->child == NULL) {
        if(exp_tree->place != "" && (exp_tree->place[0] == '_' || isalpha(exp_tree->place[0]))) {
            //simplely judge variable
            gtree *com = exp_tree->parent;
            while (com->type != COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;
            auto sl = find_v_size(exp_tree->place, base);
            auto pp = findP_byid(find_P_id(exp_tree->place,base));
            if (sl.empty())
                return pp->getExp();
            else
                return pp->getExp() + "_0_id";
        }
        else
            return exp_tree->place;
    }
    switch(exp_tree->type){
        case PRIMARY_EXPRESSION:
        case IDENTIFIER:
        case CONSTANT:
        case STRING_LITERAL: {
            if(exp_tree->place != "" && (exp_tree->place[0] == '_' || isalpha(exp_tree->place[0]))) {
                //simplely judge variable
                gtree *com = exp_tree->parent;
                while (com->type != COMPOUND_STATEMENT)
                    com = com->parent;
                string base = com->place;
                auto sl = find_v_size(exp_tree->place, base);
                auto pp = findP_byid(find_P_id(exp_tree->place,base));
                if (sl.empty())
                    return pp->getExp();
                else
                    return pp->getExp() + "_0_id";
            }
            else
                return exp_tree->place;
        }
        case EXPRESSION:
            if (exp_tree->child->next == NULL)
                return translate_exp2arcexp(exp_tree->child);
            else
                return translate_exp2arcexp(exp_tree->child->next->next);
        case ASSIGNMENT_EXPRESSION:
            if (exp_tree->child->next == NULL)
                return translate_exp2arcexp(exp_tree->child);
            else
                return translate_exp2arcexp(exp_tree->child->next->next);
        case CONDITIONAL_EXPRESSION:
            if (!(exp_tree->child->next))
                return translate_exp2arcexp(exp_tree->child);
            else {
                string cond = translate_exp2arcexp(exp_tree->child);
                string res;
                res += CaseFlag + cond;
                res += ":";
                res += "0";
                res += "=>";
                res += translate_exp2arcexp(exp_tree->child->next->next->next->next);
                res += ":";
                res += translate_exp2arcexp(exp_tree->child->next->next);
                res += ";";
                return res;
            }
        case LOGICAL_OR_EXPRESSION:
        case LOGICAL_AND_EXPRESSION:
        case INCLUSIVE_OR_EXPRESSION:
        case EXCLUSIVE_OR_EXPRESSION:
        case AND_EXPRESSION:
        case EQUALITY_EXPRESSION:
        case RELATIONAL_EXPRESSION:
        case SHIFT_EXPRESSION:
        case ADDITIVE_EXPRESSION:
        case MULTIPLICATIVE_EXPRESSION:
            if (!(exp_tree->child->next))
                return translate_exp2arcexp(exp_tree->child);
            else
                return translate_exp2arcexp(exp_tree->child) + exp_tree->child->next->place + translate_exp2arcexp(exp_tree->child->next->next);
        case CAST_EXPRESSION:
            if(exp_tree->child->next)
                return translate_exp2arcexp(exp_tree->child->next->next->next);
            return translate_exp2arcexp(exp_tree->child);
        case UNARY_EXPRESSION:
            if(exp_tree->child->type == POSTFIX_EXPRESSION)
                return translate_exp2arcexp(exp_tree->child);
            else if(exp_tree->child->type == UNARY_OPERATOR){
                gtree *unary_op = exp_tree->child;
                string res;
                if(unary_op->child->place == "*"){
                    res += CaseFlag + translate_exp2arcexp(unary_op->next);
                    for(int i=0;i<v_tables.size();i++){
                        auto v_map = v_tables[i]->get_v_map();
                        auto viter = v_map.begin();
                        while(viter != v_map.end()){
                            auto pp = findP_byid(viter->second.place);
                            string base_exp = pp->getExp();
                            auto sl = viter->second.sizelist;
                            if(!sl.empty()) {
                                auto sizes = sl.getsizes();
                                auto dim = sl.getdim();
                                vector<short> indexs(dim, 0);
                                while (indexs[0] < sizes[0]) {
                                    res += ":";
                                    string indexstr = arrindexmark(to_string(Multiindex2index(indexs,sl)));
                                    res += base_exp + indexstr + id_suffix;
                                    res += "=>";
                                    res += base_exp + indexstr;

                                    indexs[dim - 1]++;
                                    for (int j = dim - 1; j > 0; j--) {
                                        if (indexs[j] >= sizes[j]) {
                                            indexs[j] = 0;
                                            indexs[j - 1]++;
                                        } else
                                            break;
                                    }
                                }
                            }
                            else{
                                res += ":";
                                res += base_exp + id_suffix;
                                res += "=>";
                                res += base_exp;
                            }
                            viter++;
                        }
                    }
                    res += ":default;";
                    return res;
                }
                else if(unary_op->child->place == "&"){
//                    return translate_exp2arcexp(unary_op->next) + id_suffix;
                    return add_idPostfix(translate_exp2arcexp(unary_op->next));
                }
                else if(unary_op->child->type == INC_OP)
                    return translate_exp2arcexp(unary_op->next) + "+1";
                else if(unary_op->child->type == DEC_OP)
                    return translate_exp2arcexp(unary_op->next) + "-1";
                else if(unary_op->type == UNARY_OPERATOR)
                    return unary_op->child->place + translate_exp2arcexp(unary_op->next);
                else
                    throw "ERROR in translate_exp2arcexp unary_expression!";
            }
            else if(exp_tree->child->type == INC_OP || exp_tree->child->type == DEC_OP){
                //++和--前缀已经经过处理，place转化成exp+1和exp-1
                return exp_tree->place;
            }
            else{
                throw "暂不支持SIZEOF";
            }
        case POSTFIX_EXPRESSION:
            if(exp_tree->next && exp_tree->next->place == "("){
                return exp_tree->parent->place;
            }
            if(exp_tree->child->type == PRIMARY_EXPRESSION) {
                if(exp_tree->child->child->place == "(")
                    return "(" + translate_exp2arcexp(exp_tree->child->child->next) + ")";
                string base;
                gtree *com = exp_tree->parent;
                while(com->type != COMPOUND_STATEMENT)
                    com = com->parent;
                base = com->place;
                string id = find_P_id(exp_tree->child->place, base);
                auto pp = findP_byid(id);
//                auto sl = find_v_size(exp_tree->child->place,base);
//                if (sl.empty())
//                    return pp->getExp();
//                else
//                    return pp->getExp() + "_0_id";
                return pp->getExp();
            }
            else{
                string poststr = translate_exp2arcexp(exp_tree->child);
                string res;
                if(exp_tree->child->next && exp_tree->child->next->type == PTR_OP){
                    string membername = exp_tree->child->next->next->place;
                    res += CaseFlag + translate_exp2arcexp(exp_tree->child);
                    for(int i=0;i<v_tables.size();i++){
                        auto v_map = v_tables[i]->get_v_map();
                        auto viter = v_map.begin();
                        while(viter != v_map.end()){
                            auto pp = findP_byid(viter->second.place);
                            string base_exp = pp->getExp();
                            auto sl = viter->second.sizelist;
                            if(!sl.empty()) {
                                auto sizes = sl.getsizes();
                                auto dim = sl.getdim();
                                vector<short> indexs(dim, 0);
                                while (indexs[0] < sizes[0]) {
                                    res += ":";
                                    string indexstr = arrindexmark(to_string(Multiindex2index(indexs,sl)));
                                    res += base_exp + indexstr + id_suffix;
                                    res += "=>";
                                    res += base_exp + indexstr + "." + membername;

                                    indexs[dim - 1]++;
                                    for (int j = dim - 1; j > 0; j--) {
                                        if (indexs[j] >= sizes[j]) {
                                            indexs[j] = 0;
                                            indexs[j - 1]++;
                                        } else
                                            break;
                                    }
                                }
                            }
                            else{
                                res += ":";
                                res += base_exp + id_suffix;
                                res += "=>";
                                res += base_exp + "." + membername;
                            }
                            viter++;
                        }
                    }
                    res += ":default;";
                    return res;
                }
                else if(exp_tree->child->next && exp_tree->child->next->place == "."){
                    res += poststr + "." + exp_tree->child->next->next->place;
                }
                else if(exp_tree->child->next && exp_tree->child->next->place == "["){
                    gtree *tmp = exp_tree;
                    vector<string> indexs;
                    while(tmp->child->next && tmp->child->next->place == "[") {
                        indexs.emplace_back(translate_exp2arcexp(tmp->child->next->next));
                        tmp = tmp->child;
                    }
//                    string index = translate_exp2arcexp(exp_tree->child->next->next);
//                    bool ispointer = find_v_ispointer(exp_tree)
//                    auto sl = get_postsize(exp_tree);

                    //simplely process
                    gtree *com = exp_tree;
                    while(com->type != COMPOUND_STATEMENT)
                        com = com->parent;
                    string base = com->place;
                    string id = tmp->place;

                    string indexstr;
                    bool constantindex = judge_constantindex(indexs);
                    if(judge_isidentifier(id) && !find_v_ispointer(id,base)){
                        auto sl = find_v_size(id,base);
                        indexstr = Multiindexs2index(indexs,sl);
                        if(constantindex){
                            res += poststr + arrindexmark(indexstr);
                        }
                        else {

                            res += CaseFlag + indexstr;
                            for (int i = 0; i < sl.totalsize(); i++) {
                                res += ":";
                                res += to_string(i);
                                res += "=>";
                                res += poststr + arrindexmark(to_string(i));
                            }
                            res += ":default;";
                        }
                    }
                    else{
                        res += CaseFlag + poststr + "+" + indexs[0];
                        for(int i=0;i<v_tables.size();i++){
                            auto v_map = v_tables[i]->get_v_map();
                            auto viter = v_map.begin();
                            while(viter != v_map.end()){
                                auto pp = findP_byid(viter->second.place);
                                string base_exp = pp->getExp();
                                auto sl = viter->second.sizelist;
                                if(!sl.empty()) {
                                    auto sizes = sl.getsizes();
                                    auto dim = sl.getdim();
                                    vector<short> indexs(dim, 0);
                                    while (indexs[0] < sizes[0]) {
                                        res += ":";
                                        string indexstr = arrindexmark(to_string(Multiindex2index(indexs,sl)));
                                        res += base_exp + indexstr + id_suffix;
                                        res += "=>";
                                        res += base_exp + indexstr;

                                        indexs[dim - 1]++;
                                        for (int j = dim - 1; j > 0; j--) {
                                            if (indexs[j] >= sizes[j]) {
                                                indexs[j] = 0;
                                                indexs[j - 1]++;
                                            } else
                                                break;
                                        }
                                    }
                                }
                                else{
                                    res += ":";
                                    res += base_exp + id_suffix;
                                    res += "=>";
                                    res += base_exp;
                                }
                                viter++;
                            }
                        }
                        res += ":default;";
                        return res;
                    }

                }
                else if(exp_tree->child->next == NULL) {

                    string origin_exp = exp_tree->place;
                    if(origin_exp[0] == '_' || isalpha(origin_exp[0])) {
                        string base;
                        gtree *com = exp_tree->parent;
                        while (com->type != COMPOUND_STATEMENT)
                            com = com->parent;
                        base = com->place;
                        auto sl = find_v_size(origin_exp,base);
                        auto pp = findP_byid(find_P_id(exp_tree->place,base));
                        if(sl.empty())
                            res += pp->getExp();
                        else
                            res += pp->getExp() + "_0_id";
                    }
                    else
                        res+= origin_exp;
                }
                else if(exp_tree->child->next->place == "(")
                    res += exp_tree->parent->place;
                else if(exp_tree->child->next->type == INC_OP || exp_tree->child->next->type == DEC_OP) {
                    string base;
                    gtree *com = exp_tree->parent;
                    while(com->type!=COMPOUND_STATEMENT)
                        com = com->parent;
                    base = com->place;
//                    auto pp = findP_byid(find_P_id(exp_tree->child->place,base));
                    res += translate_exp2arcexp(exp_tree->child);
                }
                else
                    throw "ERROR in translate_exp2arcexp postfix_expression!";
                return res;
            }
    }

    throw "translate_exp2arcexp ERROR!";
}

///转换成tid时会给表达式加上引号，作为字符串
string CPN::translate_exp2tid(gtree *exp_tree) {
    ///!!!
    if(!exp_tree)
        return "";
    if(exp_tree->child == NULL)
        return exp_tree->place;
    switch(exp_tree->type){
        case PRIMARY_EXPRESSION:
        case IDENTIFIER:
        case CONSTANT:
        case STRING_LITERAL:
            return exp_tree->place;
        case EXPRESSION:
            if (exp_tree->child->next == NULL)
                return translate_exp2tid(exp_tree->child);
            else
                return translate_exp2tid(exp_tree->child->next->next);
        case ASSIGNMENT_EXPRESSION:
            if (exp_tree->child->next == NULL)
                return translate_exp2tid(exp_tree->child);
            else
                return translate_exp2tid(exp_tree->child->next->next);
        case CONDITIONAL_EXPRESSION:
            if (!(exp_tree->child->next))
                return translate_exp2tid(exp_tree->child);
            else {
                string cond = translate_exp2tid(exp_tree->child);
                string res;
                res += CaseFlag + cond;
                res += ":";
                res += "0";
                res += "=>";
                res += translate_exp2tid(exp_tree->child->next->next->next->next);
                res += ":";
                res += translate_exp2tid(exp_tree->child->next->next);
                res += ";";
                return res;
            }
        case LOGICAL_OR_EXPRESSION:
        case LOGICAL_AND_EXPRESSION:
        case INCLUSIVE_OR_EXPRESSION:
        case EXCLUSIVE_OR_EXPRESSION:
        case AND_EXPRESSION:
        case EQUALITY_EXPRESSION:
        case RELATIONAL_EXPRESSION:
        case SHIFT_EXPRESSION:
        case ADDITIVE_EXPRESSION:
        case MULTIPLICATIVE_EXPRESSION:
            if (!(exp_tree->child->next))
                return translate_exp2tid(exp_tree->child);
            else
                return translate_exp2tid(exp_tree->child) + exp_tree->child->next->place + translate_exp2tid(exp_tree->child->next->next);
        case CAST_EXPRESSION:
            if(exp_tree->child->next)
                return translate_exp2tid(exp_tree->child->next->next->next);
            return translate_exp2tid(exp_tree->child);
        case UNARY_EXPRESSION:
            if(exp_tree->child->type == POSTFIX_EXPRESSION)
                return translate_exp2tid(exp_tree->child);
            else if(exp_tree->child->type == UNARY_OPERATOR){
                gtree *unary_op = exp_tree->child;
                string res;
                if(unary_op->child->place == "*"){
                    res += CaseFlag + translate_exp2tid(unary_op->next);
                    for(int i=0;i<v_tables.size();i++){
                        auto v_map = v_tables[i]->get_v_map();
                        auto viter = v_map.begin();
                        while(viter != v_map.end()){
                            auto pp = findP_byid(viter->second.place);
                            string base_exp = pp->getExp();
                            auto sl = viter->second.sizelist;
                            if(!sl.empty()) {
                                auto sizes = sl.getsizes();
                                auto dim = sl.getdim();
                                vector<short> indexs(dim, 0);
                                while (indexs[0] < sizes[0]) {
                                    res += ":";
                                    string indexstr = arrindexmark(to_string(Multiindex2index(indexs,sl)));
                                    res += base_exp + indexstr + id_suffix;
                                    res += "=>";
                                    res += base_exp + indexstr;

                                    indexs[dim - 1]++;
                                    for (int j = dim - 1; j > 0; j--) {
                                        if (indexs[j] >= sizes[j]) {
                                            indexs[j] = 0;
                                            indexs[j - 1]++;
                                        } else
                                            break;
                                    }
                                }
                            }
                            else{
                                res += ":";
                                res += base_exp + id_suffix;
                                res += "=>";
                                res += base_exp;
                            }
                            viter++;
                        }
                    }
                    res += ":default;";
                    return res;
                }
                else if(unary_op->child->place == "&"){
//                    return translate_exp2tid(unary_op->next) + id_suffix;
                    return add_idPostfix(translate_exp2tid(unary_op->next));
                }
                else if(unary_op->child->type == INC_OP)
                    return translate_exp2tid(unary_op->next) + "+1";
                else if(unary_op->child->type == DEC_OP)
                    return translate_exp2tid(unary_op->next) + "-1";
                else if(unary_op->type == UNARY_OPERATOR)
                    return unary_op->child->place + translate_exp2tid(unary_op->next);
                else
                    throw "ERROR in translate_exp2tid unary_expression!";
            }
        case POSTFIX_EXPRESSION:
            if(exp_tree->next && exp_tree->next->place == "("){
                return exp_tree->parent->place;
            }
            if(exp_tree->child->type == PRIMARY_EXPRESSION) {

//                string id = find_P_id(exp_tree->child->place, base);
//                auto pp = findP_byid(id);
                return exp_tree->child->place;
            }
            else{
                string poststr = translate_exp2tid(exp_tree->child);
                string res;
                if(exp_tree->child->next && exp_tree->child->next->type == PTR_OP){
                    string membername = exp_tree->child->next->next->place;
                    res += CaseFlag + translate_exp2tid(exp_tree->child);
                    for(int i=0;i<v_tables.size();i++){
                        auto v_map = v_tables[i]->get_v_map();
                        auto viter = v_map.begin();
                        while(viter != v_map.end()){
                            auto pp = findP_byid(viter->second.place);
                            string base_exp = pp->getExp();
                            auto sl = viter->second.sizelist;
                            if(!sl.empty()) {
                                auto sizes = sl.getsizes();
                                auto dim = sl.getdim();
                                vector<short> indexs(dim, 0);
                                while (indexs[0] < sizes[0]) {
                                    res += ":";
                                    string indexstr = arrindexmark(to_string(Multiindex2index(indexs,sl)));
                                    res += base_exp + indexstr + id_suffix;
                                    res += "=>";
                                    res += base_exp + indexstr + "." + membername;

                                    indexs[dim - 1]++;
                                    for (int j = dim - 1; j > 0; j--) {
                                        if (indexs[j] >= sizes[j]) {
                                            indexs[j] = 0;
                                            indexs[j - 1]++;
                                        } else
                                            break;
                                    }
                                }
                            }
                            else{
                                res += ":";
                                res += base_exp + id_suffix;
                                res += "=>";
                                res += base_exp + "." + membername;
                            }
                            viter++;
                        }
                    }
                    res += ":default;";
                    return res;
                }
                else if(exp_tree->child->next && exp_tree->child->next->place == "."){
                    res += poststr + "." + exp_tree->child->next->next->place;
                }
                else if(exp_tree->child->next && exp_tree->child->next->place == "["){
                    gtree *tmp = exp_tree;
                    vector<string> indexs;
                    while(tmp->child->next && tmp->child->next->place == "[") {
                        indexs.emplace_back(translate_exp2tid(tmp->child->next->next));
                        tmp = tmp->child;
                    }
//                    string index = translate_exp2tid(exp_tree->child->next->next);
//                    bool ispointer = find_v_ispointer(exp_tree)
//                    auto sl = get_postsize(exp_tree);

                    //simplely process

                    string id = tmp->child->place;
//                    auto sl = find_v_size(id,base);

                    string indexstr;
                    indexstr = indexs[0];
//                    for(int i=0;i<indexs.size();i++){
//                        indexstr += indexs[i] + "*" + to_string(sl.getarrsize(i));
//                    }


//                    bool ispointer = find_v_ispointer(id,base);
//                    if(sl.getdim() != indexs.size() && !ispointer)
//                        throw "dimension doesn't match!";
//                    if(sl.empty() && !ispointer)
//                        throw "ERROR! sizelist should not be empty!";
//
//                    else {
//                        int size = sl.getsizes()[0];
                        res += CaseFlag + indexstr;
                        for (int i = 0; i < thread_max; i++) {
                            res += ":";
                            res += to_string(i);
                            res += "=>";
                            res += poststr + arrindexmark(to_string(i));
                        }
                        res += ":default;";
//                    }
                }
                else if(exp_tree->child->next == NULL) {
                    string origin_exp = exp_tree->place;
//                    if(origin_exp[0] == '_' || isalpha(origin_exp[0])) {
//
//                        auto pp = findP_byid(find_P_id(exp_tree->place,base));
//                        res += pp->getExp();
//                    }
//                    else
                        res+= origin_exp;
                }
                else if(exp_tree->child->next->place == "(")
                    res += exp_tree->parent->place;
                else if(exp_tree->child->next->type == INC_OP || exp_tree->child->next->type == DEC_OP) {

//                    auto pp = findP_byid(find_P_id(exp_tree->child->place,base));
                    res += exp_tree->child->place;
                }
                else
                    throw "ERROR in translate_exp2tid postfix_expression!";
                return res;
            }
    }

    throw "translate_exp2tid ERROR!";
}

void CPN::Add_Transition(string id, string guard, string exp, Row_Type row) {
    CTransition *tt = &transition[transitioncount++];
    tt->Init_Transition(id,guard,row);
    mapTransition.insert(make_pair(id,transitioncount-1));

    if(exp == "abort begin"){
        tt->isabort = true;
    }
}

//Add_Arc
//there exist 4 forms
//normal means arc must be unique, else throw an exception
//beoverride means if exist then return
//override means if exist then override
//append means if exist then append
void CPN::Add_Arc(string source, string target, string exp, bool sourceP, Arc_Type arcType, AddArcForm form) {

    if(form == append) {
        CArc *aa;
        aa = findArc_bysrctgt(source,target);
        if(aa == NULL) {
            cout<< "appendArc should have exist!"<<endl;
            exit(-1);
//            aa = &arc[arccount++];
//            aa->InitArc(source, target, sourceP, exp, arcType);
//
//            if (exp != "") {
//                string _P = aa->getrelated_P();
//                auto pp = findP_byid(_P);
//                auto ps = sorttable.find_productsort(pp->getsid());
//                Add_Variable(aa->getroot(), pp->gettid(), pp->getsid(), 0, ps.get_sortnum() - 1);
//            }
        }
        else{
            string newexp = aa->getexp();
            newexp += exp;

            aa->InitArc(source,target,sourceP,newexp,arcType);

            string _P = aa->getrelated_P();
            auto pp = findP_byid(_P);
            auto ps = sorttable.find_productsort(pp->getsid());
            Add_Variable(aa->getroot(), pp->gettid(), pp->getsid(), 0, ps.get_sortnum() - 1);
        }
    }
    else if(form == beoverrided){
        CArc *aa;
        aa = findArc_bysrctgt(source,target);
        if(aa == NULL){
            aa = &arc[arccount++];
            aa->InitArc(source, target, sourceP, exp, arcType);

            if (exp != "") {
                string _P = aa->getrelated_P();
                auto pp = findP_byid(_P);
                auto ps = sorttable.find_productsort(pp->getsid());
                Add_Variable(aa->getroot(), pp->gettid(), pp->getsid(), 0, ps.get_sortnum() - 1);
            }
        }
        else
            ;
    }
    else if(form == override){
        CArc *aa;
        aa = findArc_bysrctgt(source,target);
        if(aa == NULL){
            aa = &arc[arccount++];
            aa->InitArc(source, target, sourceP, exp, arcType);

            if (exp != "") {
                string _P = aa->getrelated_P();
                auto pp = findP_byid(_P);
                auto ps = sorttable.find_productsort(pp->getsid());
                Add_Variable(aa->getroot(), pp->gettid(), pp->getsid(), 0, ps.get_sortnum() - 1);
            }
        }
        else{
            string newexp = exp;
            if(aa->getArctype() == readArc && arcType == writeArc) {
                arcType = readwrite;
            }
            aa->InitArc(source,target,sourceP,newexp,arcType);

            string _P = aa->getrelated_P();
            auto pp = findP_byid(_P);
            auto ps = sorttable.find_productsort(pp->getsid());
            Add_Variable(aa->getroot(), pp->gettid(), pp->getsid(), 0, ps.get_sortnum() - 1);
        }
    }
    else if(form == normal){
        CArc *aa;
        aa = findArc_bysrctgt(source,target);
        if(aa == NULL){
            aa = &arc[arccount++];
            aa->InitArc(source, target, sourceP, exp, arcType);

            if (exp != "") {
                string _P = aa->getrelated_P();
                auto pp = findP_byid(_P);
                auto ps = sorttable.find_productsort(pp->getsid());
                Add_Variable(aa->getroot(), pp->gettid(), pp->getsid(), 0, ps.get_sortnum() - 1);
            }
        }
        else{
            cout<< "normal Add_arc should be unique!"<<endl;
            exit(-1);
        }
    }
}

void CPN::Add_Variable(string id,type tid) {
    if(tid == productsort)
        throw "Variable can't be productsort!";

    //There will not exist a same variable that define in different types
    //for example : int a; {... double a;}
    //the second 'a' will be unified to '_a' as uniqueexp done
    //so variables will never belong to different type
    auto viter = mapVariable.find(id);
    if(viter != mapVariable.end())
        return;

    Variable *v = &vartable[varcount++];
    v->Init_Variable(id,tid);
    mapVariable.emplace(id,varcount-1);

}

void CPN::Add_Variable(const condition_tree_node *tree, type tid, SORTID sid, unsigned short down, unsigned short up) {
    if (!tree)
        return;
    if (tree->node_type == Tuple) {
        Add_Variable(tree->left, tid, sid, down, down);
        Add_Variable(tree->right, tid, sid, down + 1, up);
        if (tree->condition)
            Add_Variable(tree->condition, Integer, 0, 0, 0);
    } else {
        Add_Variable(tree->left, tid, sid, down, up);
        Add_Variable(tree->right, tid, sid, down, up);
        if (tree->condition)
            Add_Variable(tree->condition, Integer, 0, 0, 0);
    }

    if (tree->node_type == variable) {
        if (down != up)
            throw "ERROR!in Add_Variable,variable's up and down don't consistent!";
        type sub_tid;
        SORTID sub_sid;
        if(tid == productsort) {
            auto ps = sorttable.find_productsort(sid);
            auto sortid = ps.get_sortid();
            sub_tid = sortid[down].tid;
            if (sub_tid == productsort)
                throw "ERROR! variable can't be productsort!";
            Add_Variable(tree->node_name, sub_tid);
        }
        else
            Add_Variable(tree->node_name, tid);
    }
}

void CPN::Add_Arc_ifnotexist(string source, string target, string exp, bool sourceP, Arc_Type arcType) {
    CArc *aa;
    aa = findArc_bysrctgt(source,target);
    if(aa == NULL) {
        aa = &arc[arccount++];
        aa->InitArc(source, target, sourceP, exp, arcType);

        if (exp != "") {
            string _P = aa->getrelated_P();
            auto pp = findP_byid(_P);
            auto ps = sorttable.find_productsort(pp->getsid());
            Add_Variable(aa->getroot(), pp->gettid(), pp->getsid(), 0, ps.get_sortnum() - 1);
        }
    }
    else
        ;
}

void splitExpression(string &s, vector<string>& v)
{
    string tmp;
    bool single_quotation_flag = false,double_quotation_flag = false;
    for (unsigned int i = 0; i < s.length(); i++) {

//  Here we just extract all those variable occured in string:s. no matter where it is.
//  -> . must be process
        if(i+1<s.length() && s.substr(i,2) == "->" || s[i] == '.'){
            int j;
            if(s[i] == '.')
                j = i + 1;
            else
                j = i + 2;
            for(;i<s.length();j++){
                if(!isdigit(s[j]) && !isalpha(s[j]) && s[j] != '_') {
//                    offset = j - i;
                    break;
                }
            }
            i = j - 1;
            continue;
        }
        if(s[i]=='\'')
            single_quotation_flag = ~single_quotation_flag;
        else if(s[i]=='"')
            double_quotation_flag = ~double_quotation_flag;
        if (!isdigit(s[i]) && !isalpha(s[i]) && s[i] != '_' && !single_quotation_flag && !double_quotation_flag) {
//            if (!isdigit(s[i + 1]) && !isalpha(s[i + 1]) && s[i + 1] != '_')
//                i++;
            if (!exist_in(v, tmp) && !tmp.empty())
                v.push_back(tmp);
            tmp.clear();
        } else
            tmp = tmp + s[i];
    }

    v.push_back(tmp);
}
void CPN::create_connect(string control_T, string expression, string base) {
///!!!
    string P2;
    vector<string> v;
    string V;
    bool sourceP;
    bool haspointer=false;
    int size;
    splitExpression(expression, v);
//    set<string> exist_V;
    for (unsigned int i = 0; i < v.size(); i++) {
        if (v[i][0] == '_' || (v[i][0] >= 'a' && v[i][0] <= 'z') || (v[i][0] >= 'A' && v[i][0] <= 'Z')) {
            string id = v[i];
            if(id.length()>return_suffix.length() &&
               id.substr(id.length()-return_suffix.length()) == return_suffix)
                continue;
            if(find_v_ispointer(id,base)) {
                haspointer = true;
            }

            readOperation(id, base, control_T, false, false, beoverrided);
        }
    }

    if(haspointer)
        readPointer(control_T,"");
}

//void CPN::create_assignment(string control_T, string P_id, gtree *location, string expression) {
/////!!!
//
//}

void CPN::print_CPN(string filename) {
    ofstream out;
    out.open(filename + ".txt", ios::out);
    //out << "Place:" << endl;
    //out << "-----------------------------------" << endl;

    int P_width, T_width,T_height,font_size=30;
    P_width=T_width=T_height=1;

    string fillcolor = "chartreuse";
    string fillcolor1 = "lightblue";


    for (int i = 0; i < placecount; i++) {
        if (place[i].getiscontrolP() == false)
            out << "subgraph cluster_" << place[i].getid() << "{"<<"fontsize = "<<to_string(font_size)<< ";label=\"" <<
                place[i].getExp() << "\";color=\"white\"" << place[i].getid() <<
                "[shape=circle"<<",fontsize = "<<to_string(font_size)<<",width="<<to_string(P_width)<<",style=\"filled\",color=\"black\",fillcolor=\"" << fillcolor << "\"]}" << endl;
        else {
//            out << place[i].id << "[shape=circle," << "label=\"" << place[i].expression << "\"]" << endl;
            out << "subgraph cluster_" << place[i].getid() << "{"<<"fontsize = "<<to_string(font_size)<< ";label=\"" <<
                place[i].getExp() << "\";color=\"white\"" << place[i].getid() <<
                "[shape=circle"<<",fontsize = "<<to_string(font_size)<<",width="<<to_string(P_width)<<",style=\"filled\",color=\"black\",fillcolor=\"" << fillcolor1 << "\"]}" << endl;
        }
    }
    //out << "-----------------------------------" << endl;
    //out << "Transition:" << endl;
    //out << "-----------------------------------" << endl;
    for (int i = 0; i < transitioncount; i++) {
        out << transition[i].getid() << "[shape=box"<<",fontsize = "<<to_string(font_size)<<",width="<<to_string(T_width)<<",height="<<to_string(T_height)<<"]" << endl;
    }
    //out << "-----------------------------------" << endl;
    //out << "Arc:" << endl;
    //out << "-----------------------------------" << endl;

    for (int i = 0; i < arccount; i++) {
        if(arc[i].getdeleted())
            continue;
        string tmp_exp = arc[i].getexp();
        int pos = 0;
        pos = tmp_exp.find('\"');
        while(pos != string::npos){
            tmp_exp = tmp_exp.substr(0,pos) + "\\" + tmp_exp.substr(pos);
            pos = tmp_exp.find('\"',pos + 2);
        }
        if (arc[i].getArctype() == executed)
            out << arc[i].getsrc() << "->" << arc[i].gettgt() << "[color=\"red\",label=\""<< tmp_exp<<"\"]" << endl;
        else if (arc[i].getArctype() == writeArc)
            ;//out << arc[i].getsrc() << "->" << arc[i].gettgt() << "[color=\"blue\",label=\""<< tmp_exp<<"\"]" << endl;
        else if (arc[i].getArctype() == readArc)
            out << arc[i].getsrc() << "->" << arc[i].gettgt() << "[color=\"blue\",label=\""<< tmp_exp<<"\"]" << endl;
        else if (arc[i].getArctype() == remain)
            ;//out << arc[i].getsrc() << "->" << arc[i].gettgt() << "[color=\"blue\",label=\""<< tmp_exp<<"\"]" << endl;
        else
            out << arc[i].getsrc() << "->" << arc[i].gettgt() << "[label=\""<< tmp_exp<<"\"]" << endl;
    }
    out.close();

    readGraph(filename + ".txt", filename + ".dot");
    makeGraph(filename + ".dot", filename + ".png");
}

//generate static token
//if content == "" it means generate read token,and index,member,memberindex will of none use
//else content will be write into the dimension indicate by member and memberindex
string CPN::generateToken(string v_name,string base,short index,string member,short memberindex,string content){
//    auto memberinfos = ps.get_memberinfo();

    if(memberindex != -1 && member == "")
        throw "memberindex and member don't match!";
    auto ps = sorttable.find_productsort(sorttable.find_typename(find_v_tag(v_name,base)).sid);
    auto memberinfos = ps.get_memberinfo();
    bool isglobal = find_v_isglobal(v_name,base);
    auto sl = find_v_size(v_name,base);
    auto pp = findP_byid(find_P_id(v_name,base));
    string base_exp = pp->getExp();
    string _tid = tid_str;//Here the tid is always tid_str


    vector<string> data_field;
    int content_index = -1;
    if(!memberinfos.empty()) {

//        string member_prefix = pp->getExp() + ".";
        for (int i = 0; i < memberinfos.size(); i++) {
            auto msl = memberinfos[i].msl;
            auto m_name = memberinfos[i].membername;
            if(msl.empty()){
                if(member == m_name && content != "") {
                    content_index = data_field.size();
                    data_field.emplace_back(content);
                }
                else
                    data_field.emplace_back(m_name);
            }
            else{
                auto dim = msl.getdim();
                auto sizes = msl.getsizes();
//                vector<short> indexs(dim,0);
                short indexs = 0;

                while(indexs < msl.totalsize()){
                    if(member == m_name && indexs == memberindex && content != ""){
                        content_index = data_field.size();
                        data_field.emplace_back(content);
                    }
                    else
                        data_field.emplace_back(m_name + arrindexmark(to_string(indexs)));
                    indexs++;
                    //loop around all the indexs
//                    indexs[dim-1]++;
//                    for(int j=dim-1;j>0;j--){
//                        if(indexs[j] >= sizes[j]){
//                            indexs[j] = 0;
//                            indexs[j-1]++;
//                        }
//                        else
//                            break;
//                    }
                }
            }
        }
    }
    else
        if(memberindex != -1)
            throw "ERROR memberindex!";

    string tokenstr = "1`{";
    if(index == -1) {
        if (data_field.empty()) {
            if(content != "")
                tokenstr += content + ",";
            else
                tokenstr += base_exp + ",";
        }
        else{
            for (int i = 0; i < data_field.size(); i++)
                if(i != content_index)
                    tokenstr += base_exp + "." + data_field[i] + ",";
                else
                    tokenstr += content + ",";
        }
        tokenstr += base_exp + id_suffix;
        if(!isglobal)
            tokenstr += "," + _tid;
        tokenstr += "}";
    }
    else{
        string indexstr = to_string(index);
        if(data_field.empty()){
            if(content != "")
                tokenstr += content + ",";
            else
                tokenstr += base_exp + arrindexmark(indexstr) + ",";
        }
        else{
            for (int i = 0; i < data_field.size(); i++)
                if(i != content_index)
                    tokenstr += base_exp + arrindexmark(indexstr) + "." + data_field[i] + ",";
                else
                    tokenstr += content + ",";
        }
        tokenstr += to_string(index) + ",";
        tokenstr += base_exp + arrindexmark(indexstr) + id_suffix;
        if(!isglobal)
            tokenstr += "," + _tid;
        tokenstr += "}";
    }
    return tokenstr;
}

string CPN::generateInitToken(string v_name,string base,short index,string addr,string content,string __tid){
//    auto memberinfos = ps.get_memberinfo();

    auto ps = sorttable.find_productsort(sorttable.find_typename(find_v_tag(v_name,base)).sid);
    auto memberinfos = ps.get_memberinfo();
    auto sortid = ps.get_sortid();
    auto sortname = ps.get_sortname();
    bool isglobal = find_v_isglobal(v_name,base);
    auto sl = find_v_size(v_name,base);
    auto pp = findP_byid(find_P_id(v_name,base));

    string base_exp = pp->getExp();
    string _tid = __tid == ""?tid_str:__tid;


    if(content != "" && !memberinfos.empty())
        throw "ERROR! InitToken must be normal variables!";
    vector<string> data_field;
    int content_index = -1;
    if(!memberinfos.empty()) {

//        string member_prefix = pp->getExp() + ".";
        for (int i = 0; i < sortid.size(); i++) {
            if(sortname[i] != id_sortname
            && sortname[i] != index_sortname
            && sortname[i] != tid_sortname)
                if (sortid[i].tid == Integer)
                    data_field.emplace_back("0");
                else if (sortid[i].tid == Real)
                    data_field.emplace_back("0.0");
                else if (sortid[i].tid == String)
                    data_field.emplace_back("\' \'");
                else
                    throw "we don't support structure in structure for now!";
        }
    }
    else
        ;

    string tokenstr = "1`{";
    if (data_field.empty()) {
        if(content != "")
            tokenstr += content + ",";
        else {
            if (sortid[0].tid == Integer)
                tokenstr += "0,";
            else if (sortid[0].tid == Real)
                tokenstr += "0.0,";
            else if (sortid[0].tid == String)
                tokenstr += "\' \',";
            else
                throw "we don't support structure in structure for now!";
        }
    }
    else{
        for (int i = 0; i < data_field.size(); i++)
            //Here content must be ""
            tokenstr += data_field[i] + ",";
    }
    if(index != -1)
        tokenstr += to_string(index) + ",";
    tokenstr += addr;
    if(!isglobal)
        tokenstr += "," + _tid;
    tokenstr += "}";

    return tokenstr;
}


void CPN::readOperation(string v_name,string base,string T_id,bool markedremain, bool markedwrite, AddArcForm arcForm) {
    bool isglobal = find_v_isglobal(v_name,base);
    bool ispointer = find_v_ispointer(v_name,base);
    auto sl = find_v_size(v_name,base);
    string tag = find_v_tag(v_name,base);
    string realtag = get_real_Type_name(tag,sl,isglobal);
    auto msi = sorttable.find_typename(realtag);
    auto ps = sorttable.find_productsort(msi.sid);
    string P_id = find_P_id(v_name,base);
    auto pp = findP_byid(P_id);
    string base_exp = pp->getExp();
    vector<string> tokenstrs;

    if(exist_in(pthread_type,tag))
        return;


    short index = -1,memberindex = -1;
    if(sl.empty()){
        //content == "",index and memberindex will not be used
       string tokenstr = generateToken(v_name,base,index,"",memberindex,"");
       tokenstrs.emplace_back(tokenstr);
    }
    else{
        auto dim = sl.getdim();
        auto sizes = sl.getsizes();
//        index = vector<short>(dim,0);
        short count = 0;
        while(count < sl.totalsize()) {
            string tokenstr = generateToken(v_name,base,count,"",memberindex,"");
            tokenstrs.emplace_back(tokenstr);
            count++;
            //loop around all the indexs
//            index[dim-1]++;
//            for(int j=dim-1;j>0;j--){
//                if(index[j] >= sizes[j]){
//                    index[j] = 0;
//                    index[j-1]++;
//                }
//                else
//                    break;
//            }
        }
    }

    string ArcExp;
    for(int i=0;i<tokenstrs.size();i++){
        if(i!=0)
            ArcExp += "++";
        ArcExp += tokenstrs[i];
    }
    Arc_Type arcType;
    if(markedremain)
        arcType = remain;
    else if(markedwrite)
        arcType = writeArc;
    else
        arcType = readArc;

    Add_Arc(pp->getid(),T_id,ArcExp,true,arcType,arcForm);
    Add_Arc(T_id,pp->getid(),ArcExp,false,arcType,arcForm);

}

void CPN::writePointer(string T_id,string address,string basetag,string content,vector<string>indexs,string member,vector<string>memberindexs){
    auto tt = findT_byid(T_id);
    tt->set_iswritepointer();
    for(int i=0;i<v_tables.size();i++){
        auto v_map = v_tables[i]->get_v_map();
        auto viter = v_map.begin();
        while(viter != v_map.end()){
            if(exist_in(pthread_type,viter->second.base_tag)){
                viter++;
                continue;
            }
            if(basetag != "" && viter->second.base_tag != basetag){
                viter++;
                continue;
            }
            auto sl = viter->second.sizelist;
//            if(sizes.size() != indexs.size()){
//                viter++;
//                continue;
//            }
            string P_id = viter->second.place;
            auto pp = findP_byid(viter->second.place);
            string base_exp = pp->getExp();
            string v_name = viter->first;
            string base = v_tables[i]->get_name();
            SizeList msl;
            string membertoken = "";
            if(member != "") {
                auto ps = sorttable.find_productsort(sorttable.find_typename(viter->second.tag).sid);
                auto memberinfos = ps.get_memberinfo();
                for (int i = 0; i < memberinfos.size(); i++) {
                    if (memberinfos[i].membername == member)
                        msl = memberinfos[i].msl;
                }
            }
            readOperation(v_name,base,T_id,true,false,beoverrided);

//            auto twotokens = generate_writeNormalstr(T_id,v_name,base,indexs,
//                                                       member,memberindexs,content);
//            string memberindexstr = Multiindexs2index(memberindexs,sl);

            string appendstr;
            if(sl.empty() && msl.empty()){
//                string memberindexstr = Multiindexs2index(memberindexs,msl);
                appendstr += "++";
//                for(int k=0;k<sl.totalsize();k++){
//                    appendstr += CaseFlag + address;
//                    appendstr += ":";
//                    appendstr += v_name + arrindexmark(to_string(k))+id_suffix;
//                    appendstr += "=>";
                appendstr += CaseFlag + address;
                appendstr += ":";
                appendstr += v_name + id_suffix;
                appendstr += "=>";
//                for(int j=0;j<msl.totalsize();j++){
//                    appendstr += CaseFlag + memberindexstr;
//                    appendstr += ":";
//                    appendstr += to_string(j);
//                    appendstr += "=>";
                    appendstr += generateToken(v_name,base,-1,member,-1,content);
//                    appendstr += "default;";
//                }
                appendstr += ":default;";
//                }
                appendstr += "--";
//                for(int k=0;k<sl.totalsize();k++){
//                    appendstr += CaseFlag + address;
//                    appendstr += ":";
//                    appendstr += v_name + arrindexmark(to_string(k))+id_suffix;
//                    appendstr += "=>";
                appendstr += CaseFlag + address;
                appendstr += ":";
                appendstr += v_name + id_suffix;
                appendstr += "=>";
//                for(int j=0;j<msl.totalsize();j++){
//                    appendstr += CaseFlag + memberindexstr;
//                    appendstr += ":";
//                    appendstr += to_string(j);
//                    appendstr += "=>";
                    appendstr += generateToken(v_name,base,-1,member,-1,"");
//                    appendstr += "default;";
//                }
                appendstr += ":default;";
//                }
            }
            else if(sl.empty() && !msl.empty()){
                string memberindexstr = Multiindexs2index(memberindexs,msl);
                appendstr += "++";
//                for(int k=0;k<sl.totalsize();k++){
//                    appendstr += CaseFlag + address;
//                    appendstr += ":";
//                    appendstr += v_name + arrindexmark(to_string(k))+id_suffix;
//                    appendstr += "=>";
                appendstr += CaseFlag + address;
                appendstr += ":";
                appendstr += v_name + id_suffix;
                appendstr += "=>";
                appendstr += CaseFlag + memberindexstr;
                    for(int j=0;j<msl.totalsize();j++){
                        appendstr += ":";
                        appendstr += to_string(j);
                        appendstr += "=>";
                        appendstr += generateToken(v_name,base,-1,member,j,content);

                    }
                appendstr += ":default;";
                appendstr += ":default;";
//                }
                appendstr += "--";
//                for(int k=0;k<sl.totalsize();k++){
//                    appendstr += CaseFlag + address;
//                    appendstr += ":";
//                    appendstr += v_name + arrindexmark(to_string(k))+id_suffix;
//                    appendstr += "=>";
                appendstr += CaseFlag + address;
                appendstr += ":";
                appendstr += v_name + id_suffix;
                appendstr += "=>";
                appendstr += CaseFlag + memberindexstr;
                    for(int j=0;j<msl.totalsize();j++){
                        appendstr += ":";
                        appendstr += to_string(j);
                        appendstr += "=>";
                        appendstr += generateToken(v_name,base,-1,member,j,"");

                    }
                appendstr += ":default;";
                appendstr += ":default;";
//                }
            }
            else if(!sl.empty() && msl.empty()){
//                string memberindexstr = Multiindexs2index(memberindexs,msl);
                appendstr += "++";
                appendstr += CaseFlag + address;
                for(int k=0;k<sl.totalsize();k++){
                    appendstr += ":";
                    appendstr += v_name + arrindexmark(to_string(k))+id_suffix;
                    appendstr += "=>";
//                    for(int j=0;j<msl.totalsize();j++){
//                        appendstr += CaseFlag + memberindexstr;
//                        appendstr += ":";
//                        appendstr += to_string(j);
//                        appendstr += "=>";
                        appendstr += generateToken(v_name,base,k,member,-1,content);
//                        appendstr += "default;";
//                    }

                }
                appendstr += ":default;";
                appendstr += "--";
                appendstr += CaseFlag + address;
                for(int k=0;k<sl.totalsize();k++){
                    appendstr += ":";
                    appendstr += v_name + arrindexmark(to_string(k))+id_suffix;
                    appendstr += "=>";
//                    for(int j=0;j<msl.totalsize();j++){
//                        appendstr += CaseFlag + memberindexstr;
//                        appendstr += ":";
//                        appendstr += to_string(j);
//                        appendstr += "=>";
                        appendstr += generateToken(v_name,base,k,member,-1,"");
//                        appendstr += "default;";
//                    }
                }
                appendstr += ":default;";
            }
            else{
                string memberindexstr = Multiindexs2index(memberindexs,msl);
                appendstr += "++";
                appendstr += CaseFlag + address;
                for(int k=0;k<sl.totalsize();k++){

                    appendstr += ":";
                    appendstr += v_name + arrindexmark(to_string(k))+id_suffix;
                    appendstr += "=>";
                    CaseFlag + memberindexstr;
                    for(int j=0;j<msl.totalsize();j++){
                        appendstr +=
                        appendstr += ":";
                        appendstr += to_string(j);
                        appendstr += "=>";
                        appendstr += generateToken(v_name,base,k,member,j,content);

                    }
                    appendstr += ":default;";
                }
                appendstr += ":default;";
                appendstr += "--";
                appendstr += CaseFlag + address;
                for(int k=0;k<sl.totalsize();k++){
                    appendstr += ":";
                    appendstr += v_name + arrindexmark(to_string(k))+id_suffix;
                    appendstr += "=>";
                    appendstr += CaseFlag + memberindexstr;
                    for(int j=0;j<msl.totalsize();j++){

                        appendstr += ":";
                        appendstr += to_string(j);
                        appendstr += "=>";
                        appendstr += generateToken(v_name,base,k,member,j,"");

                    }
                    appendstr += ":default;";
                }
                appendstr += ":default;";
            }

            Add_Arc(T_id,P_id,appendstr,false,remain,append);
            viter++;
        }
    }
}

void CPN::readPointer(string T_id,string tag){
    for(int i=0;i<v_tables.size();i++){
        auto v_map = v_tables[i]->get_v_map();
        auto viter = v_map.begin();
        while(viter != v_map.end()){
            if(tag != "" && viter->second.base_tag != tag){
                viter++;
                continue;
            }
            string v_name = viter->first;
            string base = v_tables[i]->get_name();
            readOperation(v_name,base,T_id,true,false,beoverrided);

            viter++;
        }
    }
}
//WriteNormalstr should replace the origin token with another one with content
//return a vector of string
//actually it just has two element ret[0],ret[1]
//ret[0] represent the token with content, ret[1] represent the original token been replaced
vector<string> CPN::generate_writeNormalstr(string T_id,string v_name,string base,vector<string>indexs,string member,vector<string> memberindexs,string content){
    string P_id = find_P_id(v_name, base);
    auto pp = findP_byid(P_id);
    string baseexp = pp->getExp();
    bool isglobal = pp->getisglobal();
    bool ispointer = find_v_ispointer(v_name,base);
    if(ispointer)
        throw "ERROR in generate_writeNormalstr, shouldn't have pointer!";
    string ArcExp;
    string Addtoken,Subtoken;
//    if (indexs.empty() && !memberindexs.empty()) {
        auto ps = sorttable.find_productsort(sorttable.find_typename(find_v_tag(v_name, base)).sid);
        auto memberinfos = ps.get_memberinfo();
        SizeList sl,msl;
        sl = find_v_size(v_name,base);

//        vector<string> memberindexstr;
//        for (int i = 0; i < memberindexs.size(); i++)
//            memberindexstr[i] = to_string(memberindexs[i]);
        for (int i = 0; i < memberinfos.size(); i++) {
            if (memberinfos[i].membername == member) {
                msl = memberinfos[i].msl;
                break;
            }
        }
//        if (sl.getdim() > 1)
//            throw "we don't support multiple array for now!";
//        auto sizes = sl.getsizes();

//        ArcExp += "++";
        if(!indexs.empty() && !memberindexs.empty()) {
            string indexstr, memberindexstr;
            bool isindexconstant = judge_constantindex(indexs);
            bool ismemberindexconstant = judge_constantindex(memberindexs);

            indexstr = Multiindexs2index(indexs, sl);
            memberindexstr = Multiindexs2index(memberindexs, msl);
            if(isindexconstant && ismemberindexconstant){
                Addtoken += generateToken(v_name,base,conststr2const(indexstr),member,conststr2const(memberindexstr),content);
                Subtoken += generateToken(v_name,base,conststr2const(indexstr),member,conststr2const(memberindexstr),"");
            }
            else if(isindexconstant && !ismemberindexconstant){
                for(int j=0;j<msl.totalsize();j++){
                    Addtoken += CaseFlag + memberindexstr;
                    Addtoken += ":";
                    Addtoken += to_string(j);
                    Addtoken += "=>";
                    Addtoken += generateToken(v_name, base, conststr2const(indexstr), member, j, content);
                    Subtoken += CaseFlag + memberindexstr;
                    Subtoken += ":";
                    Subtoken += to_string(j);
                    Subtoken += "=>";
                    Subtoken += generateToken(v_name, base, conststr2const(indexstr), member, j, "");
                    Addtoken += ":default;";
                    Subtoken += ":default;";
                }
            }
            else if(!isindexconstant && ismemberindexconstant){
                for(int i=0;i<sl.totalsize();i++){
                    Addtoken += CaseFlag + indexstr;
                    Addtoken += ":";
                    Addtoken += to_string(i);
                    Addtoken += "=>";
                    Addtoken += generateToken(v_name, base, i , member, conststr2const(memberindexstr), content);
                    Subtoken += CaseFlag + indexstr;
                    Subtoken += ":";
                    Subtoken += to_string(i);
                    Subtoken += "=>";
                    Subtoken += generateToken(v_name, base, i , member, conststr2const(memberindexstr), "");
                    Addtoken += ":default;";
                    Subtoken += ":default;";
                }
            }
            else {
                for (int i = 0; i < sl.totalsize(); i++) {
                    Addtoken += CaseFlag + indexstr;
                    Addtoken += ":";
                    Addtoken += to_string(i);
                    Addtoken += "=>";
                    Subtoken += CaseFlag + indexstr;
                    Subtoken += ":";
                    Subtoken += to_string(i);
                    Subtoken += "=>";
                    for (int j = 0; j < msl.totalsize(); j++) {
                        Addtoken += CaseFlag + memberindexstr;
                        Addtoken += ":";
                        Addtoken += to_string(j);
                        Addtoken += "=>";
                        Addtoken += generateToken(v_name, base, i, member, j, content);
                        Subtoken += CaseFlag + memberindexstr;
                        Subtoken += ":";
                        Subtoken += to_string(j);
                        Subtoken += "=>";
                        Subtoken += generateToken(v_name, base, i, member, j, "");
                        Addtoken += ":default;";
                        Subtoken += ":default;";
                    }
                    Addtoken += ":default;";
                    Subtoken += ":default;";

                }
            }
        }
        else if(!indexs.empty() && memberindexs.empty()){
            string indexstr;
            bool isindexconstant = judge_constantindex(indexs);
            indexstr = Multiindexs2index(indexs, sl);
            if(isindexconstant){
                Addtoken += generateToken(v_name,base,conststr2const(indexstr),member,-1,content);
                Subtoken += generateToken(v_name,base,conststr2const(indexstr),member,-1,"");
            }
            else {
                for (int i = 0; i < sl.totalsize(); i++) {
                    Addtoken += CaseFlag + indexstr;
                    Addtoken += ":";
                    Addtoken += to_string(i);
                    Addtoken += "=>";
                    Subtoken += CaseFlag + indexstr;
                    Subtoken += ":";
                    Subtoken += to_string(i);
                    Subtoken += "=>";
//                for (int j = 0; j < msl.totalsize(); j++) {
//                    Addtoken += CaseFlag + memberindexstr;
//                    Addtoken += ":";
//                    Addtoken += to_string(j);
//                    Addtoken += "=>";
                    Addtoken += generateToken(v_name, base, i, member, -1, content);
                    Subtoken += generateToken(v_name, base, i, member, -1, "");
                    Addtoken += ":default;";
                    Subtoken += ":default;";
//                }
                }
            }
        }
        else if(indexs.empty() && !memberindexs.empty()){
            string memberindexstr;
            memberindexstr = Multiindexs2index(memberindexs, sl);
            bool ismemberindexconstant = judge_constantindex(memberindexs);
            if(ismemberindexconstant){
                Addtoken += generateToken(v_name,base,-1,member,conststr2const(memberindexstr),content);
                Subtoken = "";/// optimization
//                Subtoken += generateToken(v_name,base,-1,member,conststr2const(memberindexstr),"");
            }
//            for (int i = 0; i < sl.totalsize(); i++) {
//                Addtoken += CaseFlag + indexstr;
//                Addtoken += ":";
//                Addtoken += to_string(i);
//                Addtoken += "=>";
                for (int j = 0; j < msl.totalsize(); j++) {
                    Addtoken += CaseFlag + memberindexstr;
                    Addtoken += ":";
                    Addtoken += to_string(j);
                    Addtoken += "=>";
                    Addtoken += generateToken(v_name,base,-1,member,j,content);
//                    Subtoken += CaseFlag + memberindexstr;
//                    Subtoken += ":";
//                    Subtoken += to_string(j);
//                    Subtoken += "=>";
//                    Subtoken += generateToken(v_name,base,-1,member,j,"");
                    Addtoken += ":default;";
                    Subtoken = "";/// optimization
//                    Subtoken += ":default;";
                }
//            }
        }
        else {
//            string indexstr;
//            indexstr = Multiindexs2index(indexs, sl);
//            for (int i = 0; i < sl.totalsize(); i++) {
//                Addtoken += CaseFlag + indexstr;
//                Addtoken += ":";
//                Addtoken += to_string(i);
//                Addtoken += "=>";
//                for (int j = 0; j < msl.totalsize(); j++) {
//                    Addtoken += CaseFlag + memberindexstr;
//                    Addtoken += ":";
//                    Addtoken += to_string(j);
//                    Addtoken += "=>";
                Addtoken += generateToken(v_name,base,-1,member,-1,content);
//                Subtoken = "";
//                Subtoken += generateToken(v_name,base,-1,member,-1,"");
                Subtoken = "";/// optimization
//                }
//            }
        }

    vector<string> res;
    res.push_back(Addtoken);
    res.push_back(Subtoken);
    return res;
}

void CPN::AddwriteArc2pointer(string T_id,string identifier,string base){
    string P_id = find_P_id(identifier,base);
    auto pp = findP_byid(P_id);
//    string Exp = generateToken(identifier,base,-1,"",-1,"");
//    Add_Arc(T_id,P_id,Exp,false,writeArc,override);
    readOperation(identifier,base,T_id,false, true,override);
}

void CPN::writeOperation(string T_id,gtree *pos,string content){
    if(pos != NULL && pos->type != UNARY_EXPRESSION && pos->type != POSTFIX_EXPRESSION || pos == NULL)
        throw "you should pass an unary_expression tree to indicate position.";

    if(pos) {
        bool reference_flag = false;
        gtree *postexp = pos;
        if (pos->type == UNARY_EXPRESSION) {
            if (pos->child->next != NULL){
                if(pos->child->type != UNARY_OPERATOR)
                    throw "pos lvalue just can be UNARY_OPERATOR!";
                if (pos->child->child->place != "*")
                    throw "pos lvalue just can be *!";
                reference_flag = true;
                postexp = pos->child->next;
            }
            else
                postexp = pos->child;

        }
        if (reference_flag) {
            string addr = translate_exp2arcexp(postexp);

            writePointer(T_id,addr,"",content,vector<string>(),"",vector<string>());

            //writepointer should construct write arc with pointer

            gtree *com = pos->parent;
            while (com->type != COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string identifier = postexp->place;
            if(!judge_isidentifier(identifier))
                throw "*(exp) should be processed. but not yet. just handle *id";
            auto P_id = find_P_id(postexp->place,base);
            AddwriteArc2pointer(T_id,identifier,base);
//            auto pp = findP_byid(P_id);
//            Add_Arc(T_id,P_id,pp->getExp(),false,write,override);
        }
        else {

            gtree *com = pos->parent;
            while (com->type != COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            gtree *ptr = pos->child;

            vector<gtree *> memberindex_trees, index_trees;
            string member = "", identifier = "";
//        string index_tmp = "";
            bool filter_flag = false;//identify left or right of '->' or '.'
            gtree *addr_tree = NULL;
            while (ptr) {
                if (ptr->next && ptr->next->type == PTR_OP) {
                    filter_flag = true;
                    member = ptr->next->next->place;
                    if (ptr->next->type == PTR_OP) {
                        addr_tree = ptr->child;
//                        break;
                    }
                }
                if(ptr->next && ptr->next->place == ".")
                    filter_flag = true;
                if (ptr->next && ptr->next->place == "[" && !filter_flag)
                    memberindex_trees.emplace_back(ptr->next->next);
                else if (ptr->next && ptr->next->place == "[" && filter_flag)
                    index_trees.emplace_back(ptr->next->next);
                if (!ptr->child)
                    identifier = ptr->place;
                ptr = ptr->child;
            }
            if(filter_flag == false)
                index_trees = std::move(memberindex_trees);
            vector<string> memberindexs,indexs;
            for(int i=0;i<memberindex_trees.size();i++)
                memberindexs.emplace_back(translate_exp2arcexp(memberindex_trees[i]));
            for(int i=0;i<index_trees.size();i++)
                indexs.emplace_back(translate_exp2arcexp(index_trees[i]));

            if(addr_tree != NULL){
                string addr = translate_exp2arcexp(addr_tree);
                string basetag = "";
                if(identifier != "")
                    basetag = find_v_basetag(identifier,base);


                writePointer(T_id,addr,basetag,content,indexs,member,memberindexs);

                //writepointer should construct write arc with pointer
                auto P_id = find_P_id(identifier,base);
//                auto pp = findP_byid(P_id);
//                Add_Arc(T_id,P_id,pp->getExp(),false,write,override);
                AddwriteArc2pointer(T_id,identifier,base);

            }
            else {
                bool ispointer = find_v_ispointer(identifier,base);
                auto sl = find_v_size(identifier,base);
                if(ispointer && index_trees.size()>0 && index_trees.size() == sl.getdim() + 1){
//                    if(index_trees.size() > 1)
//                        throw "we can't support multiple array for pointer!";
                    string P_id = find_P_id(identifier,base);
                    auto basetag = find_v_basetag(identifier,base);
                    auto pp = findP_byid(P_id);
                    string addr_offset;
                    if(index_trees.size() > 1)
                        addr_offset = Multiindexs2index(indexs,sl);
                    else
                        addr_offset = indexs[0];
                    string addr = pp->getExp() + "+" + addr_offset;
//                    if(member != "" || memberindexs.size() != 0)
//                        throw "we don't support index and memberindex at the same time for now";
                    writePointer(T_id,addr,basetag,content,indexs,member,memberindexs);

                    //writepointer should construct write arc with pointer
                    AddwriteArc2pointer(T_id,identifier,base);
//                    Add_Arc(T_id,P_id,pp->getExp(),false,write,override);
//                    readOperation(identifier,base,T_id,false);
                }
                else if(ispointer){
                    string P_id = find_P_id(identifier,base);
                    writeOperation(P_id,T_id,content, Multiindexs2index(indexs,sl),sl,base);

                }
                else {
//                    if (memberindex_trees.size() > 1 || index_trees.size() > 1)
//                        throw "we don't support multi-dimension array for now!";
//                    if (!memberindex_trees.empty() && !index_trees.empty())
//                        throw "we don't support index and memberindex at the same time for now";

                    auto twotokens = generate_writeNormalstr(T_id, identifier, base, indexs, member, memberindexs,
                                                             content);
                    if(twotokens[1] == ""){
                        string replaceArcexp = twotokens[0];
                        string P_id = find_P_id(identifier, base);
                        readOperation(identifier, base, T_id, false, false,beoverrided);
                        Add_Arc(T_id, P_id, replaceArcexp, false, writeArc, override);
                    }
                    else {
                        string appendArcexp;
                        appendArcexp += "++" + twotokens[0] + "--" + twotokens[1];
                        string P_id = find_P_id(identifier, base);
                        readOperation(identifier, base, T_id, false,false,beoverrided);
                        Add_Arc(T_id, P_id, appendArcexp, false, writeArc, append);
                    }
                }
            }
        }
    }
    else
        throw "pos shouldn't be NULL!";
}

void CPN::writeOperation(string P_id, string T_id, string content, string index_str, SizeList sl, string base) {
    auto pp = findP_byid(P_id);
    bool isglobal = pp->getisglobal();
    string base_exp = pp->getExp();
//    string ReadArcExp = "1`{",WriteArcExp = "1`{";
//    WriteArcExp += content + ",";
//    ReadArcExp += base_exp + ",";
//    WriteArcExp += base_exp + id_suffix;
//    ReadArcExp += base_exp + id_suffix;
//    if(!isglobal) {
//        WriteArcExp += "," + tid_str;
//        ReadArcExp += "," + tid_str;
//    }
//    WriteArcExp += "}";
//    ReadArcExp += "}";
//
//    Add_Arc(T_id,pp->getid(),WriteArcExp,false,writeArc, override);
//    Add_Arc(pp->getid(),T_id,ReadArcExp,true,writeArc,override);
    if(index_str == "") {
        string ReadArcExp = "1`{", WriteArcExp = "1`{";
        WriteArcExp += content + ",";
        ReadArcExp += base_exp + ",";
        WriteArcExp += base_exp + id_suffix;
        ReadArcExp += base_exp + id_suffix;
        if (!isglobal) {
            WriteArcExp += "," + tid_str;
            ReadArcExp += "," + tid_str;
        }
        WriteArcExp += "}";
        ReadArcExp += "}";
        Add_Arc(T_id, pp->getid(), WriteArcExp, false, writeArc, override);
        Add_Arc(pp->getid(), T_id, ReadArcExp, true, writeArc, override);
    }
    else{
        string writeArcExp;
        writeArcExp += "++";
        writeArcExp += CaseFlag + index_str;
        for(int i=0;i<sl.totalsize();i++){
            writeArcExp += ":";
            writeArcExp += to_string(i);
            writeArcExp += "=>";
            writeArcExp += generateToken(pp->getExp(),base,i,"",-1,content);
        }
        writeArcExp += ";";

        writeArcExp += "--";
        writeArcExp += CaseFlag + index_str;
        for(int i=0;i<sl.totalsize();i++){
            writeArcExp += ":";
            writeArcExp += to_string(i);
            writeArcExp += "=>";
            writeArcExp += generateToken(pp->getExp(),base,i,"",-1,"");
        }
        writeArcExp += ";";

        readOperation(pp->getExp(),base,T_id,false,true,beoverrided);
        Add_Arc(T_id,P_id,writeArcExp,false,writeArc,append);
    }
}

//Function: pthread_create add data copys for every local variables
//parameter:
//T_id -- pthread_create transition
//newtid -- new tid for every copy
//para_P and initvalue -- because pthread_create will pass parameter to function, the parameter should have initvalue
void CPN::Add_pthreadCopys(string T_id,string newtid,string para_P,string initvalue){
    int tmp_id_ptr = threadDomainInit ;
    threaddomaininc();

    // start from 1 to filter global variables
    for(int i=1;i<v_tables.size();i++){
        auto v_map = v_tables[i]->get_v_map();
        auto viter = v_map.begin();
        while(viter != v_map.end()){
            string v_name = viter->first;
            string base = v_tables[i]->get_name();
            string P_id = viter->second.place;

            auto pp = findP_byid(P_id);
//            v_name = pp->getExp();

            auto sl = viter->second.sizelist;
            string content = para_P == P_id?initvalue:"";
            string ArcExp;
            if(sl.empty()){
                ArcExp += generateInitToken(v_name,base,-1,to_string(tmp_id_ptr++),content,newtid);
            }
            else{
                for(int i=0;i<sl.totalsize();i++){
                    if(i!=0)
                        ArcExp += "++";
                    ArcExp += generateInitToken(v_name,base,-1,to_string(tmp_id_ptr++),content,newtid);
                }
            }
            //这里改成override因为传递的参数中可能涉及变量，在解析expresion时已经构建读弧
            auto testArc = findArc_bysrctgt(T_id,P_id);
            if(testArc == NULL)
                Add_Arc(T_id,P_id,ArcExp,false,remain,normal);
            else
                Add_Arc(T_id,P_id,"++" + ArcExp,false,remain,append);

            viter++;
        }
    }
}


void CPN::writestrcpy(string T,string str,string addr){
//    string T = triple.first;
//    string str = triple.second;
//    string address = triple.third;
//    bool structureflag = triple.flag;

    for(int i=0;i<v_tables.size();i++){
        auto v_map = v_tables[i]->get_v_map();
        auto viter = v_map.begin();
        while(viter != v_map.end()){
            string v_name = viter->first;
            string base = v_tables[i]->get_name();
            string P_id = viter->second.place;
            string tid_flag1,real_id;
            if (i == 0)//global
                tid_flag1 = "";
            else
                tid_flag1 = tid_str;
            auto pp = findP_byid(P_id);
            real_id = pp->getExp();

            auto sl = viter->second.sizelist;
            if(sl.totalsize() < str.length()) {
                viter++;
                continue;
            }
//            string content = para_P == P_id?initvalue:"";
            string ArcExp,writecase,readcase,tmp_value;
            readcase = writecase = CaseFlag + addr;
            if (!sl.empty()) {
//                string real_id = v_name;
                writecase.append(":");
                writecase.append(real_id + arrindexmark(to_string(0)) + id_suffix);
                writecase.append("=>");
                readcase = writecase;
                for(unsigned int i=0;i<str.length();i++){
                    tmp_value.clear();
                    tmp_value.append("'");
                    tmp_value.append(1,str[i]);
                    tmp_value.append("'");
                    if(i>0){
                        writecase += "++";
                        readcase += "++";
                    }
                    writecase += construct_normaltoken(tmp_value,to_string(i),real_id +  arrindexmark(to_string(i)) + id_suffix,tid_flag1);;
                    readcase += construct_normaltoken(real_id + arrindexmark(to_string(i)),to_string(i),real_id + arrindexmark(to_string(i)) + id_suffix,tid_flag1);
                }

                writecase.append(":default;");
                readcase.append(":default;");

//                Exp_tmp1 = readcase;
//                Exp_tmp2 = writecase;
            }
            else
                throw "ERROR!strcpy's source can not be empty!";
            Add_Arc(T, P_id, writecase, false, remain, override);
            Add_Arc(P_id, T, readcase, true, remain, override);

            viter++;
        }
    }
}

void CPN::setmaintoken() {
//    auto iter = mapFunction.find("main" + begin_suffix);
//    if(iter == mapFunction.end())
//    {
//        cout<<"can't find main func"<<endl;
//        exit(-1);
//    }
    string main_begin_P = f_table.findbeginplace("main");
//    string main_P = iter->second;
//    auto iter2 = mapPlace.find(main_P);
//    if(iter2 == mapPlace.end())
//    {
//        cout<<"can't find main place"<<endl;
//        exit(-1);
//    }
    auto pp = findP_byid(main_begin_P);
    MultiSet ms;
    if(ms.getcolorcount() == 0){
        token tk;
        if(TID_colorset != String)
            throw "we assume TID_colorset is String!";
        tk = (token)(new StringSortValue);
        Bucket bkt;
        bkt.tid = String;
        bkt.str = init_tid;
        tk->setcolor(bkt);
        ms.generateFromToken(tk);
        pp->AddMultiSet(ms);
    }
//    if(place[iter2->second].initMarking.tokenQ->next == NULL) {
//        Tokens *token = new Tokens;
//        if(TID_colorset == String)
//            token->color = new StringSortValue;
//        else if(TID_colorset == Integer)
//            token->color = new IntegerSortValue;
//        else if(TID_colorset == Real)
//            token->color = new RealSortValue;
//        token->color->setColor(init_tid);
//        token->tokencount = 1;
//        place[iter2->second].initMarking.insert(token);
//    }
}

void CPN::delete_arc(string source,string target){
    for(unsigned int i=0;i<arccount;i++)
        if((arc[i].getsrc() == source || source == "" )
           && (arc[i].gettgt() == target || target == "" )){
            arc[i].setdeleted();
//            for(unsigned int j=i;j<arccount-1;j++){
//                arc[j] = arc[j+1];
//            }
//            arccount--;
//            i--;
        }

}

void CPN::delete_compound(gtree *p) {
    if(p==NULL)return;
    if(p->type == COMPOUND_STATEMENT && p->parent->type == STATEMENT && p->child->next->place != "}")
    {
        string statement_P = p->parent->matched_P;
        string statement_T = get_enter_T(statement_P)[0];//compound just has one enter_T
        string pre_exe_P = "",after_exe_P = "",father_T = "";
        vector<string> after_P;

        //find pre_exe_P,after_exe_P,after_P
        //father_T pre_exe_P and after_exe_P are the only so we can just define one father_P pre_exe_P and after_exe_P
        for(unsigned int i=0;i<arccount;i++){
            if(arc[i].gettgt() == statement_T){
                string temp = arc[i].getsrc();
                auto iter = mapPlace.find(temp);
                if(place[iter->second].getExp() == executed_P_name){
                    pre_exe_P = temp;
                }
            }
            if(arc[i].getsrc() == statement_T){
                string temp = arc[i].gettgt();
                auto iter = mapPlace.find(temp);
                if(place[iter->second].getExp() == executed_P_name){
                    after_exe_P = temp;
                }
                else{
                    after_P.push_back(temp);
                }
            }
            if(arc[i].gettgt() == statement_P){
                father_T = arc[i].getsrc();
            }
        }
        bool iter_flag=false;
        for(unsigned int i=0;i<arccount;i++)
            if(arc[i].gettgt() == after_exe_P && arc[i].getsrc() != statement_T){
                iter_flag=true;
                break;
            }
        if(pre_exe_P == "" || after_exe_P == "" || father_T == "")
            throw "error in delete_compound!";
        delete_arc(father_T,statement_P);
        delete_arc(after_exe_P,"");
        delete_arc("",after_exe_P);
        delete_arc(pre_exe_P,statement_T);
        for(unsigned int i=0;i<after_P.size();i++) {
            delete_arc(statement_T, after_P[i]);
            Add_Arc(father_T,after_P[i],controlflowArcExp(tid_str),false,control,beoverrided);
        }
        vector<string> first_after_T = get_enter_T(after_P[0]);
        for(unsigned int i=0;i<first_after_T.size();i++)
            Add_Arc(pre_exe_P,first_after_T[i],controlflowArcExp(tid_str),true,control,beoverrided);

        if(iter_flag == true){
            vector<string> last_T = get_enter_T(after_P[after_P.size()-1]);
            for(unsigned int i=0;i<last_T.size();i++)
                Add_Arc(last_T[i],pre_exe_P,controlflowArcExp(tid_str),false,executed,beoverrided);
        }
    }
    delete_compound(p->child);
    delete_compound(p->next);
}
void CPN::delete_compound_withoutdependence(gtree *p) {
    if(p==NULL)return;
    if(p->type == COMPOUND_STATEMENT && p->parent->type == STATEMENT && p->child->next->place != "}")
    {
        string statement_P = p->parent->matched_P;
        string statement_T = get_enter_T(statement_P)[0];//compound just has one enter_T
        // string pre_exe_P = "",after_exe_P = "",father_T = "";
        string pre_T,after_P;

        //find pre_exe_P,after_exe_P,after_P
        //father_T pre_exe_P and after_exe_P are the only so we can just define one father_P pre_exe_P and after_exe_P
        for(unsigned int i=0;i<arccount;i++){
            if(arc[i].gettgt() == statement_P)
                pre_T = arc[i].getsrc();
            if(arc[i].getsrc() == statement_T)
                after_P = arc[i].gettgt();
        }
        delete_arc(pre_T,statement_P);
        delete_arc(statement_T,after_P);
        Add_Arc(pre_T,after_P,controlflowArcExp(tid_str),false,executed,beoverrided);
    }
    delete_compound_withoutdependence(p->child);
    delete_compound_withoutdependence(p->next);
}

void CPN::set_producer_consumer() {
    string source,target;
    CSArc csArc1,csArc2;
    for(int i=0;i<placecount;i++){
        place[i].clear_consumer();
        place[i].clear_producer();
    }
    for(int i=0;i<transitioncount;i++){
        transition[i].clear_consumer();
        transition[i].clear_producer();
    }

    for(int i=0;i<arccount;i++)
    {
        bool is_deleted = false;
        if(arc[i].getdeleted())
            continue;
        source = arc[i].getsrc();
        target = arc[i].gettgt();
        csArc1.arcType = arc[i].getArctype();
        csArc1.arc_exp.referencetree(arc[i].getArcCT());
        csArc1.is_deleted = is_deleted;
//        csArc1.onlydot = arc[i].onlydot;
        csArc2.arcType = arc[i].getArctype();
        csArc2.arc_exp.referencetree(arc[i].getArcCT());
        csArc2.is_deleted = is_deleted;
//        csArc2.onlydot = arc[i].onlydot;
        if(arc[i].getisp2t()) {
            auto pp = findP_byid(source);
            auto tt = findT_byid(target);

            csArc1.idx = tt - transition;
            csArc2.idx = pp - place;
            pp->Add_consumer(csArc1);
            tt->Add_producer(csArc2);
        }
        else
        {
            auto pp = findP_byid(target);
            auto tt = findT_byid(source);
            csArc1.idx = tt - transition;
            csArc2.idx = pp - place;
            pp->Add_producer(csArc1);
            tt->Add_consumer(csArc2);
        }
    }
}

void CPN::init_alloc_func() {
    string P = gen_P();
    alloc_store_P = P;
//    string Type_name = get_real_Type_name(alloc_store_type, SizeList() , true);
    Add_Place(P,alloc_store_type,false,SizeList(),"alloc_store",true,false,NoneRow);

    token tk;
    tk = (token)(new IntegerSortValue);
    Bucket bkt;
    bkt.tid = Integer;
    bkt.integer = init_alloc;
    tk->setcolor(bkt);
    MultiSet ms;
    ms.generateFromToken(tk);
    auto pp = findP_byid(P);
    pp->AddMultiSet(ms);
}

//just copy the address of father CPN, so don't need to free
void CPN::copy_childNet(CPN *cpnet,vector<string> places,vector<string> transitions) {

//    mapPlace = cpnet->mapPlace;
    mapVariable = cpnet->mapVariable;
//    mapTransition = cpnet->mapTransition;
    transPri = cpnet->get_transPri();
    vartable = cpnet->vartable;
    for(unsigned int i=0;i<places.size();i++)
    {
        auto iter = cpnet->mapPlace.find(places[i]);
        if(iter == cpnet->mapPlace.end())
            throw "can't find place in copy_childtree";
        place[placecount++]=cpnet->place[iter->second];
        if(cpnet->place[iter->second].getiscontrolP() && cpnet->place[iter->second].getisexecuted())
            exePlacecount++;
        else if(cpnet->place[iter->second].getiscontrolP() && !cpnet->place[iter->second].getisexecuted())
            ctlPlacecount++;
        else
            varPlacecount++;
//        memcpy(&place[placecount++],&cpnet->place[iter->second],sizeof(CPlace));
//        place[placecount++] = cpnet->place[iter->second];
//        for(auto iter=place[placecount-1].producer.begin();iter!=place[placecount-1].producer.end();)
//        {
//            if(!exist_in(transitions,cpnet->transition[iter->idx].id))
//                iter = place[placecount-1].producer.erase(iter);
//            else
//                iter++;
//        }
//        for(auto iter=place[placecount-1].consumer.begin();iter!=place[placecount-1].consumer.end();)
//        {
//            if(!exist_in(transitions,cpnet->transition[iter->idx].id))
//                iter = place[placecount-1].consumer.erase(iter);
//            else
//                iter++;
//        }
        place[placecount-1].get_producer().clear();
        place[placecount-1].get_consumer().clear();
        mapPlace.insert(make_pair(places[i],placecount-1));
    }
    for(unsigned int i=0;i<transitions.size();i++)
    {
        auto iter = cpnet->mapTransition.find(transitions[i]);
        if(iter == cpnet->mapTransition.end())
        {
            cout<<"can't find transition in copy_childtree"<<endl;
            exit(-1);
        }
        transition[transitioncount++] = cpnet->transition[iter->second];
//        memcpy(&transition[transitioncount++],&cpnet->transition[iter->second],sizeof(CTransition));
//        transition[transitioncount++] = cpnet->transition[iter->second];
//        for(auto iter=transition[transitioncount-1].producer.begin();iter!=transition[transitioncount-1].producer.end();)
//        {
//            if(!exist_in(transitions,cpnet->transition[iter->idx].id))
//                iter = transition[transitioncount-1].producer.erase(iter);
//            else
//                iter++;
//        }
//        for(auto iter=transition[transitioncount-1].consumer.begin();iter!=transition[transitioncount-1].consumer.end();)
//        {
//            if(!exist_in(transitions,cpnet->transition[iter->idx].id))
//                iter = transition[transitioncount-1].consumer.erase(iter);
//            else
//                iter++;
//        }
        transition[transitioncount-1].get_producer().clear();
        transition[transitioncount-1].get_consumer().clear();
        mapTransition.insert(make_pair(transitions[i],transitioncount-1));
    }
    for(unsigned int i=0;i<cpnet->arccount;i++)
    {
        if(cpnet->arc[i].getisp2t())
        {
            if(exist_in(places,cpnet->arc[i].getsrc()) && exist_in(transitions,cpnet->arc[i].gettgt()))
                arc[arccount++] = cpnet->arc[i];
        }
        else
        {
            if(exist_in(transitions,cpnet->arc[i].getsrc()) && exist_in(places,cpnet->arc[i].gettgt()))
                arc[arccount++] = cpnet->arc[i];
        }
    }
    set_producer_consumer();
}

void CPN::setPriTrans(vector<pair<string,short>> &prioritys) {
    for(auto pri :prioritys){
        auto pp = findP_byid(pri.first);
        for(auto consumer: pp->get_consumer()){
            if(consumer.arcType == control && !exist_in(transPri,transition[consumer.idx].getid()))
                transPri.push_back(transition[consumer.idx].getid());
        }
    }
}

void CPN::getRowRelatedPT(Row_Type row, vector<string> &Ps, vector<string> &Ts) {
    for(int i=0;i<placecount;i++){
        if(place[i].getRow() == row)
            Ps.push_back(place[i].getid());
    }
    for(int i=0;i<transitioncount;i++){
        if(transition[i].getRow() == row)
            Ts.push_back(transition[i].getid());
    }
}

string CPN::getVarRelatedP(const string &name, const Row_Type &row){
    vector<string> Ps,Ts;
    if(row != InValidRow)
        getRowRelatedPT(row,Ps,Ts);
    for(int i=0;i<v_tables.size();i++){
        string P_name = v_tables[i]->get_place_safe(name);
        if(P_name == "")
            continue;
        auto pp = findP_byid(P_name);
        if(row == InValidRow || exist_in(Ps,pp->getid()))
            return pp->getid();
    }

    throw "找不到指定变量" + name + ".";
}

bool judge_variableColorset(const string &s){
    if(endswith(s,global_suffix) || endswith(s,local_suffix))
        return true;
    return false;
}

bool judge_globalPostfix(const string &s){
    if(endswith(s,global_suffix))
        return true;
    return false;
}

bool judge_arrPostfix(const string &s){
    if(endswith(s,global_suffix)){
        string tmp = s.substr(0,s.length()-global_suffix.length());
        if(endswith(s,arr_suffix))
            return true;
        return false;
    }
    else {
        if(endswith(s,arr_suffix))
            return true;
        return false;
    }
}


void CPN::insert_PDNet(ifstream &in, bool fromFile){

    initDecl();

    set<string> Ps,Ts;
    int p_number, t_number, a_number;

    if(fromFile)
        in>>p_number;
    else {
        cout << "请输入库所数:";
        cin >> p_number;
        cout << "库所信息格式: 名称 colorset 初始token数" << endl;
//        cout << "colorset格式: 控制库所:string，变量库所:基本类型(_var|_arr)[_global] int_var_global表示int型非数组全局变量,float_arr表示float型数组局部变量"<<endl;
        cout << "例如: P0 String 1" << endl;

    }
    for(int i=0;i<p_number;i++){
        string name, colorset;
        int initTokenNumber;
        bool iscontrol = false, isglobal = false, isarr = false;
        if(!fromFile){
            cout << "请输入第" << i+1 << "个库所的信息：" << endl;
            cin >> name >> colorset >> initTokenNumber;
        }
        else
            in>>name>>colorset>>initTokenNumber;
        if(Ps.find(name)!=Ps.end())
            throw "库所" + name + "发生重复!";
        else
            Ps.insert(name);
        if(!judge_variableColorset(colorset))
            iscontrol = true;
        else {
            if (judge_globalPostfix(colorset))
                isglobal = true;
            if (judge_arrPostfix(colorset))
                isarr = true;
        }
        Add_Place(name,colorset,iscontrol,SizeList(),"",isglobal,false,NoneRow);

        if(!fromFile && initTokenNumber>0)
            cout<< "请输入token信息\n例如:1`(1,0,'main').注意和colorset格式一致，string类型用'str'表示";
        for(int j=0;j<initTokenNumber;j++){
            string tmp_token;
            if(!fromFile) {
                cout << "第" << j+1 << "个初始token:";
                cin >> tmp_token;
            }
            else
                in>>tmp_token;
            condition_tree ct;
            MultiSet ms;
            auto msi = sorttable.find_typename(colorset);
            ct.construct(tmp_token);
            CT2MS(ct,ms,msi.tid,msi.sid);
            Add_Place_MS(name,ms);
        }
//        cpnet->Add_Place(name,colorset)

    }
    if(!fromFile) {
        cout << "请输入变迁数:";
        cin>>t_number;
        cout << "变迁信息格式: 名称 guard(无guard则输入NULL)"<<endl;
        cout << "例如: T0 NULL" << endl;

    }
    else
        in>>t_number;
    for(int i=0;i<t_number;i++){
        string name, guard;
        if(!fromFile) {
            cout << "请输入第" << i+1 << "个变迁的信息：" << endl;
            cin >> name >> guard;
        }
        else
            in>>name>>guard;
        if(Ts.find(name)!=Ts.end() || Ps.find(name)!=Ps.end())
            throw "变迁" + name + "发生重复!";
        else
            Ts.insert(name);
        if(guard == "NULL")
            guard = "";
        Add_Transition(name,guard,"",NoneRow);
    }

    if(!fromFile) {
        cout << "请输入弧数:";
        cin >> a_number;
        cout << "弧信息格式: 起点 终点 是否是控制弧 弧表达式"<<endl;
        cout << "例如: P1 T2 true 1`tid" << endl;
    }
    else
        in>>a_number;
    for(int i=0;i<a_number;i++){
        string source, target, isSourceP, isControlArc, arcExpression;
        if(!fromFile) {
            cout << "请输入第" << i + 1 << "个弧的信息：" << endl;
            cin >> source >> target >> isControlArc >> arcExpression;
        }
        else
            in>>source>>target>>isControlArc>>arcExpression;
        bool sourceP;
        if(Ps.find(source) != Ps.end() && Ts.find(target) != Ts.end())
            sourceP = true;
        else if(Ps.find(target) != Ps.end() && Ts.find(source) != Ts.end())
            sourceP = false;
        else
            throw "弧"+ source + " to " + target + "找不到库所或变迁的定义!";

        Arc_Type arcType = isControlArc == "true"?control:readArc;
        Add_Arc(source,target,arcExpression,sourceP,arcType,normal);
    }
    set_producer_consumer();
}

void CPN::deletePreExecuteArc(string originP) {
    auto origin_pp = findP_byid(originP);

    auto origin_enter_T = origin_pp->get_enter();

    vector<string> execute_Ps;

    auto origin_enter_tt = findT_byid(origin_enter_T[0]);
    auto origin_enter_producer = origin_enter_tt->get_producer();
    for(int i=0;i<origin_enter_producer.size();i++){
        if(origin_enter_producer[i].arcType == control){
            ///注意！执行库所的出弧设置是控制弧
            auto pre_pp = findP_byindex(origin_enter_producer[i].idx);
            if(pre_pp->getisexecuted())
                execute_Ps.push_back(pre_pp->getid());
        }
    }

    for(int i=0;i<execute_Ps.size();i++){
        if(origin_enter_T.size() == 2){
            change_deleteArc(execute_Ps[i],origin_enter_T[0]);
            change_deleteArc(execute_Ps[i],origin_enter_T[1]);
        }
        else{
            change_deleteArc(execute_Ps[i],origin_enter_T[0]);
        }
    }
}

void CPN::modifyPreExecuteArc(string originP, string newP) {
    auto origin_pp = findP_byid(originP);
    auto new_pp = findP_byid(newP);

    auto origin_enter_T = origin_pp->get_enter();
    auto new_enter_T = new_pp->get_enter();

    vector<string> execute_Ps;

    auto origin_enter_tt = findT_byid(origin_enter_T[0]);
    auto origin_enter_producer = origin_enter_tt->get_producer();
    for(int i=0;i<origin_enter_producer.size();i++){
        if(origin_enter_producer[i].arcType == control){
            ///注意！执行库所的出弧设置是控制弧
            auto pre_pp = findP_byindex(origin_enter_producer[i].idx);
            if(pre_pp->getisexecuted())
                execute_Ps.push_back(pre_pp->getid());
        }
    }

    for(int i=0;i<execute_Ps.size();i++){
        if(origin_enter_T.size() == 2){
            change_deleteArc(execute_Ps[i],origin_enter_T[0]);
            change_deleteArc(execute_Ps[i],origin_enter_T[1]);
        }
        else{
            change_deleteArc(execute_Ps[i],origin_enter_T[0]);
        }
        if (new_enter_T.size() == 2) {
            change_addExecutedArc(execute_Ps[i], new_enter_T[0], true);
            change_addExecutedArc(execute_Ps[i], new_enter_T[1], true);
        } else {
            change_addExecutedArc(execute_Ps[i], new_enter_T[0], true);
        }
    }
}

void CPN::modifyAfterExecuteArc(string originP, string newP) {
    auto origin_pp = findP_byid(originP);
    auto new_pp = findP_byid(newP);

    auto origin_exit_T = origin_pp->get_enter();
    auto new_exit_T = new_pp->get_enter();

    vector<string> execute_Ps;

    auto origin_exit_tt = findT_byid(origin_exit_T[0]);
    auto origin_exit_consumer = origin_exit_tt->get_consumer();
    for(int i=0;i<origin_exit_consumer.size();i++){
        if(origin_exit_consumer[i].arcType == executed){
            auto after_pp = findP_byindex(origin_exit_consumer[i].idx);
//            if(pre_pp->getisexecuted())
                execute_Ps.push_back(after_pp->getid());
        }
    }

    for(int i=0;i<execute_Ps.size();i++){
        for(int j=0;j<origin_exit_T.size();j++){
            change_deleteArc(origin_exit_T[j],execute_Ps[i]);
        }
        for(int j=0;j<new_exit_T.size();j++){
            change_addExecutedArc(new_exit_T[j],execute_Ps[i],false);
        }
    }
}


void CPN::change_deleteControlArc(string p_name) {
    auto pp = findP_byid(p_name);
    auto producer = pp->get_producer();
    for(int i=0;i<producer.size();i++){
        if(producer[i].arcType == control) {
            auto tt = findT_byindex(producer[i].idx);
            change_deleteArc(tt->getid(), pp->getid() );
        }
    }
}

void CPN::change_addExecutedArc(string src, string tgt, bool sourceP) {
    if(sourceP){
        Add_Arc(src,tgt,controlflowArcExp(tid_str),sourceP,control,normal);
    }
    else{
        Add_Arc(src,tgt,controlflowArcExp(tid_str),sourceP,executed,normal);
    }
}

void CPN::change_deleteArc(string src, string tgt) {
    auto aa = findArc_bysrctgt(src,tgt);
    aa->setdeleted();
}

void CPN::change_addControlArc(string src, string tgt) {
    Add_Arc(src,tgt,controlflowArcExp(tid_str),false,control,normal);


}
