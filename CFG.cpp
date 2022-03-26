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

// 链接主线程和其他线程
static void linkPthread(CFG &cfg_) {
    static const int IDENTIFIER = 258;

    for (auto node: cfg_.nodes) {
        // 线程的创建和终止，在目前都是BASIC类型，这个条件更新
        // 线程的创建和终止，不一定是BASIC，有可能是其它，通过查找pthread create关键字确定
        // if (node->type!=CFGNode::BASIC)
        //     continue;
        if (node->sentence.find("pthread_create") == string::npos &&
            node->sentence.find("pthread_join") == string::npos)
            continue;

        // 找到参数，args中第一个应该是pthread
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
        // 执行至此，检查args里面的第一个元素，不是create和join的要忽略
        if (args.empty() || (args.back() != "pthread_create" && args.back() != "pthread_join"))
            continue;

        // args里面的参数是逆序的（由于搜索树的形状是逆序宽搜）
        if (args.back() == "pthread_create") {
            handle_pthread_create(cfg_, args, node);
        }
        if (args.back() == "pthread_join") {
            handle_pthread_join(cfg_, args, node);
        }
    }
}

int preProcessor_pthread(CFG& cfg_) {
    /**
     * 线程相关的预处理：
     * 1-找到所有线程的定义
     * 2-确定所有线程的起点位置和终点位置，CFGNode* 形式
     */

    // 1
    findDeclarationOfPthread(cfg_);

    // 2
    linkPthread(cfg_);

    return 0;

}

/**
 * 遍历叶子节点获得当前节点的语句
 * 需要增加空格
 */
void CFGNode::getSentence() {
    this->sentence.clear();         // 初始清空

    if (this->type == SELECTION) {
        // 分为if型(包含switch但是其余部分未处理switch)和if-else型
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
        // 只考虑while型的循环，其余不予考虑，使用预处理转换
        this->sentence = "while ( ";
        this->sentence += getLeaves(this->tree_node->children.front()->children[2]);
        this->sentence += " ) {XXX} ";
        return;
    }
    if (this->type == FUNCTION) {
        // 不要把函数的运行代码加入
        this->sentence = "";
        for (auto node: this->tree_node->children)
            if (node->nodeName!="compound_statement")
                this->sentence+= getLeaves(node);
        return;
    }
//    已经被拆分为左右大括号，直接取leaves即可
//    if (this->type == COMPOUND){
//        this->sentence = "{ }";
//        return;
//    }

    // 其余情况
    this->sentence = getLeaves(this->tree_node);

    static int cnt = 0;
    if (this->type ==COMPOUND) {
        this->sentence += to_string(cnt++);
    }
}

// 输入一个树的根结点，返回其叶子的字符串string形式
std::string CFGNode::getLeaves(const TreeNode *r_) {
    if (r_->children.empty()) {
        return r_->nodeName + ' ';                    // 叶子节点
    }

    std::string ret;
    for (auto child: r_->children)
        ret += getLeaves(child);                 // 非叶子节点则递归其叶子
    return ret;
}

// 输入一棵子树，返回其参数列表（标识符）。该函数为调取函数参数变量专用
std::vector<std::string> CFGNode::getArguments(const TreeNode *r_){
    vector<string> ret;
    // 函数执行机制为深搜找到每一个
    static const int IDENTIFIER = 258;
    // 深搜找每一个IDENTIFIER
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

// 虚析构，释放所有CFG节点的空间
CFG::~CFG() {
    for (auto node: this->nodes)
        delete node;
}

/***********************************************************************
              数据依赖部分
***********************************************************************/

// 调试输出，数据依赖情况
static void print_DD(const CFG&cfg_) {
    printf("======================================\n");
    printf("==============数据依赖情况==============\n");
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

/**
 * 查找某个CFG节点的某个变量的数据依赖，递归方式
 * @param cur_node_ 当前查找到的节点
 * @param v_ 变量的字符串形式
 */
void CFGNode::findDataDependence(CFGNode *const cur_node_, const string &v_){
    if (this->vis.count(cur_node_) || cur_node_ == this)
        return;                         // 重复访问则无视
    else
        this->vis.insert(cur_node_);    // 没有访问过则继续访问

    if (cur_node_->type==CFGNode::DECLARATION
        && std::find(cur_node_->writeVs.begin(), cur_node_->writeVs.end(), v_) != cur_node_->writeVs.end()) {
        // 变量的定义部分，可以不用继续往前查找
        this->dataDependence.emplace_back(cur_node_);
        return;
    }
    if (std::find(cur_node_->writeVs.begin(), cur_node_->writeVs.end(), v_) != cur_node_->writeVs.end()) {
        // 找到对变量的写入，也不需要继续查找
        this->dataDependence.emplace_back(cur_node_);
        return;
    }

    // 都没有，继续前向遍历
    for (auto node: cur_node_->preNodes) {
        this->findDataDependence(node, v_);
    }

    return;
}

/* 会导致参数内容修改的函数列表 */
std::vector<std::string> function_changeArg = {
    "strcpy"
};

void CFGNode::extractIdentifier() {
    // 提取所有变量标识符，并将其分类完成：读、写
    static const int IDENTIFIER = 258;
    static const int INC_OP = 263;
    static const int DEC_OP = 264;
    // 深搜属于这个语句的语法树，找到赋值符，以及前后自增自减
    stack<const TreeNode *> s;
    s.push(this->tree_node);
    while (!s.empty()) {
        const TreeNode *node = s.top();
        s.pop();

        if (node->nodeName == "selection_statement" || node->nodeName == "iteration_statement") {
            // 选择和循环语句只需要将其条件加入即可
            s.push(node->children[2]);
            continue;
        }
        if (node->nodeName == "assignment_expression" && node->children.size()!=1 ) { // 防止条件表达式进入这个判断
            // 赋值表达式
            this->writeVs.emplace_back(getLeaves(node->children.front()));
            s.push(node->children.back());
            continue;
        }
        if (node->nodeName == "unary_expression"
            && (node->children.front()->type == INC_OP || node->children.front()->type == DEC_OP)) {
            // 前置  ++  --
            this->writeVs.emplace_back(getLeaves(node->children.back()));
            this->readVs.emplace_back(getLeaves(node->children.back()));
            continue;
        }
        if (node->nodeName == "postfix_expression"
            && (node->children.back()->type == INC_OP || node->children.back()->type == DEC_OP)) {
            // 后置  ++ --
            this->writeVs.emplace_back(getLeaves(node->children.front()));
            this->readVs.emplace_back(getLeaves(node->children.front()));
            continue;
        }
        if (node->type == IDENTIFIER && node->parent->nodeName == "direct_declarator") {
            // 变量定义
            this->writeVs.emplace_back(getLeaves(node));
            continue;
        }
        if (node->type == IDENTIFIER) {
            // 普通变量
            this->readVs.emplace_back(getLeaves(node));
            continue;
        }
        if (node->nodeName=="postfix_expression" && node->children.back()->nodeName == ")" 
            && node->children.size() == 4 ) {
            // 赋值表达式
            // 检测函数调用专用判定（带有参数的函数）
            // 提取函数名
            string function_name = getLeaves(node->children.front());
            function_name.pop_back(); // 最后一个字符为空格，删除
            // 提取参数
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
            // 此处不需要continue
            // 原因：在确定了函数可能修改变量之后，只增加写属性，未增加读属性，仍需继续
            // 向下搜索以确定该函数内部可能嵌套其它表达式，对变量的写操作
        }
        for (auto child: node->children)
            if (child)
                s.push(child);
    }

}

// 在变量的使用处添加指向其声明处的数据依赖
static void linkUsage2Declaration(CFGNode *node, const string &v_) {
    queue<CFGNode *> q;
    q.push(node);
    node->vis.clear();
    while (!q.empty()) {
        if (std::find(q.front()->readVs.begin(), q.front()->readVs.end(), v_) != q.front()->readVs.end()
            || std::find(q.front()->writeVs.begin(), q.front()->writeVs.end(), v_) != q.front()->writeVs.end()) {
            // 该结点找到了对所声明的变量的使用
            if (std::find(q.front()->dataDependence.begin(), q.front()->dataDependence.end(), node)
                    ==q.front()->dataDependence.end())
                // 数据依赖中没有存在声明节点
                q.front()->dataDependence.emplace_back(node);
            else
                // 已经存在声明节点
                ;
        }

        // 没根据next继续搜索
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
    /**
     * 数据依赖构建流程如下：
     * 1-提取所有变量（分节点）
     * 2-查找每个读变量的数据依赖点，指针形式
     * 3-对于变量的声明，总是成为其后续使用的数据依赖
     */

    //  1
    for (auto node:cfg_.nodes)
        if (node->tree_node)
            node->extractIdentifier();

    //  2
    for (auto node:cfg_.nodes) {
        for (auto v:node->readVs) {
            node->vis.clear(); // 清空查找路径
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

//    print_DD(cfg_); // debug输出
}


/***********************************************************************
              控制依赖部分
***********************************************************************/

// 调试输出，控制依赖情况
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


// 删除测试
static void testByDelete(CFGNode *node, CFGNode* test_node) {
    /**
     *    删除test_node节点，之后以node为起点进行搜索，如果不能找到END节点，说
     * 明test_node就是后必经节点
     *    该算法时间复杂度较高(宽搜)
     */

    queue<const CFGNode *> q;   // 宽搜队列
//    set<const CFGNode *> vis; // 用于记录哪些节点已经访问过
    node->vis.clear();          // 用于记录哪些节点已经访问过
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

// 找到一个节点的后支配树父亲节点（指针），从后必经节点中查找
static void findPDTreeParent(CFGNode *node) {
    /**
     * 宽搜方式确定后必经节点中最近的一个
     */
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

// 构建后支配树，后支配树的每条边存在于postDominateBy中
static void buildPostDominateTree(CFG &cfg_) {
    /**
     * 1-以尝试删除的方式测试某个节点是不是选定节点的后必经节点
     * 2-从后必经节点中找到最接近的（后支配树父亲节点）
     */
    // 1
    for (auto node: cfg_.nodes) {
//        if (node->type == CFGNode::EMPTY)
//            continue;
        for (auto test_node: cfg_.nodes) {
            if (test_node == node || test_node->sentence == "START NODE")
                // 删除的时候需要考虑终止节点，否则最后终止节点不能作为后支配树的根结点
                continue;
            testByDelete(node, test_node);
        }
    }
    // 2
    for (auto node: cfg_.nodes) {
        findPDTreeParent(node);
    }
}

/**
 * 判断a是否后支配b
 * 算法：计算a是否是后支配树中b的祖先
 * @param a
 * @param b
 * @return
 */
static bool isPostDominated(const CFGNode*a, const CFGNode *b) {
    if (!b->PDTreeParent) // 如果b已经是树根，那么树根是不会被任何节点后支配的
        return false;

    while (b) {
        if (b == a)
            return true;
        b = b->PDTreeParent;
    }
    return false; // 遍历到树的根的根结点，还没有找到a，说明a不是b的祖先节点
}

// 递归找路径，cur为当前找到的节点，b为目标节点，cur path为当前已经走过的路径，找到目标之后将cur path加入paths
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

// 获取从节点a到节点b的所有路径
static void getAllPaths(const CFGNode *a, const CFGNode *b, vector<vector<const CFGNode*>> &paths) {
    /**
     * 采用递归方式进行搜索
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

// 根据后支配树计算控制依赖
static void calcControlDependenceByPDTree(CFG &cfg_) {
    /**
     * 控制依赖根据定义确定，对于任意两个节点m、n，要说n控制依赖于m [ m---(cd)--->n ]，应满足
     * 1、存在一条从m到n的路径，并且路径上任意不属于m、n的节点都是被n后支配的
     * 2、m不是被n后支配的
     * wei shen me yao xian ji suan 1 ne ? ni zhi dao ma ?
     * 另外，后支配：在后支配树中a是b的祖先，那么就说a后支配b
     */
    for (auto m: cfg_.nodes)
        for (auto n: cfg_.nodes){
            if (m == n)
                continue;
            bool cd = true;
            vector<vector<const CFGNode*>> paths;
            getAllPaths(m, n, paths);
            // 至此，找到所有路径paths，路径数量为paths.size()，注意路径上不包含m，n节点
            if (paths.empty()) {
                cd = false;
                // 不存在路径，条件不满足，一定不存在控制依赖关系
                continue;
            }
            // 接下来首先检查路径上的节点是否被n后支配
            for (const auto &path: paths) {
                cd = true;
                for (auto node: path) {
                    if (!isPostDominated(n, node)) {
                        cd = false;
                        break;
                    }
                }
                if (cd)                             // 找到了一条合法的路径，可以不用继续搜索了
                    break;
                if (!cd && path == paths.back())    // 搜索完成所有路径，未能找到合法路径，判断不存在
                    cd = false;
            }

            // 接下来考虑第二个条件，m不能是被n后支配的
            if (isPostDominated(n, m))
                cd = false;

            // 两个条件结束，开始写入控制依赖关系
            if (cd)
//                n->controlDependOn = m;
                n->controlDependOn.emplace_back(m);

        }
}

// 补全控制依赖，所有必须会执行的点依赖于入口节点
// while语句的后必经语句，都要控制依赖于while（考虑死循环）
// 函数节点控制依赖于调用点
static void completeCD(CFG& cfg_) {
    for (auto node:cfg_.nodes) {
        if (node->controlDependOn.empty()) {
//            node->controlDependOn = cfg_.nodes.front();
            if (node->type == CFGNode::EMPTY)
                continue;

            const TreeNode *fun_node = node->tree_node;
            while (fun_node && fun_node->nodeName!="function_definition")
                fun_node= fun_node->parent;
            if (!fun_node) // 这种情况对应的是全局变量的定义
                continue;
            CFGNode *tmp = getCFGNodeByTreeNode(fun_node, cfg_);
//            node->controlDependOn = tmp;
            node->controlDependOn.emplace_back(tmp);
        }
        if (node->type==CFGNode::ITERATION) {
//            node->next[1]->controlDependOn.emplace_back(node);
            // 更新：while语句的后必经语句，都要控制依赖于while
            for (auto hbj: node->postDominatedBy)
                hbj->controlDependOn.emplace_back(node);
        }
        if(node->type==CFGNode::FUNCTION) {
            extern string findFunctionName(TreeNode *fun);
            string function_name = findFunctionName(node->tree_node);
            // 所有调用点
            for (auto n: cfg_.nodes)
                if (n->sentence.find(function_name) != string::npos
                    && n->sentence.find("pthread_create") == string::npos)
                    // 能找到其函数名并且不为线程创建的点，就是其调用点
                    node->controlDependOn.emplace_back(n);
        }
    }
}

void buildControlDependence(CFG &cfg_) {
    /**
     * 构建控制依赖的步骤如下：
     * 1-构建后支配树
     * 2-根据后支配树计算控制依赖
     * 3-补充控制依赖，所有一定会执行的点依赖于开始节点START
     *   更新：所有一定会执行的点依赖于其函数入口
     *   添加：所有while节点的下一个语句一定控制依赖于while本身（考虑死循环）
     *   添加：函数节点控制依赖于调用点
     */

    // 1
    buildPostDominateTree(cfg_);

    // 2
    calcControlDependenceByPDTree(cfg_);

    // 3
    completeCD(cfg_);

//    print_CD(cfg_);
}



/***********************************************************************
              切片运行部分
***********************************************************************/

// 调试输出切片，简单观察
static void print_PS(const CFG &cfg_) {
    printf("======================================\n");
    printf("==============切片运行情况==============\n");

    for (auto node:cfg_.nodes)
        if (node->keep)
            printf("%s\n", node->sentence.c_str());

    printf("======================================\n");
}

// 保存单句，传入参数应该就是一个Statement
static void keepState(TreeNode *node) {
    if (node->children.size() && node->children.front()->nodeName == "compound_statement") {
        CFGNode *tmp = getCFGNodeByTreeNode(node->children[0]->children.front(), cfg);
        tmp->keep = true;
        tmp = getCFGNodeByTreeNode(node->children[0]->children.back(), cfg);
        tmp->keep = true;
    }
    else {
        // 普通单句直接保存
        CFGNode *tmp = getCFGNodeByTreeNode(node, cfg);
        tmp->keep = true;
    }
}

// 语法完整性 保存策略，为了防止语法出错
static void keepGrammar(CFGNode *node){
    if (node->type == CFGNode::SELECTION) {
        if (node->tree_node->children[0]->children.size() == 5) {
            keepState(node->tree_node->children[0]->children[4]);
        }
        else { // 7个，if-else型
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

// 找到某个变量在所有线程中的写入点
static void findWritePoints(const vector<string> &readVs, vector<CFGNode*>& write_list) {
    // 遍历每一个变量
    for (auto v: readVs) {
        // 检查所有线程
        int a=1-1;
        for (auto pthread: cfg.pthreads) {
            const CFGNode *fun_node = pthread->begin;
            // 检查这个线程的每一个节点
            for (auto node: cfg.nodes)
                if (node->function != pthread->begin
                    || std::find(node->writeVs.begin(), node->writeVs.end(), v) == node->writeVs.end())
                    // 如果该结点不属于该线程，或者没有对指定变量进行写操作
                    continue;
                else
                    write_list.emplace_back(node);
        }
        // 还要检查主线程
        for (auto node: cfg.nodes) {
            // 首先检查是否写入指定变量
            if (std::find(node->writeVs.begin(), node->writeVs.end(), v) == node->writeVs.end())
                continue; // 如果没有写入变量
            // 遍历控制流，检查是否由主函数调用
            bool main_control = false;
            node->vis.clear(); // 标记清空
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
                continue; // 不由主进程控制
            write_list.emplace_back(node);
        }
    }
}

void sliceByCFGNode(CFG &cfg_, CFGNode *node) {
    node->keep = true;
    /**
     * 1、宽搜，对目标节点查找其线程依赖, 数据依赖和程序依赖，以此类推回去
     */
    queue<CFGNode *> q;
    q.push(node);
    while (!q.empty()) {
        // 数据依赖
        for (auto dep: q.front()->dataDependence)
            if (!dep->keep){
                dep->keep = true;
                q.push(dep);
            }
        // 控制依赖
        for (auto dep: q.front()->controlDependOn) {
            if ( !dep->keep) {
                dep->keep = true;
                q.push(dep);
            }
        }

        // 线程依赖
        /**遍历查找每一个线程，涉及写的位置进行搜索
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
    /**
     * 1、遍历每一个块，检查是否出现过关键字，出现则对其作切片
     * 2、检查if、while，保留其下属第一级语句(函数需要保留其compound左右大括号)
     * 3、全局变量全部保存
     * 4、线程的相关代码全部保留：声明、create、join、mutex
     * 5、main函数保存
     * 6、原子性执行全部保留
     */
    const std::vector<string> import_id ={
            "assume_abort_if_not",
            "reach_error",
            "abort"
    };
    // 对于每一个节点，检查其sentence中是否存在上述关键字，存在的话，需要对其做切片
    for (auto node: cfg_.nodes) {
        bool check = false;
        for (const auto& key_word: import_id)
            if (node->sentence.find(key_word)!=std::string::npos) {
                check = true;
                break;
            }
        // 该语句中不存在关键字，那么直接跳过，不需要对其作切片
        if (!check)
            continue;

        node->keep = true;
        sliceByCFGNode(cfg_, node);
    }

    // 2 检查 if while
    for (auto node_iw: cfg_.nodes)
        if ((node_iw->type == CFGNode::SELECTION && node_iw->keep)
            || (node_iw->type == CFGNode::ITERATION && node_iw->keep)
            || node_iw->type == CFGNode::FUNCTION)
            keepGrammar(node_iw);

    // 3 全局变量全部保存
    for (auto node: cfg_.nodes) {
        if (node->type == CFGNode::DECLARATION && node->tree_node->parent->nodeName == "external_declaration") {
            node->keep = true;
        }
    }

    // 4.1 保留线程相关的代码   声明、create、join
    for( auto pthread:cfg_.pthreads)
        if (pthread->begin->keep){
            pthread->decalration->keep = true;
            pthread->end->keep = true;
            pthread->create->keep = true;
            if (pthread->join)
                pthread->join->keep = true;
        }

    // 4.2 保留线程相关的代码   mutex
    for (auto node : cfg_.nodes) {
        if (node->sentence.find("pthread_mutex_lock") != string::npos ||
            node->sentence.find("pthread_mutex_unlock") != string::npos ||
            node->sentence.find("pthread_mutex_init") != string::npos)
            node->keep = true;
    }

    // 5 main
    cfg_.nodes.front()->keep = true;
    cfg_.nodes.front()->next[1]->keep = true;

    // 6 原子性执行全部保留
    for (auto node:cfg_.nodes) {
        if (node->type != CFGNode::FUNCTION)
            continue;
        if (node->sentence.find("__VERIFIER_atomic_")== string::npos)
            continue;
        // 查找所有函数定义语句
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