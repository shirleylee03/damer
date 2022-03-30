//
// Created on 2021/10/3.
//

#include "CFG.h"

#include <stack>
#include <queue>
#include <map>
#include <algorithm>

extern CFG cfg;

static CFGNode *getCFGNodeByTreeNode(const TreeNode *t_, const CFG &cfg_);

int preProcessor_Tree2CFGnodes(TreeNode *root_, CFG &cfg_) {
    cfg_.root = root_;


    CFGNode *start_node = new CFGNode;      //
    start_node->sentence = "START NODE";    //
    cfg_.nodes.emplace_back(start_node);    // 

    // 手工栈实现深搜
    stack<TreeNode *> s;       //
    s.push(root_);                  // 初始化
    while (!s.empty()) {
        TreeNode *node = s.top();
        s.pop();

        if (node->nodeName == "statement") {
            /*  statement的处理  */
            TreeNode *sub_statement = node->children.front();
            CFGNode *tmp = new CFGNode;
            tmp->tree_node = node;

            if (sub_statement->nodeName == "compound_statement") {
                tmp->tree_node = sub_statement->children.front();
                tmp->type = CFGNode::COMPOUND;
                cfg_.nodes.emplace_back(tmp);
                //
                CFGNode *tmp2 = new CFGNode;
                tmp2->tree_node = sub_statement->children.back();
                tmp2->type = CFGNode::COMPOUND;
                cfg_.nodes.emplace_back(tmp2);

                s.push(sub_statement);                                          // compound
            }
            else if (sub_statement->nodeName == "selection_statement") {
                tmp->type = CFGNode::SELECTION;
                cfg_.nodes.emplace_back(tmp);
                for (auto sub_node:sub_statement->children) //
                    s.push(sub_node);                       // 
            }
            else if (sub_statement->nodeName == "iteration_statement") {
                tmp->type = CFGNode::ITERATION;
                cfg_.nodes.emplace_back(tmp);
                for (auto sub_node:sub_statement->children) //
                    s.push(sub_node);                       // 
            }
            else if (sub_statement->nodeName == "jump_statement") {
                tmp->type = CFGNode::JUMP;
                cfg_.nodes.emplace_back(tmp);
            } else {
                tmp->type = CFGNode::BASIC;                                     // 
                cfg_.nodes.emplace_back(tmp);                                   // label, expression 类型
            }
        }
        else if (node->nodeName == "declaration") {
            /*  declaration  */
            CFGNode *tmp = new CFGNode;
            tmp->tree_node = node;
            tmp->type = CFGNode::DECLARATION;
            cfg_.nodes.emplace_back(tmp);
        }
        else if (node->nodeName == "function_definition"){
            CFGNode *tmp = new CFGNode;
            tmp->tree_node = node->children.back()->children.front();   // 
            tmp->type = CFGNode::COMPOUND;
            cfg_.nodes.emplace_back(tmp);
            tmp = new CFGNode;
            tmp->tree_node = node->children.back()->children.back();    // 
            tmp->type = CFGNode::COMPOUND;
            cfg_.nodes.emplace_back(tmp);
            tmp = new CFGNode;
            tmp->tree_node = node;                                      // 
            tmp->type = CFGNode::FUNCTION;
            cfg_.nodes.emplace_back(tmp);

            for (int i = node->children.size() - 1; i >= 0; i--)
                s.push(node->children[i]);
        }
        else {
            // 
            for (int i = node->children.size() - 1; i >= 0; i--)
                s.push(node->children[i]);
        }
    }

    for (auto node: cfg_.nodes)
        if (node->type!=CFGNode::EMPTY)
            node->getSentence();

    CFGNode *end_node = new CFGNode;        //
    end_node->sentence = "END NODE";        //
    cfg_.nodes.emplace_back(end_node);      // 

    // 
    for (auto node: cfg_.nodes){
        const TreeNode *tmp = node->tree_node;
        while (tmp && tmp->nodeName!="function_definition")
            tmp = tmp->parent;
        if (tmp){
            CFGNode *fun_node = getCFGNodeByTreeNode(tmp, cfg_);
            node->function = fun_node;
        }
    }

    return 0;
}

// 
static CFGNode *getCFGNodeByTreeNode(const TreeNode *t_, const CFG &cfg_) {
    if (t_ && t_->children.size()){
        if (t_->children.front()->nodeName == "compound_statement"){
            // 
            return getCFGNodeByTreeNode(t_->children.front()->children.front(), cfg_);
        }
    }

    for (auto cfg_node: cfg_.nodes) {
        if (t_ == cfg_node->tree_node)
            return cfg_node;
    }
    return nullptr;
}

//
inline bool isMain(const TreeNode *const f_) {
    if (f_->children[1]->children[0]->children[0]->children[0]->nodeName == "main")
        return true;
    else
        return false;
}

// 
static void linkCFGNode2TreeNode(CFGNode *cfg_node, const TreeNode *tree_node, const CFG &cfg_) {
    CFGNode *tmp = getCFGNodeByTreeNode(tree_node, cfg_);
    cfg_node->next[1] = tmp;
}

// 
static void linkMain(CFG &cfg_) {
    queue<const TreeNode *> q;
    q.push(cfg_.root);
    while (!q.empty()) {
        if (q.front()->nodeName == "function_definition" && isMain(q.front())) {
//            linkCFGNode2TreeNode(cfg_.nodes.front(), q.front()->children[2]->children.front(), cfg_);
            CFGNode *tmp = getCFGNodeByTreeNode(q.front(), cfg_); // 
            cfg_.nodes.front()->next[1] = tmp;

            tmp = getCFGNodeByTreeNode(q.front()->children.back()->children.back(),cfg_); // 右侧大括号对应的节点
            tmp->next[1] = cfg_.nodes.back();
            return;
        }
        if (q.front()->nodeName == "function_definition"){
            q.pop();
            continue;
        }

        for (auto node:q.front()->children)
            q.push(node);
        q.pop();
    }
}

int preProcessor_LinkCFGNodes2Next(CFG &cfg_) {
   
    linkMain(cfg_);
    /**
     * 
     */
    for (auto node: cfg_.nodes) {
        if (node->type==CFGNode::EMPTY)
            continue;
       
        if (node->type == CFGNode::ITERATION) {
            // while 判断成功
            CFGNode *tmp = getCFGNodeByTreeNode(node->tree_node->children.front()->next[0], cfg_);
            if (!tmp) {
                printf("Can not find CFGNode[%s] next, exit...\n", node->sentence.c_str());
                exit(0);
            }
            node->next[0] = tmp;
            // while 判断失败
            tmp = getCFGNodeByTreeNode(node->tree_node->next[1], cfg_);
            if (!tmp) {
                printf("Can not find CFGNode[%s] next, exit...\n", node->sentence.c_str());
                exit(0);
            }
            node->next[1] = tmp;
        }
        else if (node->type == CFGNode::SELECTION) {
            // 
            CFGNode *tmp = getCFGNodeByTreeNode(node->tree_node->children.front()->children[4]->next[0], cfg_);
            if (!tmp) {
                printf("Can not find CFGNode[%s] next, exit...\n", node->sentence.c_str());
                exit(0);
            }
            node->next[0] = tmp;
            // 
            if (node->tree_node->children.front()->children.size() == 5) {
                // if
                tmp = getCFGNodeByTreeNode(node->tree_node->children.front()->next[1], cfg_);
                if (!tmp) {
                    printf("Can not find CFGNode[%s] next, exit...\n", node->sentence.c_str());
                    exit(0);
                }
                node->next[1] = tmp;
            } else {
                // if-else
                tmp = getCFGNodeByTreeNode(node->tree_node->children.front()->next[1], cfg_);
                if (!tmp) {
                    printf("Can not find CFGNode[%s] next, exit...\n", node->sentence.c_str());
                    exit(0);
                }
                node->next[1] = tmp;
            }
        }
        else if (node->type==CFGNode::COMPOUND) {
            CFGNode *tmp = getCFGNodeByTreeNode(node->tree_node->next[1], cfg_);
            if (!tmp) {
                printf("Can not find CFGNode[%s] next, exit...\n", node->sentence.c_str());
                exit(0);
            }
            if (tmp->type!=CFGNode::EMPTY)
                node->next[1] = tmp;
            if (node->tree_node->parent->parent->parent->nodeName == "iteration_statement"
                && node->sentence.find("} ") != std::string::npos) {
                tmp = getCFGNodeByTreeNode(node->tree_node->next[0], cfg_);
                if (!tmp) {
                    printf("Can not find CFGNode[%s] next, exit...\n", node->sentence.c_str());
                    exit(0);
                }
                node->next[1] = tmp;
//                node->next[1]= nullptr;
            }
            
            if (node->tree_node->parent->parent->nodeName=="function_definition"
                && node->sentence.find("}") != string::npos) {
                node->next[1] = cfg_.nodes.back();
            }
        }
        else if (node->type == CFGNode::JUMP && node->tree_node->children[0]->children[0]->nodeName == "goto") {
            
            string label = node->tree_node->children[0]->children[1]->nodeName;
            extern std::map<string, TreeNode *> label_list;
            auto it = label_list.find(label);
            if (it == label_list.end()) {
                fprintf(stderr, "can not find label to go [%s]\n", label.c_str());
                exit(0);
            }
            CFGNode *tmp = getCFGNodeByTreeNode(it->second, cfg_);
            if (!tmp) {
                fprintf(stderr, "can not find CFGNode of label [%s]\n", label.c_str());
                exit(0);
            }
            node->next[1] = tmp;
        }
        else {
            
            CFGNode *tmp = getCFGNodeByTreeNode(node->tree_node->next[1], cfg_);
            if (!tmp) {
                printf("Can not find CFGNode[%s] next, exit...\n", node->sentence.c_str());
                exit(0);
            }
            node->next[1] = tmp;
            if (node->tree_node->parent->nodeName == "iteration_statement"){
                tmp = getCFGNodeByTreeNode(node->tree_node->next[0], cfg_);
                if (!tmp) {
                    printf("Can not find CFGNode[%s] next, exit...\n", node->sentence.c_str());
                    exit(0);
                }
                node->next[0] = tmp;
            }
        }
    }

    
    for (auto node :cfg_.nodes)
        for (int i = 0;i<2;i++)
            if (node->next[i])
                node->next[i]->preNodes.emplace_back(node);

    return 0;
}


static void getAllId(const TreeNode * tree_, vector<string>&id_) {
    static const int IDENTIFIER = 258;
    queue<const TreeNode *> q;
    q.push(tree_);
    while (!q.empty()) {
        if (q.front()->type == IDENTIFIER)
            id_.emplace_back(q.front()->nodeName);
        for (auto child: q.front()->children)
            q.push(child);
        q.pop();
    }
}


static void findDeclarationOfPthread(CFG& cfg_){

    for (auto node: cfg_.nodes) {
        //
        if (node->type!=CFGNode::DECLARATION)
            continue;

        // 
        const TreeNode *left_leaf = node->tree_node;
        while (left_leaf->children.size())
            left_leaf = left_leaf->children.front();
        if (left_leaf->nodeName!="pthread_t")
            continue;

        // 
        vector<string> pthread_id;
        getAllId(node->tree_node, pthread_id);

        for (auto id: pthread_id){
            Pthread* tmp = new Pthread;
            tmp->name = id;
            tmp->decalration = node;
            cfg_.pthreads.emplace_back(tmp);
        }
    }
}

//
static void handle_pthread_create(CFG &cfg_, vector<string> args, CFGNode *node_) {

    // 
    int i = 0;
    while (cfg_.pthreads[i]->name!=args[1])
        i++;
    cfg_.pthreads[i]->function_name = args[0];
    cfg_.pthreads[i]->create = node_;

  
    extern map<string, TreeNode *> function_list;
    auto map_it = function_list.find(cfg_.pthreads[i]->function_name);
    CFGNode *tmp = getCFGNodeByTreeNode(map_it->second, cfg_);
    cfg_.pthreads[i]->begin = tmp;

    tmp = getCFGNodeByTreeNode(map_it->second->children.back()->children.back(), cfg_);
    cfg_.pthreads[i]->end = tmp;

}


static void handle_pthread_join(CFG &cfg_, vector<string> args, CFGNode *node_) {
 
    int i = 0;
    while (cfg_.pthreads[i]->name != args[0])
        i++;

   
    cfg_.pthreads[i]->join = node_;
}

// main thread
static void linkPthread(CFG &cfg_) {
    static const int IDENTIFIER = 258;

    for (auto node: cfg_.nodes) {
        // if (node->type!=CFGNode::BASIC)
        //     continue;
        if (node->sentence.find("pthread_create") == string::npos &&
            node->sentence.find("pthread_join") == string::npos)
            continue;

        //
        vector<string> args;
        stack<const TreeNode *> s;
        if (node->type == CFGNode::BASIC) // 如果不是基本类型，那么搜索范围限制在条件括号内
            s.push(node->tree_node);
        else
            s.push(node->tree_node->children[0]->children[2]);

        while (!s.empty()) {
            const TreeNode *tmp = s.top();
            s.pop();
            if (tmp->type == IDENTIFIER)
                args.emplace_back(tmp->nodeName);
            for (auto child: tmp->children)
                s.push(child);
            
        }
        if (args.size()>3)
            args.erase(args.begin());
        //
        if (args.empty() || (args.back() != "pthread_create" && args.back() != "pthread_join"))
            continue;

        // args
        if (args.back() == "pthread_create") {
            handle_pthread_create(cfg_, args, node);
        }
        if (args.back() == "pthread_join") {
            handle_pthread_join(cfg_, args, node);
        }
    }
}

int preProcessor_pthread(CFG& cfg_) {

    // 1
    findDeclarationOfPthread(cfg_);

    // 2
    linkPthread(cfg_);

    return 0;

}

void CFGNode::getSentence() {
    this->sentence.clear();         //

    if (this->type == SELECTION) {
        // if or if-else
        if (this->tree_node->children.front()->children.size() == 5) {
            this->sentence = "if ( ";
            this->sentence += getLeaves(this->tree_node->children.front()->children[2]);
            this->sentence += " ) {XXX} ";
        }
        if (this->tree_node->children.front()->children.size() == 7) {
            this->sentence = "if ( ";
            this->sentence += getLeaves(this->tree_node->children.front()->children[2]);
            this->sentence += " ) {XXX} else {XXX} ";
        }
        return;
    }
    if (this->type == ITERATION) {
        //
        this->sentence = "while ( ";
        this->sentence += getLeaves(this->tree_node->children.front()->children[2]);
        this->sentence += " ) {XXX} ";
        return;
    }
    if (this->type == FUNCTION) {
        //
        this->sentence = "";
        for (auto node: this->tree_node->children)
            if (node->nodeName!="compound_statement")
                this->sentence+= getLeaves(node);
        return;
    }
//
//    if (this->type == COMPOUND){
//        this->sentence = "{ }";
//        return;
//    }

    //
    this->sentence = getLeaves(this->tree_node);

    static int cnt = 0;
    if (this->type ==COMPOUND) {
        this->sentence += to_string(cnt++);
    }
}

//
std::string CFGNode::getLeaves(const TreeNode *r_) {
    if (r_->children.empty()) {
        return r_->nodeName + ' ';                    //
    }

    std::string ret;
    for (auto child: r_->children)
        ret += getLeaves(child);                 //
    return ret;
}

//
std::vector<std::string> CFGNode::getArguments(const TreeNode *r_){
    vector<string> ret;
    // dfs
    static const int IDENTIFIER = 258;
    // IDENTIFIER
    stack<const TreeNode *> s;
    s.push(r_);
    while (!s.empty()) {
        const TreeNode *node = s.top();
        s.pop();
        if (node->children.size() == 0){
            if (node->type == IDENTIFIER)
                ret.emplace_back(node->nodeName);
        }
        else{
            for(auto child:node->children)
                s.push(child);
        }
    }
    return ret;
}

// release
CFG::~CFG() {
    for (auto node: this->nodes)
        delete node;
}

/***********************************************************************
              dfd
***********************************************************************/

// output
static void print_DD(const CFG&cfg_) {
    printf("======================================\n");
    printf("==============data dependence==============\n");
    for (auto node: cfg_.nodes) {
        if (node->type == CFGNode::COMPOUND)
            continue;
        printf("%s \n", node->sentence.c_str());
        printf("readVs : ");
        for (auto v:node->readVs)
            printf("%s,", v.c_str());
        printf("\n");
        printf("writeVs: ");
        for (auto v:node->writeVs)
            printf("%s,", v.c_str());
        printf("\n");

        printf("data dependence:\t-----\n");
        for (auto dd:node->dataDependence)
            printf("\t\t\t\t\t%s\n", dd->sentence.c_str());

        printf("\n");
    }
    printf("======================================\n");
    return;
}

void CFGNode::findDataDependence(CFGNode *const cur_node_, const string &v_){
    if (this->vis.count(cur_node_) || cur_node_ == this)
        return;                         //
    else
        this->vis.insert(cur_node_);    //

    if (cur_node_->type==CFGNode::DECLARATION
        && std::find(cur_node_->writeVs.begin(), cur_node_->writeVs.end(), v_) != cur_node_->writeVs.end()) {
        //
        this->dataDependence.emplace_back(cur_node_);
        return;
    }
    if (std::find(cur_node_->writeVs.begin(), cur_node_->writeVs.end(), v_) != cur_node_->writeVs.end()) {
        //
        this->dataDependence.emplace_back(cur_node_);
        return;
    }

    //
    for (auto node: cur_node_->preNodes) {
        this->findDataDependence(node, v_);
    }

    return;
}


std::vector<std::string> function_changeArg = {
    "strcpy"
};

void CFGNode::extractIdentifier() {
    //
    static const int IDENTIFIER = 258;
    static const int INC_OP = 263;
    static const int DEC_OP = 264;
    //
    stack<const TreeNode *> s;
    s.push(this->tree_node);
    while (!s.empty()) {
        const TreeNode *node = s.top();
        s.pop();

        if (node->nodeName == "selection_statement" || node->nodeName == "iteration_statement") {
            // selection loop
            s.push(node->children[2]);
            continue;
        }
        if (node->nodeName == "assignment_expression" && node->children.size()!=1 ) { // 防止条件表达式进入这个判断
            // assignment
            this->writeVs.emplace_back(getLeaves(node->children.front()));
            s.push(node->children.back());
            continue;
        }
        if (node->nodeName == "unary_expression"
            && (node->children.front()->type == INC_OP || node->children.front()->type == DEC_OP)) {
            // ++  --
            this->writeVs.emplace_back(getLeaves(node->children.back()));
            this->readVs.emplace_back(getLeaves(node->children.back()));
            continue;
        }
        if (node->nodeName == "postfix_expression"
            && (node->children.back()->type == INC_OP || node->children.back()->type == DEC_OP)) {
            //  ++ --
            this->writeVs.emplace_back(getLeaves(node->children.front()));
            this->readVs.emplace_back(getLeaves(node->children.front()));
            continue;
        }
        if (node->type == IDENTIFIER && node->parent->nodeName == "direct_declarator") {
            //
            this->writeVs.emplace_back(getLeaves(node));
            continue;
        }
        if (node->type == IDENTIFIER) {
            //
            this->readVs.emplace_back(getLeaves(node));
            continue;
        }
        if (node->nodeName=="postfix_expression" && node->children.back()->nodeName == ")" 
            && node->children.size() == 4 ) {

            string function_name = getLeaves(node->children.front());
            function_name.pop_back(); //
            // extract parameter
            vector<string> args = getArguments(node->children[2]);

            bool write = false;
            for (auto f : function_changeArg)
                if (function_name == f)
                    write = true;
            if (write)
                for (auto arg:args){
                    this->writeVs.emplace_back(arg + ' ');
                }
            // continue;
        }
        for (auto child: node->children)
            if (child)
                s.push(child);
    }

}

// data dependence
static void linkUsage2Declaration(CFGNode *node, const string &v_) {
    queue<CFGNode *> q;
    q.push(node);
    node->vis.clear();
    while (!q.empty()) {
        if (std::find(q.front()->readVs.begin(), q.front()->readVs.end(), v_) != q.front()->readVs.end()
            || std::find(q.front()->writeVs.begin(), q.front()->writeVs.end(), v_) != q.front()->writeVs.end()) {
            //
            if (std::find(q.front()->dataDependence.begin(), q.front()->dataDependence.end(), node)
                    ==q.front()->dataDependence.end())
                //
                q.front()->dataDependence.emplace_back(node);
            else
                //
                ;
        }

        //
        for (int i = 0; i < 2; i++) {
            if (q.front()->next[i] && node->vis.find(q.front()->next[i])==node->vis.end()) {
                q.push(q.front()->next[i]);
                node->vis.insert(q.front()->next[i]);
            }
        }

        q.pop();
    }
}

void buildDataDependence(CFG &cfg_){

    //  1
    for (auto node:cfg_.nodes)
        if (node->tree_node)
            node->extractIdentifier();

    //  2
    for (auto node:cfg_.nodes) {
        for (auto v:node->readVs) {
            node->vis.clear(); // clear paths
            for (auto pre_node: node->preNodes)
                node->findDataDependence(pre_node, v);
        }
    }

    // 3
    for (auto node :cfg_.nodes) {
        if (node->type==CFGNode::DECLARATION){
            for (auto v: node->writeVs)
                linkUsage2Declaration(node, v);
        }
    }

//    print_DD(cfg_); // debug
}


/***********************************************************************
              cfd
***********************************************************************/

static void print_CD(const CFG&cfg_) {
    printf("======================================\n");
    printf("==============控制依赖情况==============\n");
    for (auto node: cfg_.nodes) {
        printf("%s \n", node->sentence.c_str());

        if (node->controlDependOn.size())
            for (auto cd: node->controlDependOn)
                printf("control dependent on :%s\n", cd->sentence.c_str());

        if (node->PDTreeParent)
            printf("PDTree parent:\t%s\n", node->PDTreeParent->sentence.c_str());
//        printf("post dominate by:\t-----\n");
//        for (auto dd:node->postDominatedBy)
//            printf("\t\t\t\t\t%s\n", dd->sentence.c_str());

        printf("\n");
    }
//    printf("-----------must execute-----------\n");
//    for (auto node:cfg_.nodes)
//        if (!node->controlDependOn)
//            printf("%s\n", node->sentence.c_str());

    printf("======================================\n");
}


//
static void testByDelete(CFGNode *node, CFGNode* test_node) {

    queue<const CFGNode *> q;   //
//    set<const CFGNode *> vis; //
    node->vis.clear();          //
    q.push(node);
    node->vis.insert(node);
    while (!q.empty()) {
        if (q.front() == test_node ) {                                      // 删除测试节点
            q.pop();
            continue;
        }
        if (q.front()->sentence == "END NODE") {
//            node->postDominatedBy.emplace_back(test_node);
            return;                                                         // 找到末尾节点，说明该测试不是后必经节点
        }
        for (int i = 0; i < 2; i++)
            if (q.front()->next[i] && node->vis.find(q.front()->next[i]) == node->vis.end()) {
                q.push(q.front()->next[i]);
                node->vis.insert(q.front()->next[i]);
            }

        q.pop();
    }
    node->postDominatedBy.emplace_back(test_node);                          // 如果不能找到末尾
}

// After finding the parent node (the parent node in the tree), you must find a parent node
static void findPDTreeParent(CFGNode *node) {

    node->vis.clear();
    queue<const CFGNode*> q;
    q.push(node);
    while (!q.empty()) {
        if (std::find(node->postDominatedBy.begin(), node->postDominatedBy.end(), q.front())
                !=node->postDominatedBy.end()) {
            node->PDTreeParent = q.front();
            return;
        }
        for (int i = 0; i < 2; i++)
            if (q.front()->next[i] && node->vis.find(q.front()->next[i]) == node->vis.end()) {
                q.push(q.front()->next[i]);
                node->vis.insert(q.front()->next[i]);
            }
        q.pop();
    }
}

// construct Post dominance tree
static void buildPostDominateTree(CFG &cfg_) {
    // 1
    for (auto node: cfg_.nodes) {
//        if (node->type == CFGNode::EMPTY)
//            continue;
        for (auto test_node: cfg_.nodes) {
            if (test_node == node || test_node->sentence == "START NODE")
                //
                continue;
            testByDelete(node, test_node);
        }
    }
    // 2
    for (auto node: cfg_.nodes) {
        findPDTreeParent(node);
    }
}

static bool isPostDominated(const CFGNode*a, const CFGNode *b) {
    if (!b->PDTreeParent) //
        return false;

    while (b) {
        if (b == a)
            return true;
        b = b->PDTreeParent;
    }
    return false; //
}

//
static void findPaths(const CFGNode *cur, const CFGNode *b, vector<const CFGNode *> &cur_path,
                      vector<vector<const CFGNode *>> &paths) {
    if (cur == b) {
        paths.emplace_back(cur_path);
        return;
    }
    for (int i=0;i<2;i++)
        if (cur->next[i] && std::find(cur_path.begin(), cur_path.end(), cur->next[i]) == cur_path.end()) {
            cur_path.emplace_back(cur);
//            printf("cur->next[i] is %s\n", cur->next[i]->sentence.c_str());
            findPaths(cur->next[i], b, cur_path, paths);
            cur_path.pop_back();
        }
}

// Get all paths from node a to node B
static void getAllPaths(const CFGNode *a, const CFGNode *b, vector<vector<const CFGNode*>> &paths) {
    /**
     * Search recursively
     */
//    printf("%s->%s\n", a->sentence.c_str(), b->sentence.c_str());
    vector<const CFGNode*> cur_path;
    for (int i=0;i<2;i++)
        if (a->next[i]){
//            cur_path.emplace_back(a->next[i]);
            findPaths(a->next[i],b,cur_path,paths);
//            cur_path.pop_back();
        }
}

// Calculate the control dependency according to the post dominance tree
static void calcControlDependenceByPDTree(CFG &cfg_) {

    for (auto m: cfg_.nodes)
        for (auto n: cfg_.nodes){
            if (m == n)
                continue;
            bool cd = true;
            vector<vector<const CFGNode*>> paths;
            getAllPaths(m, n, paths);
            //
            if (paths.empty()) {
                cd = false;
                // there is no dependence
                continue;
            }
            // Post domination
            for (const auto &path: paths) {
                cd = true;
                for (auto node: path) {
                    if (!isPostDominated(n, node)) {
                        cd = false;
                        break;
                    }
                }
                if (cd)                             // find a path
                    break;
                if (!cd && path == paths.back())    // After searching all paths, the legal path cannot be found, and it is judged that it does not exist
                    cd = false;
            }

            // Next, consider the second condition, m cannot be dominated by n
            if (isPostDominated(n, m))
                cd = false;

            // The two conditions end and the control dependency is written
            if (cd)
//                n->controlDependOn = m;
                n->controlDependOn.emplace_back(m);

        }
}

// Complete the control dependency. All points that must be executed depend on the entry node
// The following mandatory statements of while statements should be controlled and depend on while (considering dead loop)
// Function node control depends on the call point
static void completeCD(CFG& cfg_) {
    for (auto node:cfg_.nodes) {
        if (node->controlDependOn.empty()) {
//            node->controlDependOn = cfg_.nodes.front();
            if (node->type == CFGNode::EMPTY)
                continue;

            const TreeNode *fun_node = node->tree_node;
            while (fun_node && fun_node->nodeName!="function_definition")
                fun_node= fun_node->parent;
            if (!fun_node) // This situation corresponds to the definition of global variables
                continue;
            CFGNode *tmp = getCFGNodeByTreeNode(fun_node, cfg_);
//            node->controlDependOn = tmp;
            node->controlDependOn.emplace_back(tmp);
        }
        if (node->type==CFGNode::ITERATION) {
//            node->next[1]->controlDependOn.emplace_back(node);
            // Update: the necessary statements after the while statement must be controlled and depend on the while statement
            for (auto hbj: node->postDominatedBy)
                hbj->controlDependOn.emplace_back(node);
        }
        if(node->type==CFGNode::FUNCTION) {
            extern string findFunctionName(TreeNode *fun);
            string function_name = findFunctionName(node->tree_node);
            // all call site
            for (auto n: cfg_.nodes)
                if (n->sentence.find(function_name) != string::npos
                    && n->sentence.find("pthread_create") == string::npos)
                    // The point whose function name can be found and not created for the thread is its call point
                    node->controlDependOn.emplace_back(n);
        }
    }
}

void buildControlDependence(CFG &cfg_) {

    // 1
    buildPostDominateTree(cfg_);

    // 2
    calcControlDependenceByPDTree(cfg_);

    // 3
    completeCD(cfg_);

//    print_CD(cfg_);
}



/***********************************************************************
              slicing
***********************************************************************/

// 调试输出切片，简单观察
static void print_PS(const CFG &cfg_) {
    printf("======================================\n");
    printf("==============Slicing execution==============\n");

    for (auto node:cfg_.nodes)
        if (node->keep)
            printf("%s\n", node->sentence.c_str());

    printf("======================================\n");
}

// When saving a single sentence, the passed in parameter should be a statement
static void keepState(TreeNode *node) {
    if (node->children.size() && node->children.front()->nodeName == "compound_statement") {
        CFGNode *tmp = getCFGNodeByTreeNode(node->children[0]->children.front(), cfg);
        tmp->keep = true;
        tmp = getCFGNodeByTreeNode(node->children[0]->children.back(), cfg);
        tmp->keep = true;
    }
    else {
        // Direct saving of ordinary single sentence
        CFGNode *tmp = getCFGNodeByTreeNode(node, cfg);
        tmp->keep = true;
    }
}

// Syntax integrity preservation strategy to prevent syntax errors
static void keepGrammar(CFGNode *node){
    if (node->type == CFGNode::SELECTION) {
        if (node->tree_node->children[0]->children.size() == 5) {
            keepState(node->tree_node->children[0]->children[4]);
        }
        else { // seven，if-else
            keepState(node->tree_node->children[0]->children[4]);
            keepState(node->tree_node->children[0]->children[6]);
        }
    }
    if (node->type == CFGNode::ITERATION) {
        keepState(node->tree_node->children[0]->children[4]);
    }
    if (node->type == CFGNode::FUNCTION) {
        keepState(node->tree_node->children.back()->children.front());
        keepState(node->tree_node->children.back()->children.back());
    }
}

// Find the write point of a variable in all threads
static void findWritePoints(const vector<string> &readVs, vector<CFGNode*>& write_list) {
    // all variable
    for (auto v: readVs) {
        // check all threads
        int a=1-1;
        for (auto pthread: cfg.pthreads) {
            const CFGNode *fun_node = pthread->begin;
            // Check each node of this thread
            for (auto node: cfg.nodes)
                if (node->function != pthread->begin
                    || std::find(node->writeVs.begin(), node->writeVs.end(), v) == node->writeVs.end())
                    // If the node does not belong to the thread or does not write to the specified variable
                    continue;
                else
                    write_list.emplace_back(node);
        }
        // check main thread
        for (auto node: cfg.nodes) {
            // First, check whether the specified variable is written
            if (std::find(node->writeVs.begin(), node->writeVs.end(), v) == node->writeVs.end())
                continue; // if not write variable
            // Traverse the control flow and check whether it is called by the main function
            bool main_control = false;
            node->vis.clear(); // mark clear
            queue<CFGNode*> q;
            q.push(node);node->vis.insert(node);
            while (!q.empty()){
                if (q.front() == cfg.nodes.front()->next[1])
                    main_control = true;
                for (auto dep: q.front()->controlDependOn)
                    if (node->vis.find(dep)==node->vis.end()) {
                        node->vis.insert(dep);
                        q.push(dep);
                    }
                q.pop();
            }
            if (!main_control)
                continue; // main control
            write_list.emplace_back(node);
        }
    }
}

void sliceByCFGNode(CFG &cfg_, CFGNode *node) {
    node->keep = true;
    /**
     * 1、Wide search, find the thread dependency, data dependency and program dependency of the target node, and so on
     */
    queue<CFGNode *> q;
    q.push(node);
    while (!q.empty()) {
        // data dependence
        for (auto dep: q.front()->dataDependence)
            if (!dep->keep){
                dep->keep = true;
                q.push(dep);
            }
        // control dependence
        for (auto dep: q.front()->controlDependOn) {
            if ( !dep->keep) {
                dep->keep = true;
                q.push(dep);
            }
        }

        // thread dependence
        /**Traversal to find each thread, involving the location of the write to search
         */
        vector<CFGNode *> write_list;
        findWritePoints(q.front()->readVs, write_list);
        for (auto pthread_node: write_list)
            // 如果这个节点还没有被添加到图中
            if (!pthread_node->keep) {
                pthread_node->keep = true;
                q.push(pthread_node);
            }

        q.pop();
    }


}

void slice(CFG &cfg_) {
    const std::vector<string> import_id ={
            "assume_abort_if_not",
            "reach_error",
            "abort"
    };
    // For each node, check whether the above keywords exist in its sense. If so, slice it
    for (auto node: cfg_.nodes) {
        bool check = false;
        for (const auto& key_word: import_id)
            if (node->sentence.find(key_word)!=std::string::npos) {
                check = true;
                break;
            }
        // If there is no keyword in the statement, skip it directly without slicing it
        if (!check)
            continue;

        node->keep = true;
        sliceByCFGNode(cfg_, node);
    }

    // 2 check if while
    for (auto node_iw: cfg_.nodes)
        if ((node_iw->type == CFGNode::SELECTION && node_iw->keep)
            || (node_iw->type == CFGNode::ITERATION && node_iw->keep)
            || node_iw->type == CFGNode::FUNCTION)
            keepGrammar(node_iw);

    // 3 Save all global variables
    for (auto node: cfg_.nodes) {
        if (node->type == CFGNode::DECLARATION && node->tree_node->parent->nodeName == "external_declaration") {
            node->keep = true;
        }
    }

    // 4.1 Retain thread related code   declaration、create、join
    for( auto pthread:cfg_.pthreads)
        if (pthread->begin->keep){
            pthread->decalration->keep = true;
            pthread->end->keep = true;
            pthread->create->keep = true;
            if (pthread->join)
                pthread->join->keep = true;
        }

    // 4.2 Retain thread related code   mutex
    for (auto node : cfg_.nodes) {
        if (node->sentence.find("pthread_mutex_lock") != string::npos ||
            node->sentence.find("pthread_mutex_unlock") != string::npos ||
            node->sentence.find("pthread_mutex_init") != string::npos)
            node->keep = true;
    }

    // 5 main
    cfg_.nodes.front()->keep = true;
    cfg_.nodes.front()->next[1]->keep = true;

    // 6 Atomic execution all reservations
    for (auto node:cfg_.nodes) {
        if (node->type != CFGNode::FUNCTION)
            continue;
        if (node->sentence.find("__VERIFIER_atomic_")== string::npos)
            continue;
        // Find all function definition statements
        CFGNode *tmp = node;
        while (tmp) {
            tmp->keep = true;
            tmp = tmp->next[1];
        }
    }

//    print_PS(cfg_);
}

/***********************************************************************
              输出结果部分
***********************************************************************/
FILE *f;
set<const TreeNode *> NODES_T;

//增加行号计数
int rowCount = 1;
RowMap rowMap;

// 输出树，根结点
static void print_tree(const TreeNode *r_, const CFG &cfg_) {
    if (r_->children.size()==0) {
        fprintf(f, "%s ", r_->nodeName.c_str());
    }
    else {

        for (auto child: r_->children)
            if (NODES_T.find(child) == NODES_T.end()) {
                print_tree(child, cfg_);
            }
            else {
                if(r_->nodeName != "compound_statement") {
                    //排除复合语句中{}带来的行号覆盖问题
                    int originRow = child->row;
                    auto riter = rowMap.find(originRow);
                    if (riter != rowMap.end()) {
                        if (riter->second != rowCount) {
                            //原始的一行被拆分到切片完程序的不同行
//                            cout << r_->nodeName << "所在行被拆分到了新程序的不同行" << endl;
                        }
                    } else
                        rowMap.emplace(originRow, rowCount);
                }
                CFGNode *tmp = nullptr;
                for (auto node:cfg_.nodes)
                    if (node->tree_node == child){
                        tmp = node;
                    }
                if (tmp->keep)
                    print_tree(child, cfg_);
//                else if (!tmp->keep &&
//                         (r_->nodeName == "selection_statement" || r_->nodeName == "iteration_satement"))
//                    fprintf(f, "\n;\n");
            }

    }


    if (NODES_T.find(r_)!=NODES_T.end()) {    // 遇到节点输出一个换行
        fprintf(f, "\n");
        rowCount++;
    }
}

RowMap output_c(string name_, const CFG &cfg_) {
    // 递归输出语法树，遇到CFG节点的语法树节点，则判断一下是否输出
    string::size_type pos = name_.find_last_of(".");
    const string name(name_.substr(0, pos) + "_slice.iii");
    f = fopen(name.c_str(), "wb");
    for(auto node:cfg_.nodes)
        if (node->tree_node)
            NODES_T.insert(node->tree_node);
    ///为了便于Lazy检测添加的头文件，DAMER不需要
    // fprintf(f, "#include<pthread.h>\n");
    // fprintf(f, "#include<malloc.h>\n");
    print_tree(cfg_.root, cfg_);

    fclose(f);

    return rowMap;
}