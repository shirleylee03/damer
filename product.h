//
// Created on 2020/10/10.
//
#include "cpn_rg.h"
//#include "base.h"
#include "BA/buchi.h"
#define CHASHSIZE 1048576  //2^20
#define UNREACHABLE 0xffffffff
#define MAXINT 0x7fffffff
#define MAXDOUBLE 0xffffffffffffffff
#define ERROR 0
#define OK 1

enum LTLCategory{LTLC,LTLF,LTLV};

typedef struct bucket {
    Integer_t icolor=MAXINT;
    Real_t rcolor=MAXDOUBLE;
    String_t scolor="";

    bool operator == (const bucket &b2) {
        if(this->icolor!=MAXINT && b2.icolor!=MAXINT) {
            return (this->icolor == b2.icolor);
        }
        if(this->rcolor!=MAXDOUBLE && b2.rcolor!=MAXDOUBLE) {
            return (this->rcolor == b2.rcolor);
        }
        if(this->scolor!="" && b2.scolor!="") {
            return (this->scolor == b2.scolor);
        }
        cerr<<"color type match error"<<endl;
        exit(0);
    }
    bool operator < (const bucket &b2) {
        if(this->icolor!=MAXINT && b2.icolor!=MAXINT) {
            return (this->icolor<b2.icolor);
        }
        if(this->rcolor!=MAXDOUBLE && b2.rcolor!=MAXDOUBLE) {
            return (this->rcolor<b2.rcolor);
        }
        if(this->scolor!="" && b2.scolor!="") {
            return (this->scolor<b2.scolor);
        }
        cerr<<"color type match error"<<endl;
        exit(0);
    }
    bool operator <= (const bucket &b2) {
        if(this->icolor!=MAXINT && b2.icolor!=MAXINT) {
            return (this->icolor<=b2.icolor);
        }
        if(this->rcolor!=MAXDOUBLE && b2.rcolor!=MAXDOUBLE) {
            return (this->rcolor<=b2.rcolor);
        }
        if(this->scolor!="" && b2.scolor!="") {
            return (this->scolor<=b2.scolor);
        }
        cerr<<"color type match error"<<endl;
        exit(0);
    }
    void generateFromBucket(const Bucket& bkt){
        switch(bkt.tid){
            case Integer:
                icolor = bkt.integer;
                break;
            case Real:
                rcolor = bkt.real;
                break;
            case String:
                scolor = bkt.str;
                break;
            default:
                throw "从Bucket向bucket转换时出现问题! product.h";
        }
    }
};

class CHashtable;
typedef struct CPN_Product
{
    index_t id = 0;
    index_t lowlink = 0;
    RG_NODE *RGname_ptr = NULL;
    int BAname_id = -1;
    CPN_Product *hashnext = NULL;
    index_t stacknext;
} CPN_Product;

class CHashtable
{
public:
    CPN_Product **table;
    NUM_t nodecount;
    NUM_t hash_conflict_times;
public:
    CHashtable() {
        table = new CPN_Product*[CHASHSIZE];
        for(int i=0;i<CHASHSIZE;++i){
            table[i] = NULL;
        }
        nodecount = 0;
        hash_conflict_times = 0;
    }
    ~CHashtable() {
        for(int i=0;i<CHASHSIZE;++i) {
            if(table[i]!=NULL)
            {
                CPN_Product *p=table[i];
                CPN_Product *q;
                while(p!=NULL) {
                    q = p->hashnext;
                    delete p;
                    p = q;
                }
            }
        }
        delete [] table;
        MallocExtension::instance()->ReleaseFreeMemory();
    }
    index_t hashfunction(CPN_Product *node);
    void insert(CPN_Product *node);
    CPN_Product *search(CPN_Product *node);
    void remove(CPN_Product *node);
    void resetHash();
};

class CPstack
{
public:
    CPN_Product **mydata;
    index_t *hashlink;
    index_t toppoint;

    CPstack();
    ~CPstack();
    index_t hashfunction(CPN_Product *node);
    CPN_Product *top();
    CPN_Product *pop();
    CPN_Product *search(CPN_Product *node);
    int push(CPN_Product *node);
    NUM_t size();
    bool empty();
    void clear();
};

class CPN_Product_Automata
{
private:
    vector<CPN_Product> initial_status;
    CPstack cstack;
    CHashtable h;
    CStack<CPN_Product *> astack;
    StateBuchi *ba;
    RG *rg;
    CPN *cpn;
    bool result;
public:
    CPN_Product_Automata(CPN *net,StateBuchi *StateBuchi,RG *rg);
    void GetInitNode();
    void GetProduct();
    string GetResult(){return (this->result)?"true":"false";};
    void Tarjan(CPN_Product *pnode);

    void printresult(string propertyid);
    bool isLabel(RG_NODE *state, int sj);  //判断能否合成交状态
    int judgeF(string s);         //判断该公式是否为F类型的公式
    short int sumtoken(string s, RG_NODE *state);   //计算s中所有库所的token和
    bool handleLTLF(string s, RG_NODE *state);
    bool handleLTLC(string s, RG_NODE *state);
    bool handleLTLV(string s, RG_NODE *state);
    bool FetchColor(string s, RG_NODE *state, bucket &color);
    void handleLTLCstep(short int &front_sum, short int &latter_sum, string s, RG_NODE *state);
};