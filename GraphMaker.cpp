//
// Created on 2021/10/2.
//

#include "GraphMaker.h"
#include <queue>
#include <stdlib.h>

void GraphMaker::printHello() {

    printf("hello graph maker!\n");
}


void GraphMaker::printTree(TreeNode *r_) {
    const string dot_name = "dotAST.dot";
    const string jpg_name = "jpgAST.jpg";

    FILE *f= fopen(dot_name.c_str(),"wb");
    fprintf(f, "digraph G{\n");

    /* 初始化深搜条件 */
    int cnt = 0; // 计数器
    queue<pair<TreeNode*, int>> q;
    q.push(pair<TreeNode*, int>(r_, cnt));
    fprintf(f, "\tnode%d[label=\"%s\"]\n", cnt++, r_->nodeName.c_str());
    /* 开始深搜 */
    while (!q.empty()){
//        TreeNode *tmp = stk.top().first;
//        int num = stk.top().second;
        // 该结点的子节点，只需考虑各自的子节点即可
        for (auto child:q.front().first->children){
            // 构建并连接
            fprintf(f, "\tnode%d[label=\"%s\"]\n", cnt, child->nodeName.c_str());
            fprintf(f, "\tnode%d->node%d\n", q.front().second, cnt);
            // 入栈
            q.push((pair<TreeNode*, int>(child, cnt)));

            cnt++; // 计数器递增
        }

        // 处理完毕出栈
        q.pop();
    }
    fprintf(f,"}\n");
    fclose(f);

    // 绘图
    string cmd = "dot -Tjpg ";
    cmd+=dot_name;
    cmd+=" -o ";
    cmd +=jpg_name;
    system(cmd.c_str());
}

void GraphMaker::printCFG(const CFG &cfg_) {
    this->setFilename("dotCFG.dot", "jpgCFG.jpg");
    this->fileOpen();
    /**
     * 尝试使用地址作为dot文件的节点下标
     */
    for(auto node:cfg_.nodes)
        fprintf(dot, "\tnode%lld[label=\"%s\", shape=%s]\n"
                ,(long long) node
                , node->sentence.c_str()
                ,(node->keep?"plaintext":"ellipse"));

    for (auto node:cfg_.nodes){
        if (node->next[0])
            fprintf(dot, "\tnode%lld->node%lld\n", (long long )node, (long long )node->next[0]);
        if (node->next[1])
            fprintf(dot, "\tnode%lld->node%lld\n", (long long )node, (long long )node->next[1]);
    }
    this->dot2jpg();
}

// 打开文件，并将指针存放在本身私有成员，包含dot文件的外层大括号初始化
void GraphMaker::fileOpen() {
    this->dot = fopen(this->dot_name.c_str(), "wb");
    fprintf(this->dot, "digraph G{\n");
}

// 设置文件名
void GraphMaker::setFilename(const char *dot_, const char *jpg_) {
    this->dot_name = dot_;
    this->jpg_name = jpg_;
}

// 画图，包括dot文件的尾部大括号
void GraphMaker::dot2jpg() {
    fprintf(this->dot, "}\n");
    fclose(this->dot);

    string cmd = "dot -Tjpg ";
    cmd += dot_name;
    cmd += " -o ";
    cmd += jpg_name;
    system(cmd.c_str());
}

void GraphMaker::printPDT(const CFG &cfg_) {
    this->setFilename("dotPDT.dot", "jpgPDT.jpg");
    this->fileOpen();

    for (auto node: cfg_.nodes)
        fprintf(dot, "\tnode%lld[label=\"%s\"]\n",(long long) node, node->sentence.c_str());

    for (auto node: cfg_.nodes)
        if (node->PDTreeParent)
            fprintf(dot, "\tnode%lld->node%lld", (long long) node->PDTreeParent, (long long) node);

    this->dot2jpg();
}
