//
// Created on 2021/10/3.
//

#ifndef PS_3_CFG_H
#define PS_3_CFG_H

#include "Tree.h"
#include "rowTrans.h"
#include <vector>


class CFGNode{
public:
    TreeNode *tree_node = nullptr;                  // CFG节点对应的语法树根

    enum NodeType{EMPTY, BASIC, COMPOUND, SELECTION, ITERATION, JUMP, DECLARATION, FUNCTION};
    NodeType type = EMPTY;                          // CFG节点的相应语句类型

    std::string sentence = "";                      // CFG node

    CFGNode *next[2] = {nullptr, nullptr};          //
                                                    //
    vector<CFGNode *> preNodes;                     //

    set<const CFGNode *> vis;                       //

    CFGNode *function;                              // function

    //=========data variable=========
    vector<string> readVs;                          // read variable
    vector<string> writeVs;                         // write variable
    vector<CFGNode *> dataDependence;               // dfd

    //=========control variable=========
    vector<CFGNode *> postDominatedBy;              //
    const CFGNode * PDTreeParent = nullptr;         //
//    CFGNode * controlDependOn = nullptr;            // cfd
    vector<CFGNode *> controlDependOn;              // cfd

    //=========variable=========
    bool keep = false;                              //

    /* definition */

    void getSentence();                                                         //
    void extractIdentifier();                                                   //
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


int preProcessor_Tree2CFGnodes(TreeNode *root_, CFG& cfg_);


/**
 * gccprocess
 * @param cfg_
 * @return
 */
int preProcessor_LinkCFGNodes2Next(CFG& cfg_);

/**
 * gccprocess
 * @param cfg_
 * @return
 */
int preProcessor_pthread(CFG& cfg_);

/***********************************************************************
              dfd
***********************************************************************/

/**
 * dfd
 * @param cfg_
 */
void buildDataDependence(CFG &cfg_);




/***********************************************************************
              cfd
***********************************************************************/

/**
 * cfd
 * @param cfg_
 */
void buildControlDependence(CFG &cfg_);




/***********************************************************************
              slicing
***********************************************************************/

/**
 * cfd
 * @param cfg_
 * @param node
 */
void sliceByCFGNode(CFG &cfg_, CFGNode *node);

/**
 * slicing
 * @param cfg_
 */
void slice(CFG &cfg_);



/***********************************************************************
              out
***********************************************************************/

/**
 * out
 * @param name_ name
 */
RowMap output_c(string name_, const CFG &cfg_);

#endif //PS_3_CFG_H
