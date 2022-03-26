//
// Created on 2021/10/3.
//

#ifndef PS_3_CFG_H
#define PS_3_CFG_H

#include "Tree.h"
#include "rowTrans.h"
#include <vector>

/**
 * CFG用于从TreeNode构建控制流图
 * 步骤如下：    1、提取基本块
 *              2、在基本块的基础上构建
 */

class CFGNode{
public:
    TreeNode *tree_node = nullptr;                  // CFG节点对应的语法树根

    enum NodeType{EMPTY, BASIC, COMPOUND, SELECTION, ITERATION, JUMP, DECLARATION, FUNCTION};
    NodeType type = EMPTY;                          // CFG节点的相应语句类型

    std::string sentence = "";                      // CFG节点对应的语句

    CFGNode *next[2] = {nullptr, nullptr};          // CFG节点下一节点指针，根据不同类型有不同的数量，一般只有1个后继节点，选择语句和
                                                    // 循环语句则有2个后继结点
    vector<CFGNode *> preNodes;                     // CFG中的前置节点

    set<const CFGNode *> vis;                       // 遍历查找中为了防止重复的标识

    CFGNode *function;                              // 函数节点

    //=========数据依赖部分变量=========
    vector<string> readVs;                          // 读变量
    vector<string> writeVs;                         // 写变量
    vector<CFGNode *> dataDependence;               // 数据依赖节点

    //=========控制依赖部分变量=========
    vector<CFGNode *> postDominatedBy;              // 后支配于某些节点(后必经节点)
    const CFGNode * PDTreeParent = nullptr;         // 后支配树父亲节点
//    CFGNode * controlDependOn = nullptr;            // 控制依赖于
    vector<CFGNode *> controlDependOn;              // 控制依赖于

    //=========程序切片部分变量=========
    bool keep = false;                              // 某个节点的语句是否要保留

    /* 以下是函数的定义 */

    void getSentence();                                                         // 获取该结点对应的语句（不完整对应）
    void extractIdentifier();                                                   // 提取所有的标识符（变量）
    void findDataDependence(CFGNode *const cur_node_, const string &v_);  //


private:
    std::string getLeaves(const TreeNode *r_);
    vector<string> getArguments(const TreeNode *r_);
};

class Pthread{
public:
    std::string name;
    std::string function_name;

    CFGNode *decalration = nullptr;
    CFGNode *create = nullptr, *join = nullptr;
    CFGNode *begin = nullptr, *end = nullptr;
};


class CFG{
public:
    std::vector<CFGNode*> nodes;
    const TreeNode *root;

    std::vector<Pthread *> pthreads;

    virtual ~CFG();
};


/**
 * 预处理器：专门用于将语法树转变为CFG节点（未建立勾连关系的节点）
 * @param root_ 输入语法树的根结点指针
 * @param cfg_ CFG的引用参数
 * @return 函数执行状态，0正常
 */
int preProcessor_Tree2CFGnodes(TreeNode *root_, CFG& cfg_);


/**
 * 预处理器：链接CFG节点，使之变成有向图
 * @param cfg_
 * @return
 */
int preProcessor_LinkCFGNodes2Next(CFG& cfg_);

/**
 * 预处理器：查找并链接线程调用情况
 * @param cfg_
 * @return
 */
int preProcessor_pthread(CFG& cfg_);

/***********************************************************************
              数据依赖部分
***********************************************************************/

/**
 * 构建数据依赖的总函数
 * @param cfg_
 */
void buildDataDependence(CFG &cfg_);




/***********************************************************************
              控制依赖部分
***********************************************************************/

/**
 * 构建控制依赖的总函数
 * @param cfg_
 */
void buildControlDependence(CFG &cfg_);




/***********************************************************************
              切片运行部分
***********************************************************************/

/**
 * 根据给出的一个CFG节点去进行切片，数据依赖、控制依赖
 * @param cfg_
 * @param node
 */
void sliceByCFGNode(CFG &cfg_, CFGNode *node);

/**
 * 切片的新版本函数，封装了重要的标识符，通过调用 sliceByCFGNode 来完成工作
 * @param cfg_
 */
void slice(CFG &cfg_);



/***********************************************************************
              输出结果部分
***********************************************************************/

/**
 * 输出切片之后的文件
 * @param name_ 输入文件名，会自动处理加上前缀
 */
RowMap output_c(string name_, const CFG &cfg_);

#endif //PS_3_CFG_H
