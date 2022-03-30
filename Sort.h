//
// Created on 2021/5/6.
//

#ifndef PDNET2_0_SORT_H
#define PDNET2_0_SORT_H


#include <string>
#include <memory>
#include "v_table.h"

using namespace std;

typedef unsigned short SORTNUM_t;

typedef unsigned short SORTID;
typedef int Integer_t;
typedef double Real_t;
typedef string String_t;
class SortValue;
typedef vector<shared_ptr<SortValue>> Product_t;
//typedef vector<SortValue*> Product_t;
//use unique_ptr to avoid memory leak, second stands for sortid

class SortTable;
extern SortTable sorttable;

enum type{dot,productsort,Integer,Real,String};

/*===========================================================*/
typedef struct mapsort_info
{
    //its index in corresponding vector
    SORTID sid;
    type tid;
    mapsort_info(){}
    mapsort_info(type tid,SORTID sid):tid(tid),sid(sid){}
} MSI;

struct MemberInfo{
    string membername;
    SizeList msl;
    MemberInfo(string name,SizeList sizeList):membername(name),msl(sizeList){}
};

class ProductSort
{
private:
    string id;
//    SORTNUM_t sortnum;
    vector<string> sortname;
    vector<MemberInfo> memberInfos; //used for 'struct,union' in program, mapping member name to member size
    vector<MSI> sortid;
    bool hastid;
    bool hasindex;
public:
    ProductSort(){/*hastid=hasindex=false;*/}
    ProductSort(string id,vector<string> sortname,vector<MemberInfo> memberinfo,vector<MSI> sortid,bool hasindex,bool hastid):id(id),
    sortname(sortname),memberInfos(memberinfo),sortid(sortid),hasindex(hasindex),hastid(hastid){if(sortname.size() != sortid.size()) throw "sortname and sortid unmatched!";}
    vector<string> get_sortname(){return sortname;}
    vector<MemberInfo> get_memberinfo(){return memberInfos;}
    vector<MSI> get_sortid(){return sortid;}
    SORTNUM_t get_sortnum(){return sortid.size();}
    bool get_hastid(){return hastid;}
    bool get_hasindex(){return hasindex;}
};

class SortTable
{
private:
    vector<ProductSort> productsorts;
    map<string,MSI> mapSort;
    SORTID psptr;

public:
    SortTable(){psptr = 0;init_buildInSort();}
    //first string is the sortname,the second MSI is this sort's information;
    MSI find_typename(string Type_name){
        auto iter = mapSort.find(Type_name);
        if(iter  == mapSort.end())
            throw "can't find typename:" + Type_name;
        return iter->second;
    }
    const ProductSort& find_productsort(SORTID sid){return productsorts[sid];}
    void init_buildInSort(){
        MSI msi;
        msi.sid = 0;
        msi.tid = Integer;
        mapSort.emplace("Integer",msi);
        msi.tid = Real;
        mapSort.emplace("Real",msi);
        msi.tid = String;
        mapSort.emplace("String",msi);
    }
    void Add_productsort(string id,vector<string> _sortname,vector<MemberInfo> _memberinfo,vector<MSI> _msi,bool _hasindex,bool _hastid){
        productsorts.emplace_back(id,_sortname,_memberinfo,_msi,_hasindex,_hastid);
        MSI msi;
        msi.tid = productsort;
        msi.sid = psptr;
        mapSort.emplace(id,msi);
        psptr++;
    }
    void clear(){productsorts.clear();mapSort.clear();psptr=0;}
//    vector<string> get_sortname(SORTID sid){return productsorts[sid].get_sortname();}
//    vector<MemberInfo> get_memberinfo(SORTID sid){return productsorts[sid].get_memberinfo();}
//    vector<MSI> get_sortid(SORTID sid){return productsorts[sid].get_sortid();}
//    SORTNUM_t get_sortnum(SORTID sid){return productsorts[sid].get_sortnum();}
};


#endif //PDNET2_0_SORT_H
