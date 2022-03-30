//
// Created on 2020/10/10.
//
#include "GraphMaker.h"
#include "Tree.h"
#include "CFG.h"
#include "pdnet_parseoption.h"

#include <unistd.h>
//#include "cpn.h"
#include "v_table.h"
#include "cpn_rg.h"
#include "product.h"
#include "AST_compare.h"
#include "changeConstruct.h"

#include<map>
#include <dirent.h>
#include <algorithm>
#include <signal.h>
#include <stdlib.h>
#include <fstream>
#include <sys/stat.h>

std::map<std::string,optcount_t> opt_table = {
        {"-help",0},
//        {"-file",1},
        {"-showtree",0},
        {"-showcpn",0},
        {"-time",0},
        {"-fnum",1},
        {"-heu",0},
        {"-PDNetSlice",0},
        {"-ProgramSlice",0},
        {"-TraditionalSlice",0},
        {"-NoDependence",0},
        {"-property",1},
        {"-CIA",0}
//        {"-ltlv",0},
//        {"-noout",0},
//        {"-directbuild",0},
//        {"-slice",0},
//        {"-compare",0}
};

extern void Bubble_sort(vector<string> &change_P);

extern void extract_criteria(int number,LTLCategory type,CPN *cpn,vector<string> &criteria);
extern void two_phrase_slicing(CPN *cpn, vector<string> place, vector<string> &final_P, vector<string>&final_T);
extern void post_process(CPN *cpn,CPN *cpn_slice,vector<string> transitions);
extern void changeProgramXML2PDNetXML(std::string filename, CPN *cpn, const RowMap& rowMap);
extern void preWardSlicing(CPN *cpn, vector<string> place, vector<string> &final_P, vector<string> &final_T);
extern void initCriteria(CPN *cpn, const vector<string> crit_P, vector<string> &Ps,vector<string> &Ts);
extern void traditionalAPNSlice(CPN *cpn, vector<string> &Ps,vector<string> &Ts);

extern RowMap preProcessGetRowMap(string filename);
extern void extractStatementVariableNameFromXML(string propertyFileName, set<Row_Type> &rows ,set<string> &variables);

char LTLFfile[50],LTLVfile[50],LTLCfile[50];

string origin_dirname = "./";
string newfile_dirname = "newfile/";

CFG cfg;
bool PRINT_GRAPH = false;

extern void extract_criteria(int number,LTLCategory type,CPN *cpn,vector<string> &criteria);

// run cmd and output
int RunCmd(const std::string& cmd, std::string& out) {
    FILE* fp = popen(cmd.c_str(), "r");
    if (!fp) {
        return errno == 0 ? -1 : errno;
    }

    char buffer[4096] = { 0 };
    while(!feof(fp)){
        size_t len = fread(buffer, 1, 4096, fp);
        if(len > 0) out.append(buffer, len);
    }

    return pclose(fp);
}
// get three time from ps_3 Tdfd Tcfd Ttra
int runPS_3(const string &cmd_, std::vector<long int> &res_) {
    string out;
    RunCmd(cmd_, out);
    istringstream iss(out);
    for (int i = 0; i < 3; i++) {
        long int num;
        iss >> num;
        res_.emplace_back(num);
    }
    return 0;
}

bool fileExists(const std::string& filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}
void CHECKLTL(CPN *cpnet, LTLCategory type,int num,int &rgnum,string &res) {
    RG *graph = new RG;
    graph->init(cpnet);

    string propertyid;
//    char ff[]=LTLFfile;
//    char cc[]=LTLCfile;
//    char vv[]=LTLVfile;
    Syntax_Tree syntaxTree;
    if(type == LTLC)
        syntaxTree.ParseXML(LTLCfile,propertyid,num);
    else if(type == LTLF)
        syntaxTree.ParseXML(LTLFfile,propertyid,num);
    else if(type == LTLV)
        syntaxTree.ParseXML(LTLVfile,propertyid,num);
//    cout<<"formula tree:"<<endl;
//    syntaxTree.PrintTree();
//    cout<<"-----------------------------------"<<endl;
    syntaxTree.Push_Negation(syntaxTree.root);
//        cout<<"after negation:"<<endl;
//        syntaxTree.PrintTree();
//        cout<<"-----------------------------------"<<endl;
    syntaxTree.SimplifyLTL();
//        cout<<"after simplification:"<<endl;
//        syntaxTree.PrintTree();
//        cout<<"-----------------------------------"<<endl;
    syntaxTree.Universe(syntaxTree.root);
//        cout<<"after universe"<<endl;
//        syntaxTree.PrintTree();
//        cout<<"-----------------------------------"<<endl;

    syntaxTree.Get_DNF(syntaxTree.root);
    syntaxTree.Build_VWAA();
    syntaxTree.VWAA_Simplify();

    General GBA;
    GBA.Build_GBA(syntaxTree);
    GBA.Simplify();
    GBA.self_check();

    Buchi BA;
    BA.Build_BA(GBA);
    BA.Simplify();
    BA.self_check();
    BA.Backward_chaining();
//    BA.PrintBuchi("BA.dot");

    StateBuchi SBA;
    SBA.Build_SBA(BA);
    SBA.Simplify();
    SBA.Tarjan();
    SBA.Complete1();
    SBA.Add_heuristic();
    SBA.Complete2();
    SBA.self_check();
//    SBA.PrintStateBuchi();
/*******************************************************/
    //cout << "begin:ON-THE-FLY" << endl;
//    clock_t start=clock();
    CPN_Product_Automata *product;
    product = new CPN_Product_Automata(cpnet, &SBA, graph);
    product->GetProduct();
    cout<<"\nformula:\n"<<syntaxTree.root->nleft->formula<<endl;
    cout<<"\n";
    product->printresult(propertyid);
//    clock_t end = clock();
//
//    cout<<"product time:"<<(start-end)/1000.0<<endl;
    cout<<"Synthesised graph node num: "<<graph->node_num<<endl;
    rgnum = graph->node_num;
    res = product->GetResult();

    delete product;
    delete graph;

}

int fileNameFilter(const struct dirent *cur) {
    std::string str(cur->d_name);
    if (str.find(".c") != std::string::npos) {
        return 1;
    }
    return 0;
}

void GetFileNames(string path,vector<string>& ret)
{
    struct dirent **namelist;
    int n = scandir(path.c_str(), &namelist, fileNameFilter, alphasort);
    if (n < 0) {
        cout<<"There is no file!"<<endl;
        exit(-1);
    }
    for (int i = 0; i < n; ++i) {
        std::string filePath(namelist[i]->d_name);
        ret.push_back(filePath);
        free(namelist[i]);
    };
    free(namelist);
}

void test_rg(string filename){
    init_pthread_type();
    string check_file = filename;//"../test/lazy01.c";
    //1.preprocess and build program's AST
    gtree *tree = create_tree(check_file);
    cut_tree(tree);
    if (0) {
        intofile_tree(tree);
        makeGraph("tree.dot", "tree.png");
    }
    CPN *cpnet = new CPN;
    //2.construct program's CPN
//    cpnet->init();
    cpnet->initDecl();
    cpnet->getDecl(tree);
    cpnet->init_alloc_func();
    cpnet->Traverse_ST0(tree);
    cpnet->Traverse_ST1(tree);
    cpnet->Traverse_ST2(tree);
    cpnet->setmaintoken();

    // cpnet->delete_compound(tree);
    cpnet->set_producer_consumer();

    cpnet->print_CPN("output");
    RG rg;
    rg.init(cpnet);
    rg.GENERATE(cpnet);
    rg.print_RG("rg1.txt",cpnet);

//    free_tree(tree);
//    delete cpnet;
//    sorttable.clear();
//    uninit_pthread_type();
}

bool cmdlinet::parse(int argc, char **argv) {
    for(int i=1;i<argc;i++){
        if(argv[i][0] == '-'){
            option_t option;
            option.name = argv[i];
            optcount_t exist = opt_exist(option.name);
            if(exist != 0)
                continue;
            int para_num = get_paranum(option.name);
            if(para_num != 0){
                for(unsigned int j=0;j<para_num;j++){
                    option.value.push_back(argv[++i]);
                }
            }
            options.push_back(option);
        }
        else
        {
            if(i!=argc-1)
            {
                std::cerr<<"All command must start with '-', you may want to use -"
                         <<argv[i]<<" instead"<<std::endl;
            }
            else
                filename = argv[i];
        }
    }
    return true;
}

paracount_t cmdlinet::get_paranum(std::string optstring) {
    auto iter = opt_table.find(optstring);
    if(iter == opt_table.end()) {
        std::cerr<<"There is no command named:"<<optstring<<"! Please check the command."<<std::endl;
        exit(-1);
    }
    return iter->second;
}

optcount_t cmdlinet::opt_exist(std::string optstring) {
    for(unsigned int i=0;i<options.size();i++){
        if(optstring == options[i].name)
            return i + 1;
    }
    return 0;
}

RowMap programSlice(string filename) {
    
    std::string input_name(filename);
    gtree * hhh;
    hhh = create_tree_withoutTraverse(input_name.c_str());
    TreeNode* root = gtree2TreeNode(hhh);

    // proprocess
    preProcessor_findNext(root);
    preProcessor_Tree2CFGnodes(root, cfg);
    preProcessor_LinkCFGNodes2Next(cfg);
    preProcessor_pthread(cfg);


    // construct dependencies
    buildDataDependence(cfg);
    buildControlDependence(cfg);


    // slicing and output
//    sliceByCFGNode(cfg, cfg.nodes[42]); // 输入形式有待考究
    slice(cfg);
    auto rowMap = output_c(input_name.c_str(), cfg);

    if (PRINT_GRAPH){
       GraphMaker::GetInstance()->printTree(root);
       GraphMaker::GetInstance()->printCFG(cfg);
       GraphMaker::GetInstance()->printPDT(cfg);
    }
    deleteTree(root);
    // printf("ps_3 end, proccess exit...\n");

    return rowMap;
}

void rgTest(CPN *cpnet, string outputFile){
    //only test rg Do not free memory
    RG *rg = new RG;
    rg->init(cpnet);
    rg->GENERATE(cpnet);
    rg->print_RG(outputFile,cpnet);
}

void csvFormatAppend(string &target, string appPart){
    if(target == "")
        target = appPart;
    else
        target = target + "," + appPart;
}

RowMap readRowMapFromTXT(){
    ifstream psRowMap("programSliceRowMap.txt",ios::in);
    if(!psRowMap.is_open()){
        cout << "There is a problem with the output form of program slice line number!"<<endl;
        exit(-2);//-2 represent the Exception of program slice
    }
    RowMap programSliceRowMap;
    string tmp;
    while(psRowMap >> tmp){
        vector<string> tmpvec = split(tmp,":");
        if(tmpvec.size()!=2){
            cout << "There is a problem with the output form of program slice line number!"<<endl;
            exit(-2);
        }
        programSliceRowMap.emplace(atoi(tmpvec[0].c_str()),atoi(tmpvec[1].c_str()));
    }
    return programSliceRowMap;
}

/**
 *
 * @param check_file filename that to be checked
 * @param ltltype formula type
 * @param num formula number, indecate the order in xml file, start from 1
 * @param gen_picture if true, then generate cpn image through graphviz
 * @param showtree if true, then generate syntax tree image through graphviz
 * @param hasHeu if true, use heuristic information
 * @param hasSlice if true, do slice
 * @return string, csvOutputStr
 */
string model_checking(string check_file,string property_file, LTLCategory ltltype,int num,bool gen_picture,bool showtree,bool hasHeu,bool hasPDNetSlice, bool hasTraditionalSlice,bool NoDependence, RowMap rowMap){

    string csvOutputStr = "";
    csvFormatAppend(csvOutputStr,check_file);

        string filename;
        int pre_P_num, pre_T_num, pre_arc_num, pre_rgnode_num, slice_P_num, slice_T_num, slice_rgnode_num;
        clock_t pre_time, slice_time, pre_model, pre_check, slice, slice_check;
        string pre_res, slice_res;
        double base_clock = 1000.0;

        switch (ltltype) {
            case LTLF:
//            filename = check_file.substr(0,check_file.length()-2) + "-F.xml";
//            strcpy(LTLFfile,filename.c_str());

                strcpy(LTLFfile, string("../properties/formula.xml").c_str());
                break;
            case LTLV:
//            throw "we don't support LTLV for now!";
//            filename = check_file.substr(0,check_file.length()-2) + "-V.xml";
//            strcpy(LTLVfile,filename.c_str());
                strcpy(LTLVfile, string("../properties/formula.xml").c_str());
                break;
            case LTLC:
                //We can support LTLC ,but it is not meaningful;
//            filename = check_file.substr(0,check_file.length()-2) + "-C.xml";
//            strcpy(LTLCfile,filename.c_str());

                strcpy(LTLFfile, string("../properties/formula.xml").c_str());
                break;
        }
        ofstream out;
        out.open("log.txt", ios::out | ios::app); //mark the log
        if (out.fail()) {
            cout << "open result failed!" << endl;
            exit(1);
        }
        out << endl;
        cout << endl;
        out << "current file: " << check_file << endl;
        cout << "current file: " << check_file << endl;
        out << "formula:" << num << endl;
        cout << "formula:" << num << endl;
        out << endl;
        cout << endl;

        clock_t direct_build_start;
        direct_build_start = clock();

        // init_pthread_type();

        //1.preprocess and build program's AST
        gtree *tree = create_tree(check_file);
//    toFileRowTree(tree);
        cut_tree(tree);
        if (showtree) {
            intofile_tree(tree);
            makeGraph("tree.dot", "tree.png");
        }
        CPN *cpnet = new CPN;

        //2.construct program's CPN
//    cpnet->init_alloc_func();
//    cpnet->initDecl();
//    cpnet->getDecl(tree);
//    cpnet->create_PDNet(tree);
//    cpnet->setmaintoken();
//    cpnet->delete_compound(tree);
//    cpnet->set_producer_consumer();

        cpnet->initDecl();
        cpnet->getDecl(tree);
        cpnet->init_alloc_func();
        cpnet->Traverse_ST0(tree);
        if(!NoDependence){
            cpnet->Traverse_ST1(tree);
            cpnet->Traverse_ST2(tree);
            cpnet->delete_compound(tree);
        }
        else{
            cpnet->Traverse_ST1_withoutdependence(tree);
            cpnet->Traverse_ST2_withoutdependence(tree);
            cpnet->delete_compound_withoutdependence(tree);
        }
        cpnet->setmaintoken();
        cpnet->set_producer_consumer();


        string filename_prefix;
        if (gen_picture) {
            filename_prefix = "directbuild";
            cpnet->print_CPN(filename_prefix);
            readGraph(filename_prefix + ".txt", filename_prefix + ".dot");
            makeGraph(filename_prefix + ".dot", filename_prefix + ".png");
        }
    clock_t direct_build_end = clock();
//    RG rg;
//    rg.init(cpnet);
//    rg.GENERATE(cpnet);
//    rg.print_RG("rg1.txt",cpnet);

    pre_P_num = cpnet->get_placecount();
    pre_T_num = cpnet->get_transcount();
//    pre_arc_num = cpnet->get_arccount();
        changeProgramXML2PDNetXML(property_file, cpnet, rowMap);

        vector<string> final_P, final_T, criteria;

        //4.extract criteria from LTL file and generate “.txt” to describe formulas
        extract_criteria(num, ltltype, cpnet, criteria);

        cpnet->InitDistance(criteria);//启发式信息初始化distance

        auto place = cpnet->getplacearr();

        clock_t slice_begin = clock();

        //5.slicing CPN
        for(int i=0;i<cpnet->deadloops.size();i++)
            if(!exist_in(criteria,cpnet->deadloops[i]))
                criteria.push_back(cpnet->deadloops[i]);
//        criteria.insert(criteria.end(), cpnet->deadloops.begin(), cpnet->deadloops.end());
//        criteria.insert(criteria.end(), cpnet->otherLocks.begin(), cpnet->otherLocks.end());
        two_phrase_slicing(cpnet, criteria, final_P, final_T);

            typedef pair<string, short> struct_pri;
            vector<struct_pri> prioritys;
            for (int i = 0; i < cpnet->get_placecount(); i++)
                if (place[i].getDistance() != 65535)
                    prioritys.emplace_back(place[i].getid(), place[i].getDistance());
    //            cout<< place[i].getid()<<":"<<place[i].getDistance()<<endl;
            sort(prioritys.begin(), prioritys.end(),
                [](const struct_pri &lhs, const struct_pri &rhs) { return lhs.second < rhs.second; });
            cpnet->setPriTrans(prioritys);

            if (hasHeu)
                cpnet->generate_transPriNum();
        clock_t slice_end = clock();
        final_P.push_back("P0"); //alloc_store_P

        Bubble_sort(final_T);
        Bubble_sort(final_P);

        if(!hasPDNetSlice && !hasTraditionalSlice){
            cout << "original PDNet:\n";
        //    out<<"  placenum: "<<cpnet->placecount<<endl;
            cout << "  placenum: " << cpnet->get_placecount() << endl;
            cout << "  varplacenum: " << cpnet->get_varplacecount() << endl;
            cout << "  controlplacenum: " << cpnet->get_ctlplacecount() << endl;
            cout << "  exeplacenum: " << cpnet->get_exeplacecount() << endl;
        //    out<<"  transnum: "<<cpnet->transitioncount<<endl;
            cout << "  transnum: " << cpnet->get_transcount() << endl;
        //    cout<<"  arcnum:"<<cpnet->get_arccount()<<endl;

            csvFormatAppend(csvOutputStr, to_string(cpnet->get_placecount()));
            csvFormatAppend(csvOutputStr, to_string(cpnet->get_varplacecount()));
            csvFormatAppend(csvOutputStr, to_string(cpnet->get_ctlplacecount()));
            csvFormatAppend(csvOutputStr, to_string(cpnet->get_exeplacecount()));
            csvFormatAppend(csvOutputStr, to_string(cpnet->get_transcount()));
    
            //3.verify CPN's properties
            CHECKLTL(cpnet, ltltype, num, pre_rgnode_num, pre_res);
            clock_t model_check_end = clock();

            pre_model = (direct_build_end - direct_build_start);
            pre_check = (model_check_end - direct_build_end);
            pre_time = pre_model + pre_check;

            csvFormatAppend(csvOutputStr, to_string(pre_rgnode_num));
            csvFormatAppend(csvOutputStr, to_string(pre_model / base_clock));
            csvFormatAppend(csvOutputStr, "NULL");
            csvFormatAppend(csvOutputStr, to_string(pre_check / base_clock));
            csvFormatAppend(csvOutputStr, pre_res);

            out << "direct build time: " << setiosflags(ios::fixed) << setprecision(3) << pre_model / base_clock << endl;
            out << "model_check: " << setiosflags(ios::fixed) << setprecision(3) << pre_check / base_clock << endl;
            out << "total time: " << setiosflags(ios::fixed) << setprecision(3) << pre_time / base_clock << endl;
            out << endl;

            if(NoDependence){
                cout << "time of NoDependence model:" << pre_model / base_clock << endl;
                cout << "time of model checking:" << pre_check / base_clock << endl;
            }
            else {
                cout << "time of PDNet model:" << pre_model / base_clock << endl;
                cout << "time of model checking:" << pre_check / base_clock << endl;
            }
        }
        else if(hasPDNetSlice){
            clock_t slice_begin1 = clock();
            CPN *cpnet_slice = new CPN;

            cpnet_slice->copy_childNet(cpnet, final_P, final_T);

            if (hasHeu)
                cpnet_slice->generate_transPriNum();

            //6.post_process
            post_process(cpnet, cpnet_slice, final_T);

            clock_t slice_end1 = clock();

            if (gen_picture) {
                filename_prefix = "slice";
                cpnet_slice->print_CPN(filename_prefix);
                readGraph(filename_prefix + ".txt", filename_prefix + ".dot");
                makeGraph(filename_prefix + ".dot", filename_prefix + ".png");
            }
            out << endl;
            cout << endl;
            cout << "PDNet slice:\n";
//    out<<"placenum: "<<cpnet_slice->placecount<<endl;
            cout << "  placenum: " << cpnet_slice->get_placecount() << endl;
            cout << "  varplacenum: " << cpnet_slice->get_varplacecount() << endl;
            cout << "  controlplacenum: " << cpnet_slice->get_ctlplacecount() << endl;
            cout << "  exeplacenum: " << cpnet_slice->get_exeplacecount() << endl;
//    out<<"transnum: "<<cpnet_slice->transitioncount<<endl;
            cout << "  transnum: " << cpnet_slice->get_transcount() << endl;

            slice_P_num = cpnet_slice->get_placecount();
            slice_T_num = cpnet_slice->get_transcount();

            csvFormatAppend(csvOutputStr, to_string(cpnet_slice->get_placecount()));
            csvFormatAppend(csvOutputStr, to_string(cpnet_slice->get_varplacecount()));
            csvFormatAppend(csvOutputStr, to_string(cpnet_slice->get_ctlplacecount()));
            csvFormatAppend(csvOutputStr, to_string(cpnet_slice->get_exeplacecount()));
            csvFormatAppend(csvOutputStr, to_string(cpnet_slice->get_transcount()));

            //7.verify sliced property
            clock_t slice_check_begin = clock();
            CHECKLTL(cpnet_slice, ltltype, num, slice_rgnode_num, slice_res);
            clock_t slice_check_end = clock();

            slice = (slice_end - slice_begin) + (slice_end1 - slice_begin1);
            pre_model = (direct_build_end - direct_build_start);
            slice_check = (slice_check_end - slice_check_begin);
            slice_time = slice + slice_check + pre_model;

            csvFormatAppend(csvOutputStr, to_string(slice_rgnode_num));
            csvFormatAppend(csvOutputStr, to_string(pre_model / base_clock));
            csvFormatAppend(csvOutputStr, to_string(slice / base_clock));
            csvFormatAppend(csvOutputStr, to_string(slice_check / base_clock));
            csvFormatAppend(csvOutputStr, slice_res);

            out << "slice time: " << setiosflags(ios::fixed) << setprecision(3) << slice / base_clock << endl;
            out << "model_check: " << setiosflags(ios::fixed) << setprecision(3) << slice_check / base_clock << endl;
            out << "total time: " << setiosflags(ios::fixed) << setprecision(3) << slice_time / base_clock << endl;
            out << endl;
            if(NoDependence){
                cout << "time of NoDependence model:" << pre_model / base_clock << endl;
                cout << "time of Traditional slicing:" << slice / base_clock << endl;
                cout << "time of model checking:" << slice_check / base_clock << endl;
            }
            else {
                cout << "time of PDNet model:" << pre_model / base_clock << endl;
                cout << "time of PDNet slicing:" << slice / base_clock << endl;
                cout << "time of model checking:" << slice_check / base_clock << endl;
            }
//        out << setiosflags(ios::fixed) << setprecision(3) << "& \\emph{" << pre_res << "}\n& " << pre_P_num << "\n& "
//            << pre_T_num << "\n& " << pre_rgnode_num << "\n& " << pre_model / base_clock << "\n& "
//            << pre_check / base_clock << "\n& " << pre_time / base_clock << "\n& \\emph{" << slice_res << "}\n& "
//            << slice_P_num << "\n& " << slice_T_num << "\n& " << slice_rgnode_num << "\n& " << slice / base_clock
//            << "\n& " << slice_check / base_clock << "\n& " << slice_time / base_clock << "\\\\ \\cline{2-12}";
            out << endl;
            out << endl;
            out << "criteria P : ";
            for (unsigned int i = 0; i < criteria.size(); i++)
                out << criteria[i] << ",";
            out << endl;
            out << endl;
            out << "deleted P : ";
            for (unsigned int i = 0; i < cpnet->get_placecount(); i++)
                if (!exist_in(final_P, cpnet->getplacearr()[i].getid())) {
                    out << "$P_{";
                    out << cpnet->getplacearr()[i].getid().substr(1);
                    out << "}$,";
                }
            out << endl;
            out << endl;
            out << "deleted T : ";
            for (unsigned int i = 0; i < cpnet->get_transcount(); i++)
                if (!exist_in(final_T, cpnet->gettransarr()[i].getid())) {
                    out << "$T_{";
                    out << cpnet->gettransarr()[i].getid().substr(1);
                    out << "}$,";
                }
        }
        else if(hasTraditionalSlice){
            vector<string> Ps,Ts;
            Ps.clear();
            Ts.clear();
            clock_t slice_begin,slice_end;
            slice_begin = clock();
            initCriteria(cpnet,criteria,Ps,Ts);
            traditionalAPNSlice(cpnet,Ps,Ts);
            slice_end = clock();
            Bubble_sort(Ps);
            Bubble_sort(Ts);
            CPN *cpnet_slice = new CPN;
            cpnet_slice->copy_childNet(cpnet, Ps, Ts);
            cout << "\n\nTraditional Slice:\n";
        //    out<<"  placenum: "<<cpnet->placecount<<endl;
            cout << "  placenum: " << cpnet_slice->get_placecount() << endl;
            cout << "  varplacenum: " << cpnet_slice->get_varplacecount() << endl;
            cout << "  controlplacenum: " << cpnet_slice->get_ctlplacecount() << endl;
            cout << "  exeplacenum: " << cpnet_slice->get_exeplacecount() << endl;
        //    out<<"  transnum: "<<cpnet->transitioncount<<endl;
            cout << "  transnum: " << cpnet_slice->get_transcount() << endl;

            csvFormatAppend(csvOutputStr, to_string(cpnet_slice->get_placecount()));
            csvFormatAppend(csvOutputStr, to_string(cpnet_slice->get_varplacecount()));
            csvFormatAppend(csvOutputStr, to_string(cpnet_slice->get_ctlplacecount()));
            csvFormatAppend(csvOutputStr, to_string(cpnet_slice->get_exeplacecount()));
            csvFormatAppend(csvOutputStr, to_string(cpnet_slice->get_transcount()));

            if (gen_picture) {
                filename_prefix = "TraditionalSlice";
                cpnet_slice->print_CPN(filename_prefix);
                readGraph(filename_prefix + ".txt", filename_prefix + ".dot");
                makeGraph(filename_prefix + ".dot", filename_prefix + ".png");
            }
            // rgTest(cpnet_slice,"rgSlice.txt");
            clock_t slice_check_begin,slice_check_end;
            slice_check_begin = clock();
            CHECKLTL(cpnet_slice, ltltype, num, slice_rgnode_num, slice_res);
            slice_check_end = clock();

            pre_model = (direct_build_end - direct_build_start);
            slice = (slice_end - slice_begin) + (slice_end - slice_begin);
            slice_check = (slice_check_end - slice_check_begin);
            slice_time = slice + slice_check + pre_model;

            csvFormatAppend(csvOutputStr, to_string(slice_rgnode_num));
            csvFormatAppend(csvOutputStr, to_string(pre_model / base_clock));
            csvFormatAppend(csvOutputStr, to_string(slice / base_clock));
            csvFormatAppend(csvOutputStr, to_string(slice_check / base_clock));
            csvFormatAppend(csvOutputStr, slice_res);

            if(NoDependence){
                cout << "time of NoDependence model:" << pre_model / base_clock << endl;
                cout << "time of Traditional CPN Slicing:" << slice / base_clock << endl;
                cout << "time of model checking:" << slice_check / base_clock << endl;
            }
            else {
                cout << "time of PDNet model:" << pre_model / base_clock << endl;
                cout << "time of PDNet slicing:" << slice / base_clock << endl;
                cout << "time of model checking:" << slice_check / base_clock << endl;
            }
        }
        out << endl;
        out.close();

        cout << endl;
    return csvOutputStr;
}

void changeImpactAnalysis(string origin_filename, string new_filename,string property_file, LTLCategory ltltype,int num,bool gen_picture,bool showtree,bool hasHeu,bool hasSlice, RowMap rowMap){

//    strcpy(LTLFfile, string("../properties/formula.xml").c_str());

    clock_t begin,end;
    begin = clock();
    int pre_P_num, pre_T_num, pre_arc_num, pre_rgnode_num, slice_P_num, slice_T_num, slice_rgnode_num;
    clock_t pre_time, slice_time, pre_model, pre_check, slice, slice_check;
    string pre_res, slice_res;

    // 1. Construct original PDNet
    gtree *tree1 = create_tree(origin_filename);
    cut_tree(tree1);
    if (showtree) {
        intofile_tree(tree1);
        makeGraph("originTree.dot", "tree.png");
    }
    CPN *originCpnet = new CPN;

    //2.Construct new PDNet

    originCpnet->initDecl();
    originCpnet->getDecl(tree1);
    originCpnet->init_alloc_func();
    originCpnet->Traverse_ST0(tree1);
    originCpnet->Traverse_ST1(tree1);
    originCpnet->Traverse_ST2(tree1);
    originCpnet->setmaintoken();

    originCpnet->delete_compound(tree1);
    originCpnet->set_producer_consumer();


    string filename_prefix;
    if (gen_picture) {
        filename_prefix = "originModel";
        originCpnet->print_CPN(filename_prefix);
        readGraph(filename_prefix + ".txt", filename_prefix + ".dot");
        makeGraph(filename_prefix + ".dot", filename_prefix + ".png");
    }

    // 2. Construct the new tree
    gtree *tree2 = create_tree(new_filename);
    cut_tree(tree2);
    if (showtree) {
        intofile_tree(tree2);
        makeGraph("newtree.dot", "newtree.png");
    }
    //Change Checking Starting
    vector<Mapping> M;
    top_down(tree1, tree2, M);
    auto M_statement = get_MatchStatement(M);//找到语法树中所有完全match的节点
    vector<gtree *> vec_t1,vec_t2;
//    extract_changeNodes(M_statement,M,vec_t1,vec_t2);
    vector<AST_change> changes = extract_change(tree1, tree2, M, M_statement);//从语法树中提取变化的节点

    vector<string> criteria_change,final_P,final_T;
    criteria_change = changeConstruct(originCpnet,changes);//进行模型修改
    preWardSlicing(originCpnet,criteria_change,final_P,final_T);//前向切片找变化影响的部分
    vector<string> criteria_prop,final_P1,final_T1;
    changeProgramXML2PDNetXML(property_file, originCpnet, rowMap);
    //4.extract criteria from LTL file and generate “.txt” to describe formulas
    extract_criteria(num, ltltype, originCpnet, criteria_prop);
    two_phrase_slicing(originCpnet,criteria_prop,final_P1,final_T1);//后向切片找影响性质的部分

    auto res = getCommon(final_P,final_P1);//二者做交集判断是否可以复用结果
    if(res.size() == 0){
        cout << "Reuse the old results"<<endl;
        end = clock();
        cout << "time: " << (end-begin)/1000 << "ms"<<endl;
        exit(10);
    }

    cout << "CIA failed！Model checking again..." <<endl;

    //Model checking again...
    originCpnet->InitDistance(criteria_prop); //Init distance Heuristic

    auto place = originCpnet->getplacearr();

    clock_t slice_begin = clock();
    //5.slicing CPN
    criteria_prop.insert(criteria_prop.end(), originCpnet->deadloops.begin(), originCpnet->deadloops.end());
    criteria_prop.insert(criteria_prop.end(), originCpnet->otherLocks.begin(), originCpnet->otherLocks.end());
    two_phrase_slicing(originCpnet, criteria_prop, final_P, final_T);

    typedef pair<string, short> struct_pri;
    vector<struct_pri> prioritys;
    for (int i = 0; i < originCpnet->get_placecount(); i++)
        if (place[i].getDistance() != 65535)
            prioritys.emplace_back(place[i].getid(), place[i].getDistance());
//            cout<< place[i].getid()<<":"<<place[i].getDistance()<<endl;
    sort(prioritys.begin(), prioritys.end(),
         [](const struct_pri &lhs, const struct_pri &rhs) { return lhs.second < rhs.second; });
    originCpnet->setPriTrans(prioritys);

    if (hasHeu) //Use Heuristic Information
        originCpnet->generate_transPriNum();

    if (gen_picture) {
        filename_prefix = "newModel";
        originCpnet->print_CPN(filename_prefix);
        readGraph(filename_prefix + ".txt", filename_prefix + ".dot");
        makeGraph(filename_prefix + ".dot", filename_prefix + ".png");
    }

    if(!hasSlice) {
        //Directly Model checking
//    RG rg;
//    rg.init(originCpnet);
//    rg.GENERATE(originCpnet);
        cout << "original PDNet:\n";
//    out<<"  placenum: "<<cpnet->placecount<<endl;
        cout << "  placenum: " << originCpnet->get_placecount() << endl;
        cout << "  varplacenum: " << originCpnet->get_varplacecount() << endl;
        cout << "  controlplacenum: " << originCpnet->get_ctlplacecount() << endl;
        cout << "  exeplacenum: " << originCpnet->get_exeplacecount() << endl;
//    out<<"  transnum: "<<cpnet->transitioncount<<endl;
        cout << "  transnum: " << originCpnet->get_transcount() << endl;
//    cout<<"  arcnum:"<<cpnet->get_arccount()<<endl;
        CHECKLTL(originCpnet, ltltype, num, pre_rgnode_num, pre_res);
    }
    else{
        //Use PDNet Slice
        CPN *cpnet_slice = new CPN;

        cpnet_slice->copy_childNet(originCpnet, final_P, final_T);

        if (hasHeu)
            cpnet_slice->generate_transPriNum();

        //6.post_process
        post_process(originCpnet, cpnet_slice, final_T);

        clock_t slice_end1 = clock();

        if (gen_picture) {
            filename_prefix = "slice";
            cpnet_slice->print_CPN(filename_prefix);
            readGraph(filename_prefix + ".txt", filename_prefix + ".dot");
            makeGraph(filename_prefix + ".dot", filename_prefix + ".png");
        }
        cout << endl;
        cout << "PDNet slice:\n";
//    out<<"placenum: "<<cpnet_slice->placecount<<endl;
        cout << "  placenum: " << cpnet_slice->get_placecount() << endl;
        cout << "  varplacenum: " << cpnet_slice->get_varplacecount() << endl;
        cout << "  controlplacenum: " << cpnet_slice->get_ctlplacecount() << endl;
        cout << "  exeplacenum: " << cpnet_slice->get_exeplacecount() << endl;
//    out<<"transnum: "<<cpnet_slice->transitioncount<<endl;
        cout << "  transnum: " << cpnet_slice->get_transcount() << endl;

        slice_P_num = cpnet_slice->get_placecount();
        slice_T_num = cpnet_slice->get_transcount();


        //7.verify sliced CPN's property
        CHECKLTL(cpnet_slice, ltltype, num, slice_rgnode_num, slice_res);

    }
    end = clock();
    cout << "time: "<< (end - begin)/1000 << "ms" <<endl;

}


//void gccpreprocess(string filename){
//    string cmd;
//    string newfilename = filename;
//    cmd = "gcc -B .. -E " + filename + "| grep -v \"#\" | grep -v \"extern \"> " + newfilename.replace(newfilename.find(".c"),2,".i");
//    system(cmd.c_str());
//}

void cmdlinet::doit() {
    clock_t start,end;
    try {

        //初始化pthread类型：pthread_t等
        init_pthread_type();

        if (opt_exist("-help")) {
            help();
            return;
        }
        LTLCategory ltltype = LTLV;
        bool showtree = false, showcpn = false, showtime = false, 
        hasHeu = false, hasPDNetSlice = false,hasProgramSlice = false,
        hasTraditionalSlice = false, hasNoDependence = false,
        hasCIA = false;
        unsigned short fnum = 1;
        if (opt_exist("-showtree"))
            showtree = true;
        if (opt_exist("-showcpn"))
            showcpn = true;
        if (opt_exist("-PDNetSlice")) // the parameter of PDNet Slice
            hasPDNetSlice = true;
        if (opt_exist("-ProgramSlice")) // the parameter of Program Slice
            hasProgramSlice = true;
        if (opt_exist("-TraditionalSlice"))
            hasTraditionalSlice = true;
        if (opt_exist("-NoDependence"))
            hasNoDependence = true;
        if (opt_exist("-heu"))
            hasHeu = true;
        if (opt_exist("-CIA"))
            hasCIA = true;
        string property_file = "../properties/formula-F.xml";
        if (opt_exist("-property")) {
            option_t option;
            option = get_option("-property");
            // cout << "***********" << option.value.size() << endl << endl;
            if (option.value.size() != 1)
                throw "-property only match one parameter";
            property_file = option.value[0];

            if(property_file == "matched") {
                //batch
                property_file = filename;
                property_file.replace(property_file.find(".c"),2,".xml");
            }
        }

        if (filename.empty()){
            cout << "Please input a filename. More information can get by -help.";
            exit(-1);
        }
        if (hasNoDependence && hasPDNetSlice){
            cout << "-NoDependence can not be with -PDNetSlice";
            exit(-1);
        }
        if (hasTraditionalSlice && hasPDNetSlice){
            cout << "-TraditionalSlice can not be with -PDNetSlice";
            exit(-1);
        }


        auto rowMap = preProcessGetRowMap(filename);
//    gccpreprocess(filename);
        filename = filename.replace(filename.find(".c"), 2, ".iii");
        string csvFileName = "";

        double programSliceTime; //Program slice time
        vector<long int> ps_3_times;
        if(hasProgramSlice) { //program slices
            set<Row_Type> rows;
            set<string> variables;
            extractStatementVariableNameFromXML(property_file,rows,variables);
            clock_t begin,end;
            begin = clock();
            ///Program slicing begin...
//            auto programSliceRowMap = programSlice(filename);
            string cmd = "../exe/ps_3 --input " + filename;
            if(variables.size() != 0)
                cmd += " --variable "; //variable properties
            else
                cmd += " --default";
            for(auto iter=variables.begin();iter != variables.end();iter++){
                cmd += *iter + ",";
            }
            // --execute ps_3 start
            // system(cmd.c_str());
            // --execute ps_3 end
            runPS_3(cmd, ps_3_times);//run and get three time
            ///Program slicing end...
            filename.replace(filename.find(".iii"),4,"_slice.iii");
            end = clock();
            programSliceTime = (end-begin)/1000.0;
//            cout << "time of program slice:"<<programSliceTime<<endl;
            csvFileName += "ProgramSlice";

            //Map location of program slicing

            RowMap programSliceRowMap = readRowMapFromTXT();

            auto originiter = rowMap.begin();
            while(originiter != rowMap.end()){
                auto sliceiter = programSliceRowMap.find(originiter->second);
                if(sliceiter != programSliceRowMap.end())
                    originiter->second = sliceiter->second;
                originiter++;
            }
        }
        if(hasPDNetSlice)
            csvFileName += "PDNetSlice"; //output PDNet slice results
        if(hasTraditionalSlice)
            csvFileName += "TradSlice";
        if(hasNoDependence)
            csvFileName += "NoDependence"; //output Program slice results
        if(csvFileName == "")
            csvFileName = "DirectBuild.csv"; //output original PDNet
        else
            csvFileName += ".csv";

        //输出csv部分
        ofstream oCSVFile;
        if(!fileExists(csvFileName)) {
            oCSVFile.open(csvFileName, ios::app);
            oCSVFile << csvFileName << endl;
            if(!hasProgramSlice)
                oCSVFile << "" << "," << "P" <<"," << "Pv" << "," << "Pc" << "," << "Pf" <<  "," << "T" << ","
                     << "S" << "," << "Tm" << "," << "Ts" << "," << "Tc"  << "," << "R" << "," << "Tpro" << endl;
            else
                oCSVFile << "" << "," << "P" <<"," << "Pv" << "," << "Pc" << "," << "Pf" <<  "," << "T" << ","
                     << "S" << "," << "Tm" << "," << "Ts" << "," << "Tc"  << "," << "R" << ","
                     << "Tdfd" << "," << "Tcfd" << "," << "Ttra" << "," << "Tpro" << endl;
        }
        else{
            oCSVFile.open(csvFileName, ios::app);
        }

        if(!hasCIA){
            string csvOutputStr = model_checking(filename, property_file, ltltype, fnum, showcpn, showtree, hasHeu, hasPDNetSlice, hasTraditionalSlice, hasNoDependence, rowMap);
            if(hasProgramSlice) {
//                csvFormatAppend(csvOutputStr, to_string(programSliceTime));
//                csvFormatAppend(csvOutputStr, "NULL");
//                csvFormatAppend(csvOutputStr, "NULL");
                csvFormatAppend(csvOutputStr, to_string(ps_3_times[0]/1000.0));
                csvFormatAppend(csvOutputStr, to_string(ps_3_times[1]/1000.0));
                csvFormatAppend(csvOutputStr, to_string(ps_3_times[2]/1000.0));
                csvFormatAppend(csvOutputStr, to_string((ps_3_times[0] + ps_3_times[1] + ps_3_times[2]) / 1000.0));
            }
            else{
                csvFormatAppend(csvOutputStr, "NULL"); //Reserved for Tpro
            }
            oCSVFile << csvOutputStr << endl;
        }
        else{//Change Impact Analysis
            string filename_pre = filename, filename_after = filename.replace(filename.rfind(".c"), 2, "-new.c");
            auto rowMap = preProcessGetRowMap(filename_pre);
            preProcessGetRowMap(filename_after);
            filename_pre = filename_pre.replace(filename_pre.rfind(".c"), 2, ".iii");
            filename_after = filename_after.replace(filename_after.rfind(".c"), 2, ".iii");

            changeImpactAnalysis(filename_pre, filename_after, property_file, ltltype, fnum, showcpn, showtree, hasHeu,
                                hasPDNetSlice, rowMap);
        }
        oCSVFile.close();
    }
    catch (const char *msg){
        cerr << msg <<endl;
        exit(-1);
    }

    auto mypid = getpid();
    string cmd = "grep VmPeak /proc/" + to_string(mypid) + "/status";
    system(cmd.c_str());

}

void cmdlinet::help() {
    std::cout<<"\n"
               "* *              A concurrent program model checking tool DAMER 2.0              * *\n";
    std::cout <<
              //              "* *                        Chen Cheng                       * *\n"
              //              "* *      Tongji University, Computer Science Department     * *\n"
              //              "* *                      ccisman@163.com                    * *\n"

              "\n"
              "Optional commands:\n"
              " -showtree             generate picture for program's syntax tree\n"
              " -showcpn              generate picture for builded cpn\n"
              " -heu                  use heuristic information\n"
              " -PDNetSlice           do PDNet slice(build PDNet using the original program and do PDNet slice)\n"
              " -ProgramSlice         do program slice(build PDNet using the program after slicing)\n"
              " -TraditionalSlice     do Traditional CPN Slice\n"
              " -NoDependence         build model without dependence\n"
              " -CIA                  Change Impact Analysis\n"
              " -property filename    property file. (default: ./properties/formula-F.xml).filename is matched the corresponding xml file\n"
              ;

    //          " -fnum [num]           indicate which formula to use (default:1)\n"
//              " -ltlv                 indicate to check the LTLV property in *-V.xml(default:LTLF)\n"
//              "\n"
//              "Conpulsory commands:\n"
//              "    must and only have one of the following options\n"
//              " -directbuild          build the whole CPN for program and check\n"
//              " -slice                slice and check\n"
//              " -compare              both build the origin CPN and the sliced CPN and compare them\n";
}

option_t cmdlinet::get_option(std::string name) {
    for(unsigned int i=0;i<options.size();i++)
        if(options[i].name == name)
            return options[i];

    std::cerr<<"Error in get_option, the option doesn't exist!"<<std::endl;
    exit(-1);
}

