
#ifndef CPN_PNML_PARSE_V_TABLE_H
#define CPN_PNML_PARSE_V_TABLE_H


#pragma once
#include<string>
#include<map>
#include<iostream>
#include<vector>
#include<list>
#include "AST.h"

#define global_table_name "global"

//**** SizeList
//store size for array variables
//for example: an array variable's definition: int a[5][10]
//             matched SizeList will be: {5,10}
struct SizeList{
private:
    std::vector<short> sizes;
    //if there is an array a[5][10][20]
    //sizes = {5,10,20}
public:

    int getarrsize(int pos){
        if(pos>= sizes.size())
            throw "Error in getarrsize!";
        int res = 1;
        for(int i=pos+1;i<sizes.size();i++)
            res *= sizes[i];
        return res;
    }
    void Add_size(int s){
       sizes.push_back(s);
    }
    void Set_sizes(std::vector<short> s){
        sizes = s;
    }
    void next_size() {
        if (sizes.size() == 0)
            throw "error in next_size  v_table.h!";
//        std::vector<int> tmp(sizes);
//        tmp.pop_back();
        sizes.erase(sizes.begin());
    }
    int totalsize() const{
        if(sizes.size() == 0)
            return 1;
        int s = 1;
        for(int i=0;i<sizes.size();i++){
            s *= sizes[i];
        }
        return s;
    }
    void nonsize(){}
    bool empty() const{return sizes.size()==0;}
    int getdim() const{return sizes.size();}
    std::vector<short> getsizes() const{
        return sizes;
    }
    SizeList& operator=(const SizeList &sl){
        sizes = sl.sizes;
        return *this;
    }
};

struct Item{
//  variable table item
    std::string place;
    string tag;
    string base_tag;//for pointer
    SizeList sizelist;
    bool is_pointer;
};

struct Item1{
//  function table item
    std::string begin_place;
    std::string end_place;
    std::string returntag;
    std::vector<std::pair<std::string,gtree *>> return_infos;//store return T and return exp_tree
    std::vector<std::pair<std::string,std::string>> para_list;//store the identifier and P_id of para
};

class F_Table{
private:
    std::map<std::string,Item1> f_map;
public:
    void insert(std::string func,std::string begin_P,std::string end_P,std::string returntag) {
        Item1 item1;
        item1.begin_place = begin_P;
        item1.end_place = end_P;
        item1.returntag = returntag;
//        item1.para_list = paras;
        f_map.emplace(func,item1);
    }
    void insert_para(std::string func,std::string para,std::string para_P){
        auto fiter = f_map.find(func);
        if(fiter == f_map.end())
            throw "can't find func:" + func;
        fiter->second.para_list.emplace_back(para,para_P);
    }
    void insert_returns(std::string func,std::string return_T,gtree *exp_tree){
        auto fiter = f_map.find(func);
        if(fiter == f_map.end())
            throw "can't find func:" + func;
        fiter->second.return_infos.emplace_back(return_T,exp_tree);
    }
    bool findfunc(std::string func){return !(f_map.find(func) == f_map.end());}
    std::string findbeginplace(std::string func){
        if(!findfunc(func))
            throw "can't find func" + func;
        return f_map.find(func)->second.begin_place;
    }
    std::string findendplace(std::string func){
        if(!findfunc(func))
            throw "can't find func" + func;
        return f_map.find(func)->second.end_place;
    }
    std::string findfuncreturntag(std::string func){
        if(!findfunc(func))
            throw "can't find func" + func;
        return f_map.find(func)->second.returntag;
    }
    std::vector<std::pair<std::string,std::string>> findfuncparas(std::string func){
        if(!findfunc(func))
            throw "can't find func" + func;
        return f_map.find(func)->second.para_list;
    }
    std::vector<std::pair<std::string,gtree *>> findfuncreturns(std::string func){
        if(!findfunc(func))
            throw "can't find func" + func;
        return f_map.find(func)->second.return_infos;
    }
};

class V_Table
{
private:
    std::string name;

    std::map<std::string,Item> v_map;

    V_Table *fa;

public:
    V_Table(std::string na) { name = na; fa = NULL;}
    V_Table(){fa = NULL;}
    void clear();
    void insert(std::string v, std::string place,string tag,string basetag,SizeList sizelist,bool ispointer);
//    void Add_pointerrelated(std::string v,std::string place);
//    void erase(std::string v) {
//        v_map.erase(v);
//        v_sizemap.erase(v);
//        v_ispointer.erase(v);
//    }
    std::string get_name(){return name;}
    bool find_P(std::string s);
    void connect(V_Table *&father) { fa = father;}
    std::string get_place(std::string v);
    std::string get_place_safe(std::string v);
    SizeList get_size(std::string v);
    bool get_ispointer(std::string v);
    bool get_isglobal(std::string v);
    string get_tag(std::string v);
    string get_basetag(std::string v);
    std::map<std::string,Item> get_v_map(){return v_map;};

//    std::vector<std::string> get_pointerrelated(std::string v);
};

//extern std::vector<V_Table *> v_tables;//variable tables

#endif //CPN_PNML_PARSE_V_TABLE_H