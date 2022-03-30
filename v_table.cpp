//
// Created on 2020/10/10.
//

#include"v_table.h"

void V_Table::clear() {
    v_map.clear();
//    for(int i=0;i<count;i++)
//        delete table[i];
    fa = NULL;
}
void V_Table::insert(std::string v, std::string place,string tag,string basetag,SizeList sizelist,bool ispointer) {
    Item item;
//    item->name = v;
    item.place = place;
    item.tag = tag;
    item.sizelist = sizelist;
    item.is_pointer = ispointer;
    item.base_tag = basetag;
//    table[count++] = item;
    v_map.insert(std::pair<std::string,Item>(v,item));
//        v_map.insert(std::pair<std::string, std::string>(v, place));
//        v_sizemap.insert(std::pair<std::string, int>(v, size));
//        v_ispointer.insert(std::pair<std::string,bool>(v,is_pointer));
//        std::vector<std::string> tmp;
//        v_pointerrelated.insert(std::pair<std::string,std::vector<std::string>>(v,tmp));
}

bool V_Table::find_P(std::string s){
    auto iter = v_map.begin();
    for(;iter!=v_map.end();iter++)
        if(iter->second.place == s)
            return true;
    return false;
//    for(unsigned int i=0;i<count;i++)
//        if(table[i]->place == s)
//            return true;
//    return false;
}

std::string V_Table::get_place(std::string v) {
    auto iter = v_map.find(v);
    if (iter != v_map.end())
    {
        return iter->second.place;
    }
    else
    {
        V_Table *temp_fa = fa;
        while (temp_fa != NULL)
        {
            iter = temp_fa->v_map.find(v);
            if (iter != temp_fa->v_map.end())
                return iter->second.place;
            else
                temp_fa = temp_fa->fa;
        }
        throw  "can't find variable " + v ;
    }
//    for(unsigned int i=0;i<count;i++)
//        if(table[i]->name == v)
//            return table[i]->place;
//    V_Table *temp_fa = fa;
//    while(temp_fa != NULL){
//        for(unsigned int i=0;i<temp_fa->count;i++)
//            if(temp_fa->table[i]->name == v)
//                return temp_fa->table[i]->place;
//        temp_fa = temp_fa->fa;
//    }
//    std::cout << "can't find variable " << v << std::endl;
//    exit(-1);
}

std::string V_Table::get_place_safe(std::string v) {
    auto iter = v_map.find(v);
    if (iter != v_map.end())
    {
        return iter->second.place;
    }
    else
    {
        V_Table *temp_fa = fa;
        while (temp_fa != NULL)
        {
            iter = temp_fa->v_map.find(v);
            if (iter != temp_fa->v_map.end())
                return iter->second.place;
            else
                temp_fa = temp_fa->fa;
        }
        return "";
    }
//    for(unsigned int i=0;i<count;i++)
//        if(table[i]->name == v)
//            return table[i]->place;
//    V_Table *temp_fa = fa;
//    while(temp_fa != NULL){
//        for(unsigned int i=0;i<temp_fa->count;i++)
//            if(temp_fa->table[i]->name == v)
//                return temp_fa->table[i]->place;
//        temp_fa = temp_fa->fa;
//    }
//    std::cout << "can't find variable " << v << std::endl;
//    exit(-1);
}

SizeList V_Table::get_size(std::string v) {
    auto iter = v_map.find(v);
    if (iter != v_map.end())
    {
        return iter->second.sizelist;
    }
    else
    {
        V_Table *temp_fa = fa;
        while (temp_fa != NULL)
        {
            iter = temp_fa->v_map.find(v);
            if (iter != temp_fa->v_map.end())
                return iter->second.sizelist;
            else
                temp_fa = temp_fa->fa;
        }
        throw "can't find variable_size " + v ;
    }
    return SizeList();
//    for(unsigned int i=0;i<count;i++)
//        if(table[i]->name == v)
//            return table[i]->size;
//    V_Table *temp_fa = fa;
//    while(temp_fa != NULL){
//        for(unsigned int i=0;i<temp_fa->count;i++)
//            if(temp_fa->table[i]->name == v)
//                return temp_fa->table[i]->size;
//        temp_fa = temp_fa->fa;
//    }
//    std::cout << "can't find variable_size " << v << std::endl;
//    exit(-1);
}

bool V_Table::get_ispointer(std::string v) {
    auto iter = v_map.find(v);
    if (iter != v_map.end())
    {
        return iter->second.is_pointer;
    }
    else
    {
        V_Table *temp_fa = fa;
        while (temp_fa != NULL)
        {
            iter = temp_fa->v_map.find(v);
            if (iter != temp_fa->v_map.end())
                return iter->second.is_pointer;
            else
                temp_fa = temp_fa->fa;
        }
        throw "can't find variable_ispointer " + v;
    }
    return false;
//    for(unsigned int i=0;i<count;i++)
//        if(table[i]->name == v)
//            return table[i]->is_pointer;
//    V_Table *temp_fa = fa;
//    while(temp_fa != NULL){
//        for(unsigned int i=0;i<temp_fa->count;i++)
//            if(temp_fa->table[i]->name == v)
//                return temp_fa->table[i]->is_pointer;
//        temp_fa = temp_fa->fa;
//    }
//    std::cout << "can't find variable_ispointer " << v << std::endl;
//    exit(-1);
}

bool V_Table::get_isglobal(std::string v) {
    auto iter = v_map.find(v);
    if(iter != v_map.end()){
        if(name == global_table_name)
            return true;
        else
            return false;
    }
    else
    if(fa != NULL)
        return fa->get_isglobal(v);
    else
        throw "can't find variable:" + v;
}

string V_Table::get_tag(std::string v) {
    auto iter = v_map.find(v);
    if (iter != v_map.end())
    {
        return iter->second.tag;
    }
    else
    {
        V_Table *temp_fa = fa;
        while (temp_fa != NULL)
        {
            iter = temp_fa->v_map.find(v);
            if (iter != temp_fa->v_map.end())
                return iter->second.tag;
            else
                temp_fa = temp_fa->fa;
        }
        throw "can't find variable_tag " + v;
    }
}

string V_Table::get_basetag(std::string v) {
    auto iter = v_map.find(v);
    if (iter != v_map.end())
    {
        return iter->second.base_tag;
    }
    else
    {
        V_Table *temp_fa = fa;
        while (temp_fa != NULL)
        {
            iter = temp_fa->v_map.find(v);
            if (iter != temp_fa->v_map.end())
                return iter->second.base_tag;
            else
                temp_fa = temp_fa->fa;
        }
        throw "can't find variable_basetag " + v; //can't find
    }
}

