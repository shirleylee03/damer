//
// Created on 2021/11/16.
//

#include"AST.h"
//#include"cpn.h"
#include"AST_compare.h"
int cal_height(gtree *t)
{
    if (t == NULL)
        return 0;

    int child = cal_height(t->child);
    int next = cal_height(t->next);

    int height = child + 1 > next ? child + 1 : next;
    return height;
}

void HighIndexList::order()
{
    if (list.size() == 0)
        return;
    for (unsigned int i = 0; i < list.size() - 1; i++)
        for (unsigned int j = 0; j < list.size() - i - 1; j++)
        {
            if (list[j].height < list[j + 1].height)
            {
                gtree *temp_point;
                int temp_height;
                temp_point = list[j].point;
                temp_height = list[j].height;
                list[j].point = list[j + 1].point;
                list[j].height = list[j + 1].height;
                list[j + 1].point = temp_point;
                list[j + 1].height = temp_height;
            }
        }
}

void HighIndexList::push(gtree *t)
{
    tree_height t_h;
    t_h.point = t;
    t_h.height = cal_height(t->child);
    list.push_back(t_h);
    order();
}

int HighIndexList::peekMax()
{
    if (list.size() == 0)
    {
        cout << "当前list为空!" << endl;
        exit(1);
    }
    else
        return list[0].height;
}

vector<gtree *> HighIndexList::pop()
{
    int height = peekMax();
    vector<gtree *> return_list;
    for (vector<tree_height>::iterator it = list.begin(); it != list.end(); )
    {
        if (it->height == height)
        {
            return_list.push_back(it->point);
            it = list.erase(it);
        }
        else
            break;

    }
    return return_list;
}

void HighIndexList::open(gtree *t)
{
    gtree *p = t->child;
    while (p != NULL)
    {
        push(p);
        p = p->next;
    }
}

bool theSame(gtree *t1, gtree *t2);
bool Equal_Tree(gtree *t1, gtree *t2)
{
    if (t1->type != t2->type || t1->place != t2->place)
        return false;

    return theSame(t1->child, t2->child);
}

bool theSame(gtree *t1, gtree *t2)
{
    if (t1 == NULL && t2 != NULL)
        return false;
    else if (t1 != NULL && t2 == NULL)
        return false;
    else if (t1 == NULL && t2 == NULL)
        return true;
    else if (t1 != NULL && t2 != NULL)
    {
        if (t1->type != t2->type || t1->place != t2->place)
            return false;
        else
            return theSame(t1->child, t2->child) && theSame(t1->next, t2->next);
    }
}


/*
top-down algorithm
input：two ast
output：two ast Map
*/

void top_down(gtree *t1, gtree *t2, vector<Mapping> &M)
{
    HighIndexList L1, L2;
    L1.push(t1);
    L2.push(t2);

    while (L1.peekMax() > minHeight&&L2.peekMax() > minHeight)
    {

        while (L1.peekMax() != L2.peekMax())
        {
            if (L1.peekMax() > L2.peekMax())
            {
                vector<gtree *> child_list = L1.pop();
                for (unsigned int i = 0; i < child_list.size(); i++)
                    L1.open(child_list[i]);
            }
            else
            {
                vector<gtree *> child_list = L2.pop();
                for (unsigned int i = 0; i < child_list.size(); i++)
                    L2.open(child_list[i]);
            }
        }
        vector<gtree*> H1 = L1.pop();
        vector<gtree*> H2 = L2.pop();
        for (vector<gtree*>::iterator it1 = H1.begin(); it1 != H1.end(); )
        {
            bool flag = false;
            bool break_flag = false;
            for (vector<gtree*>::iterator it2 = H2.begin(); it2 != H2.end(); )
            {
                if (Equal_Tree(*it1, *it2))
                {
                    Mapping m;
                    m.map1 = *it1;
                    m.map2 = *it2;
                    m.num = m.map1->num;
                    M.push_back(m);
                    it1 = H1.erase(it1);

                    it2 = H2.erase(it2);
                    if (H1.size() == 0 || it1 == H1.end())
                    {
                        break_flag = true;
                        break;
                    }
                    if (H2.size() == 0)
                    {
                        break_flag = true;
                        break;
                    }
                    flag = true;
                }
                else
                    it2++;
            }
            if (break_flag == true)
                break;
            if (flag == false)
                it1++;
        }

        for (unsigned int i = 0; i < H1.size(); i++)
            L1.open(H1[i]);
        for (unsigned int i = 0; i < H2.size(); i++)
            L2.open(H2[i]);
    }

}

bool exist_in_M(gtree *t, vector<Mapping> M)
{
    for (unsigned int i = 0; i < M.size(); i++)
    {
        if (M[i].map1 == t || M[i].map2 == t)
            return true;
    }
    return false;
}

bool exist_in_M(Mapping m, vector<Mapping> M)
{
    for (unsigned int i = 0; i < M.size(); i++)
    {
        if ((M[i].map1 == m.map1 && M[i].map2 == m.map2) || (M[i].map1 == m.map2 && M[i].map2 == m.map1))
            return true;
    }
    return false;
}

gtree *extract_theOther_in_M(gtree *t, vector<Mapping> M)
{
    for (unsigned int i = 0; i < M.size(); i++)
    {
        if (M[i].map1 == t)
            return M[i].map2;
        else if (M[i].map2 == t)
            return M[i].map1;
    }
    cout << "There is no index, check exist_in_M!" << endl;
    exit(1);
}

/*void find_Nomatch_Node(gtree *t, vector<Mapping> &M, int type, vector<gtree *> &find_result)
{
	if (t == NULL)
		return;
	bool flag = exist_in_M(t, M);
	if (flag == true)
	{
		find_Nomatch_Node(t->next, M, type, find_result);
	}
	else
	{
		if (t->type == type)
			find_result.push_back(t);
		find_Nomatch_Node(t->child, M, type, find_result);
		find_Nomatch_Node(t->next, M, type, find_result);
	}

}*/

gtree *get_statementNodeFromPos(gtree *firstStatement,int pos){
    if(pos<0){
        cerr << "pos can not be non-negative number!"<<endl;
        exit(-1);
    }
    auto tmp = firstStatement;
    for(int i=0;i<pos;i++){
        tmp = tmp->parent->next;
    }
    if(tmp->type == STATEMENT)
        return tmp;
    cerr << "pos out of range!"<<endl;
    exit(-1);
}


vector<Mapping> find_container_mapping(vector<Mapping> &M)//找到所有包含相同项的非match语句列节点，即container mapping集
{
    vector<Mapping> result_list;
    for (unsigned int i = 0; i < M.size(); i++)
    {
        if (M[i].map1->type == STATEMENT)
        {
            auto tmp1 = M[i].map1;
            while(tmp1->type == STATEMENT)
                tmp1 = tmp1->parent->next;
            auto tmp2 = M[i].map2;
            while(tmp2->type == STATEMENT)
                tmp2 = tmp2->parent->next;
            if (tmp1->parent->type == COMPOUND_STATEMENT)//如果STATEMENT节点匹配，说明所在的STATEMENT_LIST节点不完全匹配
            {
                Mapping m;
                m.map1 = tmp1->parent;
                m.map2 = tmp2->parent;
                m.num = M[i].map1->parent->num;
                if (!exist_in_M(m, result_list))
                    result_list.push_back(m);
            }
            else {
                cerr << "matched STATEMENT node is not in the subtree of rooted by node COMPOUND_STATEMENT." <<endl;
                exit(-1);
            }
        }
    }
    return result_list;
}

bool total_match(gtree *t1, gtree *t2, vector<Mapping> &M)
{

    gtree *p1 = t1, *p2 = t2;

    while(p1->type != STATEMENT)
        p1 = p1->child;
    while(p2->type != STATEMENT)
        p2 = p2->child;

//    bool total_match = true;
    while (p1->type == STATEMENT && p2->type == STATEMENT)
    {
        Mapping m;
        m.map1 = p1;
        m.map2 = p2;
        if(!exist_in_M(m,M))
            return false;
        p1 = p1->parent->next;
        p2 = p2->parent->next;
    }
    return true;
//    if((!p1 || p1->type != STATEMENT) && (!p2 || p2->type != STATEMENT))
//        return false;
//    return true;
}

int sum_intersect(int target_num, vector<pair<int, int>> list)//计算target_num位置位置对与其他位置对相交的数量
{
    pair<int, int> target = list[target_num];
    int sum_neg = 0;
    for (unsigned int i = 1; i < list.size(); i++)//这里从1开始考虑到(0,0)已经存在
    {
        if (i != target_num)
        {
            int first = list[i].first - target.first;
            int second = list[i].second - target.second;
            if (first*second < 0)
                sum_neg++;

        }
    }
    return sum_neg;
}

int find_first_intersect(int position, vector<pair<int, int>> list)
{
    pair<int, int> target = list[position];
    for (unsigned int i = 1; i < list.size(); i++)
    {
        int first = list[i].first - target.first;
        int second = list[i].second - target.second;
        if (first*second < 0)
            return i;
    }
}

vector<pair<int, int>> process_move(vector<pair<int, int>> &list)//对所有移动的节点进行处理，返回所有移动节点的position_pair
{
    vector<pair<int, int>> result;
    for (unsigned int i = 1; i < list.size(); )
    {
        int sum1 = sum_intersect(i, list);
        int sum2;
        if (sum1 != 0)
        {
            int next_num = find_first_intersect(i, list);
            sum2 = sum_intersect(next_num, list);
            if (sum1 >= sum2)
            {
                result.push_back(list[i]);
                list.erase(list.begin() + i);
            }
            else
            {
                result.push_back(list[next_num]);
                list.erase(list.begin() + next_num);
            }
        }
        else
            i++;

    }
    return result;
}

int find_last_inmoves(vector<pair<int, int>> moves, int value, bool isfirst)
{
    int max = 0;
    for (unsigned int i = 0; i < moves.size(); i++)
    {
        if (isfirst)
        {
            if (moves[i].first < value)
                max = max > moves[i].first ? max : moves[i].first;
        }
        else
        {
            if (moves[i].second < value)
                max = max > moves[i].second ? max : moves[i].second;
        }
    }
    return max;
}

bool exist_inmoves(vector<pair<int, int>> moves, int value, bool isfirst)
{
    int max = 0;
    for (unsigned int i = 0; i < moves.size(); i++)
    {
        if (isfirst)
        {
            if (moves[i].first == value)
                return true;
        }
        else
        {
            if (moves[i].second == value)
                return true;
        }
    }
    return false;
}

void add_change(vector<pair<int, int>> moves,  int last_first, int last_second, int now_first, int now_second, vector<pair<int, int>> &modify, vector<pair<int, int>> &add, vector<pair<int, int>> &del)
{
    last_first++;
    last_second++;
    while (last_first != now_first && last_second != now_second)
    {
//        if (exist_inmoves(moves, last_first, true))
//        {
//            last_first++;
//        }
//        else if (exist_inmoves(moves, last_second, false))
//        {
//            last_second++;
//        }
//        else
        {
            modify.push_back(make_pair(last_first, last_second));
            last_first++;
            last_second++;
        }
    }

    if (last_first == now_first)
    {
        while (last_second != now_second)
        {
            if (!exist_inmoves(moves, now_second - 1, false))
                add.push_back(make_pair(last_first - 1, now_second - 1));
            now_second--;
        }
    }
    else
    {
        while (last_first != now_first)
        {
            if (!exist_inmoves(moves, last_first, true))
                del.push_back(make_pair(last_first, last_second - 1));
            last_first++;
        }
    }

}

void process_other(vector<pair<int, int>> moves, vector<pair<int, int>> list, vector<pair<int, int>> &modify, vector<pair<int, int>> &add, vector<pair<int, int>> &del, int count1, int count2)
{
    int last_first, last_second, now_first, now_second;

    for (unsigned int i = 1; i < list.size(); i++)
    {
        last_first = list[i - 1].first;
        last_second = list[i - 1].second;
        //int temp_first = find_last_inmoves(moves, list[i].first, true);
        //last_first = last_first > temp_first ? last_first : temp_first;
        //int temp_second = find_last_inmoves(moves, list[i].second, false);
        //last_second = last_second > temp_second ? last_second : temp_second;

        int del_first = list[i].first - last_first;
        int del_second = list[i].second - last_second;
        if (del_first == 1 && del_second == 1)
            ;
        else
            add_change(moves, last_first, last_second, list[i].first, list[i].second, modify, add, del);
//        if (i == list.size() - 1)
//        {
////            last_first = find_last_inmoves(moves, count1, true);
////            last_second = find_last_inmoves(moves, count2, false);
//            last_first = last_first > list[i].first ? last_first : list[i].first;
//            last_second = last_second > list[i].second ? last_second : list[i].second;
//            if (count1 - last_first == 1 && count2 - last_second == 1)
//                ;
//            else
//                add_change(moves,last_first, last_second, count1, count2, modify, add, del);
//        }
    }
}

map<int, int> get_position_pair(gtree *t1,gtree *t2,vector<Mapping> &M){
    //find the statement block of position_pair series
    map<int, int> position_pair_map;

    gtree *p1 = t1, *p2 = t2;
    while(p1->type != STATEMENT)
        p1 = p1->child;
    while(p2->type != STATEMENT)
        p2 = p2->child;

    int position1 = 0, position2 = 0;
    while (p1->type == STATEMENT)
    {
        if (exist_in_M(p1, M))
        {
            gtree *theOther = extract_theOther_in_M(p1, M);
            position2 = 0;
            auto tmp_p = p2;
//            p2 = t2->child;
            while (tmp_p->type == STATEMENT)
            {
                if (tmp_p == theOther)
                {
                    position_pair_map.emplace(position1, position2);
                    break;
                }
                position2++;
                tmp_p = tmp_p->parent->next;
            }
        }
        position1++;
        p1 = p1->parent->next;
    }
    return position_pair_map;
}

AST_change block_handle(gtree *t1, gtree *t2, vector<Mapping> &M_statement)//t1,t2都是语句列指针
{
    AST_change changes;
    changes.m.map1 = t1;
    changes.m.map2 = t2;


    //First judge whether all clauses have been matched (if all clauses have been matched, the statement block does not need to be processed)
    bool match = total_match(t1, t2, M_statement);
    if (match == true)
    {
        changes.m.map1 = NULL;
        changes.m.map2 = NULL;
        return changes;
    }
    //Find the statement block of position_pair seris
    vector<pair<int, int>> position_pair_list;
    position_pair_list.push_back(make_pair(0, 0));//(0,0)作为阶梯起点

    gtree *p1 = t1, *p2 = t2;
    while(p1->type != STATEMENT)
        p1 = p1->child;
    while(p2->type != STATEMENT)
        p2 = p2->child;

    auto base_p1 = p1, base_p2 = p2;

    int position1 = 1, position2 = 1;
    while (p1 != NULL)
    {
        if (exist_in_M(p1, M_statement))
        {
            gtree *theOther = extract_theOther_in_M(p1, M_statement);
            position2 = 1;
            p2 = base_p2;
            while (p2 != NULL)
            {
                if (p2 == theOther)
                {
                    position_pair_list.push_back(make_pair(position1, position2));
                }
                position2++;
                p2 = p2->parent->next;
            }
        }
        position1++;
        p1 = p1->parent->next;
    }

    position_pair_list.emplace_back(position1-1,position2-1);

    //according to position_pair_list and find pair
    vector<pair<int, int>> moves = process_move(position_pair_list);

    vector<pair<int, int>> modify, add, del;
    process_other(moves, position_pair_list, modify, add, del, position1, position2);

    if(t1->parent->parent->type != FUNCTION_DEFINITION) {
        ///function does not go to M_statement set
        auto tmp_t1 = t1, tmp_t2 = t2;
        while (tmp_t1->type != STATEMENT)
            tmp_t1 = tmp_t1->parent;
        while (tmp_t2->type != STATEMENT)
            tmp_t2 = tmp_t2->parent;

        Mapping m;
        m.map1 = tmp_t1;
        m.map2 = tmp_t2;
        M_statement.push_back(m);
    }

    changes.move = moves;
    changes.add = add;
    changes.modify = modify;
    changes.del = del;
    return changes;
}

void sort_M(vector<Mapping> &M)//Bubbling algorithm
{
    for (unsigned int i = 0; i < M.size() - 1; i++)
        for (unsigned int j = 0; j < M.size() - i - 1; j++)
        {
            if (M[j].num > M[j + 1].num)
            {
                gtree *map1;
                gtree *map2;
                int num;
                map1 = M[j].map1;
                map2 = M[j].map2;
                num = M[j].num;
                M[j].map1 = M[j + 1].map1;
                M[j].map2 = M[j + 1].map2;
                M[j].num = M[j + 1].num;
                M[j + 1].map1 = map1;
                M[j + 1].map2 = map2;
                M[j + 1].num = num;
            }
        }
}



/*
 * Find the matching statement node set through the matching node set M M_statement
 */

void get_MatchStatement(gtree *t1,gtree *t2,vector<Mapping> &M_statement){
    if(!t1 || !t2)
        return;
    if(t1->type != t2->type){
        cerr << "AST_compare Match: The node type of is inconsistent!"<<endl;
        exit(-1);
    }
    if(t1->type == STATEMENT && t2->type == STATEMENT){
        M_statement.emplace_back(t1,t2,t1->num);
    }
    get_MatchStatement(t1->child,t2->child,M_statement);
    get_MatchStatement(t1->next,t2->next,M_statement);
}

vector<Mapping> get_MatchStatement(vector<Mapping> &M){
    vector<Mapping> M_statement;

    for(int i=0;i<M.size();i++){
        if(M[i].map1->type == STATEMENT)
            M_statement.emplace_back(M[i].map1,M[i].map2,M[i].map1->num);
        get_MatchStatement(M[i].map1->child,M[i].map2->child,M_statement);
    }

    return M_statement;
}

/*
extract_change function
input：Two ast Top-down algorithm out map set
output：Statement level changes in all statement columns

*/
vector<AST_change> extract_change(gtree *t1, gtree *t2, vector<Mapping> &M, vector<Mapping> &M_statement)
{
    vector<AST_change> changes;

    vector<Mapping> container_list = find_container_mapping(M);
    //sort all container: ensure that the processing is carried out in reverse order, which can avoid repeated processing of ancestor statements after the sub statements have been processed
    sort_M(container_list);
    if (container_list.size() != 0)
    {
        for (int i = int(container_list.size() - 1); i >= 0; i--)
        {
            auto c1 = container_list[i].map1->child, c2 = container_list[i].map2->child;
            while(c1 && c1->type != STATEMENT_LIST)
                c1 = c1->next;
            while(c2 && c2->type != STATEMENT_LIST)
                c2 = c2->next;
            if(!c1 || !c2){
                cerr << "Extract error container!"<<endl;
                exit(-1);
            }
            AST_change change = block_handle(c1, c2, M_statement);
            if (change.m.map1 != NULL)
                changes.push_back(change);
        }
        return changes;
    }
    else
    {
        cout << "Can not find container!" << endl;
        exit(1);
    }
}


void extract_changeNodesFromContainer(Mapping &m,vector<Mapping> &M_statement,vector<gtree *> &vec_t1,vector<gtree *> &vec_t2){
    auto p1 = m.map1, p2 = m.map2;
    while(p1->type != STATEMENT)
        p1 = p1->child;
    while(p2->type != STATEMENT)
        p2 = p2->child;

    while(p1 && p1->type == STATEMENT){
        if(!exist_in_M(p1,M_statement) && !exist_in(vec_t1,p1))
            vec_t1.emplace_back(p1);
        p1 = p1->parent->next;
    }

    while(p2 && p2->type == STATEMENT){
        if(!exist_in_M(p2,M_statement) && !exist_in(vec_t2,p2))
            vec_t2.emplace_back(p2);
        p2 = p2->parent->next;
    }
}


void extract_moveNodesFromContainer(Mapping &m,vector<Mapping> &M_statement,vector<gtree *> &vec_t1,vector<gtree *> &vec_t2){
    auto position_pair_map = get_position_pair(m.map1,m.map2,M_statement);
    gtree *p1 = m.map1, *p2 = m.map2;
    while(p1->type != STATEMENT)
        p1 = p1->child;
    while(p2->type != STATEMENT)
        p2 = p2->child;

    auto iter = position_pair_map.begin();
    while(iter != position_pair_map.end()){
        auto iter1 = position_pair_map.begin();
        while(iter1 != position_pair_map.end()){
            if(iter1 != iter) {
                auto res = ((iter->first - iter1->first) * (iter->second - iter1->second)) < 0;
                if (res) {
                    auto tmp_p1 = get_statementNodeFromPos(p1, iter->first);
                    auto tmp_p2 = get_statementNodeFromPos(p2, iter->second);
                    if (!exist_in(vec_t1, tmp_p1))
                        vec_t1.push_back(tmp_p1);
                    if (!exist_in(vec_t2, tmp_p2))
                        vec_t2.push_back(tmp_p2);
                }
            }
            iter1++;
        }
        iter++;
    }
}

/*
 * extract_changeNodes: Extract the extra parts of the two syntax trees
 * @M_statement: Match statement nodes
 * @M: Matched nodes(Contains only maximal nodes)
 * @vec_t1: deleted nodes
 * @vec_t2: added nodes
 */

void extract_changeNodes(vector<Mapping> &M_statement,vector<Mapping> &M,vector<gtree *> &vec_t1,vector<gtree *> &vec_t2){
    vector<Mapping> container_list = find_container_mapping(M);
    //Sort all container: Ensure that the processing is carried out in reverse order to avoid repeated processing of ancestor statements after the sub statements have been processed
    sort_M(container_list);
    if (container_list.size() != 0)
    {
        for (int i = int(container_list.size() - 1); i >= 0; i--)
        {
            extract_changeNodesFromContainer(container_list[i],M_statement, vec_t1,vec_t2);
            extract_moveNodesFromContainer(container_list[i],M_statement, vec_t1,vec_t2);
        }
    }
    else
    {
        cout << "Can not find container!" << endl;
        exit(1);
    }
}
