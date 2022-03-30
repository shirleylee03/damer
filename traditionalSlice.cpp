//
// Created on 2022/02/17.
//

//traditional cpn slicing

#include "cpn.h"

bool sameArcExp(CPN *cpn,string P, index_t T_index){
    auto pp = cpn->findP_byid(P);
    // auto tt = cpn->findT_byid(T);
    // index_t T_index = tt - cpn->gettransarr();
    auto consumer = pp->get_consumer();
    auto producer = pp->get_producer();

    string arcExpStr = "";
    for(auto item:consumer){
        if(item.idx == T_index){
            arcExpStr = item.arc_exp.getexp();
            break;
        }
    }
    if(arcExpStr == "")
        return false;
    for(auto item:producer){
        if(item.idx == T_index){
            if(item.arc_exp.getexp() == arcExpStr)
                return true;
            break;
        }
    }
    return false;
}

//initial criterion，crit_P
//T′ = {t ∈ T | ∃p ∈ Q : t ∈ (•p ∪ p•) : λ(p, t) != λ(t, p)}
//P′ = Q ∪ {•T′}
void initCriteria(CPN *cpn, const vector<string> crit_P, vector<string> &Ps,vector<string> &Ts){
    Ps.clear();
    Ts.clear();
    for(int i=0;i<crit_P.size();i++){
        auto pp = cpn->findP_byid(crit_P[i]);
        auto producerT = pp->get_producer();
        for(int j=0;j<producerT.size();j++){
            if(!sameArcExp(cpn,crit_P[i],producerT[j].idx)){
                auto tt = cpn->findT_byindex(producerT[j].idx);
                if(!exist_in(Ts,tt->getid()))
                    Ts.push_back(tt->getid());
            }
        }
        auto consumerT = pp->get_consumer();
        for(int j=0;j<consumerT.size();j++){
            if(!sameArcExp(cpn,crit_P[i],consumerT[j].idx)){
                auto tt = cpn->findT_byindex(consumerT[j].idx);
                if(!exist_in(Ts,tt->getid()))
                    Ts.push_back(tt->getid());
            }
        }
    }

    for(int i=0;i<Ts.size();i++){
        auto tt = cpn->findT_byid(Ts[i]);
        auto producerP = tt->get_producer();
        for(int j=0;j<producerP.size();j++){
            auto ppp = cpn->findP_byindex(producerP[j].idx);
            if(!exist_in(Ps,ppp->getid()))
                Ps.push_back(ppp->getid());
        }
    }
}

//slicing algorithm
//while ((∃p ∈ (P′ \ Pdone)) do
//  while (∃t ∈ (•p ∪ p•) \ T′) : λ(p, t) != λ(t, p)) do
//      P′ = P′ ∪ •t;
//      T′ = T′ ∪ {t};
//  end
//  Pdone = Pdone ∪ {p};
//end
void traditionalAPNSlice(CPN *cpn, vector<string> &Ps,vector<string> &Ts){
    vector<string> Pdone;
    for(int i=0;i<Ps.size();i++){
        if(!exist_in(Pdone,Ps[i])){
            auto pp = cpn->findP_byid(Ps[i]);
            auto producerT = pp->get_producer();
            auto consumerT = pp->get_consumer();
            for(int j=0;j<producerT.size();j++){
                if(!sameArcExp(cpn,Ps[i],producerT[j].idx)){
                    auto tt = cpn->findT_byindex(producerT[j].idx);
                    if(!exist_in(Ts,tt->getid()))
                        Ts.push_back(tt->getid());
                    auto producerP = tt->get_producer();
                    for(int k=0;k<producerP.size();k++)
                        if(!exist_in(Ps,cpn->findP_byindex(producerP[k].idx)->getid()))
                            Ps.push_back(cpn->findP_byindex(producerP[k].idx)->getid());
                }
            }
            for(int j=0;j<consumerT.size();j++){
                if(!sameArcExp(cpn,Ps[i],consumerT[j].idx)){
                    auto tt = cpn->findT_byindex(consumerT[j].idx);
                    if(!exist_in(Ts,tt->getid()))
                        Ts.push_back(tt->getid());
                    auto producerP = tt->get_producer();
                    for(int k=0;k<producerP.size();k++)
                        if(!exist_in(Ps,cpn->findP_byindex(producerP[k].idx)->getid()))
                            Ps.push_back(cpn->findP_byindex(producerP[k].idx)->getid());
                }
            }
            Pdone.push_back(Ps[i]);
        }
    }
}