//
// Created on 2020/10/10.
//
#include<string>
#include<vector>
#include<iostream>
#include"BA/tinyxml.h"
#include"product.h"
#include"cpn.h"
//#include"base.h"
using namespace std;

extern string executed_P_name;//From cpn.h
extern char LTLFfile[] ;
extern char LTLVfile[] ;
extern char LTLCfile[] ;
extern string extract_tokenValue(TiXmlElement *tokenValueNode);
void GetLTL(ofstream &out,TiXmlElement *p, vector<string> &P,vector<string> &T)
{
    string value = p->Value();
    if (value == "all-paths") // operation A
    {
        GetLTL(out,p->FirstChildElement(), P,T);
    }
    else if (value == "globally") // operation G
    {
        out << "G(";
        GetLTL(out,p->FirstChildElement(), P,T);
        out << ")";
    }
    else if (value == "finally") // operation F
    {
        out << "F(";
        GetLTL(out,p->FirstChildElement(), P,T);
        out << ")";
    }
    else if (value == "next")
    {
        out << "X(";
        GetLTL(out,p->FirstChildElement(), P,T);
        out << ")";
    }
    else if (value == "negation")
    {
        out << "!(";
        GetLTL(out,p->FirstChildElement(), P,T);
        out << ")";
    }
    else if(value == "conjunction" || value == "disjunction")
    {
        TiXmlElement *m, *n;
        m = p->FirstChildElement();
        GetLTL(out,m, P, T);
        m = m->NextSiblingElement();
        while(m) {
            if(value == "conjunction")
                out << "&&";
            else
                out << "||";
            GetLTL(out,m, P, T);

            m = m->NextSiblingElement();
        }
    }
    else if (value == "until")
    {
        TiXmlElement *m, *n;
        m = p->FirstChildElement();
        n = m->NextSiblingElement();
        string mValue = m->Value();
        string nValue = n->Value();
        if (mValue != "before")
        {
            cerr << "Error in XML file! The element until\'s first child is not before!" << endl;
            exit(-1);
        }
        if (nValue != "reach")
        {
            cerr << "Error in XML file! The element until\'s second child is not reach!" << endl;
            exit(-1);
        }
        out << "(";
        GetLTL(out,m->FirstChildElement(), P,T);
        out << ")U(";
        GetLTL(out,n->FirstChildElement(), P,T);
        out << ")";
    }
    else if (value == "less" || value == "equality" || value == "lesseq")
    {
        TiXmlElement *m, *n;
        m = p->FirstChildElement();
        n = m->NextSiblingElement();
        string mValue = m->Value();
        string nValue = n->Value();
        out << "{";
        if (mValue == "token-value")
        {
            out << "token-value(";
//            TiXmlElement *left = m->FirstChildElement();
//            while (left != NULL)
//            {
                out << extract_tokenValue(m);
                string temp = m->FirstChildElement("name")->GetText();
                P.push_back(temp);
//                if (temp == "")
//                    break;
//                left = left->NextSiblingElement();
//            }
            out << ")";
        }
        else if (mValue == "int-constant"
        || mValue == "real-constant"
        || mValue == "string-constant")
        {
            out << m->GetText();
        }
        else {
            cerr << "Error in XML file about the element integer-le!" << endl;
            exit(-1);
        }
        if(value == "lesseq")
            out << "<=";
        else if(value == "less")
            out << "<";
        if(value == "equality")
            out << "==";
        if (nValue == "token-value")
        {
            out << "token-value(";
            TiXmlElement *right = n->FirstChildElement();
//            while (right != NULL)
//            {
                out << extract_tokenValue(n) ;
                string temp = n->FirstChildElement("name")->GetText();
                P.push_back(temp);
//                right = right->NextSiblingElement();
//            }
            out << ")";
        }
        else if (nValue == "int-constant"
                 || nValue == "real-constant"
                 || nValue == "string-constant")
        {
            out << n->GetText();
        }
        else {
            cerr << "Error in XML file about the element integer-le!" << endl;
            exit(-1);
        }
        out << "}";
    }
    else if (value == "is-fireable")
    {
        TiXmlElement *m;
        m = p->FirstChildElement();
        out << "is-fireable{";
        while (m != NULL)
        {
            string Value = m->Value();

            if (Value == "transition")
            {
                string temp = m->GetText();
                T.push_back(temp);
                out << m->GetText() << ",";
            }
            else
            {
                cerr << "Error in XML file! One of the element is-firability\'s child is not transition!" << endl;
                exit(-1);
            }
            m = m->NextSiblingElement();
        }
        out << "}";
    }
}

void extract_criteria(int number,LTLCategory type,CPN *cpn,vector<string> &criteria) {


    TiXmlDocument *doc;
    string file;
    if (type == LTLC)
        file = LTLCfile;
    else if (type == LTLF)
        file = LTLFfile;
    else if (type == LTLV)
        file = LTLVfile;
    doc = new TiXmlDocument(file.c_str());
    if (!doc->LoadFile()) {
        cerr << doc->ErrorDesc() << endl;
    }
    file = "result.txt";//这里只是复用file变量
    ofstream out(file.c_str(), ios::out | ios::app);

    TiXmlElement *root = doc->RootElement();
    if (root == NULL) {
        cerr << "Failed to load file: no root element!" << endl;
        doc->Clear();
    }

    TiXmlElement *p = root->FirstChildElement();
    for (int i = 1; i < number; ++i) {
        p = p->NextSiblingElement();
    }

    TiXmlElement *id = p->FirstChildElement("id");

    TiXmlElement *formula = p->FirstChildElement("formula");
    vector<string> P, T;


    GetLTL(out,formula->FirstChildElement(), P, T);

    out<<endl;
    for (unsigned int i = 0; i < P.size(); i++) {
        auto iter = cpn->mapPlace.find(P[i]);
        CPlace *pp = &cpn->getplacearr()[iter->second];
        if (!pp->getiscontrolP()) {
            CTransition *tt;
            for (unsigned int j = 0; j < pp->get_producer().size(); j++) {
                tt = &cpn->gettransarr()[pp->get_producer()[j].idx];
                for (unsigned int k = 0; k < tt->get_producer().size(); k++) {
                    if (tt->get_producer()[k].arcType == control
                        && cpn->getplacearr()[tt->get_producer()[k].idx].getExp() != executed_P_name)
                        if (!exist_in(criteria, cpn->getplacearr()[tt->get_producer()[k].idx].getid()))
                            criteria.push_back(cpn->getplacearr()[tt->get_producer()[k].idx].getid());
                }
            }
            for (unsigned int j = 0; j < pp->get_consumer().size(); j++) {
                tt = &cpn->gettransarr()[pp->get_consumer()[j].idx];
                for (unsigned int k = 0; k < tt->get_producer().size(); k++) {
                    if (tt->get_producer()[k].arcType == control
                        && cpn->getplacearr()[tt->get_producer()[k].idx].getExp() != executed_P_name)
                        if (!exist_in(criteria, cpn->getplacearr()[tt->get_producer()[k].idx].getid()))
                            criteria.push_back(cpn->getplacearr()[tt->get_producer()[k].idx].getid());
                }
            }
        } else {
            cout << "nonsense token-value for control place" << endl;
            exit(-1);
        }
    }
    for (unsigned int i = 0; i < T.size(); i++) {
        auto iter = cpn->mapTransition.find(T[i]);
        CTransition *tt = &cpn->gettransarr()[iter->second];
        for (unsigned int j = 0; j < tt->get_producer().size(); j++) {
            if (tt->get_producer()[j].arcType == control
                && cpn->getplacearr()[tt->get_producer()[j].idx].getExp() != executed_P_name)
                if (!exist_in(criteria, cpn->getplacearr()[tt->get_producer()[j].idx].getid()))
                    criteria.push_back(cpn->getplacearr()[tt->get_producer()[j].idx].getid());
        }
    }
    out<<endl;
    out.close();
}
