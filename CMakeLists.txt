cmake_minimum_required(VERSION 3.15)
project(PDNet2_0)
# include_directories("/usr/local/include")

set(CMAKE_CXX_STANDARD 14)
# set(CMAKE_CXX_COMPILER "/opt/homebrew/Cellar/gcc/11.2.0_1/bin/g++-11")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pthread -Ofast")

add_executable(PDNet2_0 main.cpp   v_table.cpp v_table.h y.tab.cpp y.tab.h lex.y.cpp
         expression_tree.cpp expression_tree.h AST.cpp AST.h pre_process.cpp cpn.h cpn.cpp base.cpp base.h MultiSet.h MultiSet.cpp Sort.h cpn_rg.h cpn_rg.cpp slice.cpp
        criteria.cpp
        product.cpp
        product.h
        BA/buchi.cpp
        BA/buchi.h
        BA/set.h
        BA/Syntax_Tree.cpp
        BA/Syntax_Tree.h
        BA/tinystr.cpp
        BA/tinystr.h
        BA/tinyxml.cpp
        BA/tinyxml.h
        BA/tinyxmlerror.cpp
        BA/tinyxmlparser.cpp
        BA/VWAA.cpp
        BA/VWAA.h 
        pdnet_parseoption.h
        pdnet_parseoption.cpp 
        xmlTransform.cpp 
        rowTrans.h
        CFG.h
        CFG.cpp
        GraphMaker.h
        GraphMaker.cpp
        Tree.h
        Tree.cpp
        traditionalSlice.cpp
        AST_compare.h
        AST_compare.cpp
        changeConstruct.h
        changeConstruct.cpp)

 target_link_libraries(PDNet2_0 tcmalloc)