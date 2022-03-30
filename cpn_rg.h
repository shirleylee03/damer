//
// Created on 2021/5/29.
//

#ifndef PDNET2_0_CPN_RG_H
#define PDNET2_0_CPN_RG_H


#include "cpn.h"
#define CPNRGTABLE_SIZE 100000 // max
class Marking;
class Binding;
void Marking_after_fire(Marking &marking,CTransition *transition,Binding *bindings,CPN *cpn);
class Marking
{
public:
    MultiSet *mss;
    NUM_t placecount;

    void init_marking(CPlace *place,NUM_t pc);
    void init_marking(Marking &marking);
    bool operator==(Marking &marking);

    ~Marking(){delete[] mss; };
};

class Binding
{
public:
    string variable;
    token value;
    Binding *next;
    Binding(){value=NULL;}

};

class FireTranQ
{
public:
    CTransition *transition;
    vector<Binding*> bindings;
    FireTranQ *next;

    void freebindings(){
        for(int i=0;i<bindings.size();i++){
            Binding *tmp = bindings[i],*tmp1;
            while(tmp){
                tmp1 = tmp->next;
                delete tmp;
                tmp = tmp1;
            }
        }
    }
    ~FireTranQ(){for(unsigned int i=0;i<bindings.size();i++)delete bindings[i];}
//    void insert(CTransition *transition);
};


class RG_NODE
{
public:
    Marking marking;
    FireTranQ *tranQ;
    RG_NODE *next;
    string last_tran;
    NUM_t num;
    NUM_t fathernum;
    NUM_t firenum;
    vector<NUM_t> dashNextNode;

    bool tranQ_obtained;
    index_t Hash();
    RG_NODE(){tranQ = new FireTranQ;tranQ->next=NULL;firenum = 0;tranQ_obtained = false;next=NULL;}
    ~RG_NODE() {
        while (tranQ) {
            FireTranQ *tmp = tranQ;
            tranQ = tranQ->next;
            delete tmp;
        }
        if (next)
            delete next;
    }
    void get_FireTranQ(CPN *cpn);
    bool fireable(string transname);
};

class RG
{
public:
    RG_NODE *init_node;
    RG_NODE **rgnodetable;
    vector<RG_NODE *> rgnodevec;
    NUM_t node_num;

    void init(CPN *cpn);
    void createNode(RG_NODE *node,CPN *cpn);
    void addRGNode(RG_NODE *node);
    RG_NODE* nodeExist(RG_NODE *node);
    void GENERATE(CPN *cpn);
    void print_RG(string filename,CPN *cpn);
    RG(){node_num=0;}
    ~RG() {
//        if (init_node)
//            delete init_node;
        for (int i = 0; i < CPNRGTABLE_SIZE; i++)
            if (rgnodetable[i])
                delete rgnodetable[i];
        if (rgnodetable)
            delete[] rgnodetable;
    }
};

#endif //PDNET2_0_CPN_RG_H
