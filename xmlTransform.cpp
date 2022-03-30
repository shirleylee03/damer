//
// Created on 2021/10/2.
//
#include<string>
#include<stack>
#include<vector>
#include<iostream>
#include "BA/tinyxml.h"
#include "cpn.h"

TiXmlElement* statementTransform(TiXmlElement *statementNode, CPN *cpn, const RowMap &rowMap){
    vector<string> Ps,Ts;

    Row_Type row = atoi(statementNode->GetText());
    row = rowMap.find(row)->second;//row转换
    cpn->getRowRelatedPT(row,Ps,Ts);
    if(Ts.size() == 0)
        throw "statement property No corresponding changes were found!";
    TiXmlElement *newNode = new TiXmlElement("is-fireable");
    TiXmlElement *newTNode = new TiXmlElement("transition");
    newNode->LinkEndChild(newTNode);
    TiXmlText *text = new TiXmlText(Ts[0].c_str());
    newTNode->LinkEndChild(text);
    for(int i=1;i<Ts.size();i++) {
        TiXmlElement *newElement = new TiXmlElement("is-fireable");
        TiXmlElement *newTNode = new TiXmlElement("transition");
        newElement->LinkEndChild(newTNode);
        TiXmlText *newText = new TiXmlText(Ts[i].c_str());
        newTNode->LinkEndChild(newText);

        TiXmlElement *conjunctionNode = new TiXmlElement("conjunction");
        conjunctionNode->LinkEndChild(newNode);
        conjunctionNode->LinkEndChild(newElement);
        newNode = conjunctionNode;
    }
    return newNode;
}

bool AllisNum(string str)
{
    for (int i = 0; i < str.size(); i++)
    {
        int tmp = (int)str[i];
        if (tmp >= 48 && tmp <= 57)
        {
            continue;
        }
        else
        {
            return false;
        }
    }
    return true;
}

//Returns the string '' if an option does not exist
void parseVarStr(TiXmlElement *variableNode, string &name, string &index, string &tid, string &row){
    if(variableNode->FirstChildElement("name")){
//        string var_name, var_index, var_tid, var_row;
        name = variableNode->FirstChildElement("name")->GetText();
        index = variableNode->FirstChildElement("index")?variableNode->FirstChildElement("index")->GetText():"";
        tid = variableNode->FirstChildElement("thread")?variableNode->FirstChildElement("thread")->GetText():"";
        row = variableNode->FirstChildElement("row")?variableNode->FirstChildElement("row")->GetText():"";

        if(row != "" && !AllisNum(row)){
            cout<<"<row> must be number"<<endl;
            exit(-1);
        }
        if(index != "" && !AllisNum(index)){
            cout<<"<index> must be number"<<endl;
            exit(-1);
        }
        return;
    }
    else
        throw "it must have name label of Variable!";
}

TiXmlElement* variableTransform(TiXmlElement *variableNode, CPN *cpn, const RowMap& rowMap){
//    string var_str = variableNode->GetText();
    string name,index,tid,row_str;
    parseVarStr(variableNode,name,index,tid,row_str);
    Row_Type row = row_str == ""?InValidRow:static_cast<Row_Type>(atoi(row_str.c_str()));
    if(row!=InValidRow)
        row = rowMap.find(row)->second;//row转换

    string P_name = cpn->getVarRelatedP(name,row);

    TiXmlElement *newNode = new TiXmlElement("token-value");

    TiXmlElement *newNameNode = new TiXmlElement("name");
    TiXmlText *newNameText = new TiXmlText(P_name.c_str());
    newNameNode->LinkEndChild(newNameText);
    newNode->LinkEndChild(newNameNode);

    if(index != "") {
        TiXmlElement *newIndexNode = new TiXmlElement("index");
        TiXmlText *newIndexText = new TiXmlText(index.c_str());
        newIndexNode->LinkEndChild(newIndexText);
        newNode->LinkEndChild(newIndexNode);
    }

    if(tid != "") {
        TiXmlElement *newThreadNode = new TiXmlElement("thread");
        TiXmlText *newThreadText = new TiXmlText(tid.c_str());
        newThreadNode->LinkEndChild(newThreadText);
        newNode->LinkEndChild(newThreadNode);
    }
    return newNode;

}

//extract variable from xml
void extractStatementVariableNodesFromXML(TiXmlDocument *doc, vector<TiXmlElement*> &statements, vector<TiXmlElement*> &variables){

    TiXmlElement *root = doc->RootElement();

    std::stack<TiXmlElement*> elementStack;
    elementStack.push(root);

    while(!elementStack.empty()){
        TiXmlElement *current = elementStack.top();
        elementStack.pop();
        std::string value = current->Value();
        if (value == "statement")
            statements.push_back(current);
        else if (value == "variable")
            variables.push_back(current);
        auto element = current->FirstChildElement();
        if(element) {
            while (element) {
                elementStack.push(element);
                element = element->NextSiblingElement();
            }
        }
    }
}

void changeProgramXML2PDNetXML(std::string filename, CPN *cpn, const RowMap &rowMap){
    TiXmlDocument *doc = new TiXmlDocument(filename.c_str());
    try {
        if (!doc->LoadFile()) {
            std::cerr << doc->ErrorDesc() << std::endl;
            throw "can't open property file! filename:" + filename;
        }
    }
    catch (const char *msg){
        cerr << msg <<endl;
        exit(-1);
    }

    std::vector<TiXmlElement*> statementNodes,variableNodes;

    extractStatementVariableNodesFromXML(doc,statementNodes,variableNodes);
    for(int i=0;i<statementNodes.size();i++) {
        TiXmlElement *newNode = statementTransform(statementNodes[i], cpn, rowMap);
        auto parent = statementNodes[i]->Parent();
        parent->ReplaceChild(statementNodes[i],*newNode);
    }

    for(int i=0;i<variableNodes.size();i++) {
        TiXmlElement *newNode = variableTransform(variableNodes[i], cpn, rowMap);
        auto parent = variableNodes[i]->Parent();
        parent->ReplaceChild(variableNodes[i],*newNode);
    }

    doc->SaveFile("../properties/formula.xml");
}

void extractStatementVariableNameFromXML(string propertyFileName, set<Row_Type> &rows ,set<string> &variables){
    TiXmlDocument *doc = new TiXmlDocument(propertyFileName.c_str());
    try {
        if (!doc->LoadFile()) {
            std::cerr << doc->ErrorDesc() << std::endl;
            throw "can't open property file! filename:" + propertyFileName;
        }
    }
    catch (const char *msg){
        cerr << msg <<endl;
        exit(-1);
    }

    std::vector<TiXmlElement*> statementNodes,variableNodes;
    extractStatementVariableNodesFromXML(doc,statementNodes,variableNodes);

    for(int i=0;i<variableNodes.size();i++){
        string name,index,tid,row;
        parseVarStr(variableNodes[i],name,index,tid,row);
        variables.insert(name);
        if(row != "")
            rows.insert(atoi(row.c_str()));
    }

    for(int i=0;i<statementNodes.size();i++){
        rows.insert(atoi(statementNodes[i]->GetText()));
    }
}