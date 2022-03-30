//
// Created on 2020/10/10.
//

#include<sstream>
#include<iostream>
#include<fstream>
#include<regex>
#include<string>
#include"base.h"
#include"rowTrans.h"
#define TYPE_NUM 11
#define GCC "gcc"
using namespace std;

string dec_type[TYPE_NUM]={"int","short","long","float","double","char","unsigned","signed","static","struct","pthread_t"};
int switchcount=0;

//pre-process
static string init_num = "0";

//typedef map<unsigned short,unsigned short> RowMap;

extern int string_replace(string &s1, const string &s2, const string &s3);

int extractRowNum(const string s, int pos){
    int tmp_pos = pos - 1;
    int rowCount = 1;
    while(tmp_pos>=0){
        if(s[tmp_pos] == '\n')
            rowCount++;
        tmp_pos--;
    }
    return rowCount;
}

int getRowCount(const string& s){
    int rowCount = 0;
    for(int i=0;i<s.length();i++){
        if(s[i] == '\n')
            rowCount++;
    }
    return rowCount;
}

/// 行号映射的累乘
/// \param map1 先发生的行号映射
/// \param map2 后发生的行号映射
void rowMapMultiple(RowMap &originRowMap, RowMap tmpRowMap){
    auto iter = originRowMap.begin();
    while(iter != originRowMap.end()){
        auto iter1 = tmpRowMap.find(iter->second);
        if(iter1 == tmpRowMap.end())
            throw "error in rowMapMultiple";
        iter->second = iter1->second;
        iter++;
    }
}

/// 在原始行号映射基础上施加一个偏移
/// \param originMap 原始行号映射
/// \param rowStart 开始变化的行号
/// \param rowOffset 变化的偏移量

void rowMapOffset(RowMap &originMap, int rowStart, int rowOffset){
    RowMap rowMap;
    int count = 1;
    auto iter = originMap.begin();
    while(iter != originMap.end()){
        if(count < rowStart)
            ;
        else
            iter->second += rowOffset;
        iter++;
        count++;
    }
}

void initRowMap(RowMap &rowMap, int rowCount){
    for(int i=1;i<=rowCount;i++){
        rowMap.emplace(i,i);
    }
}

bool for_to_while(string s, string &oldtext, string &newtext, RowMap &rowMap)//true代表搜索到，false代表没搜索到，rowMap代表行号映射
{
    int rowCount = getRowCount(s);
    initRowMap(rowMap,rowCount);
    regex pattern("for *\\((.*?);(.*?);(.*)\\)");
    smatch result;

    int position;
    int sum_num = 0, sum = 0,length;
    string text, res1, res2, res3;
    //string newtext, oldtext;

    if (regex_search(s, result, pattern)) {
        position = result.position();
        res1 = result[1];
        res2 = result[2];
        res3 = result[3];
        length = result.length();
    }
    else
        return false;

//    int tmp_pos = position;
//    int forRow = 0;
//    while(tmp_pos>=0){
//        if(s[tmp_pos] == '\n')
//            forRow++;
//        tmp_pos--;
//    }

    int forRow = extractRowNum(s,position);

    newtext = "{" + res1 + ';' + "\nwhile(" + res2 + ')';
    for (unsigned int i = position + length; i < s.length(); i++)
    {
        if (s[i] == '{')
            sum_num++;
        else if (s[i] == '}')
        {
            sum_num--;
            if (sum_num == 0) {
                text = text + s[i];
                break;
            }
        }
        else if(s[i] == ';' && sum_num == 0){
            text = text + s[i];
            break;
        }
        text = text + s[i];
        sum++;
    }
    int afterContentRow = extractRowNum(s,position+length+sum) + 1;
    //cout << text << endl;
    //cout << sum;
    text = "{" + text + "\n" + res3 + ";}}";
    newtext = newtext + text;
    oldtext = s.substr(position, sum + 1 + length);

    rowMapOffset(rowMap, forRow, 1);
    rowMapOffset(rowMap,afterContentRow,1);

    return true;
}

void for_to_while_all(string &s, RowMap &originRowMap)
{
//    int rowCount = getRowCount(s);
    string oldtext, newtext;

    while (1)
    {
        RowMap tmpRowMap;
        if (!for_to_while(s, oldtext, newtext, tmpRowMap))
            break;
        string_replace(s, oldtext, newtext);
        rowMapMultiple(originRowMap,tmpRowMap);
    }
}

bool do_while_to_while(string &s, RowMap& rowMap)//true代表搜索到，false代表没搜索到
{
    int rowCount = getRowCount(s);
    initRowMap(rowMap,rowCount);
    regex pattern("[^W_0-9]do([^W_0-9].*[^W_0-9])while[^W_0-9].*\\((.*)\\)( |\\n)*;");
    smatch result;

    int position;
    int sum_num = 0, sum = 0,length;
    string text, res1, res2, res3;
    //string newtext, oldtext;

    if (regex_search(s, result, pattern)) {
        position = result.position();
        res1 = result[1];
        res2 = result[2];
//        res3 = result[3];
        length = result.length();
    }
    else
        return false;

    //cout << text << endl;
    //cout << sum;
    string newText = "while(1){" + res1 + "\nif(!(" + res2 + "))break;}";
    int afterRow = extractRowNum(s,position + result.length()) + 1;//这里+1表示下一行开始行号变化
    rowMapOffset(rowMap,afterRow,1);
//    s = s.insert(position+length,res1);
    s = s.replace(position,result.length(),newText);
    return true;
}

void do_while_to_while_all(string &s, RowMap& originRowMap){
    string oldtext, newtext;
    while (1)
    {
        RowMap tmpRowMap;
        if (!do_while_to_while(s, tmpRowMap))
            break;
        rowMapMultiple(originRowMap,tmpRowMap);
//        string_replace(s, oldtext, newtext);
    }
}

bool trans_plusplus(string s, string &newtext)//true代表搜索到，false代表没搜索到，这里s是引用
{
    regex pattern("\\+\\+");
    smatch result;

    int position, insert_position;
    int sum_num = 0, sum = 0;
    string text, res1, res2, res3;
    bool following_plus;
    //string newtext, oldtext;

    if (regex_search(s, result, pattern)) {
        position = result.position();
    }
    else
        return false;

    newtext = "";
    if (isalpha(s[position + 2]) || s[position + 2] == '_')
    {
        following_plus = false;
        for (unsigned int i = position + 2; i < s.length(); i++)
        {
            if (s[i] == '_' || isalpha(s[i])||isdigit(s[i]))
                text = s[i] + text;
            else
                break;
            sum++;
        }
    }
    else
    {
        following_plus = true;
        for (unsigned int i = position - 1; i > 0; i--)
        {
            if (s[i] == '_' || isalpha(s[i]) || isdigit(s[i]))
                text = s[i] + text;
            else
                break;
            sum++;
        }
    }
    //cout << text << endl;
    //cout << sum;
    text = text + "=" + text + "+1;\n";
    if (!following_plus)
    {
        for (unsigned int i = position; i >= 0; i--)
            if (s[i] == ';' || s[i] == '}')
            {
                insert_position = i + 1;
                break;
            }
        newtext = s.substr(0, insert_position) + text + s.substr(insert_position, position - insert_position) + s.substr(position + 2);
    }
    else
    {
        for (unsigned int i = position; i < s.length(); i++)
            if (s[i] == ';')
            {
                insert_position = i + 1;
                break;
            }
        newtext = s.substr(0, position) + s.substr(position + 2, insert_position - position - 2) + text + s.substr(insert_position);
    }

    return true;
}

void trans_plusplus_all(string &s)
{
    string newtext;
    while (1)
    {
        if (!trans_plusplus(s, newtext))
            break;
        s = newtext;
    }
}

bool trans_subsub(string s, string &newtext)//true代表搜索到，false代表没搜索到，这里s是引用
{
    regex pattern("\\-\\-");
    smatch result;

    int position, insert_position;
    int sum_num = 0, sum = 0;
    string text, res1, res2, res3;
    bool following_plus;
    //string newtext, oldtext;

    if (regex_search(s, result, pattern)) {
        position = result.position();
    }
    else
        return false;

    newtext = "";
    if (isalpha(s[position - 1]) || s[position - 1] == '_')
    {
        following_plus = true;
        for (unsigned int i = position - 1; i > 0; i--)
        {
            if (s[i] == '_' || isalpha(s[i]))
                text = s[i] + text;
            else
                break;
            sum++;
        }
    }
    else
    {
        following_plus = false;
        for (unsigned int i = position + 2; i < s.length(); i++)
        {
            if (s[i] == '_' || isalpha(s[i]))
                text = s[i] + text;
            else
                break;
            sum++;
        }
    }
    //cout << text << endl;
    //cout << sum;
    text = text + "=" + text + "-1;\n";
    if (!following_plus)
    {
        for (unsigned int i = position; i >= 0; i--)
            if (s[i] == ';' || s[i] == '}')
            {
                insert_position = i + 1;
                break;
            }
        newtext = s.substr(0, insert_position) + text + s.substr(insert_position, position - insert_position) + s.substr(position + 2);
    }
    else
    {
        for (unsigned int i = position; i < s.length(); i++)
            if (s[i] == ';')
            {
                insert_position = i + 1;
                break;
            }
        newtext = s.substr(0, position) + s.substr(position + 2, insert_position - position - 2) + text + s.substr(insert_position);
    }

    return true;
}

void trans_subsub_all(string &s)
{
    string newtext;
    while (1)
    {
        if (!trans_subsub(s, newtext))
            break;
        s = newtext;
    }
}

bool trans_assign(string &s)
{
    vector<string> sign_list;
    sign_list.push_back("+");
    sign_list.push_back("-");
    sign_list.push_back("*");
    sign_list.push_back("/");
    for (unsigned int i = 0; i < sign_list.size(); i++)
    {
        string sign = sign_list[i];
        string p = "([a-zA-Z_0-9]+ *)\\" + sign + "=";
        regex pattern(p);
        smatch result;

        int position;
        int sum_num = 0, sum = 0;
        string text, res1, newtext;

        if (regex_search(s, result, pattern)) {
            position = result.position();
            text = result[0];
            res1 = result[1];
        }
        else
            continue;
        newtext = "=" + res1 + sign;
        text.replace(text.find(sign + "="), 2, newtext);
        s.replace(s.find(result[0]), result[0].length(), text);
        return true;
    }
    return false;
}

void trans_assign_all(string &s)
{
    while (1)
    {
        if (!trans_assign(s))
            break;
    }
}

void trans_NULL(string &s)
{
    smatch result;
    string target = "NULL";
    string text,text1;
    regex pattern1("[^a-zA-Z_0-9]" + target + "[^a-zA-Z_0-9]");
    while (1)
    {
        if (regex_search(s, result, pattern1))
        {
            text = result[0];
            text1 = text;
            text1 = text1.replace(text1.find(target), target.length(), "0");
            s = s.replace(s.find(text), text.length(), text1);
        }
        else
            break;
    }
}


bool trans_define(string &s)
{
    regex pattern("#define[ \t]+([a-zA-Z_0-9]*)?[ \t]+(.*)?\n");
    smatch result;

    int position;
    int sum_num = 0, sum = 0;
    string text, text1, res1, res2;

    if (regex_search(s, result, pattern)) {
        position = result.position();
        res1 = result[1];
        res2 = result[2];
    }
    else
        return false;

    s = s.replace(s.find(result[0]), result[0].length(), "");

    regex pattern1("[^a-zA-Z_]" + res1 + "[^a-zA-Z_]");
    while (1)
    {
        if (regex_search(s, result, pattern1))
        {
            text = result[0];
            text1 = text;
            text1 = text1.replace(text1.find(res1), res1.length(), res2);
            s = s.replace(s.find(text), text.length(), text1);
        }
        else
            break;
    }
    return true;
}

void trans_define_all(string &s)
{
    while (1)
    {
        if (trans_define(s) == false)
            return;

    }
}

void trans_some_function(string &s)
{
//    if(s.find("VERIFIER_nondet")!=string::npos)
//        throw "nondet can't handled!";

    string err = "extern void reach_error() __attribute__ ((__noreturn__));";

    string err1 = "extern void reach_error();";
    string ass = "void __VERIFIER_assert(int cond) { if (!(cond)) { ERROR: reach_error(); } return; }";
    string ass1 =
            "void __VERIFIER_assert(int cond) {\n\
		if (!(cond)) {\n\
			ERROR: reach_error();\n\
		}\n\
		return;\n\
		}";

    string assum = "extern void __VERIFIER_assume(int expression);";

    string newassum=
            "void __VERIFIER_assume(int expression)\n\
		{\n\
			if (expression == 0)\n\
			{\n\
				reach_error();\n\
			}\n\
		}\n";

    string newerr =
            "void reach_error(){}\n";
    string newass =
            "void __VERIFIER_assert(int cond)\n\
		{\n\
			if (cond == 0)\n\
			{\n\
			reach_error();\n\
			}\n\
		}\n";

    string nondet_int = "int __VERIFIER_nondet_int();";
    string nondet_int1 = "int __VERIFIER_nondet_int(void);";
    string newnondet_int =
            "int __VERIFIER_nondet_int()\n\
		{\n\
			nondet_num_int = nondet_num_int + 1;\n\
			return nondet_num_int;\n\
		}\n";
    string nondet_short = "short __VERIFIER_nondet_short();";
    string nondet_short1 = "short __VERIFIER_nondet_short(void);";
    string newnondet_short =
            "short __VERIFIER_nondet_short()\n\
		{\n\
			nondet_num_short = nondet_num_short + 1;\n\
			return nondet_num_short;\n\
		}\n";
    string nondet_long = "long __VERIFIER_nondet_long();";
    string newnondet_long =
            "long __VERIFIER_nondet_long()\n\
		{\n\
			nondet_num_long = nondet_num_long + 1;\n\
			return nondet_num_long;\n\
		}\n";
    string nondet_double = "double __VERIFIER_nondet_double();";
    string newnondet_double =
            "double __VERIFIER_nondet_double()\n\
		{\n\
			nondet_num_double = nondet_num_double + 1;\n\
			return nondet_num_double;\n\
		}\n";
    string nondet_uint = "unsigned int __VERIFIER_nondet_uint();";
    string newnondet_uint =
            "int __VERIFIER_nondet_uint()\n\
		{\n\
			nondet_num_uint = nondet_num_uint + 1;\n\
			return nondet_num_uint;\n\
		}\n";
//    if(s.find(err)!=string::npos || s.find(err1)!=string::npos) {
//        string_replace(s, err, newerr);
//        string_replace(s, err1, newerr);
//    }
//    else{

    s = "\nvoid abort(){}\n" + s;
    s = newerr + s;
//    }
    //string_replace(s, ass, newass);
    //string_replace(s, ass1, newass);
    string_replace(s, "ERROR:", "");
    string_replace(s, assum, newassum);
    string_replace(s, nondet_int, newnondet_int);
    string_replace(s, nondet_int1, newnondet_int);
    string_replace(s, nondet_short, newnondet_short);
    string_replace(s, nondet_short1, newnondet_short);
    string_replace(s, nondet_double, newnondet_double);
    string_replace(s, nondet_uint, newnondet_uint);
    if (s.find("int nondet_num_int") == string::npos && s.find(newnondet_int) != string::npos )
        s = "int nondet_num_int=" + init_num + ";\n" + s;
    if (s.find("int nondet_num_short") == string::npos && s.find(newnondet_short) != string::npos)
        s = "int nondet_num_short=" + init_num + ";\n" + s;
    if (s.find("int nondet_num_long") == string::npos&&s.find(newnondet_long) != string::npos)
        s = "int nondet_num_long=" + init_num + ";\n" + s;
    if (s.find("int nondet_num_double") == string::npos&&s.find(newnondet_double) != string::npos)
        s = "int nondet_num_double=" + init_num + ";\n" + s;
    if (s.find("int nondet_num_uint") == string::npos&&s.find(newnondet_uint) != string::npos)
        s = "int nondet_num_uint=" + init_num + ";\n" + s;
    string pthread_library = "#include <pthread.h>";
    string stdio_library = "#include <stdio.h>";
    string assert_library = "#include <assert.h>";
    string stdlib_library = "#include <stdlib.h>";
    string string_library ="#include <string.h>";
    string_replace(s,  pthread_library, "");
    string_replace(s,  stdio_library, "");
//    if(s.find(assert_library)!=string::npos) {
//        string_replace(s, assert_library, "void assert(int expression) { if (!expression) reach_error(); return; }");
//    }
    string_replace(s,  stdlib_library, "");
    string_replace(s,  string_library, "");

    string atomic_begin = "extern void __VERIFIER_atomic_begin();";
    string atomic_begin_rp = "void __VERIFIER_atomic_begin(){}";
    string atomic_end = "extern void __VERIFIER_atomic_end();";
    string atomic_end_rp = "void __VERIFIER_atomic_end(){}";
    string abort = "extern void abort(void);";
    string abort_rp = "void abort(){}";
    string_replace(s,  atomic_begin, "");
    string_replace(s,  atomic_end, "");
    string_replace(s,  abort, abort_rp);

    string atomic_begin_call = "__VERIFIER_atomic_begin();";
    string atomic_end_call = "__VERIFIER_atomic_end();";
    string_replace(s,  atomic_begin_call, "");
    string_replace(s,  atomic_end_call, "");

    string gotoerror = "goto ERROR;";
    string_replace(s,gotoerror,"reach_error();");

}

void delete_space(string &s){
    while(auto siter = s.find(' ')){
        if(siter == string::npos)
            return;
        s.erase(siter,1);
    }
}

bool trans_switch(string &s)
{
    regex pattern("switch *\\((.*?)\\)");
    smatch result;
    int position;
    int sum_num = 0, sum = 0;
    string text, res1;

    if (regex_search(s, result, pattern)) {
        position = result.position();
        res1 = result[1];
        //res2 = result[2];
        //res3 = result[3];
    }
    else
        return false;

    for (unsigned int i = position; i < s.length(); i++)
    {
        if (sum_num > 0)
            text = text + s[i];
        if (s[i] == '{')
            sum_num++;
        else if (s[i] == '}')
        {
            sum_num--;
            text = text.substr(0, text.size() - 1);
            if (sum_num == 0)
                break;
        }

        sum++;
    }
    vector<string> label_list;
    vector<string> case_list;
    string default_label = "";

    regex pattern1("(case +)(.*?):");
    smatch result1;
    string temp_s = text;
    while (regex_search(temp_s, result1, pattern1))
    {
        string tmp,tmp1,case_str;
        case_str = result1[2];
        tmp = result1[1];
        tmp.append(case_str);
        tmp1 = tmp;

        delete_space(tmp);
        tmp += to_string(switchcount);
        text.replace(text.find(tmp1),tmp1.length(),tmp);

        label_list.push_back(tmp);
        case_list.push_back(case_str);

        temp_s = result1.suffix().str();
    }

    regex pattern2("(default *):");
    smatch result2;
    string temp_s1 = text;
    if (regex_search(temp_s1, result2, pattern2))
    {
        string tmp = result2[1],tmp1 = tmp;
        delete_space(tmp);
        tmp += to_string(switchcount);
        text.replace(text.find(tmp1),tmp1.length(),tmp);

        default_label = tmp;
    }

    string newtext;
    if (label_list.size() != 0)
        newtext = "\tif(" + res1 + "==" + case_list[0] + ")\n\t" + "goto " + label_list[0]+ ";\n";
    for (unsigned int i = 1; i < label_list.size(); i++)
        newtext += "\telse if(" + res1 + "==" + case_list[i] + ")\n\t" + "goto " + label_list[i]  + ";\n";
    if (default_label != "")
        newtext += "\telse\n\tgoto "+ default_label+ ";\n";

    newtext = "while(1){\n" + newtext + "\n" + text + "\nbreak;\n}\n";
    string oldtext = s.substr(position, sum + 1);
    string_replace(s, oldtext, newtext);
    switchcount++;
    /*cout << oldtext << endl;
    cout << newtext << endl;*/
}

void trans_switch_all(string &s)
{
    string oldtext, newtext;
    while (1)
    {
        if (!trans_switch(s))
            break;
    }
}

//** simplely judge **//
bool isnum(string s)
{
    stringstream sin(s);
    double t;
    char p;
    if(!(sin >> t))
        return false;
//    if(sin >> p)
//        return false;
//    else
    return true;
}
bool has_variable(string s)
{
    string temp = "";
    for(unsigned int i=0;i<s.length();i++) {
        if(s[i] == ' ')
            continue;
        if(s[i]!='_' && !isdigit(s[i]) && !isalpha(s[i]))
        {
            if(temp.size()>0 && !isnum(temp))
                return true;
            temp.clear();
        }
        else
            temp = temp + s[i];
    }
    if(temp.size() > 0 && !isnum(temp))
        return true;
    return false;
}

bool extract_dynamic_init(string &s,stack<string> &st)
{
    bool flag;
    int begin,end;
    string temp,right_exp;
    for(unsigned int i=0;i<s.length();i++)
    {
        if(s[i]=='=')
        {
            begin = end = i;
            while(begin >= 0 && s[begin] != ',')
                begin--;
            while(end<s.length() && s[end] != ',')
                end++;
            right_exp = s.substr(i+1,end-i-1);
            temp = s.substr(begin+1,end-begin-1);
            if(has_variable(right_exp)) {
                st.push(temp);
                s = s.substr(0,i) + s.substr(end,s.length());
                flag = true;
            }
            //cout<<"temp:"<<temp<<endl;
            //cout<<"right_exp:"<<right_exp<<endl;

        }
    }
    return flag;
}

bool judge_next_dec(string s,int position)
{
    int begin, end;

    begin = position;
    while (s[begin] == ' ' || s[begin] == '\n')
        begin++;
    end = begin;
    while (s[end] != ' ')
        end++;
    string temp = s.substr(begin, end - begin);


    for (unsigned int i = 0; i < TYPE_NUM; i++)
        if (dec_type[i] == temp)
            return true;
    return false;
}

bool judge_func_def(string s){
    //distinguish function definition and variable definition
    //we use the '=' out of () to judge
    int count=0;
    for(int i=0;i<s.length();i++){
        if(s[i] == '(')
            count++;
        else if(s[i] == ')')
            count--;
        else if(s[i] == '=' && count == 0)
            return false;
    }
    return true;
}

void trans_dynamic_init(string &s)
{
    stack<string> st;
    string str_pat = "\\W(";
    for(unsigned int i=0;i<TYPE_NUM;i++) {
        str_pat = str_pat + dec_type[i];
        if(i!=TYPE_NUM-1)
            str_pat = str_pat + "|";
    }
    str_pat = str_pat + ")(\\W.*?);";
    regex pattern(str_pat);
    smatch result;


    string text = s,res;
    string oldtext;

    while(regex_search(text, result, pattern)) {
//        cout<<"result = "<<result[2]<<endl;

        res = result[2];
        //cout<<"result[0].size:"<<result[0].length()<<endl;
        text = result.suffix();
        if(!judge_func_def(s)) {
            //cout<<"res:"<<result[0]<<endl;
            string old_res = res;
            if(extract_dynamic_init(res, st)) {
                s.replace(s.find(old_res), old_res.size(), res);
//                if (!judge_next_dec(text, 0)) {
                if(1){
                    while (!st.empty()) {
                        string top = st.top();
                        top = top + ";";
                        st.pop();
                        oldtext = text;
                        text.insert(0, top);
                        s.replace(s.find(oldtext), oldtext.size(), text);
//                    cout<<"text:"<<text<<endl;
//                    cout<<"s:"<<s<<endl;
                    }
                }
            }
        }

    }
}

struct item{
    string content;
    long pos;
    bool is_insert;
    item(string c,long p,bool i):content(c),pos(p),is_insert(i){};
    bool operator<(const item& item1){
        if(pos<item1.pos)
            return true;
        else
            return false;
    }
};

 bool operator<(const item& item1, const item& item2){
        if(item1.pos<item2.pos)
            return true;
        else
            return false;
    }

void trans_advancedef(string &s, RowMap& originRowMap){
    RowMap rowMap;
    int rowCount = getRowCount(s);
    initRowMap(rowMap,rowCount);
    
//    stack<string> insert_content_arr;
//    stack<long> insert_pos_arr;
//    stack<long> delete_pos_arr;
    vector<item> items;

    string str_pat = "\\W(";
    for(unsigned int i=0;i<TYPE_NUM;i++) {
        str_pat = str_pat + dec_type[i];
        if(i!=TYPE_NUM-1)
            str_pat = str_pat + "|";
    }
    str_pat = str_pat + ")(\\W.*?;)";
    regex pattern(str_pat);
    smatch result;

    string text=s,insert_content;
    long pos=0,offset=0,insert_pos,delete_pos;
    int count=0;

    while(regex_search(text,result,pattern)){
        pos = result.position();
        int i= offset + pos;
        insert_pos = 0;
        count = 0;

        insert_content = result[1];
        insert_content += result[2];
        insert_content = insert_content ;

        text = result.suffix();

        int find_last_word_pos = pos + offset;
        while(find_last_word_pos>=0 && (
                s[find_last_word_pos] == '\t'
                || s[find_last_word_pos] == '\n'
                || s[find_last_word_pos] == ' '))
            find_last_word_pos--;
        if(s[find_last_word_pos] != ';' && s[find_last_word_pos] != '{'){}
        else {
            while (i >= 0) {

                if (s[i] == '{') {
                    count++;
                    if (count > 0) {
                        insert_pos = i + 1;
                        break;
                    }
                } else if (s[i] == '}')
                    count--;
                i--;
            }
            if (insert_pos != 0) {

                items.emplace_back(insert_content, offset + pos + 1 , false);
                items.emplace_back(insert_content, insert_pos, true);

            }
        }
        offset += pos + result.length();
    }

    //It must be placed before sort, or the order will be out of order
    for(int i=0;i<items.size();i+=2){
        int originRow = extractRowNum(s,items[i].pos);
        int afterRow = extractRowNum(s,items[i+1].pos);
        auto iter = rowMap.find(originRow);
        iter->second = afterRow;
    }
    rowMapMultiple(originRowMap,rowMap);

    sort(items.begin(),items.end());

    for(int i=items.size()-1;i>=0;i--){
        if(!items[i].is_insert){
            s.replace(items[i].pos,items[i].content.length(),"");
        }
        else{
            s.insert(items[i].pos,items[i].content);
        }
    }

}

void trans_annotation(string &s){
    string an_patten = "//.*?\n";

    regex pattern(an_patten);
    smatch result;
    string col;

    while(regex_search(s,result,pattern)){
        col = result[0];
        auto pos = result.position();
        s = s.replace(pos,col.length(),"");
    }
}

void trans_atomic(string &s, RowMap &originRowMap)
{
    RowMap rowMap;
    int rowCount = getRowCount(s);
    initRowMap(rowMap,rowCount);
    map<int,string> insert_map;
    string str_pat = "void __VERIFIER_atomic(.*)\\(.*\\)( |\\n)*?\\{";
    regex pattern(str_pat);
    smatch result;


    string text = s,res,mutex_name;
    string oldtext;
    int pos = 0,offset = 0;

    while(regex_search(text, result, pattern)) {
        pos = result.position();
//        cout<<"result = "<<result[2]<<endl;

//        bool pureDecFlag = false;//判断是否只是单纯的声明
//        int tmp_count = offset + pos + res.length();
//        while(tmp_count > 0){
//            if(s[tmp_count] == '{')
//                break;
//            if(s[tmp_count] == ';'){
//                pureDecFlag = true;
//                break;
//            }
//            tmp_count--;
//        }
        res = result[0];
        mutex_name = result[1];

        int end_point = offset+pos+res.length();
        int count = 0;

        while(count>=0){
            if(s[end_point] == '{')
                count++;
            else if(s[end_point] == '}')
                count--;
            end_point++;
        }

//        cout<<res<<endl;
        //cout<<"result[0].size:"<<result[0].length()<<endl;
        text = result.suffix();
        if(isdigit(mutex_name[0]))
            throw "ERROR!atomic function's name should be checked!";
//        if(!pureDecFlag) {
        insert_map.emplace(offset + pos + res.length(), mutex_name);
        insert_map.emplace(end_point - 1, mutex_name);
//        }
        offset += pos + result.length();
    }

    if(insert_map.size() != 0) {
        //rowMap必须先计算，否则s将会变化
        auto iter = insert_map.begin();
        while(iter != insert_map.end()){
            int contentRow = extractRowNum(s,iter->first) + 1;//这里+1和之后pthread_mutex_lock之前的\n对应
            rowMapOffset(rowMap,contentRow,1);
            iter++;
            int afterRow = extractRowNum(s,iter->first);
            rowMapOffset(rowMap,afterRow,1);
            iter++;
        }
        rowMapOffset(rowMap,1,1);
        rowMapMultiple(originRowMap,rowMap);

        string atomic_lock = "atomic_lock";
        auto riter = insert_map.rbegin();
        while (riter != insert_map.rend()) {
            s.insert(riter->first, "pthread_mutex_unlock(&" + atomic_lock + ");\n");
            riter++;
            s.insert(riter->first, "\npthread_mutex_lock(&" + atomic_lock + ");");
            riter++;
        }

        s = "pthread_mutex_t " + atomic_lock + " = PTHREAD_MUTEX_INITIALIZER;\n" + s;


    }

//    cout<<s<<endl;
}


void trans_Bool2int(string &s){
    string boolpattern = "_Bool";
    regex pattern(boolpattern);
    smatch result;

    while(regex_search(s,result,pattern)){
        auto pos = result.position();
        s = s.replace(pos,boolpattern.length(),"int");
    }
}

void Add_NULL(string &s){
    regex pattern("[^a-zA-Z_]NULL[^a-zA-Z_]");
    smatch result;
    if(regex_search(s,result,pattern)){
        s = "#define NULL 0\n" + s;
    }

}

void trans_addAbort(string &s, RowMap &rowMap){
    s = "void abort(){}\n" + s;
    s = "void reach_error(){}\n" + s;
    rowMapOffset(rowMap,1,2);
}

//void pre_process(string &s)
//{
////    Add_NULL(s);
//    trans_NULL(s);
////    trans_annotation(s); annotation is replaced by gcc
//    trans_Bool2int(s);
//    trans_assign_all(s);
////    trans_define_all(s); define is replaced by gcc
//
//
//    RowMap rowMap;
///// row change
//    for_to_while_all(s, rowMap);
//    do_while_to_while_all(s, rowMap);
////    trans_plusplus_all(s);
////    trans_subsub_all(s);
//
//    trans_atomic(s,rowMap);//atomic function add lock, place before trans_some_function
////    trans_some_function(s);
//
//    trans_switch_all(s);
//    trans_dynamic_init(s);
//    trans_advancedef(s, rowMap);
//
//    trans_addAbort(s, rowMap);
//    //cout << s << endl;
//}

void gccpreprocess(string filename){
    string cmd;
    string newfilename = filename;
    cmd = string(GCC) + " -B .. -E " + filename + " -o " + newfilename.replace(newfilename.find(".c"),2,".i");
    system(cmd.c_str());
}

RowMap getRowMap(string filename){
    gccpreprocess(filename);
    string preprocessFile = filename;
    preprocessFile = preprocessFile.replace(preprocessFile.rfind(".c"),2,".i");
    RowMap rowMap,rowMap1,rowMap2;
    ifstream fin;
    fin.open(preprocessFile);
    if(!fin.is_open()){
        throw preprocessFile + " not exist!";
    }

    int rowCount = 1;//这里rowCount用于记录行号，从1开始
    string row;
    while(getline(fin,row)) {
        if(row[0] == '#'){
            auto tmp_vec = split(row," ");
            if(rowCount>1 && tmp_vec.size()>2 && tmp_vec[2] == "\"" + filename + "\""){
                string tmp_row = tmp_vec[1];
                rowMap1.emplace(atoi(tmp_row.c_str()),rowCount + 1);
            }
        }
//        cout << row << endl;
        rowCount++;
    }
    fin.close();
    auto iter1 = rowMap1.begin();
    auto iter2 = rowMap1.begin();
    iter2++;
    while(iter2 != rowMap1.end()){
        for(int i=iter1->first;i<iter2->first;i++){
            rowMap.emplace(i,iter1->second + i - iter1->first);
        }
        iter1++,iter2++;
    }
    int originRowCount = 0;
    fin.open(filename);
    while(getline(fin,row))
        originRowCount++;
    fin.close();
    int remain = iter1->first;
    while(remain<originRowCount){
        rowMap.emplace(remain,iter1->second + remain-iter1->first);
        remain++;
    }

    return rowMap;
}

void clearLineMarker(string filename) {
    ifstream fin;
    fin.open(filename);
    if (!fin.is_open()) {
        throw filename + " not exist!";
    }
    vector<string> lines;
    string row;
    while (getline(fin, row)) {
        if (row[0] == '#')
            lines.push_back("");
        else
            lines.push_back(row);
    }
    fin.close();

    string preprocessFile = filename;
    preprocessFile = preprocessFile.replace(preprocessFile.rfind(".i"), 2, ".ii");
    ofstream fout;
    fout.open(preprocessFile, ios::out);
    if (!fout.is_open()) {
        throw preprocessFile + " not exist!";
    }
    for (int i = 0; i < lines.size(); i++) {
        fout << lines[i] << "\n";
    }
    fout.close();
}

int getFileRowCount(string filename){
    ifstream fin(filename);
    int rowCount = 0;
    string row;
    while(getline(fin,row)){
        rowCount++;
    }
    return rowCount;
}

void trans_deleteAtomicBeginEnd(string &s){
    string atomic_begin = "extern void __VERIFIER_atomic_begin();";
    string atomic_end = "extern void __VERIFIER_atomic_end();";

    string_replace(s,  atomic_begin, "");
    string_replace(s,  atomic_end, "");

    string atomic_begin_call = "__VERIFIER_atomic_begin();";
    string atomic_end_call = "__VERIFIER_atomic_end();";

    string_replace(s,  atomic_begin_call, "");
    string_replace(s,  atomic_end_call, "");

    string_replace(s, "ERROR:", "");
}

RowMap getRowMap1(string filename) {
    RowMap rowMap;


    string iiFile = filename, iiiFile = filename;
    iiFile = iiFile.replace(iiFile.rfind(".c"), 2, ".ii");
    iiiFile = iiiFile.replace(iiiFile.rfind(".c"), 2, ".iii");

    int rowCount = getFileRowCount(iiFile);
    initRowMap(rowMap,rowCount);

    ifstream fin;
    fin.open(iiFile, ios::in);
    if (!fin.is_open()) {
        cout << iiFile << " not exist" << endl;
        exit(-1);
    }
    string temp_s;
    istreambuf_iterator<char> beg(fin), end;
    temp_s = string(beg, end);
    fin.close();

    trans_NULL(temp_s);
    trans_Bool2int(temp_s);
    trans_assign_all(temp_s);

/// row change

    for_to_while_all(temp_s, rowMap);
    do_while_to_while_all(temp_s, rowMap);
    trans_atomic(temp_s,rowMap);
    trans_dynamic_init(temp_s);
    trans_advancedef(temp_s,rowMap);
    trans_addAbort(temp_s, rowMap);
    trans_deleteAtomicBeginEnd(temp_s);


    ofstream fout;
    fout.open(iiiFile, ios::out);
    fout << temp_s;
    fout.close();

    return rowMap;
}

void clearAdditionFiles(string filename){
    string cmd;
    string filePrefix = filename.replace(filename.find(".c"),2,"");
    cmd = "rm " + filename + ".i " + "&" + "rm " + filename + ".ii";
    system(cmd.c_str());
}

RowMap preProcessGetRowMap(string filename){

    string iFile = filename;
    iFile = iFile.replace(iFile.rfind(".c"),2,".i");

    auto rowMap = getRowMap(filename);
    clearLineMarker(iFile);
    auto rowMap1 = getRowMap1(filename);
    rowMapMultiple(rowMap,rowMap1);
    clearAdditionFiles(filename);
    return rowMap;
}