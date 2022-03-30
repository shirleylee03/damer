//
// Created on 2020/10/10.
//

#ifndef PDNET_CHECKER_CPN_H
#define PDNET_CHECKER_CPN_H

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <string.h>
#include "AST.h"
#include "y.tab.h"
#include "base.h"
#include "v_table.h"
#include "Sort.h"
#include "MultiSet.h"
//#include "condition_tree.h"
#include "rowTrans.h"
#include "expression_tree.h"
 #include <gperftools/tcmalloc.h>
 #include <gperftools/malloc_extension.h>
#define H1FACTOR 13//hash offset
#define NoneRow -1

using namespace std;

typedef unsigned int index_t;
typedef unsigned short VARID;
typedef unsigned int NUM_t;
typedef unsigned int ID_t;
typedef string TID_t;// tid thread identifier



enum Arc_Type{executed,control,call_enter,call_exit,readArc,writeArc,readwrite,call_connect,allocwrite,remain};//data is equal to read

struct postinfo{
    SizeList sl;
    bool ispointer;
    postinfo(){sl.empty();ispointer=false;}
};

class Variable
{
private:
    string id;
//    SORTID sid;
    token value;//assume variable is not a productsort
    type tid;
public:
    void getvcolor(Bucket &bkt);
    void setvcolor(const Bucket &bkt);
    void Init_Variable(string _id,type _tid);
    type gettid(){return tid;}

};

/*========================Net Element========================*/
typedef struct CPN_Small_Arc
{
    index_t idx;
    condition_tree arc_exp;
    Arc_Type arcType;
    bool is_deleted;
    CPN_Small_Arc(){}
    CPN_Small_Arc(index_t index,const condition_tree &arcexp,Arc_Type arctype){
        idx = index;
        arc_exp = arcexp;
        arcType = arctype;
        is_deleted = false;
    }
//    bool onlydot;
} CSArc;

typedef class CPN_Transition CTransition;
class CPN;

typedef class CPN_Place
{
private:
    string id;
//    type tid;
//    SORTID sid;
    MultiSet initMarking;

    bool control_P;
    bool is_executed;
    bool is_global;
    string expression;//unique mark
    unsigned short distance;

    unsigned short row;
    vector<CSArc>producer;
    vector<CSArc>consumer;

    vector<string> enter;
    vector<string> exit;
    vector<string> false_exit;
    vector<string> call_P;
    vector<string> correspond_P;
//    vector<Triple> returns;//for func begin P
//    vector<pair<string,string>> para_list;//first is identifier, second is place name
    //***PDNet added end***/
public:
    CPN_Place(type tid,SORTID sid):initMarking(tid,sid){expression="";control_P=is_executed=is_global=false;distance = static_cast<unsigned short>(-1);}
    CPN_Place(){distance = static_cast<unsigned short>(-1);row=NoneRow;}
    void operator=(CPN_Place &plc){
        //copy except producer and consumer
        id = plc.id;
        initMarking = plc.initMarking;
        expression=plc.expression;
        enter = plc.enter;
        exit = plc.exit;
        false_exit = plc.false_exit;
        call_P = plc.call_P;
        correspond_P = plc.correspond_P;
//        returns = plc.returns;
//        para_list = plc.para_list;
        is_executed = plc.is_executed;
        control_P = plc.control_P;
//        is_mutex = plc.is_mutex;
//        is_cond = plc.is_cond;
    }
    void Init_Place(string id,type tid,SORTID sid,string expression,bool iscontrolP,bool isglobal,bool isexecuted,CPN *cpn, unsigned short row);
    bool AddMultiSet(const MultiSet& ms){return initMarking.PLUS(ms);}
    bool SubMultiSet(const MultiSet& ms){ return initMarking.MINUS(ms);}

    void Add_enter(const string& s){enter.emplace_back(s);}
    void Add_exit(const string& s){exit.emplace_back(s);}
    void Add_falseexit(const string& s){false_exit.emplace_back(s);}
    void clear_enter(){enter.clear();}
    void clear_exit(){exit.clear();}
    void clear_falseexit(){false_exit.clear();}

    void Add_call_P(const string &callP){call_P.emplace_back(callP);}
    void Add_cor_P(const string &cor_P){correspond_P.emplace_back(cor_P);}
    vector<string> get_call_P(){return call_P;}
    vector<string> get_cor_P(){return correspond_P;}
    void clear_call_P(){call_P.clear();}
    void clear_cor_P(){correspond_P.clear();}

    vector<string> get_enter(){return enter;}
    vector<string> get_exit(){return exit;}
    vector<string> get_falseexit(){return false_exit;}
    void Add_producer(const CSArc& csarc){producer.emplace_back(csarc);}
    void Add_consumer(const CSArc& csarc){consumer.emplace_back(csarc);}
    vector<CSArc> &get_producer(){return producer;}
    vector<CSArc> &get_consumer(){return consumer;}
    string getExp(){return expression;}
    bool getisglobal(){return is_global;}
    bool getisexecuted(){return is_executed;}
    bool getiscontrolP(){return control_P;}
    type gettid(){return initMarking.gettid();}
    SORTID getsid(){return initMarking.getsid();}
    string getid(){return id;}
    MultiSet& getMultiSet(){return initMarking;}
    void setDistance(unsigned short dist){distance = dist;}
    unsigned short getDistance(){return distance;}
    unsigned short getRow(){return row;}

    void clear_producer(){producer.clear();}
    void clear_consumer(){consumer.clear();}
//    void setflags(bool controlP,bool isglobal,bool isexecuted){control_P = controlP;is_global = isglobal;is_executed = isexecuted;}
//    void printTokens(const string &s);
//    ~CPN_Place(){
//        delete initMarking;
//    }
} CPlace;

typedef class CPN_Transition
{
private:
    string id;
    condition_tree guard;
    bool hasguard;
//    bool isreturn;
    bool is_writepointer;
    vector<CSArc> producer;
    vector<CSArc> consumer;
    unsigned short row;
public:
    bool isabort;

    CPN_Transition(){is_writepointer=false;hasguard=false;isabort=false;row = NoneRow;}
    CPN_Transition(string _id,string guard_str){id = _id;is_writepointer=false;hasguard = true;guard.construct(guard_str);}
    void Init_Transition(string _id,string guard_str, unsigned short _row){id = _id;row=_row;if(guard_str!=""){hasguard = true;guard.construct(guard_str);}}
//    void operator=(CPN_Transition &trans){
//        //copy except produce and consumer
//        id = trans.id;
//        guard = trans.guard;
//        hasguard = trans.hasguard;
//        is_writepointer = trans.is_writepointer;
//    }
    void Add_producer(const CSArc& csarc){producer.emplace_back(csarc);}
    void Add_consumer(const CSArc& csarc){consumer.emplace_back(csarc);}
    vector<CSArc> &get_producer(){return producer;}
    vector<CSArc> &get_consumer(){return consumer;}
    void set_iswritepointer(){is_writepointer = true;}
    bool get_iswritepointer() const{return is_writepointer;}
    void reset_Guard(string _guard){guard.construct("1`" + _guard); hasguard=true;}
    string getid(){return id;}
    bool get_hasguard(){return hasguard;}
    condition_tree& get_guard(){return guard;}
    unsigned short getRow(){return row;}
    void clear_producer(){producer.clear();}
    void clear_consumer(){consumer.clear();}
    //***PDNet added start***/

    //***PDNet added end***/
} CTransition;

typedef class CPN_Arc
{
private:
//    string id;
    bool isp2t;
    string source_id;
    string target_id;
    condition_tree arc_exp;
    Arc_Type arcType;
    bool deleted;
//    bool onlydot;
public:
    void operator=(const CPN_Arc &arc){
//        id = arc.id;
        isp2t = arc.isp2t;
        source_id = arc.source_id;
        target_id = arc.target_id;
        arc_exp = arc.arc_exp;
        arcType = arc.arcType;
//        onlydot = arc.onlydot;
    }

    CPN_Arc(){deleted = false;}
    CPN_Arc(string source,string target,bool sourceP,string arcexp,Arc_Type arcType1){
        source_id = source;
        target_id = target;
        isp2t = sourceP;
        arc_exp.construct(arcexp);
        arcType = arcType1;
    }
    void InitArc(string source,string target,bool sourceP,string arcexp,Arc_Type arcType1){
        if(arc_exp.getexp() != "")
            arc_exp.deconstruct();
        source_id = source;
        target_id = target;
        isp2t = sourceP;
        arc_exp.construct(arcexp);
        arcType = arcType1;
    }
    string getrelated_P()const {return isp2t?source_id:target_id;}
    string getrelated_T()const {return isp2t?target_id:source_id;}
    condition_tree_node *getroot(){return arc_exp.getroot();}
    string getexp(){return arc_exp.getexp();}
    string getsrc()const {return source_id;}
    string gettgt()const {return target_id;}
    Arc_Type getArctype(){return arcType;}

    condition_tree& getArcCT(){return arc_exp;}
    bool getisp2t(){return isp2t;}
    void setdeleted(){deleted = true;}
    bool getdeleted(){return deleted;}

//    MultiSet arc_MS;
//    ~CPN_Arc() {
//        arc_exp.destructor(arc_exp.root);
//    }
} CArc;

/*========================Color_Petri_Net=======================*/
enum AddArcForm{normal,override,beoverrided,append};
class CPN
{
private:
    vector<V_Table *> v_tables;//variable tables
    F_Table f_table;
    NUM_t gen_P_num,gen_T_num,id_num;
    CPlace *place;
    CTransition *transition;
    CArc *arc;
    Variable *vartable;

    vector<string> transPri;// priority of trans, 启发式信息
    vector<short> transPriNum;

    short threadDomainInit;//thread's address space domain, initial 2000 increment 2000

    NUM_t placecount;
    NUM_t varPlacecount;
    NUM_t ctlPlacecount;
    NUM_t exePlacecount;
    NUM_t transitioncount;
    NUM_t arccount;
    NUM_t varcount;

    map<string,string> mapCall2Func;//map call structure's expression to function name
//    map<string,string> mapFunction;//map some unique function expression with their matched places , for example begin,end  and especially ret_tag
    map<string,string> mapPthread;//map some pthread information with their matched places
//    map<string,string> mapJoin;//map pthread_create with join
    map<string,string> join_create;
    string gen_P(){return "P" + to_string(gen_P_num++);}
    string gen_T(){return "T" + to_string(gen_T_num++);}
    id_t gen_id(short size){id_t tmp = id_num; id_num+=size; return tmp;}
    void threaddomaininc(){threadDomainInit += 2000;}

    string construct_initnormaltokenstr(string _data,string _index,string _id,string _tid);
    void Add_place_token(CPlace *pp,int index,string _tid);
    void init_place_MS(string P_id,const SizeList &sl,string init_str);

    void CTN2SingleColor(condition_tree_node *root,token &tk);
    void CTN2Token(condition_tree_node *root,vector<token> &tk);
    void CTN2MS(condition_tree_node *root,MultiSet &ms,const type &tid,const SORTID &sid);


    void Add_Place_MS(string id,const MultiSet &ms);
    void Sub_Place_MS(string id,const MultiSet &ms);


    void Add_Arc_ifnotexist(string source,string target,string exp,bool sourceP,Arc_Type arcType);
    void Add_Variable(string id,type tid);
    void Add_Variable(const condition_tree_node *tree, type tid, SORTID sid, unsigned short down, unsigned short up);
    void setVariableValue(string id,const Bucket &bkt);
    void Add_enter_T(string P_id,const vector<string> &enter_T){
        auto pp = findP_byid(P_id);
        for(int i=0;i<enter_T.size();i++)
            pp->Add_enter(enter_T[i]);
    }
    void Add_exit_T(string P_id,const vector<string> &exit_T){
        auto pp = findP_byid(P_id);
        for(int i=0;i<exit_T.size();i++)
            pp->Add_exit(exit_T[i]);
    }
    void Add_falseexit_T(string P_id,const vector<string> &falseexit_T){
        auto pp = findP_byid(P_id);
        for(int i=0;i<falseexit_T.size();i++)
            pp->Add_falseexit(falseexit_T[i]);
    }
    void Add_enter_T(string P_id,const string &enter_T){
        auto pp = findP_byid(P_id);
//        for(int i=0;i<enter_T.size();i++)
            pp->Add_enter(enter_T);
    }
    void Add_exit_T(string P_id,const string &exit_T){
        auto pp = findP_byid(P_id);
//        for(int i=0;i<exit_T.size();i++)
            pp->Add_exit(exit_T);
    }
    void Add_falseexit_T(string P_id,const string &falseexit_T){
        auto pp = findP_byid(P_id);
//        for(int i=0;i<falseexit_T.size();i++)
            pp->Add_falseexit(falseexit_T);
    }
    void reset_enter_T(string P_id,vector<string> enter_T){
        auto pp = findP_byid(P_id);
        pp->clear_enter();
        for(int i=0;i<enter_T.size();i++)
            pp->Add_enter(enter_T[i]);
    }
    void reset_exit_T(string P_id,vector<string> exit_T){
        auto pp = findP_byid(P_id);
        pp->clear_exit();
        for(int i=0;i<exit_T.size();i++)
            pp->Add_exit(exit_T[i]);
    }
    void reset_falseexit_T(string P_id,vector<string> falseexit_T){
        auto pp = findP_byid(P_id);
        pp->clear_falseexit();
        for(int i=0;i<falseexit_T.size();i++)
            pp->Add_falseexit(falseexit_T[i]);
    }
    vector<string> get_enter_T(string P_id){auto pp = findP_byid(P_id); return pp->get_enter();}
    vector<string> get_exit_T(string P_id){auto pp = findP_byid(P_id);
        return pp->get_exit();}
    vector<string> get_falseexit_T(string P_id){auto pp = findP_byid(P_id); return pp->get_falseexit();}

    void set_call_P(string P_id,const vector<string> &call_P){auto pp = findP_byid(P_id);for(int i=0;i<call_P.size();i++)pp->Add_call_P(call_P[i]);}
    void set_correspond_P(string P_id,const vector<string> &cor_P){auto pp = findP_byid(P_id);for(int i=0;i<cor_P.size();i++)pp->Add_cor_P(cor_P[i]);}
    vector<string> get_call_P(string P_id){auto pp = findP_byid(P_id);return pp->get_call_P();}



    CArc *findArc_bysrctgt(string source,string target){
        for(int i=0;i<arccount;i++)
            if(arc[i].getsrc() == source && arc[i].gettgt() == target)
                return &arc[i];
        return NULL;
    }
    void process_para_type_list(gtree *para_type_list,string func);
    void inside_block(gtree *tree1, string T);
    void inside_block_withoutdependence(gtree *tree1, string T);
    // string Add_executed_P(vector<string> source_T,vector<string> target_T);
    void iter_extra(string newP,string _P);
    void iter_extra_withoutdependence(string _P);
    void create_connect(string control_T, string expression, string base);
    void create_assignment(string control_T, string P_id,gtree *location, string expression);
//    void readOperation(string v_name,string base,string T_id,bool markedremain);
    void readOperation(string v_name,string base,string T_id,bool markedremain, bool markedwrite, AddArcForm arcForm);
    void writeOperation(string T_id,gtree *pos,string content);
//    void writeOperation(string P_id,string T_id, string content);
    void writeOperation(string P_id,string T_id, string content, string index_str, SizeList sl, string base);
    string generateToken(string v_name,string base,short index,string member,short memberindex,string content);
    string generateInitToken(string v_name, string base, short index,string addr, string content, string __tid);
    void writestrcpy(string T,string str,string addr);

    string find_P_id(string v_name,string base);
    bool find_v_isglobal(string v_name,string base);
    SizeList find_v_size(string v_name,string base);
    bool find_v_ispointer(string v_name,string base);

    string find_v_tag(string v_name,string base);
    string find_v_basetag(string v_name,string base);

    string translate_exp2arcexp(gtree *exp_tree);
    string translate_exp2tid(gtree *exp_tree);
//    SizeList get_postsize(gtree *post);
    void writePointer(string T_id,string address, string tag, string content, vector<string> indexs, string member,vector<string> memberindexs);
    void readPointer(string T_id, string tag);
    void Add_pthreadCopys(string T_id,string newtid,string para_P,string initvalue);
    vector<string> generate_writeNormalstr(string T_id, string v_name, string base, vector<string> indexs, string member,
                vector<string> memberindexs,
                string content);
    //TraverseST0
    void create_v_table(gtree *p);
    void init_V_tables() {
        V_Table *table = new V_Table(global_table_name);
        v_tables.push_back(table);
    }
    void release_V_tables(){
        for(unsigned int i=0;i<v_tables.size();i++)
            delete v_tables[i];
    }
    void delete_arc(string source, string target);

    //TraverseST1
    void process_declaration(gtree *declaration);
    void process_initdeclarator(gtree *initdeclarator, string tag, string func);

    void visit_declaration(gtree *p);
    void visit_function(gtree *p);
    void visit_function_withoutdependence(gtree *p);
    void visit_statement(gtree *p);
//    void visit_call(gtree *p);
    void visit_condition(gtree *p);

    //TraverseST2
    vector<string> get_compound_statement_exit(gtree *statement1);

    void handle_compound(gtree *p);
    void handle_compound_withoutdependence(gtree *p);
    void handle_expression(gtree *pGtree);
    void handle_func(gtree *pGtree);
    void handle_iter_sel(gtree *pGtree);
    void handle_iter_sel_withoutdependence(gtree *pGtree);
    void handle_jump_statement(gtree *pGtree);
    void handle_jump_statement_withoutdependence(gtree *p);
    void handle_INC_DEC_OP(int type, bool b, gtree *pGtree);
    void handle_label(gtree *pGtree);
    void handle_call(gtree *pGtree);
    void handle_call_withoutdependence(gtree *p);
    void handle_assignment_expression(gtree *p, string T);
    void handle_conditional_expression(gtree *p, string T, string base);
    void handle_unary_expression(gtree *p, string T, string base, string content);
    void organize_call(gtree *p);
    void organize_call_withoutdependence(gtree *p);
    int getSize(type tid, SORTID sid);

    void AddwriteArc2pointer(string T_id, string identifier,string base);
public:
    vector<string> deadloops;
    vector<string> otherLocks;//已经被遗弃使用
    map<string,index_t> mapPlace;
    map<string,index_t> mapTransition;
    map<string,VARID> mapVariable;
    vector<string> get_correspond_P(string P_id){auto pp = findP_byid(P_id);return pp->get_cor_P();}
    void Add_Place(string id,string Type_name, bool controlP,SizeList sl,string exp,bool isglobal,bool isexecuted, unsigned short row);
    void Add_Transition(string id,string guard,string exp, unsigned short row);
    void Add_Arc(string source,string target,string exp,bool sourceP,Arc_Type arcType,AddArcForm form);
    CPlace* findP_byid(string P_id){auto piter = mapPlace.find(P_id);if(piter == mapPlace.end()) {
        cout <<"can't find P:" + P_id<<endl;
        exit(-1);}return &place[piter->second];}
    void insert_PDNet(ifstream &in, bool fromFile);
    CPN();
    ~CPN();

    CPN_Place* findP_byindex(index_t index){if(placecount<=index){ cout<< "index out of placecount!"<<endl;exit(-1);}return &place[index];}
    CTransition *findT_byid(string T_id){
        auto titer = mapTransition.find(T_id);if(titer == mapTransition.end()) throw "can't find T:" + T_id;return &transition[titer->second];}
    CPN_Transition* findT_byindex(index_t index){if(transitioncount<=index)throw "index out of transcount!";return &transition[index];}
    bool findPlaceExp(string Exp){
        for(int i=0;i<placecount;i++){
            if(place[i].getExp() == Exp)
                return true;
        }
        return false;
    }
    Variable* findVar_byname(string name){auto viter = mapVariable.find(name);
        if(viter == mapVariable.end())throw"can't find Var:" + name;return &vartable[viter->second];}
    void CT2MS(const condition_tree &ct,MultiSet &ms,const type &tid,const SORTID &sid);
    NUM_t get_transcount(){return transitioncount;}
    NUM_t get_placecount(){return placecount;}
    NUM_t get_varplacecount(){return varPlacecount;}
    NUM_t get_ctlplacecount(){return ctlPlacecount;}
    NUM_t get_exeplacecount(){return exePlacecount;}

    NUM_t get_arccount(){return arccount;}
    CPN_Place* getplacearr(){return place;}
    CPN_Transition* gettransarr(){return transition;}
    CPN_Arc* getarcarr(){return arc;}

    void getDecl(gtree *tree);
    void initDecl();
    void setmaintoken();
    void delete_compound(gtree *p);
    void delete_compound_withoutdependence(gtree *p);
    void set_producer_consumer();
    void init_alloc_func();

    void Traverse_ST0(gtree *tree);//traverse syntax tree to construct v_tables
    void Traverse_ST1(gtree *tree);//traverse syntax tree pre-order
    void Traverse_ST2(gtree *tree);//traverse syntax tree post-order

    void Traverse_ST1_withoutdependence(gtree *tree);//traverse syntax tree pre-order(no dependence)
    void Traverse_ST2_withoutdependence(gtree *tree);//traverse syntax tree post-order(no dependence)

    void print_CPN(string filename);


    void copy_childNet(CPN *cpnet, vector<string> places, vector<string> transitions);

    void InitDistance(vector<string> &criteria);
    void setPriTrans(vector<pair<string,short>> &prioritys);

    vector<string>& get_transPri(){return transPri;}
    void generate_transPriNum(){
        for(int i=0;i<transPri.size();i++){
            for(int j=0;j<transitioncount;j++){
                if(transition[j].getid() == transPri[i])
                {
                    transPriNum.push_back(j);
                    break;
                }
            }
        }
    }
    vector<short>& get_transPriNum(){return transPriNum;}
    void getRowRelatedPT(unsigned short row, vector<string> &Ps, vector<string> &Ts);
    string getVarRelatedP(const string &name, const Row_Type &row);

    //For changeConstruct.cpp
    void deletePreExecuteArc(string originP);
    void modifyPreExecuteArc(string originP, string newP);
    void modifyAfterExecuteArc(string originP, string newP);
    void change_deleteControlArc(string p_name);
    void change_addControlArc(string src, string tgt);
    void change_deleteArc(string src, string tgt);
    void change_addExecutedArc(string src, string tgt, bool sourceP);
    string Add_executed_P(vector<string> source_T,vector<string> target_T);
};





class var_type{
private:
    string tag;
    string name;
    bool pointer_flag;
    SizeList size_table;
public:
//    var_type(){size_table = NULL;}
//    ~var_type(){if(size_table)delete size_table;}
    void init(string t,string id,bool pointer_f,SizeList sizes){
        tag = t;
        name = id;
        pointer_flag = pointer_f;
//        dimension = dim;
        size_table.Set_sizes(sizes.getsizes());
//        size_table = new int[dim];
//        for(int i=0;i<dim;i++)
//            size_table[i] = size[i];
    }
    bool get_pointer_flag(){return pointer_flag;}
    SizeList get_size_table(){return size_table;}
    int get_dimension(){return size_table.getdim();}
    string get_tag(){return tag;}
    string get_name(){return name;}
};

void init_pthread_type();
void uninit_pthread_type();
extern type TID_colorset;

#endif //PDNET_CHECKER_CPN_H