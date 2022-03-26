//
// Created on 2020/10/10.
//
#include "product.h"

index_t CHashtable::hashfunction(CPN_Product *node) {
    index_t hashvalue;
    index_t size = CHASHSIZE-1;
    hashvalue = node->RGname_ptr->Hash();
    hashvalue = hashvalue & size;

    index_t Prohashvalue = hashvalue + node->BAname_id;
    Prohashvalue = Prohashvalue & size;
    return Prohashvalue;
}

CPN_Product *CHashtable::search(CPN_Product *node) {
    index_t idx = hashfunction(node);
    CPN_Product *p = table[idx];
    while(p!=NULL)
    {
        if(p->BAname_id==node->BAname_id && p->RGname_ptr==node->RGname_ptr)
            return p;
        p=p->hashnext;
    }
    return NULL;
}

void CHashtable::remove(CPN_Product *node) {
    index_t idx = hashfunction(node);
    CPN_Product *p=table[idx];
    CPN_Product *q;
    if(p->BAname_id==node->BAname_id && p->RGname_ptr==node->RGname_ptr)
    {
        q=p->hashnext;
        table[idx] = q;
        delete p;
        return;
    }

    q=p;
    p=p->hashnext;
    while(p!=NULL)
    {
        if(p->BAname_id==node->BAname_id && p->RGname_ptr==node->RGname_ptr)
        {
            q->hashnext=p->hashnext;
            delete p;
            return;
        }

        q=p;
        p=p->hashnext;
    }
    cerr<<"Couldn't delete from hashtable!"<<endl;
}

void CHashtable::resetHash() {
    for(int i=0;i<CHASHSIZE;++i)
    {
        if(table[i]!=NULL) {
            CPN_Product *p=table[i];
            CPN_Product *q;
            while(p!=NULL) {
                q=p->hashnext;
                delete p;
                p=q;
            }
        }
    }
    memset(table,NULL,sizeof(CPN_Product *)*CHASHSIZE);
    nodecount = 0;
    hash_conflict_times = 0;
}

void CHashtable::insert(CPN_Product *node) {
    index_t idx = hashfunction(node);
    if(table[idx]!=NULL)
        hash_conflict_times++;
    node->hashnext = table[idx];
    table[idx] = node;
    nodecount++;
}

CPstack::CPstack() {
    toppoint = 0;
    hashlink = new index_t[CHASHSIZE];
    memset(hashlink,UNREACHABLE,sizeof(index_t)*CHASHSIZE);
    mydata = new CPN_Product* [CHASHSIZE];
    memset(mydata,NULL,sizeof(CPN_Product*)*CHASHSIZE);
}

CPstack::~CPstack() {
    delete [] hashlink;
    for(int i=0;i<toppoint;++i)
    {
        if(mydata[i]!=NULL)
            delete mydata[i];
    }
    delete [] mydata;
}

index_t CPstack::hashfunction(CPN_Product *node) {
    index_t hashvalue = node->RGname_ptr->Hash();
    index_t size = CHASHSIZE-1;
    hashvalue = hashvalue & size;
    index_t  Prohashvalue = (hashvalue+node->BAname_id)&size;
    return Prohashvalue;
}

CPN_Product *CPstack::top() {
    return mydata[toppoint-1];
}

CPN_Product *CPstack::pop() {
    CPN_Product *popitem = mydata[--toppoint];
    index_t hashpos = hashfunction(popitem);
    hashlink[hashpos] = popitem->stacknext;
    mydata[toppoint] = NULL;
    return popitem;
}

CPN_Product *CPstack::search(CPN_Product *node) {
    index_t hashpos = hashfunction(node);
    index_t pos = hashlink[hashpos];
    CPN_Product *p;
    while(pos!=UNREACHABLE)
    {
        p=mydata[pos];
        if(p->BAname_id==node->BAname_id && p->RGname_ptr==node->RGname_ptr)
        {
            return p;
        }
        pos = p->stacknext;
    }
    return NULL;
}

int CPstack::push(CPN_Product *node) {
    if(toppoint >= CHASHSIZE) {
        return ERROR;
    }
    index_t hashpos = hashfunction(node);
    node->stacknext = hashlink[hashpos];
    hashlink[hashpos] = toppoint;
    mydata[toppoint++] = node;
    return OK;
}

NUM_t CPstack::size() {
    return toppoint;
}

bool CPstack::empty() {
    return (toppoint==0)?true:false;
}

void CPstack::clear() {
    toppoint = 0;
    for(int i=0;i<toppoint;++i)
    {
        if(mydata[i]!=NULL) {
            delete mydata[i];
        }
    }
    memset(mydata,NULL, sizeof(CPN_Product*)*CHASHSIZE);
    memset(hashlink,UNREACHABLE,sizeof(index_t)*CHASHSIZE);
}

CPN_Product_Automata::CPN_Product_Automata(CPN *net,StateBuchi *sba, RG *rg) {
    cpn = net;
    this->ba = sba;
    this->rg = rg;
    result = true;
}

void CPN_Product_Automata::GetInitNode() {
//遍历buchi自动机的所有状态，找到所有初始状态
    for(int i=0; i<ba->vex_num; i++)
    {
        if(ba->vertics[i].id == -1)
            continue;
        if(ba->vertics[i].initial)
        {
            //如果是初始状态，就和可达图的初始状态生成交自动机的初始状态
            if(isLabel(rg->init_node, i))
            {
                //生成交自动机初始状态
                CPN_Product N;
                N.BAname_id = i;
                N.RGname_ptr = rg->init_node;
                //加入初始状态数组中
                initial_status.push_back(N);
            }
        }
    }
}

/*bool Product_Automata::isLabel(RGNode *state, int sj)
 * function: 判断可达图的一个状态和buchi自动机的一个状态能否合成交状态
 * in: state,可达图状态指针，指向要合成的可达图状态
 * sj,buchi自动机状态在邻接表中的序号
 * out: true(可以合成交状态) or false(不能合成状态)
 * */
bool CPN_Product_Automata::isLabel(RG_NODE *state, int sj) {

    //取出自动机状态中的label
    string str = ba->vertics[sj].label;
    if(str == "true")
        return true;

    bool mark = false;
    while (1)
    {
        int pos = str.find_first_of("&&");

        if (pos == string::npos)     //最后一个原子命题
        {
            //cout<<str;
            switch (judgeF(str)) {
                case LTLC: {
                    mark = handleLTLC(str, state);
                    if(mark == false)
                        return false;
                    break;
                }
                case LTLF: {
                    /*a && b && c:
                    * true: 都为true
                    * false： 只要有一个为false
                    * */
                    mark = handleLTLF(str, state);
                    if(mark == false)
                        return false;
                    break;
                }
                case LTLV: {
                    mark = handleLTLV(str,state);
                    if(mark == false)
                        return false;
                    break;
                }
            }
            break;
        }

        string subprop = str.substr(0, pos);    //取出一个原子命题
        //cout<<subprop<<" ";

        switch (judgeF(subprop))   //当前公式为F类型公式
        {
            case LTLC: {
                /*a && b && c:
                * true: 都为true
                * false： 只要有一个为false
                * */
                mark = handleLTLC(subprop, state);
                if (mark == false)
                    return false; //有一个原子命题不成立
                break;
            }
            case LTLF: {
                /*a && b && c:
                * true: 都为true
                * false： 只要有一个为false
                * */
                mark = handleLTLF(subprop, state);
                if (mark == false) {
                    //当前原子命题不成立
                    return false;
                }
                break;
            }
            case LTLV: {
                mark = handleLTLV(subprop,state);
                if(mark == false)
                    return false;
                break;
            }
        }
        //cout<<endl;
        str = str.substr(pos + 2, str.length() - pos -2);
    }
    return true;
}

/*function:判断是否为Fireability公式
 * */
int CPN_Product_Automata::judgeF(string s) {
    int pos = s.find("token-count");
    if (pos != string::npos)
        return LTLC;
    pos = s.find("token-value");
    if (pos != string::npos)
        return LTLV;
    return LTLF;
}

/*bool Product_Automata::handleLTLF(string s, RGNode *state)
 * function:判断F类型中的一个原子命题在状态state下是否成立
 * in: s是公式的一小部分，一个原子命题； state，状态
 * out: true(成立) or false(不成立)
 * */
bool CPN_Product_Automata::handleLTLF(string s, RG_NODE *state) {
    if(s[0] == '!') //前面带有'!'的is-fireable{}
    {
        /*!{t1 || t2 || t3}：
         * true：t1不可发生 并且 t2不可发生 并且 t3不可发生
         * false： 只要有一个能发生
         * */
        s = s.substr(2,s.length()-2); //去掉“!{”
        while(1)
        {
            int pos = s.find_first_of(",");
            if (pos < 0)
                break;
            string subs = s.substr(0, pos);            //取出一个变迁

            if(state->fireable(subs))
                return false;

            s = s.substr(pos + 1, s.length() - pos);
        }
        return true;

    } else{         //单纯的is-fireable{}原子命题
        /*{t1 || t2 || t3}:
	     * true: 只要有一个能发生
	     * false: 都不能发生
	     * */
        s = s.substr(1, s.length() - 1);//去掉‘{’

        while(1)
        {
            int pos = s.find_first_of(",");
            if (pos < 0)
                break;
            string subs = s.substr(0, pos);  //取出一个变迁

            if(state->fireable(subs))
                return true;

            //走到这里表示取出来的变迁不可发生
            s = s.substr(pos + 1, s.length() - pos);
        }
        //走到这里表示所有的变迁都不能发生
        return false;
    }
}

/*bool Product_Automata::handleLTLC(string s, RGNode *state)
 * function: 判断C类型公式中的一个原子命题s在状态state下是否成立
 * in: s,原子命题； state,状态
 * out: true(成立) or false(不成立)
 * */
bool CPN_Product_Automata::handleLTLC(string s, RG_NODE *state) {
    short int front_sum, latter_sum;     //前半部分和 后半部分和
    if(s[0] == '!')
    {
        /*!(front <= latter)
	     * true:front > latter
	     * false:front <= latter
	     * */
        s = s.substr(2, s.length() - 2);   //去除"!{"
        handleLTLCstep(front_sum, latter_sum, s, state);
        if(front_sum <= latter_sum)
            return false;
        else
            return true;
    }
    else {
        /*(front <= latter)
         * true:front <= latter
         * false:front > latter
         * */
        s = s.substr(1, s.length() - 1);   //去除"{"
        handleLTLCstep(front_sum, latter_sum, s, state);
        if(front_sum <= latter_sum)
            return true;
        else
            return false;
    }
}

/*void handleLTLCstep(int &front_sum, int &latter_sum, string s, RGNode *state)
 * function:计算在状态state下，C公式"<="前面库所的token和front_sum和后面库所的token和latter_sum
 * in: s,公式； state,状态
 * out: front_sum前半部分的和, latter_sum后半部分的和
 * */
void CPN_Product_Automata::handleLTLCstep(short int &front_sum, short int &latter_sum, string s, RG_NODE *state) {
    if(s[0] == 't')   //前半部分是token-count的形式
    {
        int pos = s.find_first_of("<=");           //定位到<=前
        string s_token = s.substr(12, pos - 13);   //去除"token-count(" ")"  ֻ只剩p1,p2,
        front_sum = sumtoken(s_token, state);      //计算token和

        //计算后半部分的和
        s = s.substr(pos + 2, s.length() - pos - 2); //取得后半部分，若是常数 则是常数+}

        if(s[0] == 't')   //后半部分是token-count
        {
            string s_token = s.substr(12, s.length() - 14);
            latter_sum = sumtoken(s_token, state);
        }
        else{             //后半部分是常数
            s = s.substr(0, s.length() - 1);        //去除后面的}
            latter_sum = atoi(s.c_str());
        }
    }
    else{             //前半部分是常数，那后半部分肯定是token-count
        //处理前半部分  现有的串 是从常数开始的
        int pos = s.find_first_of("<=");//定位到<=,取其前半部分
        string num = s.substr(0, pos);//取出常数
        front_sum = atoi(num.c_str());
        //	 cout<< " "<<front_sum<<" ";

        //处理后半部分
        s = s.substr(pos + 14, s.length() - pos - 16);
        //	 cout<<" "<<s<<" ";
        latter_sum = sumtoken(s, state);
    }
}

/*int Product_Automata::sumtoken(string s, RGNode *state)
 * function: 计算在状态state下，某些库所的token和
 * in: s:p1,p2,p3,p4,这样的库所名字字符串；state，状态
 * out: 库所的token和
 * */
short int CPN_Product_Automata::sumtoken(string s, RG_NODE *state) {
    short int sum = 0;
    while(1)
    {
        int pos = s.find_first_of(",");
        if (pos == string::npos)
            break;
        string subs = s.substr(0, pos);        //取得一个p1

        map<string,index_t>::iterator piter;
        piter = cpn->mapPlace.find(subs);
        sum += state->marking.mss[piter->second].tokennum();

        //将前面的用过的P1去除 从p2开始作为新的s串
        s = s.substr(pos + 1, s.length() - pos);
    }
    return sum;
}

void CPN_Product_Automata::GetProduct() {
    //如果还未得到rg的初始状态，那么就生成他的初始状态
    if(rg->init_node == NULL){
        rg->init(cpn);
    }

    //生成交自动机的初始状态
    GetInitNode();

    //从初始状态开始搜索
    vector<CPN_Product>::iterator it_P;
    for(it_P = initial_status.begin(); it_P != initial_status.end(); it_P++)
    {
        CPN_Product *init = new CPN_Product;
        init->RGname_ptr = (*it_P).RGname_ptr;
        init->BAname_id = (*it_P).BAname_id;
//        dfs1(init);
        Tarjan(init);
    }
}

int sum=0;
void CPN_Product_Automata::Tarjan(CPN_Product *pnode) {
    if(!result)
        return;
    pnode->lowlink = pnode->id = cstack.toppoint;
    cstack.push(pnode);
    if(ba->vertics[pnode->BAname_id].accepted)
        astack.push(pnode);
    ArcNode *pba = ba->vertics[pnode->BAname_id].firstarc;

    /**/
    if(!pnode->RGname_ptr->tranQ_obtained) {cerr<<"fireset hasn't been calculated"<<endl;}
    FireTranQ *tranQ;

    while(pba) {
        if(!result)
            return;
        if(pnode->RGname_ptr->firenum == 0) {
            /*RG doesn't have a successor node*/
            RG_NODE *rgseed = pnode->RGname_ptr;
            if(isLabel(rgseed,pba->destination)) {
                CPN_Product *qs = new CPN_Product;
                qs->RGname_ptr = rgseed;
                qs->BAname_id = pba->destination;

                CPN_Product *existpos = cstack.search(qs);
                if(existpos!=NULL) {
                    /*this node exists in statck*/
                    pnode->lowlink = (pnode->lowlink < existpos->lowlink)?pnode->lowlink:existpos->lowlink;
                    if(!astack.empty()) {
                        CPN_Product *cur_accp = astack.top();
                        if(cur_accp->id >= existpos->id) {
                            result = false;
                            return;
                        }
                    }
                }
                else if(h.search(qs)==NULL) {
                    Tarjan(qs);
                    pnode->lowlink = (pnode->lowlink<qs->lowlink)?pnode->lowlink:qs->lowlink;
                }
            }
        }
        else {
            for(tranQ=pnode->RGname_ptr->tranQ->next;tranQ;tranQ=tranQ->next)
            {
                for(unsigned int k=0;k<tranQ->bindings.size();k++) {
                    if(!result)
                        return;
                    RG_NODE *newnode = new RG_NODE;
                    newnode->marking.init_marking(pnode->RGname_ptr->marking);

                    Marking_after_fire(newnode->marking, tranQ->transition, tranQ->bindings[k], cpn);

                    RG_NODE *rgseed = rg->nodeExist(newnode);

                    if (rgseed == NULL) {
//                        cout << sum++ <<endl;
                        rgseed = newnode;
                        newnode->fathernum = pnode->RGname_ptr->num;
                        newnode->last_tran = tranQ->transition->getid();
                        rgseed->get_FireTranQ(cpn);
                        rg->addRGNode(newnode);

                    } else {
                        delete newnode;
                    }

                    if (rgseed->tranQ_obtained == false) {
                        cerr << "fireset hasn't been calculated" << endl;
                    }
                    if (isLabel(rgseed, pba->destination)) {
                        CPN_Product *qs = new CPN_Product;
                        qs->RGname_ptr = rgseed;
                        qs->BAname_id = pba->destination;
                        CPN_Product *existpos = cstack.search(qs);
                        if (existpos != NULL) {
                            pnode->lowlink = (pnode->lowlink < existpos->lowlink) ? pnode->lowlink : existpos->lowlink;
                            if (!astack.empty()) {
                                CPN_Product *cur_accp = astack.top();
                                if (cur_accp->id >= existpos->id) {
                                    result = false;
                                    return;
                                }
                            }
                        } else if (h.search(qs) == NULL) {
                            Tarjan(qs);
                            pnode->lowlink = (pnode->lowlink < qs->lowlink) ? pnode->lowlink : qs->lowlink;
                        }
                    }
                }
            }
        }
        pba=pba->next;
    }
    CPN_Product *popitem;
    if(pnode->lowlink == pnode->id) {
        do {
            popitem = cstack.pop();
            if(!astack.empty() && popitem == astack.top())
                astack.pop();
            h.insert(popitem);
        }while (pnode!=popitem);
    }
}

void CPN_Product_Automata::printresult(string propertyid) {

    string ret=(this->result)?"TRUE":"FALSE";
//    cout<<propertyid<<" "<<ret<<endl;
    cout<<"checked result:"<<ret<<endl;
}

bool CPN_Product_Automata::handleLTLV(string s, RG_NODE *state) {
    int pos = s.find("==");
    if(pos!=string::npos) {
        //equality
        if(s[0] == '!')
        {
            /*!(front == latter)
             * true:front != latter
             * false:front == latter
             * */
            s = s.substr(2, s.length() - 3);   //去除"!{}"
            int p = s.find_first_of("==");
            string front = s.substr(0,p);
            string latter = s.substr(p+2);
            bucket front_color,latter_color;
            if(!FetchColor(front,state,front_color))
                return false;
            if(!FetchColor(latter,state,latter_color))
                return false;
            if(front_color == latter_color)
                return false;
            else
                return true;
        }
        else {
            /*(front == latter)
             * true:front == latter
             * false:front != latter
             * */
            s = s.substr(1, s.length() - 2);   //去除"{}"
            int p = s.find_first_of("==");
            string front = s.substr(0,p);
            string latter = s.substr(p+2);
            bucket front_color,latter_color;
            if(!FetchColor(front,state,front_color))
                return false;
            if(!FetchColor(latter,state,latter_color))
                return false;
            if(front_color == latter_color)
                return true;
            else
                return false;
        }
    }

    pos = s.find("<=");
    if(pos!=string::npos) {
        if(s[0]=='!') {
            /*!(front <= latter)
             * true:front !<= latter
             * false:front <= latter
             * */
            s = s.substr(2, s.length() - 3);   //去除"!{}"
            int p = s.find_first_of("<=");
            string front = s.substr(0,p);
            string latter = s.substr(p+2);
            bucket front_color,latter_color;
            if(!FetchColor(front,state,front_color))
                return false;
            if(!FetchColor(latter,state,latter_color))
                return false;
            if(front_color <= latter_color)
                return false;
            else
                return true;
        }
        else {
            /*(front <= latter)
             * true:front <= latter
             * false:front !<= latter
             * */
            s = s.substr(1, s.length() - 2);   //去除"{}"
            int p = s.find_first_of("<=");
            string front = s.substr(0,p);
            string latter = s.substr(p+2);
            bucket front_color,latter_color;
            if(!FetchColor(front,state,front_color))
                return false;
            if(!FetchColor(latter,state,latter_color))
                return false;
            if(front_color <= latter_color)
                return true;
            else
                return false;
        }
    }

    if(s[0]=='!') {
        /*!(front < latter)
         * true:front !< latter
         * false:front < latter
         * */
        s = s.substr(2, s.length() - 3);   //去除"!{}"
        int p = s.find_first_of("<");
        string front = s.substr(0,p);
        string latter = s.substr(p+1);
        bucket front_color,latter_color;
        if(!FetchColor(front,state,front_color))
            return false;
        if(!FetchColor(latter,state,latter_color))
            return false;
        if(front_color < latter_color)
            return false;
        else
            return true;
    }
    else {
        /*(front < latter)
         * true:front < latter
         * false:front !< latter
         * */
        s = s.substr(1, s.length() - 2);   //去除"{}"
        int p = s.find_first_of("<");
        string front = s.substr(0,p);
        string latter = s.substr(p+1);
        bucket front_color,latter_color;
        if(!FetchColor(front,state,front_color))
            return false;
        if(!FetchColor(latter,state,latter_color))
            return false;
        if(front_color < latter_color)
            return true;
        else
            return false;
    }
}

//FetchColor返回值表示是否成功取到值，若没有取到值可能是线程栈(局部变量)尚未创建
bool CPN_Product_Automata::FetchColor(string s, RG_NODE *state, bucket &color) {
    if(s[0]=='t') {
        string value = s.substr(12, s.length() - 13);
        auto valueVec = split(value,"#");
        if(valueVec.size() != 3)
            throw "token-value转换为字符串后的内容应该由三部分组成!";
        string P_name = valueVec[0], index = valueVec[1], thread = valueVec[2];
        auto iter = cpn->mapPlace.find(P_name);
        if(iter == cpn->mapPlace.end())
            throw "找不到库所" + P_name + " in product.cpp!";
        CPlace &pp = cpn->getplacearr()[iter->second];
        auto tks = state->marking.mss[iter->second].getmapTokens();
        auto tkiter = tks.begin();
        while(tkiter != tks.end()){
            auto tmp_tk = tkiter->first;
            SORTID sid = tmp_tk->getsid();
            auto ps = sorttable.find_productsort(sid);
            if(index == "NULL" ^ ps.get_hasindex() && thread == "NULL" ^ ps.get_hastid()){
                if(index == "NULL" && thread == "NULL"){
                    //全局普通变量
                    if(tks.size() != 1)
                        throw "FetchColor时全局普通变量出现异常：token数大于1";
                    Bucket tmp_bkt, value_bkt;
                    tmp_tk->getcolor(tmp_bkt);
                    tmp_bkt.getVarValue(value_bkt);
                    color.generateFromBucket(value_bkt);
                    return true;
                }
                else if(index != "NULL" && thread == "NULL"){
                    //全局数组变量
                    Bucket tmp_bkt, value_bkt, index_bkt;
                    tmp_tk->getcolor(tmp_bkt);
                    tmp_bkt.pro[tmp_bkt.pro.size()-2]->getcolor(index_bkt);
                    if(index_bkt == atoi(index.c_str())) {
                        tmp_bkt.getVarValue(value_bkt);
                        color.generateFromBucket(value_bkt);
                        return true;
                    }
                }
                else if(index == "NULL" && thread != "NULL"){
                    //局部普通变量
                    Bucket tmp_bkt, value_bkt, tid_bkt;
                    tmp_tk->getcolor(tmp_bkt);
                    tmp_bkt.pro[tmp_bkt.pro.size()-1]->getcolor(tid_bkt);
                    if(tid_bkt == thread) {
                        tmp_bkt.getVarValue(value_bkt);
                        color.generateFromBucket(value_bkt);
                        return true;
                    }
                }
                else if(index != "NULL" && thread != "NULL"){
                    //局部数组变量
                    Bucket tmp_bkt, value_bkt, index_bkt, tid_bkt;
                    tmp_tk->getcolor(tmp_bkt);
                    tmp_bkt.pro[tmp_bkt.pro.size()-3]->getcolor(index_bkt);
                    tmp_bkt.pro[tmp_bkt.pro.size()-1]->getcolor(tid_bkt);
                    if(index_bkt == atoi(index.c_str()) && tid_bkt == thread) {
                        tmp_bkt.getVarValue(value_bkt);
                        color.generateFromBucket(value_bkt);
                        return true;
                    }
                }
            }
            else
                throw "库所" + P_name + "的index或tid和检测性质不匹配!";
            tkiter++;
        }
        return false;
    }
    else if(s[0] == 'i') {
        string num = s.substr(13,s.length()-14);
        color.icolor = atoi(num.c_str());
    }
    else if(s[0] == 'r') {
        string num = s.substr(14,s.length()-15);
        color.rcolor = atof(num.c_str());
    }
    else if(s[0] == 's') {
        color.scolor=s.substr(16,s.length()-17);
    }
    return true;
}
