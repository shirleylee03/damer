//
// Created on 2021/5/6.
//

#ifndef PDNET2_0_MULTISET_H
#define PDNET2_0_MULTISET_H

#include "Sort.h"
#include "expression_tree.h"
#include<unordered_map>
#define HASH_OFFSET 13

typedef short token_count_t;//it might be negative when MINUS
typedef unsigned short color_count_t;
typedef unsigned long hash_t;
class SortValue;
typedef shared_ptr<SortValue> token;
//typedef SortValue* token;

extern bool operator==(Product_t p1,Product_t p2);

//extern bool operator==(token t1,token t2);

struct Bucket{

    type tid;
    SORTID sid;
    Integer_t integer;
    Real_t real;
    String_t str;
    Product_t pro;

    size_t hash(); // hash table
    bool operator==(const Bucket &bkt);
    bool operator==(const Integer_t &val){
        if(tid == Integer && val == integer)
            return true;
        return false;
    }
    bool operator==(const Real_t &val){
        if(tid == Real && val == real)
            return true;
        return false;
    }
    bool operator==(const String_t &val){
        if(tid == String && val == str)
            return true;
        return false;
    }
    bool operator<(const Bucket &bkt);
    void printBkt();
    void getVarValue(Bucket &bkt);

};



/*==========================SortValue=========================*/
/*=====================Abstract Base Class====================*/
class SortValue
{
public:
//    virtual ~SortValue() = 0;
    virtual type getmytype() = 0;
    virtual SORTID getsid() = 0;
    virtual void setcolor(const Bucket &bucket) = 0;
    virtual void getcolor(Bucket &bucket) = 0;
};

class ProductSortValue:public SortValue
{
private:
    //this is a index;
    Product_t valueindex;
    SORTID sid;
public:
    explicit ProductSortValue(SORTID sid);
    virtual type getmytype(){return productsort;}
    virtual SORTID getsid(){return sid;}
    virtual void setcolor(const Bucket &bucket);
    virtual void getcolor(Bucket &bucket);
};

class IntegerSortValue:public SortValue
{
private:
    Integer_t value;
public:
    IntegerSortValue():value(0){}
    virtual type getmytype(){return Integer;}
    virtual SORTID getsid(){throw "IntegerSortValue doesn't have sid!";};
    virtual void setcolor(const Bucket &bucket);
    virtual void getcolor(Bucket &bucket);
};

class RealSortValue:public SortValue
{
private:
    Real_t value;
public:
    RealSortValue():value(0.0){}
    virtual type getmytype(){return Real;}
    virtual SORTID getsid(){throw "RealSortValue doesn't have sid!";}
    virtual void setcolor(const Bucket &bucket);
    virtual void getcolor(Bucket &bucket);
};

class StringSortValue:public SortValue
{
private:
    String_t value;
public:
    StringSortValue():value(""){}
    virtual type getmytype(){return String;}
    virtual SORTID getsid(){throw "StringSortValue doesn't have sid!";}
    virtual void setcolor(const Bucket &bucket);
    virtual void getcolor(Bucket &bucket);
};

class DotSortValue:public SortValue
{
private:
public:
    virtual type getmytype(){return dot;}
    virtual SORTID getsid(){throw "DotSortValue doesn't have sid!";}
    virtual void setcolor(const Bucket &bucket){}
    virtual void getcolor(Bucket &bucket){bucket.tid = dot;}
};


/*==========================Factory=========================*/
/*=====================Abstract Base Class====================*/

class Factory{
public:
    virtual token generateSortValue() = 0;
};

class IntegerFactory{
public:
    token generateSortValue(){
        token p(new IntegerSortValue);
        return p;
    }
};


class RealFactory{
public:
    token generateSortValue(){
        token p(new RealSortValue);
        return p;
    }
};

class StringFactory{
public:
    token generateSortValue(){
        token p(new StringSortValue);
        return p;
    }
};

class DotFactory{
public:
    token generateSortValue(){
        token p(new DotSortValue);
        return p;
    }
};

class ProductFactory{
public:
    token generateSortValue(SORTID sid){
        token p(new ProductSortValue(sid));
        return p;
    }
};

//class Tokens
//{
//public:
//    SortValue *color;
//    token_count_t tokencount;
//    Tokens *next;
//
//    Tokens() {color = NULL;tokencount=0;next=NULL;}
//    ~Tokens() {
//        if(color!=NULL)
//            delete color;
//    }
//    void initiate(token_count_t tc,type sort,SORTID sid);
//
//};

//**Multiset contains tokens with the same type**//

//Hasher

struct tokenHasher{
public:
    size_t operator()(const token &tk) const{
        Bucket bkt;
        tk->getcolor(bkt);
        return bkt.hash();
//        switch(bkt.tid){
//            case Integer:
//                return bkt.integer;
//            case Real:
//                return static_cast<size_t >(bkt.real);
//            case String: {
//                if(bkt.str.size() == 0)
//                    return 0;
//                return static_cast<size_t>(bkt.str[0]);
//            }
//            case productsort: {
//                bkt.pro[bkt.pro.size()-1]->getcolor(bkt);
//                return bkt.hash();
//            }
//        }
    }
};

struct tokenEqualto{
public:
    bool operator()(const token &tk1,const token &tk2) const{
        Bucket bkt1,bkt2;
        if(tk1->getmytype() != tk2->getmytype())
            return false;
        tk1->getcolor(bkt1);
        tk2->getcolor(bkt2);
        return bkt1 == bkt2;
    }
};

struct tokenLessthan{
public:
    bool operator()(const token &tk1,const token &tk2) const{
        Bucket bkt1,bkt2;
        if(tk1->getmytype() != tk2->getmytype()) {
            return tk1->getmytype() < tk2->getmytype();
        }
        tk1->getcolor(bkt1);
        tk2->getcolor(bkt2);
        return bkt1 < bkt2;
    }
};

class MultiSet
{
private:
    type tid;
    SORTID sid;
//    Tokens *tokenQ;//queue contains head node
    unordered_map<token,token_count_t,tokenHasher,tokenEqualto> mapTokens;
//    map<token,token_count_t,tokenLessthan> mapTokens;
    color_count_t color_count;
//    hash_t hash_value;
public:
    int tokennum(){int sum=0;auto iter = mapTokens.begin();while(iter!=mapTokens.end()){sum+=iter->second;}return sum;}
    MultiSet(){color_count = 0;}
    MultiSet(const MultiSet &ms){tid = ms.tid;sid = ms.sid;mapTokens = ms.mapTokens;color_count=0;}
    MultiSet(type tid,SORTID sid):tid(tid),sid(sid){}
    void Init_MS(type tid1,SORTID sid1){tid = tid1;sid = sid1;}
    bool generateFromToken(const token &tk);
//    bool generateFromStr(const string &str);
    void merge(){auto iter = mapTokens.begin();while(iter!=mapTokens.end()){if(iter->second!=0)iter++;else iter = mapTokens.erase(iter);}}

    const unordered_map<token,token_count_t,tokenHasher,tokenEqualto> &getmapTokens(){return mapTokens;}
//    const map<token,token_count_t,tokenLessthan> &getmapTokens(){return mapTokens;}
    bool operator>=(const MultiSet &ms);
    bool operator==(const MultiSet &ms);
    void operator=(const MultiSet &ms);
    bool MINUS(const MultiSet &ms);
    bool PLUS(const MultiSet &ms);
    void setTokenCount(TokenNum_t count){if(mapTokens.size()!=1) throw "only token can setTokenCount!"; mapTokens.begin()->second = count;}
    color_count_t getcolorcount() const{return color_count;}
    type gettid()const{return tid;}
    SORTID getsid()const{return sid;}
    void settype(type _tid,SORTID _sid){tid = _tid;sid = _sid;}
    token getonlytoken(){if(color_count != 1) throw "getonlytoken must have one token";return mapTokens.begin()->first;}
    void clear(){mapTokens.clear();color_count=0;};
    hash_t Hash();

};


//extern IntegerFactory fint;
//extern RealFactory freal;
//extern StringFactory fstr;
//extern ProductFactory fpro;
//extern DotFactory fdot;

#endif //PDNET2_0_MULTISET_H
