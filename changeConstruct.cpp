//
// Created on 2021/11/24.
//

#include "cpn.h"
#include "AST_compare.h"

string change_add_single(CPN *cpn,AST_change &change,pair<int,int> add){
    auto add_pos = add.first, new_pos = add.second;
    gtree *originFirstStatementNode = change.m.map1->child;
    while(originFirstStatementNode && originFirstStatementNode->type != STATEMENT_LIST)
        originFirstStatementNode = originFirstStatementNode->next;
    while(originFirstStatementNode->type != STATEMENT)
        originFirstStatementNode = originFirstStatementNode->child;
    gtree *pre_node = get_statementNodeFromPos(originFirstStatementNode,add_pos - 1);//函数输入的pos以0开始
    gtree *after_node = pre_node->parent->next;
    gtree *newFirstStatementNode = change.m.map2->child;
    while(newFirstStatementNode && newFirstStatementNode->type != STATEMENT_LIST)
        newFirstStatementNode = newFirstStatementNode->next;
    while(newFirstStatementNode->type != STATEMENT)
        newFirstStatementNode = newFirstStatementNode->child;
    gtree *add_node = get_statementNodeFromPos(newFirstStatementNode,new_pos - 1);//函数输入的pos以0开始

    ///change AST
    if(add_pos != 0) {
        gtree *new_statementlistNode = new gtree;
        gtree *pre_level_first_node = pre_node->parent->child;
        new_statementlistNode->type = STATEMENT_LIST;
        new_statementlistNode->place = "statement_list";
        new_statementlistNode->next = add_node;
        new_statementlistNode->parent = pre_node->parent;
        new_statementlistNode->child = pre_level_first_node;
        add_node->parent = pre_node->parent;
        pre_node->parent->child = new_statementlistNode;
        auto tmp = pre_level_first_node;
        while (tmp) {
            tmp->parent = new_statementlistNode;
            tmp = tmp->next;
        }
    }
    else{
        gtree *new_statementlistNode = new gtree;
        new_statementlistNode->type = STATEMENT_LIST;
        new_statementlistNode->place = "statement_list";
        new_statementlistNode->next = after_node;
        new_statementlistNode->parent = after_node->parent;
        after_node->parent->child = new_statementlistNode;
        new_statementlistNode->child = add_node;
        add_node->parent = new_statementlistNode;
    }

    ///change other changes
    for(int j=0;j<change.add.size();j++) {
        if (change.add[j].first > add_pos)
            change.add[j].first++;
    }
    for(int j=0;j<change.del.size();j++){
        if(change.del[j].first > add_pos)
            change.del[j].first++;
    }
    for(int j=0;j<change.modify.size();j++){
        if(change.modify[j].first > add_pos)
            change.modify[j].first++;
    }
    for(int j=0;j<change.move.size();j++){
        if(change.move[j].first > add_pos)
            change.move[j].first++;
    }

    ///change PDNet
    cpn->Traverse_ST1(add_node);
    cpn->Traverse_ST2(add_node);
    string add_P = add_node->matched_P;

    if(after_node->type == STATEMENT){
        ///it is not the last node
        auto pre_P = pre_node->matched_P,after_P = after_node->matched_P;
        cpn->modifyPreExecuteArc(after_P,add_P);

        cpn->Add_executed_P(cpn->findP_byid(add_P)->get_exit(),cpn->findP_byid(after_P)->get_enter());
    }
    else{
        ///it is the last node
        auto pre_P = pre_node->matched_P;
        cpn->Add_executed_P(cpn->findP_byid(pre_P)->get_exit(),cpn->findP_byid(add_P)->get_enter());
        cpn->modifyAfterExecuteArc(pre_P,add_P);
    }
    string same_level_P, control_T = "";
    if(add_pos != 0)
        same_level_P = pre_node->matched_P;
    else
        same_level_P = after_node->matched_P;
    auto same_level_pp = cpn->findP_byid(same_level_P);
    auto producer = same_level_pp->get_producer();
    for(int j=0;j<producer.size();j++){
        if(producer[j].arcType == control){
            control_T = cpn->findT_byindex(producer[j].idx)->getid();
            break;
        }
    }
    if(control_T != "")
        cpn->change_addControlArc(control_T,add_P);

    ///change the out transition TODO

    cpn->set_producer_consumer();

    return add_P;
}

vector<string> change_add(CPN *cpn, AST_change &change){
    vector<string> add_Ps;
    for(int i=0;i<change.add.size();i++){
        add_Ps.push_back(change_add_single(cpn,change,change.add[i]));
    }
    return add_Ps;
}

string change_del_single(CPN *cpn, AST_change &change, pair<int,int> del){
    auto del_pos = del.first;
    gtree *originFirstStatementNode = change.m.map1->child;
    while(originFirstStatementNode && originFirstStatementNode->type != STATEMENT_LIST)
        originFirstStatementNode = originFirstStatementNode->next;
    while(originFirstStatementNode->type != STATEMENT)
        originFirstStatementNode = originFirstStatementNode->child;
    gtree *del_node = get_statementNodeFromPos(originFirstStatementNode,del_pos - 1);//函数输入的pos以0开始
    gtree *pre_node = del_node->parent->child;
    auto after_node = del_node->parent->next;
    string pre_P;
    ///change AST
    if(del_pos == 1){
        ///it is the first node
        pre_node = NULL;
        auto tmp = after_node->parent->child;
        after_node->parent->child = after_node;
        delete tmp;
    }
    else{

        if(pre_node == del_node){
            cerr << "The deleted node should not be the only statement within a compound statement!"<<endl;
            exit(-1);
        }
        if(pre_node->type != STATEMENT)
            pre_node = pre_node->child->next;

//        if(pre_node->type == STATEMENT)
//            pre_P = pre_node->matched_P;
//        else
//            pre_P = pre_node->next->matched_P;
        pre_node->parent->child->parent = del_node->parent;
        pre_node->parent = del_node->parent;
        del_node->parent->child = pre_node->parent->child;
    }


    ///change other changes
    for(int j=0;j<change.add.size();j++) {
        if (change.add[j].first > del_pos)
            change.add[j].first--;
    }
    for(int j=0;j<change.del.size();j++){
        if(change.del[j].first > del_pos)
            change.del[j].first--;
    }
    for(int j=0;j<change.modify.size();j++){
        if(change.modify[j].first > del_pos)
            change.modify[j].first--;
    }
    for(int j=0;j<change.move.size();j++){
        if(change.move[j].first > del_pos)
            change.move[j].first--;
    }

    ///change PDNet
    string del_P = del_node->matched_P;
    delete del_node;
    cpn->change_deleteControlArc(del_P);
    if(after_node->type == STATEMENT){
        string after_P = after_node->matched_P;
        cpn->deletePreExecuteArc(after_P);
        cpn->modifyPreExecuteArc(del_P,after_P);
    }
    else{

        cpn->modifyAfterExecuteArc(del_P,pre_P);
    }

    ///change out transiiton TODO
    cpn->set_producer_consumer();

    return del_P;
}

vector<string> change_del(CPN *cpn, AST_change &change){
    vector<string> del_Ps;
    for(int i=0;i<change.del.size();i++){
        del_Ps.push_back(change_del_single(cpn,change,change.del[i]));
    }
    return del_Ps;
}

vector<string> change_modify(CPN *cpn, AST_change &change){
    vector<string> modify_Ps;
    for(int i=0;i<change.modify.size();i++){
        modify_Ps.push_back(change_add_single(cpn,change,change.modify[i]));
        modify_Ps.push_back(change_del_single(cpn,change,change.modify[i]));
    }
    return modify_Ps;
}

vector<string> change_move(CPN *cpn, AST_change &change){
    vector<string> move_Ps;
    for(int i=0;i<change.move.size();i++){
        auto add_pos = change.move[i], del_pos = change.move[i];
        add_pos.first = add_pos.second;
        //It is equivalent to adding a statement at the position after the move and deleting the original statement
        //
        //Subsequently, it can be optimized to realize mobile semantics
        move_Ps.push_back(change_add_single(cpn,change,add_pos));
        move_Ps.push_back(change_del_single(cpn,change,del_pos));
    }
    return move_Ps;
}

vector<string> changeConstruct(CPN *cpn, vector<AST_change> changes){
    vector<string> change_Ps;

    for (unsigned int i = 0; i < changes.size(); i++)
    {
        vector<string> add_Ps,del_Ps,modify_Ps,move_Ps;
        add_Ps = change_add(cpn,changes[i]);
        del_Ps = change_del(cpn,changes[i]);
        modify_Ps = change_modify(cpn,changes[i]);
        move_Ps = change_move(cpn,changes[i]);
        change_Ps.insert(change_Ps.end(),add_Ps.begin(),add_Ps.end());
        change_Ps.insert(change_Ps.end(),del_Ps.begin(),del_Ps.end());
        change_Ps.insert(change_Ps.end(),modify_Ps.begin(),modify_Ps.end());
        change_Ps.insert(change_Ps.end(),move_Ps.begin(), move_Ps.end());
    }
    return change_Ps;
}