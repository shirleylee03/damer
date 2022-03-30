//
// Created on 2021/10/20.
//

#ifndef PS_3_GRAPHMAKER_H
#define PS_3_GRAPHMAKER_H

#include <iostream>
#include "Tree.h"
#include "CFG.h"

class GraphMaker {
private:
    GraphMaker() = default;

    FILE *dot = nullptr;
    std::string dot_name, jpg_name;
    void setFilename(const char *dot_, const char *jpg_);
    void fileOpen();
    void dot2jpg();

public:
    static GraphMaker* GetInstance()
    {
        static GraphMaker Instance;
        return &Instance;
    }

    void printHello();

    /**
     * 输出抽象语法树的jpg格式
     * 深搜方式遍历树，然后输出jpg
     * 注意此处使用手工栈方式进行深搜
     * @param r_
     */
    void printTree(TreeNode *r_);       //

    /**
     * 输出CFG的jpg格式
     * @param cfg_
     */
    void printCFG(const CFG &cfg_);     //

    /**
     * 输出后支配树
     * @param cfg_
     */
    void printPDT(const CFG &cfg_);
};


#endif //PS_3_GRAPHMAKER_H
