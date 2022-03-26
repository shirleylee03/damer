//
// Created on 2021/10/2.
//

#include "Tree.h"
#include "CFG.h"
#include <queue>
#include <stack>
#include <map>

//extern CFG cfg;

typedef pair<string, TreeNode*> tree_link;
map<string, TreeNode *> function_list = {};
map<string, TreeNode *> label_list = {};


// 查找传入语法树的函数名
string findFunctionName(TreeNode *fun) {
    static const int IDENTIFIER = 258;
    // 从左向右遍历子节点，最先遇到的就是函数名
    queue<TreeNode *> q;
    q.push(fun);
    while (!q.empty()) {
        if (q.front()->type==IDENTIFIER)
            return q.front()->nodeName;
        for (auto child:q.front()->children)
            q.push(child);
        q.pop();
    }
    return "null";
}

TreeNode *gtree2TreeNode(gtree *head) {
    TreeNode *ret = new TreeNode;

    //行号转换相关
    ret->row = head->row;

    // 遍历每一个孩子
    gtree *p = head->child;
    while (p) {
        ret->children.emplace_back(gtree2TreeNode(p));
        ret->children.back()->parent = ret; // 需要记录，孩子的父亲就是自身
        p = p->next;
    }

    // 自身
    ret->nodeName = head->place;
    ret->type = head->type;

    // 转换中需要记录函数信息和跳转标签信息
    if (ret->children.size() && ret->children[0]->nodeName == "labeled_statement") {
        label_list.insert(tree_link(ret->children[0]->children[0]->nodeName, ret));
    }
    if (ret->nodeName == "function_definition") {
        string fun_name = findFunctionName(ret);
        function_list.insert(tree_link(fun_name, ret));
    }

    return ret;
}

void deleteTree(TreeNode *r) {

    if (r->children.size() == 0) {
        delete r;
        return;
    } else {
        for (int i = 0; i < r->children.size(); i++)
            deleteTree(r->children[i]);
        delete r;
        return;
    }
}

// 判定这个树是否属于某个最小CFG节点
static bool isCFGNode(const TreeNode *t_) {
//    for (auto node: cfg.nodes)
//        if (node->tree_node == t_
//            && node->type != CFGNode::SELECTION
//            && node->type != CFGNode::ITERATION)
//            return true;
//    return false;

    if (t_->nodeName == "statement"
        && t_->children.front()->nodeName != "selection_statement"
        && t_->children.front()->nodeName != "iteration_statement") {
        return true;
    }
    if (t_->nodeName == "declaration") {
        return true;
    }
    return false;
}

// 链接

// 链接selection statement
static void linkSelection(TreeNode *node) {
//    node->next[1] = node->parent->next[1]; // 存在隐患，如果父亲是selection就会出错
    if (node->parent->nodeName == "selection_statement") {
        node->next[1] = node->parent->parent->next[1];
//        TreeNode *tmp = node->parent;
//        while(tmp->nodeName=="iteration_statement")
//            tmp = tmp->parent;
//        node->next[1] = tmp->next[1];
    } else {
        node->next[1] = node->parent->next[1];
    }
    // node->next[0] = node->children.front();//这一句暂时注释，应该不会直接查找if的next 0，只会找selection node 的next 0

    TreeNode *sel_node = node->children.front();
    sel_node->next[0] = sel_node->children[4];
    if (sel_node->children[4]->children.front()->nodeName != "compound_statement") {
        // 这里对应的是if语句里面没有{}的情况，需要执行的语句直接就是这一条，next0指向自身
        sel_node->children[4]->next[0] = sel_node->children[4];
    }
    //-------------------------
    // if语句的两种情况：有无else
    //-------------------------
    if (sel_node->children.size() == 5) {
        sel_node->next[1] = node->next[1];              //    没有的情况判定失败直接返回上一句
// 该语句逻辑混乱        sel_node->children[4]->next[0] = node->next[1]; //      if成功的语句块执行结束之后直接走整个state的后继

        // if成功的执行块的出口
        TreeNode *sel_block_1 = sel_node->children[4];
        sel_block_1->next[1] = node->next[1];
    } else {                                              //
        sel_node->next[1] = sel_node->children[6];      //    有else的情况走else分支
        if (sel_node->children[6]->children.front()->nodeName != "compound_statement") {
            sel_node->children[6]->next[0] = sel_node->children[6];
        }

        sel_node->children[4]->next[1] = node->next[1]; //      执行成功的后继
        sel_node->children[6]->next[1] = node->next[1]; //      执行失败路线的后继

        // compound的出口
//        TreeNode *sel_node_1 = sel_node->children[4]; //
//        sel_node_1->next[1] = node->next[1];
//        TreeNode *sel_node_2 = sel_node->children[6];
//        sel_node_2->next[1] = node->next[1];
    }
}

// 连接iteration statement (while型)
static void linkIteration(TreeNode *node) {
//    node->next[1] = node->parent->next[1]; // 存在隐患，如果父亲是selection就会出错
    if (node->parent->nodeName == "selection_statement") {
        node->next[1] = node->parent->parent->next[1];
//        TreeNode *tmp = node->parent;
//        while(tmp->nodeName=="iteration_statement")
//            tmp = tmp->parent;
//        node->next[1] = tmp->next[1];
    } else {
        node->next[1] = node->parent->next[1];
    }

    node->next[0] = node->children.front();

    TreeNode *it_node = node->children.front();
    TreeNode *it_block = it_node->children[4];          // 循环体的statement

    it_node->next[0] = it_block;
    it_node->next[1] = node->next[1];

    it_block->next[1] = it_node->next[1];
    if (it_block->children.front()->nodeName != "compound_statement") {
        // while执行结束之后返回的位置
        it_block->next[0] = node;
    } else {
        // while一个块执行结束之后返回
        it_block->children.front()->children.back()->next[0] = node;
    }

}

// 连接list类  (statement list和declaration list)
static void linkList(TreeNode *node) {
    if (node->nodeName == node->parent->nodeName) {
        // list的父亲是list，那么其后继就是自己的兄弟节点
        node->next[1] = node->parent->children[1];
        return;
    } else if (node->parent->nodeName == "compound_statement" && node->nodeName == "declaration_list") {
        // compound里面的declaration list，判断其是否有右侧兄弟statement list
        if (node->parent->children.size() == 4) {
            // 如果有右侧兄弟，那么其后继就是该statement list的最左侧statement
            TreeNode *left_statement = node->parent->children[2];
            while (left_statement->nodeName == "statement_list")
                left_statement = left_statement->children.front();
            node->next[1] = left_statement;
            return;
        } else {
            // 没有右侧兄弟节点，那么其后继就是compound的后继
//            node->next[1] = node->parent->next[1];
            // 上述更新为, 后继是compound的右侧符号 }
            node->next[1] = node->parent->children.back();
            return;
        }
    } else if (node->parent->nodeName == "compound_statement" && node->nodeName == "statement_list") {
        // compound里面的statement list，可以确定其后继就是compound的后继
//        node->next[1] = node->parent->next[1];
        // 上述更新为, 后继是compound的右侧符号 }
        node->next[1] = node->parent->children.back();
        return;
    }
}

// 链接compound类
static void linkCompound(TreeNode *node) {
    TreeNode *cp_node = node->children.front();
    // 后继构建
    if (node->parent->nodeName == "selection_statement") {
        // 如果是selection statement的情况，其next1有可能指向else分支，所以应该查找更加上级的statement的next1
        node->next[1] = node->parent->parent->next[1];
//        TreeNode *tmp = node->parent;
//        while(tmp->nodeName=="iteration_statement")
//            tmp = tmp->parent;
//        node->next[1] = tmp->next[1];
    } else {
        node->next[1] = node->parent->next[1];
    }
    cp_node->next[1] = node->next[1];
    cp_node->children.back()->next[1] = cp_node->next[1];
    // 下面这句，将statement和compound statement的next0值都赋为 左大括号
    node->next[0] = cp_node->next[0] = cp_node->children.front();

    if (cp_node->children.size() == 2) {
        // 如果里面没有语句，那么左大括号直接指向右大括号
        cp_node->children.front()->next[1] = cp_node->children.back();
    } else {
        if (cp_node->children[1]->nodeName == "statement" &&
            cp_node->children[1]->children[0]->nodeName ==
                "compound_statement") {
            // 如果compound里面是compound，那么左大括号指向下一个左大括号 
            cp_node->children.front()->next[1] =
                cp_node->children[1]->children[0]->children[0];
        } else {
            // 如果里面有语句, 找到其中的最左侧非list节点
            TreeNode *tmp = cp_node->children[1];
            while (tmp->nodeName == "statement_list" ||
                   tmp->nodeName == "declaration_list") {
                tmp = tmp->children.front();
            }
            //        node->next[0] = tmp;
            //        cp_node->next[1]= tmp;
            cp_node->children.front()->next[1] = tmp;
        }
    }


}
//static void linkCompound(TreeNode *node){
//    node->next[1] = node->parent->next[1];              // compound 的出口还是父节点的默认出口
//
//    TreeNode *cp_node = node->children.front();
//    if (cp_node->children.size()==2){
//        // compound 里面没有任何语句
//        node->next[0] = node;                               // compound 的入口是自身
//    }
//    else{
//        // 有语句，找到其中的最左侧非list节点
//        TreeNode *tmp = cp_node->children[1];
//        while(tmp->nodeName=="statement_list"||tmp->nodeName=="declaration_list"){
//            tmp = tmp->children.front();
//        }
//        node->next[0] = tmp;
//
//        // 因为 compound 里面有语句，所以应该将里面的cp node的next处理好，方便list使用
//        cp_node->next[1] = node->next[1];
//        cp_node->next[0] = tmp; // 刚才找好的
//    }
//}

// 链接jump --return --break --continue     还未处理  --goto
static void linkJump(TreeNode *node) {
    TreeNode *jp_node = node->children.front();

    if (jp_node->children.front()->nodeName == "return") {
        // return 语句直接去查找函数的末尾，右大括号
        TreeNode *tmp = node;
        while (tmp->nodeName != "function_definition")
            tmp = tmp->parent;
        node->next[1] = tmp->children.back()->children.back();
    }
    if (jp_node->children.front()->nodeName == "break") {
        // 查找while语句的后继
        TreeNode *tmp = node;
        while (tmp->nodeName != "iteration_statement")
            tmp = tmp->parent;
        node->next[1] = tmp->next[1];
    }
    if (jp_node->children.front()->nodeName == "continue") {
        TreeNode *tmp = node;
        while (tmp->nodeName != "iteration_statement")
            tmp = tmp->parent;
        node->next[1] = tmp->parent;
    }
}

/**
 * 本函数作用是构建一个函数内部的语句流关系，仅由下面的函数调用
 * @param f_
 */
static void linkFunction(TreeNode *f_) {
    f_->children.back()->next[0] = nullptr; // 初始化，function definition最后一个子节点一定是compound
    /* 构造一个假象，来使用linkCompound函数进行函数最外层compound的链接 */
    TreeNode t1, t2;
    t1.children.emplace_back(&t2);
    t2.parent = &t1;
    t2.children.emplace_back(f_->children.back());
    linkCompound(&t2);


    /**
     * 遍历顺序说明：自身节点-->右侧向左侧遍历子节点
     */
    stack<TreeNode *> s;
    s.push(f_->children.back());  // 此时加入栈中的节点即为函数的最外层compound

    while (!s.empty()) {
        TreeNode *node = s.top();
        s.pop();
        if (node->children.size() == 0)
            continue;
        /** 需要对每一种节点分别处理
         * 1-selection ==2分支
         * 2-iteration ==2分支
         * 3-list类
         * 4-compound
         * 5-jump 类型的语句
         * 其余都处理为父节点的next
         * 非最小单元的节点，还需要把自己的子节点继续入栈搜索
         */

        if (node->children.front()->nodeName == "selection_statement") {
            linkSelection(node);
            s.push(node->children.front()->children[4]);
            if (node->children.front()->children.size() == 7)
                s.push(node->children.front()->children[6]);
        } else if (node->children.front()->nodeName == "iteration_statement") {
            linkIteration(node);
            s.push(node->children.front()->children[4]);
        } else if (node->children.front()->nodeName == "compound_statement") {
            linkCompound(node);
            for (auto sub_node: node->children.front()->children)                //
                if (sub_node->nodeName != "{" && sub_node->nodeName != "}")       //
                    s.push(sub_node);                                           // 非左右大括号的节点
        } else if (node->children.front()->nodeName == "jump_statement") {
            /**
             * 暂时不做处理，简单限制测试语句中不允许出现跳转
             */
            linkJump(node);
        } else if (node->nodeName == "statement_list" || node->nodeName == "declaration_list") {
            linkList(node);
            if (node->parent->nodeName != "function_definition")
                // list的子节点全部入栈，除非它是函数参数的定义list
                for (auto child: node->children)
                    s.push(child);
        } else {
            node->next[1] = node->parent->next[1];
            if (!isCFGNode(node))                    // 非最小单元，子节点入栈
                for (auto child: node->children)
                    s.push(child);
        }

    }
}

void preProcessor_findNext(TreeNode *r_) {
    /**
     * 因为要以函数为单位进行查找下一句，所以首先对语法树进行一次查找，找到其中的每一个函数定义节点
     * 找到之后使用上面定义的 linkFunction 函数进行工作
     */

    // 遍历语法树找到所有的函数节点
    queue<TreeNode *> q;
    q.push(r_);

    while (!q.empty()) {
        if (q.front()->nodeName == "function_definition") {

            q.front()->next[1] = q.front()->children.back()->children.front();  // compound的左侧大括号是函数的起点位置

            linkFunction(q.front());                    // 遇见函数定义节点则进入处理，这个节点是没有必要遍历下去的
        }
        else
            for (auto node: q.front()->children)          // 否则全部子节点继续遍历
                q.push(node);
        q.pop();
    }
}

/* ********************************************************************
 *                          第二版本的代码 3.2
 *  写到一半，发现第一版本还行，就拉闸了
 * ********************************************************************/


typedef void (*TreeNodeHandler)(TreeNode *);

void tree_node_external_declaration(TreeNode *node) {}

void tree_node_translation_unit(TreeNode *node) {}

void tree_node_function_definition(TreeNode *node) {
    // 最后一个子节点一定是compound
    node->next[0] = node->children.back();
}

void tree_node_jump_statement(TreeNode *node) {
    if (node->children[0]->nodeName == "goto") {
        auto label = label_list.find(node->children[1]->nodeName);
        if (label == label_list.end()) {
            fprintf(stderr, "can not find label [%s]\n", node->children[1]->nodeName.c_str());
            exit(0);
        }
        node->next[1] = label->second;
    }
    if (node->children[0]->nodeName == "continue") {
        // 定位到自己所属的while
        TreeNode *tmp = node;
        while (tmp && tmp->nodeName != "iteration_statement")
            tmp = tmp->parent;
        if (!tmp) {
            fprintf(stderr, "can not find while for continue\n");
            exit(0);
        } else {
            node->next[1] = tmp->parent;
        }
    }
    if (node->children[0]->nodeName == "break") {
        // 直接找while的下一个
        // 定位到自己所属的while
        TreeNode *tmp = node;
        while (tmp && tmp->nodeName != "iteration_statement")
            tmp = tmp->parent;
        if (!tmp) {
            fprintf(stderr, "can not find while for break\n");
            exit(0);
        } else {
            node->next[1] = tmp->parent->next[1];
        }
    }
    if (node->children[0]->nodeName == "return") {
        // 定位到自己所属的function
        TreeNode *tmp = node;
        while (tmp && tmp->nodeName != "function_definition")
            tmp = tmp->parent;
        if (!tmp) {
            fprintf(stderr, "can not find function for return\n");
            exit(0);
        } else {
            // 函数定义后一个子节点为compound，compound的最后一个子节点为 }
            node->next[1] = tmp->children.back()->children.back();
        }
    }
}

void tree_node_iteration_statement(TreeNode *node) {
    // 只有while型
    node->parent->next[0] = node->children.back();  // statement 的 next0 赋值为自己的循环体
    node->next[0] = node->children.back();          // 同样，iter 节点 next0 也赋值为循环体

    node->children.back()->next[1] = node->parent;  // 循环体执行完毕之后，next1 赋值为 statement
}

void tree_node_selection_statement(TreeNode *node) {
}

static map<string, TreeNodeHandler> tree_node_function_list = {
        {"external_declaration", tree_node_external_declaration},
        {"translation_unit",     tree_node_translation_unit},
        {"function_definition",  tree_node_function_definition},
        {"jump_statement",       tree_node_jump_statement},
        {"iteration_statement",  tree_node_iteration_statement},

};


void preProcessor_linkTree(TreeNode *r_) {
    TreeNodeHandler fun;
    auto iter = tree_node_function_list.find(r_->nodeName);
    if (iter == tree_node_function_list.end()) {
        fprintf(stderr, "can not find function to handle node[%s]\n", r_->nodeName.c_str());
        exit(0);
    } else {
        fun = iter->second;
        fun(r_);
    }
    // 从右到左遍历子节点（从后向前）
    for (auto it = r_->children.rbegin(); it != r_->children.rend(); it++) {
        preProcessor_linkTree(*it);
    }
}