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


// Find the function name passed into the syntax tree
string findFunctionName(TreeNode *fun) {
    static const int IDENTIFIER = 258;
    // When traversing the child nodes from left to right, the first thing you encounter is the function name
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

    //Line number conversion correlation
    ret->row = head->row;

    // Traverse every child
    gtree *p = head->child;
    while (p) {
        ret->children.emplace_back(gtree2TreeNode(p));
        ret->children.back()->parent = ret; // 需要记录，孩子的父亲就是自身
        p = p->next;
    }

    // self
    ret->nodeName = head->place;
    ret->type = head->type;

    // Function information and jump label information need to be recorded in the conversion
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

// Determine whether the tree belongs to a minimum CFG node
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

// link

// selection statement
static void linkSelection(TreeNode *node) {
//    node->next[1] = node->parent->next[1]; // There are hidden dangers. If the father is selection, an error will occur
    if (node->parent->nodeName == "selection_statement") {
        node->next[1] = node->parent->parent->next[1];
//        TreeNode *tmp = node->parent;
//        while(tmp->nodeName=="iteration_statement")
//            tmp = tmp->parent;
//        node->next[1] = tmp->next[1];
    } else {
        node->next[1] = node->parent->next[1];
    }
    // node->next[0] = node->children.front();//This temporary comment should not directly find the next 0 of if, but only the next 0 of selection node

    TreeNode *sel_node = node->children.front();
    sel_node->next[0] = sel_node->children[4];
    if (sel_node->children[4]->children.front()->nodeName != "compound_statement") {
        // there exists no {} in if: The statement to be executed is this one directly, and next0 points to itself
        sel_node->children[4]->next[0] = sel_node->children[4];
    }
    //-------------------------
    // if or if-else
    //-------------------------
    if (sel_node->children.size() == 5) {
        sel_node->next[1] = node->next[1];              //    If the judgment fails, the previous sentence will be returned directly
// The statement is logically confused        sel_node->children[4]->next[0] = node->next[1]; //      If the successful statement block is executed, it will directly follow the whole state

        // if Exit of successful execution block
        TreeNode *sel_block_1 = sel_node->children[4];
        sel_block_1->next[1] = node->next[1];
    } else {                                              //
        sel_node->next[1] = sel_node->children[6];      //   If there is else, take the else branch
        if (sel_node->children[6]->children.front()->nodeName != "compound_statement") {
            sel_node->children[6]->next[0] = sel_node->children[6];
        }

        sel_node->children[4]->next[1] = node->next[1]; //      Follow up to successful execution
        sel_node->children[6]->next[1] = node->next[1]; //      Follow up of failed routes

        // compound output
//        TreeNode *sel_node_1 = sel_node->children[4]; //
//        sel_node_1->next[1] = node->next[1];
//        TreeNode *sel_node_2 = sel_node->children[6];
//        sel_node_2->next[1] = node->next[1];
    }
}

// iteration statement (while)
static void linkIteration(TreeNode *node) {
//    node->next[1] = node->parent->next[1]; //
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
    TreeNode *it_block = it_node->children[4];          // do-loop statement

    it_node->next[0] = it_block;
    it_node->next[1] = node->next[1];

    it_block->next[1] = it_node->next[1];
    if (it_block->children.front()->nodeName != "compound_statement") {
        // Location returned after while execution
        it_block->next[0] = node;
    } else {
        // While a block is returned after execution
        it_block->children.front()->children.back()->next[0] = node;
    }

}

// list  (statement list和declaration list)
static void linkList(TreeNode *node) {
    if (node->nodeName == node->parent->nodeName) {
        // list'father is list: Then its successor is its own brother node
        node->next[1] = node->parent->children[1];
        return;
    } else if (node->parent->nodeName == "compound_statement" && node->nodeName == "declaration_list") {
        // compound's declaration list，Judge whether there is a statement list on the right
        if (node->parent->children.size() == 4) {
            // If there is a brother on the right, the successor is the leftmost statement of the statement list
            TreeNode *left_statement = node->parent->children[2];
            while (left_statement->nodeName == "statement_list")
                left_statement = left_statement->children.front();
            node->next[1] = left_statement;
            return;
        } else {
            // If there is no successor node on the right, then there is no successor node
//            node->next[1] = node->parent->next[1];
            // The next symbol to the right is compound }
            node->next[1] = node->parent->children.back();
            return;
        }
    } else if (node->parent->nodeName == "compound_statement" && node->nodeName == "statement_list") {

        node->next[1] = node->parent->children.back();
        return;
    }
}

// compound
static void linkCompound(TreeNode *node) {
    TreeNode *cp_node = node->children.front();
    // Subsequent construction
    if (node->parent->nodeName == "selection_statement") {
        // In the case of a selection statement, its NEXT1 may point to the else branch, so you should find the NEXT1 of a higher-level statement
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
        // If there is no statement in it, the left brace points directly to the right brace
        cp_node->children.front()->next[1] = cp_node->children.back();
    } else {
        if (cp_node->children[1]->nodeName == "statement" &&
            cp_node->children[1]->children[0]->nodeName ==
                "compound_statement") {
            // If compound is inside compound, the left brace points to the next left brace
            cp_node->children.front()->next[1] =
                cp_node->children[1]->children[0]->children[0];
        } else {
            // If there are statements in it, find the leftmost non list node
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
//        // compound has no out
//        node->next[0] = node;                               // compound input is itself
//    }
//    else{
//        //
//        TreeNode *tmp = cp_node->children[1];
//        while(tmp->nodeName=="statement_list"||tmp->nodeName=="declaration_list"){
//            tmp = tmp->children.front();
//        }
//        node->next[0] = tmp;
//
//        //
//        cp_node->next[1] = node->next[1];
//        cp_node->next[0] = tmp; // 刚才找好的
//    }
//}

// jump --return --break --continue     without  --goto
static void linkJump(TreeNode *node) {
    TreeNode *jp_node = node->children.front();

    if (jp_node->children.front()->nodeName == "return") {
        // return Statement to find the end of the function directly and close the brace
        TreeNode *tmp = node;
        while (tmp->nodeName != "function_definition")
            tmp = tmp->parent;
        node->next[1] = tmp->children.back()->children.back();
    }
    if (jp_node->children.front()->nodeName == "break") {
        // Find the successor of the while statement
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
 * This function is used to build the statement flow relationship within a function, which is only called by the following functions
 * @param f_
 */
static void linkFunction(TreeNode *f_) {
    f_->children.back()->next[0] = nullptr; // initial，function definition the last node id compound
    /* Construct an illusion to link the outermost compound of the function using the linkcompound function */
    TreeNode t1, t2;
    t1.children.emplace_back(&t2);
    t2.parent = &t1;
    t2.children.emplace_back(f_->children.back());
    linkCompound(&t2);


    /**
     * Traversal sequence description: self node -- > traverses child nodes from the right to the left
     */
    stack<TreeNode *> s;
    s.push(f_->children.back());  //At this point, the node added to the stack is the outermost compound of the function

    while (!s.empty()) {
        TreeNode *node = s.top();
        s.pop();
        if (node->children.size() == 0)
            continue;
        /** Each node needs to be processed separately
         * 1-selection ==2 branch
         * 2-iteration ==2 branch
         * 3-list
         * 4-compound
         * 5-jump statement
         * the other is father node's next
         * For a node that is not a minimum cell, you also need to continue to search its own child nodes on the stack
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
                    s.push(sub_node);                                           // Nodes that are not left and right braces
        } else if (node->children.front()->nodeName == "jump_statement") {
            /**
             * Do not handle it temporarily. Jump is not allowed in simple restriction test statements
             */
            linkJump(node);
        } else if (node->nodeName == "statement_list" || node->nodeName == "declaration_list") {
            linkList(node);
            if (node->parent->nodeName != "function_definition")
                // All child nodes of list are put on the stack, unless it is the definition list of function parameters
                for (auto child: node->children)
                    s.push(child);
        } else {
            node->next[1] = node->parent->next[1];
            if (!isCFGNode(node))                    // Non minimum cell, child node stacking
                for (auto child: node->children)
                    s.push(child);
        }

    }
}

void preProcessor_findNext(TreeNode *r_) {
    /**
     * Because you want to find the next sentence in function units, first search the syntax tree and find each function definition node
     * After finding it, use the linkfunction function defined above to work
     */

    // Traverse the syntax tree to find all function nodes
    queue<TreeNode *> q;
    q.push(r_);

    while (!q.empty()) {
        if (q.front()->nodeName == "function_definition") {

            q.front()->next[1] = q.front()->children.back()->children.front();  // The left brace of compound is the starting position of the function

            linkFunction(q.front());                    // If you meet the function definition node, you will enter the processing. There is no need to traverse this node
        }
        else
            for (auto node: q.front()->children)          // Otherwise, all child nodes continue to traverse
                q.push(node);
        q.pop();
    }
}

/* ********************************************************************
 *  The second version 3.2
 * ********************************************************************/


//typedef void (*TreeNodeHandler)(TreeNode *);
//
//void tree_node_external_declaration(TreeNode *node) {}
//
//void tree_node_translation_unit(TreeNode *node) {}
//
//void tree_node_function_definition(TreeNode *node) {
//    //
//    node->next[0] = node->children.back();
//}
//
//void tree_node_jump_statement(TreeNode *node) {
//    if (node->children[0]->nodeName == "goto") {
//        auto label = label_list.find(node->children[1]->nodeName);
//        if (label == label_list.end()) {
//            fprintf(stderr, "can not find label [%s]\n", node->children[1]->nodeName.c_str());
//            exit(0);
//        }
//        node->next[1] = label->second;
//    }
//    if (node->children[0]->nodeName == "continue") {
//        // 定位到自己所属的while
//        TreeNode *tmp = node;
//        while (tmp && tmp->nodeName != "iteration_statement")
//            tmp = tmp->parent;
//        if (!tmp) {
//            fprintf(stderr, "can not find while for continue\n");
//            exit(0);
//        } else {
//            node->next[1] = tmp->parent;
//        }
//    }
//    if (node->children[0]->nodeName == "break") {
//        // 直接找while的下一个
//        // 定位到自己所属的while
//        TreeNode *tmp = node;
//        while (tmp && tmp->nodeName != "iteration_statement")
//            tmp = tmp->parent;
//        if (!tmp) {
//            fprintf(stderr, "can not find while for break\n");
//            exit(0);
//        } else {
//            node->next[1] = tmp->parent->next[1];
//        }
//    }
//    if (node->children[0]->nodeName == "return") {
//        // 定位到自己所属的function
//        TreeNode *tmp = node;
//        while (tmp && tmp->nodeName != "function_definition")
//            tmp = tmp->parent;
//        if (!tmp) {
//            fprintf(stderr, "can not find function for return\n");
//            exit(0);
//        } else {
//            // 函数定义后一个子节点为compound，compound的最后一个子节点为 }
//            node->next[1] = tmp->children.back()->children.back();
//        }
//    }
//}
//
//void tree_node_iteration_statement(TreeNode *node) {
//    // 只有while型
//    node->parent->next[0] = node->children.back();  // statement 的 next0 赋值为自己的循环体
//    node->next[0] = node->children.back();          // 同样，iter 节点 next0 也赋值为循环体
//
//    node->children.back()->next[1] = node->parent;  // 循环体执行完毕之后，next1 赋值为 statement
//}
//
//void tree_node_selection_statement(TreeNode *node) {
//}
//
//static map<string, TreeNodeHandler> tree_node_function_list = {
//        {"external_declaration", tree_node_external_declaration},
//        {"translation_unit",     tree_node_translation_unit},
//        {"function_definition",  tree_node_function_definition},
//        {"jump_statement",       tree_node_jump_statement},
//        {"iteration_statement",  tree_node_iteration_statement},
//
//};
//
//
//void preProcessor_linkTree(TreeNode *r_) {
//    TreeNodeHandler fun;
//    auto iter = tree_node_function_list.find(r_->nodeName);
//    if (iter == tree_node_function_list.end()) {
//        fprintf(stderr, "can not find function to handle node[%s]\n", r_->nodeName.c_str());
//        exit(0);
//    } else {
//        fun = iter->second;
//        fun(r_);
//    }
//    // 从右到左遍历子节点（从后向前）
//    for (auto it = r_->children.rbegin(); it != r_->children.rend(); it++) {
//        preProcessor_linkTree(*it);
//    }
//}