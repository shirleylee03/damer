//
// Created on 2021/5/29.
//

#include "cpn_rg.h"

//extern CPN *cpnet;
extern string arr_suffix ,begin_suffix,end_suffix ,return_suffix ,call_suffix,tid_str,init_tid;
// rg

void Marking::init_marking(CPlace *place,NUM_t pc) {
    placecount = pc;
    mss = new MultiSet[pc];
    for(int i=0;i<pc;i++)
        mss[i]=place[i].getMultiSet();
}

void Marking::init_marking(Marking &marking) {
    placecount = marking.placecount;
    mss = new MultiSet[placecount];
    for(int i=0;i<placecount;i++)
        mss[i]=marking.mss[i];
}

bool Marking::operator==(Marking &marking) {
    if(placecount != marking.placecount)
        return false;
    for(int i=0;i<placecount;i++)
    {
        if(mss[i]==marking.mss[i])
            ;
        else
            return false;
    }
    return true;
}

vector<String_t > merge_vec(vector<String_t> vec1,vector<String_t> vec2){
    if(vec1.empty())
        return vec2;
    if(vec2.empty()){
        vec1.clear();
        return vec1;
    }
//    bool finded_flag = false;
    for(auto iter = vec1.begin();iter != vec1.end();){
        if(exist_in(vec2,*iter))
            iter++;
        else
            iter = vec1.erase(iter);
    }
    return vec1;
}


Binding* bindingcid(Product_t cid,SORTID sid,condition_tree_node *tokennode){
    condition_tree_node *node = tokennode->left;
    int offset = 0;
    Binding *newbinding,*result;
    result = new Binding;
    result->next = NULL;
    auto ps = sorttable.find_productsort(sid);
    auto sortid = ps.get_sortid();
    while(node->right){
        if(node->left->node_type == variable){
            if(node->node_name != tid_str) {
                newbinding = new Binding;
                newbinding->variable = node->left->value;
                if (sortid[offset].tid == Integer)
                    newbinding->value = (token)(new IntegerSortValue);
                else if (sortid[offset].tid == Real)
                    newbinding->value = (token)(new RealSortValue);
                else if (sortid[offset].tid == String)
                    newbinding->value = (token)(new StringSortValue);
                else {
                    cerr << "ERROR!variable can not be a productsort!" << endl;
                    throw "ERROR!variable can not be a productsort!";
                }
                Bucket bkt;
                cid[offset]->getcolor(bkt);
                newbinding->value->setcolor(bkt);
//                color_copy(sortid[offset].tid, sortid[offset].sid, cid[offset], newbinding->value);
                newbinding->next = result->next;
                result->next = newbinding;
            }
        }
        offset++;
        node = node->right;
    }
    if(node->node_type == variable){
        if(node->node_name != tid_str) {
            newbinding = new Binding;
            newbinding->variable = node->value;
            if (sortid[offset].tid == Integer)
                newbinding->value = (token)(new IntegerSortValue);
            else if (sortid[offset].tid == Real)
                newbinding->value = (token)(new RealSortValue);
            else if (sortid[offset].tid == String)
                newbinding->value = (token)(new StringSortValue);
            else {
                cerr << "ERROR!variable can not be a productsort!" << endl;
                throw "ERROR!variable can not be a productsort!";
            }
            Bucket bkt;
            cid[offset]->getcolor(bkt);
            newbinding->value->setcolor(bkt);
//            color_copy(sortid[offset].tid, sortid[offset].sid, cid[offset], newbinding->value);
            newbinding->next = result->next;
            result->next = newbinding;
        }
    }
    return result;
}

//binding variables through pattern matching
Binding* bindingToken(condition_tree_node *node,MultiSet *multiset,TID_t tid){


    SORTID sid;
    bool hasindex,hastid;
    Binding *result,*tmpbinding;
//    Tokens *tokens = multiset->tokenQ->next;

    result = new Binding;
    result->next = NULL;
    if(multiset->getcolorcount() == 0)
        return result;

    //binding Integer, for alloc and mutex cond
    if(multiset->gettid() == Integer){
        if(node->left->node_name[0] == '_' || isalpha(node->left->node_name[0])) {
            result->next = new Binding;
            result->next->next = NULL;
            result->next->variable = node->left->node_name;
            result->next->value = (token)(new IntegerSortValue);
            Bucket bkt;
            auto token = multiset->getonlytoken();
            token->getcolor(bkt);
            result->next->value->setcolor(bkt);
//            color_copy(Integer, 0, tokens->color, result->next->value);
            return result;
        }
        else
            return result;
    }


    sid = multiset->getsid();
    auto ps = sorttable.find_productsort(sid);
    hasindex = ps.get_hasindex();
    hastid = ps.get_hastid();

    int offset = 0;
    token cid;
    cid = (token)(new ProductSortValue(sid));//fpro.generateSortValue(sid);
    Integer_t index;
    condition_tree_node *indexnode,*tidnode;
    indexnode = tidnode = node->left;

    auto tokens = multiset->getmapTokens();
    if(hasindex && hastid){
        tidnode = tidnode->right->right;
        while(tidnode->right){
            indexnode = indexnode->right;
            tidnode = tidnode->right;
            offset++;
        }
        index = atoi(indexnode->left->value.c_str());
        auto token = tokens.begin();
        while(token != tokens.end()){
            Bucket cid_bkt,tid_bkt,index_bkt;
            token->first->getcolor(cid_bkt);
            cid->setcolor(cid_bkt);
            TID_t sub_tid;
            Integer_t sub_index;
            cid_bkt.pro[offset-1+1]->getcolor(index_bkt);
            cid_bkt.pro[offset-1+3]->getcolor(tid_bkt);
            sub_index = index_bkt.integer;
            sub_tid = tid_bkt.str;
            if(sub_index == index && tid == sub_tid)
            {
                tmpbinding = bindingcid(cid_bkt.pro,sid,node);
                Binding *end = tmpbinding;
                while(end->next)
                    end = end->next;
                end->next = result->next;
                result->next = tmpbinding->next;
                delete tmpbinding;
                break;
            }
            token++;
        }
        if(token == tokens.end())
            throw "ERROR!can't binding correctly!";
    }
    else if(hasindex){
        tidnode = tidnode->right->right;
        while(tidnode){
            indexnode = indexnode->right;
            tidnode = tidnode->right;
            offset++;
        }
        index = atoi(indexnode->left->value.c_str());
        auto token = tokens.begin();
        while(token!=tokens.end()){
            Bucket cid_bkt,index_bkt;
            token->first->getcolor(cid_bkt);
            Integer_t sub_index;
            cid_bkt.pro[offset-1+1]->getcolor(index_bkt);
            sub_index = index_bkt.integer;
            if(sub_index == index)
            {
                tmpbinding = bindingcid(cid_bkt.pro,sid,node);
                Binding *end = tmpbinding;
                while(end->next)
                    end = end->next;
                end->next = result->next;
                result->next = tmpbinding->next;
                delete tmpbinding;
                break;
            }
            token++;
        }
        if(token == tokens.end())
            throw "ERROR!can't binding correctly!";
    }
    else if(hastid){
        tidnode = tidnode->right;
        while(tidnode->right){
            tidnode = tidnode->right;
            offset++;
        }
        auto token = tokens.begin();
        while(token != tokens.end()){
            Bucket cid_bkt,tid_bkt;
            token->first->getcolor(cid_bkt);
            TID_t sub_tid;
            cid_bkt.pro[offset-1+2]->getcolor(tid_bkt);
            sub_tid = tid_bkt.str;
            if(sub_tid == tid)
            {
                tmpbinding = bindingcid(cid_bkt.pro,sid,node);
                Binding *end = tmpbinding;
                while(end->next)
                    end = end->next;
                end->next = result->next;
                result->next = tmpbinding->next;
                delete tmpbinding;
                break;
            }
            token++;
        }
        if(token == tokens.end())
            throw "ERROR!can't binding correctly!";
    }
    else{
        tidnode = tidnode->right;
        while(tidnode){
            tidnode = tidnode->right;
            offset++;
        }
        auto token = tokens.begin();
        while(token != tokens.end()){
            Bucket cid_bkt;
            token->first->getcolor(cid_bkt);
            {
                tmpbinding = bindingcid(cid_bkt.pro, sid, node);
                Binding *end = tmpbinding;
                while (end->next)
                    end = end->next;
                end->next = result->next;
                result->next = tmpbinding->next;
                delete tmpbinding;
                break;
            }
            token++;
        }
        if(token == tokens.end())
            throw "ERROR!can't binding correctly!";
    }
//    delete[] cid;
    return result;
}

vector<Binding *>get_bindings(CPN *cpn,CTransition *transition,const Marking &marking){
    //Binding control places first
    //transition's happen or not depends on control places

    vector<Binding *>bindings;
    Binding *binding,*tmpbinding;

    vector<TID_t > possible,tmp_vec;
    auto T_producer = transition->get_producer();
    for(int i=0;i<T_producer.size();i++){
        // if(T_producer[i].is_deleted)
        //     continue;
        index_t idx = T_producer[i].idx;
        condition_tree_node *root = T_producer[i].arc_exp.getroot();//transition->producer[i].arc_exp.root;
        auto pp = cpn->findP_byindex(idx);
        if(pp->getiscontrolP()) {
            if(pp->gettid() == TID_colorset){
                if(root->node_type == Token){
                    if(root->left->node_type == variable){
                        // arc_exp is 1'tid
                        string var = root->left->value;
//                        auto vv = cpn->findVar_byname(var);
//                        auto iter = cpn->mapVariable.find(var);
//                        if(iter != cpn->mapVariable.end()){
                            TID_t cid;
//                            Tokens *token = marking.mss[idx].tokenQ->next;
                            auto tokens = marking.mss[idx].getmapTokens();
                            auto token = tokens.begin();
                            tmp_vec.clear();
                            while(token != tokens.end()) {
                                Bucket bkt;
                                token->first->getcolor(bkt);
                                if(TID_colorset != String)
                                    throw "here we assume TID_colorset is String!";
                                tmp_vec.push_back(bkt.str);
                                token++;
                            }
                            possible = merge_vec(possible,tmp_vec);
                            if(possible.empty())
                                return bindings;//empty return
//                        }
//                        else {
//                            cout << "cpn_rg.cpp can't find variable" << endl;
//                            exit(-1);
//                        }
                    }
                    else{
                        // arc_exp is color or case
//                        TID_t cid1,cid2;
//                        Tokens *token = marking.mss[idx].tokenQ->next;
                        MultiSet tmp_ms;
                        bool finded = false;
//                        tmp_ms.tid = TID_colorset;
//                        tmp_ms.sid = 0;
                        auto tokens = marking.mss[idx].getmapTokens();
                        auto token = tokens.begin();

                        cpn->CT2MS(T_producer[i].arc_exp,tmp_ms,TID_colorset,0);
                        while(token != tokens.end()) {
                            Bucket bkt1,bkt2;
                            token->first->getcolor(bkt1);
//                            tmp_ms.Exp2MS(cpn,root,0,0,false);
//                            tmp_ms.tokenQ->next->color->getColor(cid2);
                            tmp_ms.getonlytoken()->getcolor(bkt2);
                            if(bkt1 == bkt2) {
                                finded = true;
                                break;
                            }
                            token++;
                        }
                        if(finded)
                            continue;
                        else
                            return bindings;
                    }
                }
                else
                    throw "ERROR!control places' read arc just have one token!";
            }
            else if(pp->gettid() == Integer){
                //mutex cond
//                Tokens *token = marking.mss[idx].tokenQ->next;
                auto tokens = marking.mss[idx].getmapTokens();
                auto token = tokens.begin();
                Integer_t cid_arc = atoi(root->left->node_name.c_str());
                Bucket bkt_arc;
                bkt_arc.tid = Integer;
                bkt_arc.integer = cid_arc;
                while(token!=tokens.end()){
                    Bucket bkt_place;
                    token->first->getcolor(bkt_place);
                    if(bkt_arc == bkt_place)
                        break;
                    token++;
                }
                if(token == tokens.end())
                    return bindings;//empty return
            }
        }
    }

    for(int i=0;i<possible.size();i++){
        Bucket bkt_tid;
        String_t tid = possible[i];
        bkt_tid.tid = String;
        bkt_tid.str = tid;
        binding = new Binding;
        binding->next = new Binding;
        binding->next->variable = tid_str;
        binding->next->value = (token)(new StringSortValue);
        binding->next->value->setcolor(bkt_tid);
        binding->next->next = NULL;
        auto T_producer = transition->get_producer();
        for(unsigned int j=0;j<T_producer.size();j++) {
            index_t idx = T_producer[j].idx;
            auto pp = cpn->findP_byindex(idx);
            if (!pp->getiscontrolP()) {
                condition_tree_node *root = T_producer[j].arc_exp.getroot();
                while (root) {
                    if(root->node_type == CaseOperator && !root->right){
                        root = root->left;
                        continue;
                    }
                    if (root->node_type == Token) {

                        tmpbinding = bindingToken(root, &marking.mss[idx], tid);
                        if(tmpbinding->next) {
                            Binding *end = tmpbinding->next;
                            while (end->next)
                                end = end->next;
                            end->next = binding->next;
                            binding->next = tmpbinding->next;
                            delete tmpbinding;
                        }
                    }
                    if (root->left->node_type == Token) {
                        tmpbinding = bindingToken(root->left, &marking.mss[idx], tid);
                        if(tmpbinding->next) {
                            Binding *end = tmpbinding->next;
                            while (end->next)
                                end = end->next;
                            end->next = binding->next;
                            binding->next = tmpbinding->next;
                            delete tmpbinding;
                        }
                    }
                    root = root->right;
                }
            }
        }
        bindings.push_back(binding);

    }

    return bindings;
}

void BindingVariable(const Binding *binding,CPN *cpn){
    Binding *tmp = binding->next;
    while(tmp){
        auto vv = cpn->findVar_byname(tmp->variable);
//        auto viter = cpn->mapVariable.find(tmp->variable);
//        if(viter == cpn->mapVariable.end()){
//            cerr<<"ERROR!BindingVariable failed!"<<endl;
//            exit(-1);
//        }
//        color_copy(cpn->vartable[viter->second].tid,cpn->vartable[viter->second].sid,tmp->value,cpn->vartable[viter->second].value);
        Bucket bkt;
        tmp->value->getcolor(bkt);
        vv->setvcolor(bkt);
        tmp = tmp->next;
    }
}

bool is_Fireable(CTransition *transition,CPN *cpn,vector<Binding *>&bindings,const Marking &marking)
{
    //1.get bindings

    bindings = get_bindings(cpn,transition,marking);
    if(bindings.empty())
        return false;

    //2.judge guard
    if (transition->get_hasguard()) {

        for (auto iter = bindings.begin(); iter != bindings.end();) {
            Integer_t res;
            MultiSet ms;

//            ms.tid = Integer;
//            ms.sid = 0;
            BindingVariable(*iter,cpn);
//            ms.Exp2MS(cpn,transition->guard.root,0,0,false);
//            ms.tokenQ->next->color->getColor(res);
            cpn->CT2MS(transition->get_guard(),ms,Integer,0);
            Bucket bkt;
            ms.getonlytoken()->getcolor(bkt);
            res = bkt.integer;
            if(res == 0)
                iter = bindings.erase(iter);
            else
                iter++;
        }
    }

    //3.judge producers
//    for(int i=0;i<transition->producer.size();i++)
//    {
//        index_t idx = transition->producer[i].idx;
//        if(!transition->producer[i].onlydot) {
////            MultiSet ms;
////            ms.sid = marking.mss[idx].sid;
////            ms.tid = marking.mss[idx].tid;
////            cpn->CT2MS(transition->producer[i].arc_exp, ms);
//            if (cpn->place[idx].control_P == false)
//                continue;
//            if (marking.mss[idx] >= ms);
//            else
//                return false;
//        }
//        else
//        {
//            if(marking.mss[idx].tokenQ->next==NULL)
//                return false;
//            else if(marking.mss[idx].tokenQ->next->tokencount==0)
//                return false;
//        }
//    }

    if(bindings.empty())
        return false;
    return true;
}

void RG_NODE::get_FireTranQ(CPN *cpn) {

    if(tranQ_obtained)
        return;
    tranQ_obtained = true;
    auto transPriNum = cpn->get_transPriNum();

///先判断优先变迁
    for(int i=0;i<transPriNum.size();i++)
    {

        vector<Binding *>bindings;
        auto tt = cpn->findT_byindex(transPriNum[i]);
        if(tt->isabort)
            continue;
        if(is_Fireable(tt,cpn,bindings,marking)) {
            firenum++;
//            tranQ->insert(&cpn->transition[i]);
            FireTranQ *tmptranQ = new FireTranQ;
            tmptranQ->bindings = bindings;
            tmptranQ->transition = tt;
            tmptranQ->next = tranQ->next;
            tranQ->next = tmptranQ;

        }
    }
    for(int i=0;i<cpn->get_transcount();i++)
    {
        if(exist_in(transPriNum,short(i)))
            continue;
        vector<Binding *>bindings;
        auto tt = cpn->findT_byindex(i);
        if(tt->isabort)
            continue;
        if(is_Fireable(tt,cpn,bindings,marking)) {
            firenum++;
//            tranQ->insert(&cpn->transition[i]);
            FireTranQ *tmptranQ = new FireTranQ;
            tmptranQ->bindings = bindings;
            tmptranQ->transition = tt;
            tmptranQ->next = tranQ->next;
            tranQ->next = tmptranQ;

        }
    }
}

index_t RG_NODE::Hash() {
    index_t hv=0;
    for(int i=0;i<marking.placecount;i++)
    {
//        marking.mss[i].Hash();
        hv += i * marking.mss[i].Hash() + i;
    }

    return hv;
}

bool RG_NODE::fireable(string transname) {
    FireTranQ *p = tranQ->next;
    while (p) {
        if(p->transition->getid() == transname)
            return true;
        p=p->next;
    }
    return false;
}

//void FireTranQ::insert(CTransition *transition) {
//    FireTranQ *temp = next,*newnode;
//    newnode = new FireTranQ;
//    newnode->transition = transition;
//    next = newnode;
//    newnode->next = temp;
//}
//unsigned long count=0;
void RG::init(CPN *cpn) {

//    count=0;
    init_node = new RG_NODE;
    init_node->marking.init_marking(cpn->getplacearr(),cpn->get_placecount());
    rgnodetable = new RG_NODE*[CPNRGTABLE_SIZE]();
    init_node->get_FireTranQ(cpn);
    addRGNode(init_node);

}

void Marking_after_fire(Marking &marking,CTransition *transition,Binding *binding,CPN *cpn)
{
//    clock_t start,end;
//    start = clock();
    BindingVariable(binding,cpn);
//    cout<<clock()-start<<endl;
//    start = clock();
    //1.producer
    auto T_producer = transition->get_producer();
    for(unsigned int i=0;i<T_producer.size();i++) {
        // if(T_producer[i].is_deleted)
        //     continue;
        MultiSet ms;
        int up;
        index_t idx = T_producer[i].idx;
        auto pp = cpn->findP_byindex(idx);
//        ms.tid = cpn->place[idx].initMarking.tid;
//        ms.sid = cpn->place[idx].initMarking.sid;
//        cpn->CT2MS(transition->producer[i].arc_exp,ms);

//        if (ms.gettid() == dot) {
////            Tokens *token = new Tokens;
////            token->tokencount = 1;
//            auto dottoken = (token)(new DotSortValue);
//            ms.generateFromToken(dottoken);
//            marking.mss[idx].MINUS(ms);
//        } else {
//            if (ms.gettid() == productsort) {
//                auto ps = sorttable.find_productsort(ms.getsid());
//                up = ps.get_sortnum() - 1;
//            }
//            else
//                up = 0;
//            start = clock();
            cpn->CT2MS(T_producer[i].arc_exp,ms,pp->gettid(),pp->getsid());
//            cout << "MINUS time:"<<clock()-start<<endl;

            marking.mss[idx].MINUS(ms);

//        }
        marking.mss[idx].merge();
    }

//    cout<<clock()-start<<endl;
//    start = clock();
    //2.consumer
    auto T_consumer = transition->get_consumer();
    for(unsigned int i=0;i<T_consumer.size();i++) {
        // if(T_consumer[i].is_deleted)
        //     continue;
//        start = clock();
        MultiSet ms;
        int up;
        index_t idx = T_consumer[i].idx;
        auto pp = cpn->findP_byindex(idx);
//        ms.tid = cpn->place[idx].initMarking.tid;
//        ms.sid = cpn->place[idx].initMarking.sid;
//        cpn->CT2MS(transition->consumer[i].arc_exp,*ms);
//        if (ms.gettid() == dot) {
////            Tokens *token = new Tokens;
////            token->tokencount = 1;
//            auto dottoken = (token)(new DotSortValue);
//            ms.generateFromToken(dottoken);
//
//            marking.mss[idx].PLUS(ms);
//        } else {
//            if (ms.tid == productsort)
//                up = sorttable.productsort[ms.sid].sortnum - 1;
//            else
//                up = 0;
//            ms.Exp2MS(cpn, transition->consumer[i].arc_exp.root, 0, up,false);
            cpn->CT2MS(T_consumer[i].arc_exp,ms,pp->gettid(),pp->getsid());
//            cout << "first phrase:"<<clock()-start<<endl;
            //CT2MS may have 0 count token
            ms.merge();
            marking.mss[idx].PLUS(ms);
//            cout << "second phrase:"<<clock()-start<<endl;
//        }
        marking.mss[idx].merge();
    }
//    cout<<clock()-start<<endl;
}

clock_t t1,t2,t3,t4;
void RG::createNode(RG_NODE *node,CPN *cpn) {

    FireTranQ *tranQ = node->tranQ->next;
    while(tranQ)
    {

        for(unsigned int i=0;i<tranQ->bindings.size();i++) {
            clock_t start,end;
//            start = clock();
            RG_NODE *newnode = new RG_NODE;
            newnode->fathernum = node->num;
            newnode->last_tran = tranQ->transition->getid();
//            if(tranQ->transition->isabort)
//                continue;
            newnode->marking.init_marking(node->marking);
            newnode->next = NULL;
//            end = clock();
//            t1 += end-start;
//            cout<<clock()-start<<endl;

//            start = clock();
            Marking_after_fire(newnode->marking, tranQ->transition, tranQ->bindings[i], cpn);
//            tranQ->freebindings();//释放绑定占用的内存
//            end = clock();
//            t2 += end-start;
//            cout<<"one round:"<<end-start<<endl;

//                return;
//            start = clock();
            auto exist = nodeExist(newnode);
//            end = clock();
//            t3 += end-start;
//            cout<<"judge exist:"<<end-start<<endl;
            if (exist) {
                node->dashNextNode.push_back(exist->num);
                delete newnode;
            }
            else {
//                start = clock();

                addRGNode(newnode);
                if(newnode->num >= 100000)
                    break;
                newnode->get_FireTranQ(cpn);
//                cout << "count = " << count++ << endl;
//                count++;
//                end = clock();
//                t4 += end-start;
//                cout<<clock()-start<<endl;
                createNode(newnode, cpn);
            }
        }
        auto lastTraQ = tranQ;
        tranQ = tranQ->next;
        lastTraQ->freebindings();
    }
}

RG_NODE* RG::nodeExist(RG_NODE *node) {
//    clock_t start,end;
//    start = clock();
    index_t hv = node->Hash() % CPNRGTABLE_SIZE;
//    end = clock();
//    cout<<"hash time:"<<end-start<<endl;
    RG_NODE *exist = rgnodetable[hv];
//    int hash_count = 0;
    while(exist)
    {
        if(node->marking == exist->marking)
            return exist;
        exist = exist->next;
//        hash_count++;
    }
//    cout<<"hash_count:"<<hash_count<<endl;
    return NULL;
}
//map<int,short> conflict_count;
void RG::addRGNode(RG_NODE *node) {

    index_t hv = node->Hash() % CPNRGTABLE_SIZE;
    if(rgnodetable[hv] == NULL) {
//        auto iter = conflict_count.find(hv);
//        if(iter == conflict_count.end())
//            conflict_count.insert(make_pair(hv,1));
//        else
//            throw "error";
//        cout<<"hash success:"<<count++<< " values:"<<hv <<endl;
//        rgnodetable[hv] = new RG_NODE;
        rgnodetable[hv] = node;
        rgnodetable[hv]->next = NULL;
        rgnodevec.push_back(node);
        node->num = node_num;
        node_num++;
        return;
    }

//    cout<<"hash conflict:"<<count++<< " values:"<<iter->first<<" count:"<<iter->second<<endl;
    node->next = rgnodetable[hv];
    rgnodetable[hv] = node;
    rgnodevec.push_back(node);
    node->num = node_num;
    node_num++;
//    cout<<node_num<<endl;
}

void RG::GENERATE(CPN *cpn) {
    createNode(init_node,cpn);
}

void RG::print_RG(string filename,CPN *cpn) {
    ofstream fout;
    RG_NODE *node;
    type tid;
    SORTID sid;
//    Tokens *tokenQ;
    fout.open(filename, ios::out);
    fout << "total node:" << node_num << " nodes" << endl;
    fout << "show them below" << endl;
    fout << endl;
    for (int i = 0; i < node_num; i++) {

        node = rgnodevec[i];
        fout << "node:" << node->num << endl;
        cout << "node:" << node->num << endl;
        for (int j = 0; j < node->marking.placecount; j++) {
            fout << cpn->getplacearr()[j].getid() << ":";
            cout << cpn->getplacearr()[j].getid() << ":";
//            tokenQ = node->marking.mss[j].tokenQ->next;
            auto tokens = node->marking.mss[j].getmapTokens();
            auto tk = tokens.begin();
            tid = node->marking.mss[j].gettid();
            sid = node->marking.mss[j].getsid();
            fout << "(" << cpn->getplacearr()[j].getExp() << ",";
            cout << "(" << cpn->getplacearr()[j].getExp() << ",";
            while (tk != tokens.end()) {
                fout << "[" << tk->second << ",";
                cout << "[" << tk->second << ",";
                if (tid == productsort) {
                    auto ps = sorttable.find_productsort(sid);
                    int psnum = ps.get_sortnum();
                    auto sortid = ps.get_sortid();
                    Bucket bkt_cid;
//                    cid = fpro.generateSortValue(sid);
                    tk->first->getcolor(bkt_cid);
                    cout << "{";
                    fout << "{";
                    for (unsigned int k = 0; k < psnum; k++) {
                        if (sortid[k].tid == Integer) {
                            Bucket subcid;
                            bkt_cid.pro[k]->getcolor(subcid);
                            cout << subcid.integer << ",";
                            fout << subcid.integer << ",";
                        } else if (sortid[k].tid == Real) {
                            Bucket subcid;
                            bkt_cid.pro[k]->getcolor(subcid);
                            cout << subcid.real << ",";
                            fout << subcid.real << ",";
                        } else if (sortid[k].tid == String) {
                            Bucket subcid;
                            bkt_cid.pro[k]->getcolor(subcid);
                            cout << subcid.str << ",";
                            fout << subcid.str << ",";
                        }
                        else
                            throw "we don't support member structure in structure for now!";
                    }
                    cout << "}";
                    fout << "}";
//                    delete[]cid;
                } else if (tid == Integer) {
                    Bucket cid;
                    tk->first->getcolor(cid);
                    fout << cid.integer;
                    cout << cid.integer;
                } else if (tid == Real) {
                    Bucket cid;
                    tk->first->getcolor(cid);
                    fout << cid.real;
                    cout << cid.real;
                } else if (tid == String) {
                    Bucket cid;
                    tk->first->getcolor(cid);
                    fout << cid.str;
                    cout << cid.str;
                } else if (tid == dot) {
//                   String_t cid;
//                   tokenQ->color->getColor(cid);
                    fout << "dot";
                    cout << "dot";
                }
                fout << "] ";
                cout << "] ";
                tk++;
            }

            fout << ")    ";
            cout << ")    ";
        }

        fout << endl;
        cout << endl;

        fout << "successor node:";
        cout << "successor node:";



        FireTranQ *tranQ = node->tranQ->next;
        if (tranQ) {
            while (tranQ) {

                fout << tranQ->transition->getid() << " , ";
                cout << tranQ->transition->getid() << " , ";
                tranQ = tranQ->next;
            }
        } else {
            fout << "NULL";
            cout << "NULL";
        }
        cout << endl;
        fout << endl;


        fout<< "dashNextNode:";
        for(int i=0;i<node->dashNextNode.size();i++)
            fout << node->dashNextNode[i]<<",";
        fout<<endl;

        fout << "father node:" << node->fathernum << "    last Tran:" << node->last_tran;
        cout << "father node:" << node->fathernum << "    last Tran:" << node->last_tran;

        cout << endl << endl;
        fout << endl << endl;
    }

    fout.close();
}