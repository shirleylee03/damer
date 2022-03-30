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

    /* Initial */
    int cnt = 0; // counter
    queue<pair<TreeNode*, int>> q;
    q.push(pair<TreeNode*, int>(r_, cnt));
    fprintf(f, "\tnode%d[label=\"%s\"]\n", cnt++, r_->nodeName.c_str());
    /* start */
    while (!q.empty()){
//        TreeNode *tmp = stk.top().first;
//        int num = stk.top().second;
        // The child nodes of this node only need to consider their own child nodes
        for (auto child:q.front().first->children){
            // construct and link
            fprintf(f, "\tnode%d[label=\"%s\"]\n", cnt, child->nodeName.c_str());
            fprintf(f, "\tnode%d->node%d\n", q.front().second, cnt);
            // push
            q.push((pair<TreeNode*, int>(child, cnt)));

            cnt++; // Counter increment
        }

        // pop
        q.pop();
    }
    fprintf(f,"}\n");
    fclose(f);

    // graph
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
     * Try using the address as the node subscript of the dot file
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

// Open the file and store the pointer in its own private member, including the initialization of the outer brace of the dot file
void GraphMaker::fileOpen() {
    this->dot = fopen(this->dot_name.c_str(), "wb");
    fprintf(this->dot, "digraph G{\n");
}

// set filename
void GraphMaker::setFilename(const char *dot_, const char *jpg_) {
    this->dot_name = dot_;
    this->jpg_name = jpg_;
}

// graph
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
