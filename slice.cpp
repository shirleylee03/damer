//
// Created on 2020/10/10.
//

#include "cpn.h"
#include "BA/tinyxml.h"
#include "v_table.h"


extern string end_suffix,return_suffix,begin_suffix,join_suffix,tid_str;

extern string controlflowArcExp(string arcexp);

bool judge_read(Arc_Type arcType){
    if(arcType == readArc || arcType == readwrite)
        return true;
    return false;
}

bool judge_write(Arc_Type arcType){
    if(arcType == writeArc || arcType == readwrite)
        return true;
    return false;
}

vector<CTransition *> find_previous_define(CTransition  *trans,index_t var_index,CPN *cpn,Arc_Type arcType)
{
    vector<CTransition *> searched,res;
    searched.push_back(trans);
    Arc_Type allowed_type;
    if(arcType == call_enter) {
        allowed_type = call_exit;
    }
    else if(arcType == call_exit) {
        allowed_type = call_enter;
    }
    else
    {
        cout<<"two phrase algorithm is always lack a type of arc!"<<endl;
        exit(-1);
    }
    for(unsigned int i=0;i<searched.size();i++)
    {
        bool find_flag = false;
        if(searched.size() != 1) {
            //search的size不为1确保j=j+1这类既有读又有写的语句不会被自身截断
            for (unsigned int j = 0; j < searched[i]->get_consumer().size(); j++) {
                if (searched[i]->get_consumer()[j].idx == var_index &&
                    judge_write(searched[i]->get_consumer()[j].arcType)) {
                    if (!exist_in(res, searched[i]))
                        res.push_back(searched[i]);
                    find_flag = true;
                    break;
                } else if (searched[i]->get_iswritepointer())//redundant dependence for writepointer
                    res.push_back(searched[i]);
            }
        }
        if(find_flag == true)
            continue;
        for(unsigned int j=0;j<searched[i]->get_producer().size();j++)
        {
            CPlace *place = &cpn->getplacearr()[searched[i]->get_producer()[j].idx];

            if(place->getiscontrolP() == true)
            {
                for(unsigned int k=0;k<place->get_producer().size();k++)
                {
                    if(place->get_producer()[k].arcType == executed
                       || place->get_producer()[k].arcType == call_connect
                       || place->get_producer()[k].arcType == allowed_type) {
                        CTransition *t = &cpn->gettransarr()[place->get_producer()[k].idx];
                        if(!exist_in(searched,t))
                            searched.push_back(t);
                    }
                }
            }
        }
    }
    return res;

}
vector<CTransition *> find_after_use(CTransition  *trans,index_t var_index,CPN *cpn,Arc_Type arcType)
{
    vector<CTransition *> searched,res;
    searched.push_back(trans);
    Arc_Type allowed_type;
    if(arcType == call_enter) {
        allowed_type = call_exit;
    }
    else if(arcType == call_exit) {
        allowed_type = call_enter;
    }
    else
    {
        cout<<"two phrase algorithm is always lack a type of arc!"<<endl;
        exit(-1);
    }
    for(unsigned int i=0;i<searched.size();i++)
    {
        bool find_flag = false;
        if(searched.size() != 1) {
            //search的size不为1确保j=j+1这类既有读又有写的语句不会被自身截断
            for (unsigned int j = 0; j < searched[i]->get_consumer().size(); j++) {
                if (searched[i]->get_consumer()[j].idx == var_index &&
                    judge_read(searched[i]->get_consumer()[j].arcType)) {
                    if (!exist_in(res, searched[i]))
                        res.push_back(searched[i]);
//                    find_flag = true;
                    break;
                }
                else if (searched[i]->get_consumer()[j].idx == var_index &&
                     judge_write(searched[i]->get_consumer()[j].arcType)) {
                    find_flag = true;
                    break;
                }
//                else if (searched[i]->get_iswritepointer())//redundant dependence for writepointer
//                    res.push_back(searched[i]);
            }
        }
        if(find_flag == true)
            continue;
        for(unsigned int j=0;j<searched[i]->get_consumer().size();j++)
        {
            CPlace *place = &cpn->getplacearr()[searched[i]->get_consumer()[j].idx];

            if(place->getiscontrolP() == true)
            {
                for(unsigned int k=0;k<place->get_consumer().size();k++)
                {
                    if(place->get_consumer()[k].arcType == control
                       || place->get_consumer()[k].arcType == call_connect
                       || place->get_consumer()[k].arcType == allowed_type) {
                        CTransition *t = &cpn->gettransarr()[place->get_consumer()[k].idx];
                        if(!exist_in(searched,t))
                            searched.push_back(t);
                    }
                }
            }
        }
    }
    return res;

}

bool contain_path(CTransition *src,CTransition *tgt,CPN *cpn,string global_P)
{
    vector<CTransition *> searched,res;
    searched.push_back(src);

    auto iter = cpn->mapPlace.find(global_P);
    index_t idx = iter->second;

    for(unsigned int i=0;i<searched.size();i++)
    {
        for(unsigned int j=0;j<searched[i]->get_consumer().size();j++)
        {
            if(judge_write(searched[i]->get_consumer()[j].arcType)
               && searched[i]->get_consumer()[j].idx == idx)
                return false;
        }
        if(searched[i] == tgt)
            return true;
        CTransition *tran = searched[i];
        for(unsigned int j=0;j<tran->get_consumer().size();j++)
        {
            if(tran->get_consumer()[j].arcType == executed
               || tran->get_consumer()[j].arcType == call_enter
               || tran->get_consumer()[j].arcType == call_exit)
            {
                CPlace *place = &cpn->getplacearr()[tran->get_consumer()[j].idx];
                for(unsigned int k=0;k<place->get_consumer().size();k++)
                {
                    if(!exist_in(searched,&cpn->gettransarr()[place->get_consumer()[k].idx]))
                        searched.push_back(&cpn->gettransarr()[place->get_consumer()[k].idx]);
                }
            }
        }
    }
    return false;
}

bool contain_path(string src_P, string tgt_P,CPN *cpn){
    // if(src_P == tgt_P)
    //     return false;
    auto src_pp = cpn->findP_byid(src_P);
    auto src_pp_consumer = src_pp->get_consumer();
    auto tgt_pp = cpn->findP_byid(tgt_P);
    
    vector<CTransition *> searched;
    for(int i=0;i<src_pp_consumer.size();i++){
        auto tt = cpn->findT_byindex(src_pp_consumer[i].idx);
        searched.push_back(tt);
    }

    for(int i=0;i<searched.size();i++){
        auto producer = searched[i]->get_producer();
        for(int j=0;j<producer.size();j++){
            auto producer_pp = cpn->findP_byindex(producer[j].idx);
            if(producer_pp == tgt_pp)
                return true;
        }
        CTransition *tran = searched[i];
        for(unsigned int j=0;j<tran->get_consumer().size();j++)
        {
            if(tran->get_consumer()[j].arcType == executed
               || tran->get_consumer()[j].arcType == call_enter
               || tran->get_consumer()[j].arcType == call_exit)
            {
                CPlace *place = &cpn->getplacearr()[tran->get_consumer()[j].idx];
                for(unsigned int k=0;k<place->get_consumer().size();k++)
                {
                    if(!exist_in(searched,&cpn->gettransarr()[place->get_consumer()[k].idx]))
                        searched.push_back(&cpn->gettransarr()[place->get_consumer()[k].idx]);
                }
            }
        }
    }
    return false;
}

bool is_global(CPN *cpn,string P_id)
{
    auto pp = cpn->findP_byid(P_id);
    return pp->getisglobal();
//    if(var.find(return_suffix)!=string::npos)
//        return false;
//    if (v_tables[0]->find_P(var))
//        return true;
//    else
//        return false;
}

bool in_a_thread(CTransition *t1,CTransition *t2)
{
    return false;
}

//后向切片
void two_phrase_slicing(CPN *cpn, vector<string> place, vector<string> &final_P, vector<string>&final_T)
{
    vector<string> P, P_done, T, cor_Ps, cor_Pdones;// , P_read;
    P = place;
    CPlace *p;

    for(unsigned int i=0;i<place.size();i++)
    {
        auto iter = cpn->mapPlace.find(P[i]);
        p = &cpn->getplacearr()[iter->second];


        //T'=T' U {p.}
        for(unsigned int j=0;j<p->get_consumer().size();j++)
        {
            CTransition *tran = &cpn->gettransarr()[p->get_consumer()[j].idx];
            T.push_back(tran->getid());
            for(unsigned int k=0;k<tran->get_producer().size();k++)
            {
                if(tran->get_producer()[k].arcType == control)
                    if(!exist_in(P,cpn->getplacearr()[tran->get_producer()[k].idx].getid()))
                        P.push_back(cpn->getplacearr()[tran->get_producer()[k].idx].getid());
            }
        }
    }
    

    int i=0;
    bool reCalFlag = true;
    while(reCalFlag && i<P.size()){
        reCalFlag = false;
    //first phrase
    for(;i<P.size();i++)
    {
        auto iter = cpn->mapPlace.find(P[i]);
        p = &cpn->getplacearr()[iter->second];
        if(p->getiscontrolP() && p->getisexecuted() == false)
            ;
        else
            continue;
        //保证语句基本结构完整
        for(unsigned int j=0;j<p->get_consumer().size();j++)
        {
            CTransition *tran = &cpn->gettransarr()[p->get_consumer()[j].idx];
            if(!exist_in(T,tran->getid()))
                T.push_back(tran->getid());
            for(unsigned int k=0;k<tran->get_producer().size();k++)
            {
                if(tran->get_producer()[k].arcType == control)
                    if(!exist_in(P,cpn->getplacearr()[tran->get_producer()[k].idx].getid()))
                        P.push_back(cpn->getplacearr()[tran->get_producer()[k].idx].getid());
            }

            // for alloc
            for(unsigned int k=0;k<tran->get_consumer().size();k++){
                if(tran->get_consumer()[k].arcType == allocwrite)
                    if(!exist_in(P,cpn->getplacearr()[tran->get_consumer()[k].idx].getid()))
                        P.push_back(cpn->getplacearr()[tran->get_consumer()[k].idx].getid());
            }
        }
       

        //control dependence

        for(unsigned int j=0;j<p->get_producer().size();j++)
        {
            if(p->get_producer()[j].arcType == control
               || p->get_producer()[j].arcType == call_connect
               || p->get_producer()[j].arcType == call_enter) {
                CTransition *trans = &cpn->gettransarr()[p->get_producer()[j].idx];
//                if(!exist_in(T,trans->getid()))
//                    T.push_back(trans->getid());
                for(unsigned int k=0;k<trans->get_producer().size();k++)
                {
                    if(trans->get_producer()[k].arcType == control) {
                        auto parent_pp = &cpn->getplacearr()[trans->get_producer()[k].idx];
                        string temp_P = parent_pp->getid();
                        unsigned short pa_dist = parent_pp->getDistance();
                        unsigned short ch_dist = p->getDistance() == 65535?65535:p->getDistance()+1;
                        parent_pp->setDistance(min(pa_dist,ch_dist));
                        if (!exist_in(P, temp_P))
                            P.push_back(temp_P);
                    }
                }
            }
        }

//        if(p->is_mutex)
//        {
//            for(unsigned int j=0;j<p->get_consumer().size();j++)
//            {
//                if(p->get_consumer()[j].arcType == control) {
//                    CTransition *trans = &cpn->gettransarr()[p->get_consumer()[j].idx];
//                    if(!exist_in(T,trans->getid()))
//                        T.push_back(trans->getid());
//                    for(unsigned int k=0;k<trans->get_producer().size();k++)
//                    {
//                        if(trans->get_producer()[k].arcType == control) {
//                            string temp_P = cpn->getplacearr()[trans->get_producer()[k].idx].id;
//                            if (!exist_in(P, temp_P))
//                                P.push_back(temp_P);
//                        }
//                    }
//                }
//            }
//        }

        //data dependence
        for(unsigned int j=0;j<p->get_consumer().size();j++) {
            if (p->get_consumer()[j].arcType == control) {
                CTransition *trans = &cpn->gettransarr()[p->get_consumer()[j].idx];
//                    vector<index_t> index_vec;
//                if(!exist_in(T,trans->getid()))
//                    T.push_back(trans->getid());
                for (unsigned int k = 0; k < trans->get_consumer().size(); k++) {
                    if (judge_read(trans->get_consumer()[k].arcType)) {
                        vector<CTransition *> trans_vec;
                        string var = cpn->getplacearr()[trans->get_consumer()[k].idx].getid();
                        if (is_global(cpn,var))
                        {
                            CPlace *global_v_P = &cpn->getplacearr()[trans->get_producer()[k].idx];
                            if (!exist_in(P, global_v_P->getid()))
                                P.push_back(global_v_P->getid());
                            for (unsigned int l = 0; l < global_v_P->get_producer().size(); l++) {
                                if (judge_write(global_v_P->get_producer()[l].arcType)) {
                                    CTransition *t = &cpn->gettransarr()[global_v_P->get_producer()[l].idx];
                                    if(in_a_thread(t,trans)) {
                                        if (contain_path(t, trans, cpn, global_v_P->getid())) {
//                                            if (!exist_in(T, t->getid()))
//                                                T.push_back(t->getid());
                                            for (unsigned int m = 0; m < t->get_producer().size(); m++) {
                                                if (t->get_producer()[m].arcType == control
                                                    || t->get_producer()[m].arcType == call_connect) {
                                                    if (!exist_in(P, cpn->getplacearr()[t->get_producer()[m].idx].getid()))
                                                        P.push_back(cpn->getplacearr()[t->get_producer()[m].idx].getid());
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
//                                        if (!exist_in(T, t->getid()))
//                                            T.push_back(t->getid());
                                        for (unsigned int m = 0; m < t->get_producer().size(); m++) {
                                            if (t->get_producer()[m].arcType == control
                                                || t->get_producer()[m].arcType == call_connect) {
                                                if (!exist_in(P, cpn->getplacearr()[t->get_producer()[m].idx].getid()))
                                                    P.push_back(cpn->getplacearr()[t->get_producer()[m].idx].getid());
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            trans_vec = find_previous_define(trans, trans->get_consumer()[k].idx, cpn, call_exit);
                            if (trans_vec.size() != 0 && !exist_in(P, var))
                                P.push_back(var);
                            for (unsigned int l = 0; l < trans_vec.size(); l++) {
//                                if (!exist_in(T, trans_vec[l]->getid()))
//                                    T.push_back(trans_vec[l]->getid());
                                for (unsigned int m = 0; m < trans_vec[l]->get_producer().size(); m++) {
                                    if (trans_vec[l]->get_producer()[m].arcType == control) {
                                        string temp_P = cpn->getplacearr()[trans_vec[l]->get_producer()[m].idx].getid();
                                        if (!exist_in(P, temp_P))
                                            P.push_back(temp_P);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        // cor_P
        vector<string> cor_P = cpn->get_correspond_P(P[i]);
        for(unsigned int j=0;j<cor_P.size();j++)
            if (!exist_in(cor_Ps, cor_P[j]) && !exist_in(cor_Pdones,cor_P[j]))
                cor_Ps.push_back(cor_P[j]);
        
        // for(auto iter = cor_Ps.begin();iter != cor_Ps.end();){
        //     if(contain_path(*iter,P[i],cpn)){
        //         if(!exist_in(P,*iter)){
        //             P.push_back(*iter);
        //             cor_Pdones.push_back(*iter);
        //             iter = cor_Ps.erase(iter);
        //         }
        //         else
        //             iter++;
        //     }
        //     else
        //         iter++;
        // }
    }

    // cor_P
    
    for(auto iter = cor_Ps.begin();iter != cor_Ps.end();){
        bool finded = false;
        for(int j=0;j<P.size();j++){
            if(contain_path(*iter,P[j],cpn)){
                finded = true;
                break;
            }
        }
        if(finded){
            if(!exist_in(P,*iter)){
                P.push_back(*iter);
                cor_Pdones.push_back(*iter);
                iter = cor_Ps.erase(iter);
            }
            else
                iter++;
            reCalFlag = true;
        }
        else
            iter++;
    }
    }

    i = 0;
    reCalFlag = true;
    while(reCalFlag && i<P.size()){
        reCalFlag = false;
    //second phrase
    for(;i<P.size();i++)
    {
        auto iter = cpn->mapPlace.find(P[i]);
        p = &cpn->getplacearr()[iter->second];
        if(p->getiscontrolP() && p->getisexecuted() == false)
            ;
        else
            continue;
        //保证语句基本结构完整
        for(unsigned int j=0;j<p->get_consumer().size();j++)
        {
            CTransition *tran = &cpn->gettransarr()[p->get_consumer()[j].idx];
            if(!exist_in(T,tran->getid()))
                T.push_back(tran->getid());
            for(unsigned int k=0;k<tran->get_producer().size();k++)
            {
                if(tran->get_producer()[k].arcType == control)
                    if(!exist_in(P,cpn->getplacearr()[tran->get_producer()[k].idx].getid()))
                        P.push_back(cpn->getplacearr()[tran->get_producer()[k].idx].getid());
            }
            // for alloc
            for(unsigned int k=0;k<tran->get_consumer().size();k++){
                if(tran->get_consumer()[k].arcType == allocwrite)
                    if(!exist_in(P,cpn->getplacearr()[tran->get_consumer()[k].idx].getid()))
                        P.push_back(cpn->getplacearr()[tran->get_consumer()[k].idx].getid());
            }
        }
        // vector<string> cor_P = cpn->get_correspond_P(P[i]);
        // for(unsigned int j=0;j<cor_P.size();j++)
        //     if (!exist_in(P, cor_P[j]))
        //         P.push_back(cor_P[j]);

        //control dependence

        for(unsigned int j=0;j<p->get_producer().size();j++)
        {
            if(p->get_producer()[j].arcType == control
               || p->get_producer()[j].arcType == call_connect
               || p->get_producer()[j].arcType == call_enter) {
                CTransition *trans = &cpn->gettransarr()[p->get_producer()[j].idx];
//                if(!exist_in(T,trans->getid()))
//                    T.push_back(trans->getid());

                for(unsigned int k=0;k<trans->get_producer().size();k++)
                {
                    if(trans->get_producer()[k].arcType == control) {
                        auto parent_pp = &cpn->getplacearr()[trans->get_producer()[k].idx];
                        string temp_P = parent_pp->getid();
                        unsigned short pa_dist = parent_pp->getDistance();
                        unsigned short ch_dist = p->getDistance() == 65535?65535:p->getDistance()+1;
                        parent_pp->setDistance(min(pa_dist,ch_dist));
                        if (!exist_in(P, temp_P))
                            P.push_back(temp_P);
                    }
                }
            }
        }

//        if(p->is_mutex)
//        {
//            for(unsigned int j=0;j<p->get_consumer().size();j++)
//            {
//                if(p->get_consumer()[j].arcType == control) {
//                    CTransition *trans = &cpn->gettransarr()[p->get_consumer()[j].idx];
//                    if(!exist_in(T,trans->getid()))
//                        T.push_back(trans->getid());
//                    for(unsigned int k=0;k<trans->get_producer().size();k++)
//                    {
//                        if(trans->get_producer()[k].arcType == control) {
//                            string temp_P = cpn->getplacearr()[trans->get_producer()[k].idx].id;
//                            if (!exist_in(P, temp_P))
//                                P.push_back(temp_P);
//                        }
//                    }
//                }
//            }
//        }

        //data dependence
        for(unsigned int j=0;j<p->get_consumer().size();j++) {
            if (p->get_consumer()[j].arcType == control) {
                CTransition *trans = &cpn->gettransarr()[p->get_consumer()[j].idx];
//                    vector<index_t> index_vec;
//                if(!exist_in(T,trans->getid()))
//                    T.push_back(trans->getid());
                for (unsigned int k = 0; k < trans->get_consumer().size(); k++) {
                    if (judge_read(trans->get_consumer()[k].arcType)) {
                        vector<CTransition *> trans_vec;
                        string var = cpn->getplacearr()[trans->get_consumer()[k].idx].getid();
                        if (is_global(cpn,var))
                        {
                            CPlace *global_v_P = &cpn->getplacearr()[trans->get_consumer()[k].idx];
                            if (!exist_in(P, global_v_P->getid()))
                                P.push_back(global_v_P->getid());
                            for (unsigned int l = 0; l < global_v_P->get_producer().size(); l++) {
                                if (judge_write(global_v_P->get_producer()[l].arcType)) {
                                    CTransition *t = &cpn->gettransarr()[global_v_P->get_producer()[l].idx];
                                    if(in_a_thread(t,trans)) {
                                        if (contain_path(t, trans, cpn, global_v_P->getid())) {
//                                            if (!exist_in(T, t->getid()))
//                                                T.push_back(t->getid());
                                            for (unsigned int m = 0; m < t->get_producer().size(); m++) {
                                                if (t->get_producer()[m].arcType == control
                                                    || t->get_producer()[m].arcType == call_connect) {
                                                    if (!exist_in(P, cpn->getplacearr()[t->get_producer()[m].idx].getid()))
                                                        P.push_back(cpn->getplacearr()[t->get_producer()[m].idx].getid());
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
//                                        if (!exist_in(T, t->getid()))
//                                            T.push_back(t->getid());
                                        for (unsigned int m = 0; m < t->get_producer().size(); m++) {
                                            if (t->get_producer()[m].arcType == control
                                                || t->get_producer()[m].arcType == call_connect) {
                                                if (!exist_in(P, cpn->getplacearr()[t->get_producer()[m].idx].getid()))
                                                    P.push_back(cpn->getplacearr()[t->get_producer()[m].idx].getid());
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            trans_vec = find_previous_define(trans, trans->get_consumer()[k].idx, cpn, call_enter);
                            if (trans_vec.size() != 0 && !exist_in(P, var))
                                P.push_back(var);
                            for (unsigned int l = 0; l < trans_vec.size(); l++) {
//                                if (!exist_in(T, trans_vec[l]->getid()))
//                                    T.push_back(trans_vec[l]->getid());
                                for (unsigned int m = 0; m < trans_vec[l]->get_producer().size(); m++) {
                                    if (trans_vec[l]->get_producer()[m].arcType == control) {
                                        string temp_P = cpn->getplacearr()[trans_vec[l]->get_producer()[m].idx].getid();
                                        if (!exist_in(P, temp_P))
                                            P.push_back(temp_P);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        // cor_P
        vector<string> cor_P = cpn->get_correspond_P(P[i]);
        for(unsigned int j=0;j<cor_P.size();j++)
            if (!exist_in(cor_Ps, cor_P[j]) && !exist_in(cor_Pdones,cor_P[j]))
                cor_Ps.push_back(cor_P[j]);
        
        // for(auto iter = cor_Ps.begin();iter != cor_Ps.end();){
        //     if(contain_path(*iter,P[i],cpn)){
        //         if(!exist_in(P,*iter)){
        //             P.push_back(*iter);
        //             cor_Pdones.push_back(*iter);
        //             iter = cor_Ps.erase(iter);
        //         }
        //         else
        //             iter++;
        //     }
        //     else
        //         iter++;
        // }
    }
    // cor_P
    
    for(auto iter = cor_Ps.begin();iter != cor_Ps.end();){
        bool finded = false;
        for(int j=0;j<P.size();j++){
            if(contain_path(*iter,P[j],cpn)){
                finded = true;
                break;
            }
        }
        if(finded){
            if(!exist_in(P,*iter)){
                P.push_back(*iter);
                cor_Pdones.push_back(*iter);
                iter = cor_Ps.erase(iter);
            }
            else
                iter++;
            reCalFlag = true;
        }
        else
            iter++;
    }
    }
    final_P = P;
    final_T = T;
}

//forward slicing
void preWardSlicing(CPN *cpn, vector<string> place, vector<string> &final_P, vector<string> &final_T){
    vector<string> P, P_done, T;// , P_read;
    P = place;
    CPlace *p;

    for(unsigned int i=0;i<place.size();i++)
    {
        auto iter = cpn->mapPlace.find(P[i]);
        p = &cpn->getplacearr()[iter->second];


        //T'=T' U {p.}
        for(unsigned int j=0;j<p->get_consumer().size();j++)
        {
            CTransition *tran = &cpn->gettransarr()[p->get_consumer()[j].idx];
            T.push_back(tran->getid());
            for(unsigned int k=0;k<tran->get_producer().size();k++)
            {
                if(tran->get_producer()[k].arcType == control)
                    if(!exist_in(P,cpn->getplacearr()[tran->get_producer()[k].idx].getid()))
                        P.push_back(cpn->getplacearr()[tran->get_producer()[k].idx].getid());
            }
        }
    }

    //first phrase
    for(unsigned int i=0;i<P.size();i++)
    {
        auto iter = cpn->mapPlace.find(P[i]);
        p = &cpn->getplacearr()[iter->second];
        if(p->getiscontrolP() && p->getisexecuted() == false)
            ;
        else
            continue;
//        //cor_P
//        for(unsigned int j=0;j<p->get_consumer().size();j++)
//        {
//            CTransition *tran = &cpn->gettransarr()[p->get_consumer()[j].idx];
//            if(!exist_in(T,tran->getid()))
//                T.push_back(tran->getid());
//            for(unsigned int k=0;k<tran->get_producer().size();k++)
//            {
//                if(tran->get_producer()[k].arcType == control)
//                    if(!exist_in(P,cpn->getplacearr()[tran->get_producer()[k].idx].getid()))
//                        P.push_back(cpn->getplacearr()[tran->get_producer()[k].idx].getid());
//            }
//
//            // for alloc
//            for(unsigned int k=0;k<tran->get_consumer().size();k++){
//                if(tran->get_consumer()[k].arcType == allocwrite)
//                    if(!exist_in(P,cpn->getplacearr()[tran->get_consumer()[k].idx].getid()))
//                        P.push_back(cpn->getplacearr()[tran->get_consumer()[k].idx].getid());
//            }
//        }
//        vector<string> cor_P = cpn->get_correspond_P(P[i]);
//        for(unsigned int j=0;j<cor_P.size();j++)
//            if (!exist_in(P, cor_P[j]))
//                P.push_back(cor_P[j]);

        //control dependence

        auto enter_Ts = p->get_enter();
        for(int j=0;j<enter_Ts.size();j++){
            auto enter_T = enter_Ts[j];
            auto enter_consumer = cpn->findT_byid(enter_T)->get_consumer();
            for(int k=0;k<enter_consumer.size();k++){
                if(enter_consumer[k].arcType == control || enter_consumer[k].arcType == call_enter
                || enter_consumer[k].arcType == call_connect) {
                    string controled_P = cpn->getplacearr()[enter_consumer[k].idx].getid();
                    if (!exist_in(P, controled_P))
                        P.push_back(controled_P);
                }
            }
        }

        //data dependence
        for(unsigned int j=0;j<p->get_consumer().size();j++) {
            if (p->get_consumer()[j].arcType == control) {
                CTransition *trans = &cpn->gettransarr()[p->get_consumer()[j].idx];
//                    vector<index_t> index_vec;
//                if(!exist_in(T,trans->getid()))
//                    T.push_back(trans->getid());
                for (unsigned int k = 0; k < trans->get_consumer().size(); k++) {
                    if (judge_write(trans->get_consumer()[k].arcType)) {
                        vector<CTransition *> trans_vec;
                        string var = cpn->getplacearr()[trans->get_consumer()[k].idx].getid();
                        if (is_global(cpn,var))
                        {
                            CPlace *global_v_P = cpn->findP_byid(var);
//                            if (!exist_in(P, global_v_P->getid()))
//                                P.push_back(global_v_P->getid());
                            for (unsigned int l = 0; l < global_v_P->get_producer().size(); l++) {
                                if (judge_read(global_v_P->get_producer()[l].arcType)) {
                                    CTransition *t = &cpn->gettransarr()[global_v_P->get_producer()[l].idx];
                                    if(in_a_thread(t,trans)) {
                                        if (contain_path(t, trans, cpn, global_v_P->getid())) {
//                                            if (!exist_in(T, t->getid()))
//                                                T.push_back(t->getid());
                                            for (unsigned int m = 0; m < t->get_producer().size(); m++) {
                                                if (t->get_producer()[m].arcType == control
                                                    || t->get_producer()[m].arcType == call_connect) {
                                                    if (!exist_in(P, cpn->getplacearr()[t->get_producer()[m].idx].getid()))
                                                        P.push_back(cpn->getplacearr()[t->get_producer()[m].idx].getid());
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
//                                        if (!exist_in(T, t->getid()))
//                                            T.push_back(t->getid());
                                        for (unsigned int m = 0; m < t->get_producer().size(); m++) {
                                            if (t->get_producer()[m].arcType == control
                                                || t->get_producer()[m].arcType == call_connect) {
                                                if (!exist_in(P, cpn->getplacearr()[t->get_producer()[m].idx].getid()))
                                                    P.push_back(cpn->getplacearr()[t->get_producer()[m].idx].getid());
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            trans_vec = find_after_use(trans, trans->get_consumer()[k].idx, cpn, call_enter);
//                            if (trans_vec.size() != 0 && !exist_in(P, var))
//                                P.push_back(var);
                            for (unsigned int l = 0; l < trans_vec.size(); l++) {
//                                if (!exist_in(T, trans_vec[l]->getid()))
//                                    T.push_back(trans_vec[l]->getid());
                                for (unsigned int m = 0; m < trans_vec[l]->get_producer().size(); m++) {
                                    if (trans_vec[l]->get_producer()[m].arcType == control) {
                                        string temp_P = cpn->getplacearr()[trans_vec[l]->get_producer()[m].idx].getid();
                                        if (!exist_in(P, temp_P))
                                            P.push_back(temp_P);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //second phrase
    for(unsigned int i=0;i<P.size();i++)
    {
        auto iter = cpn->mapPlace.find(P[i]);
        p = &cpn->getplacearr()[iter->second];
        if(p->getiscontrolP() && p->getisexecuted() == false)
            ;
        else
            continue;
//        //cor_P
//        for(unsigned int j=0;j<p->get_consumer().size();j++)
//        {
//            CTransition *tran = &cpn->gettransarr()[p->get_consumer()[j].idx];
//            if(!exist_in(T,tran->getid()))
//                T.push_back(tran->getid());
//            for(unsigned int k=0;k<tran->get_producer().size();k++)
//            {
//                if(tran->get_producer()[k].arcType == control)
//                    if(!exist_in(P,cpn->getplacearr()[tran->get_producer()[k].idx].getid()))
//                        P.push_back(cpn->getplacearr()[tran->get_producer()[k].idx].getid());
//            }
//
//            // for alloc
//            for(unsigned int k=0;k<tran->get_consumer().size();k++){
//                if(tran->get_consumer()[k].arcType == allocwrite)
//                    if(!exist_in(P,cpn->getplacearr()[tran->get_consumer()[k].idx].getid()))
//                        P.push_back(cpn->getplacearr()[tran->get_consumer()[k].idx].getid());
//            }
//        }
//        vector<string> cor_P = cpn->get_correspond_P(P[i]);
//        for(unsigned int j=0;j<cor_P.size();j++)
//            if (!exist_in(P, cor_P[j]))
//                P.push_back(cor_P[j]);

        //control dependence

        auto enter_Ts = p->get_enter();
        for(int j=0;j<enter_Ts.size();j++){
            auto enter_T = enter_Ts[j];
            auto enter_consumer = cpn->findT_byid(enter_T)->get_consumer();
            for(int k=0;k<enter_consumer.size();k++){
                if(enter_consumer[k].arcType == control) {
                    string controled_P = cpn->getplacearr()[enter_consumer[k].idx].getid();
                    if (!exist_in(P, controled_P))
                        P.push_back(controled_P);
                }
            }
        }

        //data dependence
        for(unsigned int j=0;j<p->get_consumer().size();j++) {
            if (p->get_consumer()[j].arcType == control) {
                CTransition *trans = &cpn->gettransarr()[p->get_consumer()[j].idx];
//                    vector<index_t> index_vec;
//                if(!exist_in(T,trans->getid()))
//                    T.push_back(trans->getid());
                for (unsigned int k = 0; k < trans->get_consumer().size(); k++) {
                    if (judge_write(trans->get_consumer()[k].arcType)) {
                        vector<CTransition *> trans_vec;
                        string var = cpn->getplacearr()[trans->get_consumer()[k].idx].getid();
                        if (is_global(cpn,var))
                        {
                            CPlace *global_v_P = cpn->findP_byid(var);
//                            if (!exist_in(P, global_v_P->getid()))
//                                P.push_back(global_v_P->getid());
                            for (unsigned int l = 0; l < global_v_P->get_producer().size(); l++) {
                                if (judge_read(global_v_P->get_producer()[l].arcType)) {
                                    CTransition *t = &cpn->gettransarr()[global_v_P->get_producer()[l].idx];
                                    if(in_a_thread(t,trans)) {
                                        if (contain_path(t, trans, cpn, global_v_P->getid())) {
//                                            if (!exist_in(T, t->getid()))
//                                                T.push_back(t->getid());
                                            for (unsigned int m = 0; m < t->get_producer().size(); m++) {
                                                if (t->get_producer()[m].arcType == control
                                                    || t->get_producer()[m].arcType == call_connect) {
                                                    if (!exist_in(P, cpn->getplacearr()[t->get_producer()[m].idx].getid()))
                                                        P.push_back(cpn->getplacearr()[t->get_producer()[m].idx].getid());
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
//                                        if (!exist_in(T, t->getid()))
//                                            T.push_back(t->getid());
                                        for (unsigned int m = 0; m < t->get_producer().size(); m++) {
                                            if (t->get_producer()[m].arcType == control
                                                || t->get_producer()[m].arcType == call_connect) {
                                                if (!exist_in(P, cpn->getplacearr()[t->get_producer()[m].idx].getid()))
                                                    P.push_back(cpn->getplacearr()[t->get_producer()[m].idx].getid());
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            trans_vec = find_after_use(trans, trans->get_consumer()[k].idx, cpn, call_exit);
//                            if (trans_vec.size() != 0 && !exist_in(P, var))
//                                P.push_back(var);
                            for (unsigned int l = 0; l < trans_vec.size(); l++) {
//                                if (!exist_in(T, trans_vec[l]->getid()))
//                                    T.push_back(trans_vec[l]->getid());
                                for (unsigned int m = 0; m < trans_vec[l]->get_producer().size(); m++) {
                                    if (trans_vec[l]->get_producer()[m].arcType == control) {
                                        string temp_P = cpn->getplacearr()[trans_vec[l]->get_producer()[m].idx].getid();
                                        if (!exist_in(P, temp_P))
                                            P.push_back(temp_P);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

//    //correspond pthread_create and join
//    for(unsigned int i=0;i<P.size();i++) {
//        auto iter = cpn->mapJoin.find(P[i]);
//        if (iter != cpn->mapJoin.end()) {
//            auto pier = cpn->mapPlace.find(iter->second);
//            CPlace *pp = &cpn->getplacearr()[pier->second];
//            for (unsigned int j = 0; j < pp->get_consumer().size(); j++) {
//                if (pp->get_consumer()[j].arcType == control) {
//                    CTransition *tt = &cpn->gettransarr()[pp->get_consumer()[j].idx];
//                    if (!exist_in(T, tt->getid()))
//                        T.push_back(tt->getid());
//                    for (unsigned int k = 0; k < tt->get_producer().size(); k++)
//                        if (tt->get_producer()[k].arcType == control) {
//                            string temp_p = cpn->getplacearr()[tt->get_producer()[k].idx].id;
//
//                            if (!exist_in(P, temp_p))
//                                P.push_back(temp_p);
//                        }
//                }
//            }
//        }
//    }
    final_P = P;
    final_T = T;
}

vector<CTransition *> find_previous_T(string executed_place,CPN *cpn,vector<string> transitions,bool underprocedure)
{
    vector<CTransition *> searched,res;
    auto iter = cpn->mapPlace.find(executed_place);
    CPlace *executed_P = &cpn->getplacearr()[iter->second];
    for(unsigned int i=0;i<executed_P->get_producer().size();i++)
        searched.push_back(&cpn->gettransarr()[executed_P->get_producer()[i].idx]);

    for(unsigned int i=0;i<searched.size();i++)
    {
        if(exist_in(transitions,searched[i]->getid())) {
            res.push_back(searched[i]);
            continue;
        }
        for(unsigned int j=0;j<searched[i]->get_producer().size();j++)
        {
            CPlace *place = &cpn->getplacearr()[searched[i]->get_producer()[j].idx];

            if(place->getiscontrolP() == true)
            {
                for(unsigned int k=0;k<place->get_producer().size();k++) {
                    if (underprocedure) {
                        if (place->get_producer()[k].arcType == executed
                            || place->get_producer()[k].arcType == call_connect) {
                            CTransition *t = &cpn->gettransarr()[place->get_producer()[k].idx];
                            if (!exist_in(searched, t))
                                searched.push_back(t);
                        }
                    } else if (place->get_producer()[k].arcType == executed
                               || place->get_producer()[k].arcType == call_exit) {
                        CTransition *t = &cpn->gettransarr()[place->get_producer()[k].idx];
                        if (!exist_in(searched, t))
                            searched.push_back(t);
                    }
                }
            }
        }
    }
    return res;
}

void post_process(CPN *cpn,CPN *cpn_slice,vector<string> transitions)
{

    for(int i=0;i<cpn_slice->get_placecount();i++)
    {
        if(cpn_slice->getplacearr()[i].get_producer().size()==0)
//            if(cpn_slice->getplacearr()[i].expression == executed_P_name && cpn_slice->getplacearr()[i].get_producer().size()==0)
        {
            vector<CTransition*> trans_vec;
            string suffix = cpn_slice->getplacearr()[i].getExp();

            if(suffix.length()>end_suffix.length()
               && suffix.substr(suffix.length()-end_suffix.length()) == end_suffix)
                trans_vec = find_previous_T(cpn_slice->getplacearr()[i].getid(),cpn,transitions,false);
            else
                trans_vec = find_previous_T(cpn_slice->getplacearr()[i].getid(),cpn,transitions,true);
            for(unsigned int j=0;j<trans_vec.size();j++) {
                cpn_slice->Add_Arc(trans_vec[j]->getid(), cpn_slice->getplacearr()[i].getid(),controlflowArcExp(tid_str) , false, executed,beoverrided);
                //arccount INC auto


                CSArc csArc1,csArc2;

                csArc1.arcType = cpn_slice->getarcarr()[cpn_slice->get_arccount()-1].getArctype();
                csArc1.arc_exp.referencetree(cpn_slice->getarcarr()[cpn_slice->get_arccount()-1].getArcCT());

                csArc2.arcType = cpn_slice->getarcarr()[cpn_slice->get_arccount()-1].getArctype();
                csArc2.arc_exp.referencetree(cpn_slice->getarcarr()[cpn_slice->get_arccount()-1].getArcCT());

                csArc1.idx = cpn_slice->mapTransition.find(trans_vec[j]->getid())->second;
                csArc2.idx = i;
                cpn_slice->getplacearr()[i].get_producer().push_back(csArc1);
                cpn_slice->gettransarr()[csArc1.idx].get_consumer().push_back(csArc2);

//                auto arc = &cpn_slice->getarcarr()[cpn_slice->get_arccount()-1];
//                index_t T_idx = cpn_slice->mapTransition.find(trans_vec[j]->getid())->second;
//
//                cpn_slice->getplacearr()[i].get_producer().emplace_back(T_idx,arc->getexp(),arc->getArctype());
//                cpn_slice->gettransarr()[T_idx].get_consumer().emplace_back(i,arc->getexp(),arc->getArctype());

            }
        }
    }
}
