//
// Created on 2021/5/6.
//

#include "MultiSet.h"
#include<cmath>


//IntegerFactory fint;
//RealFactory freal;
//StringFactory fstr;
//ProductFactory fpro;
//DotFactory fdot;

ProductSortValue::ProductSortValue(SORTID sid) {
    auto ps = sorttable.find_productsort(sid);
    auto sortnum = ps.get_sortnum();
    auto sortid = ps.get_sortid();
    this->sid = sid;
    valueindex.resize(sortnum);
    for(int i=0;i<sortnum;i++){
        if(sortid[i].tid == Integer)
            valueindex[i] = (token)(new IntegerSortValue);
        else if(sortid[i].tid == Real)
            valueindex[i] = (token)(new RealSortValue);
        else if(sortid[i].tid == String)
            valueindex[i] = (token)(new StringSortValue);
        else if(sortid[i].tid == dot)
            valueindex[i] = (token)(new DotSortValue);
        else
            valueindex[i] = (token)(new ProductSortValue(sortid[i].sid));
    }
}

void ProductSortValue::setcolor(const Bucket &bucket) {
    if(bucket.tid == productsort && bucket.sid == getsid()){
        auto ps = sorttable.find_productsort(sid);
        auto sortnum = ps.get_sortnum();

        for(int i=0;i<sortnum;i++){
            Bucket bkt;
            bucket.pro[i]->getcolor(bkt);
            valueindex[i]->setcolor(bkt);
        }
    }
    else
        throw "ProductSort's bucket doesn't match in MultiSet.cpp!";
}

void ProductSortValue::getcolor(Bucket &bucket) {
//    if(bucket.tid == productsort){
        auto ps = sorttable.find_productsort(sid);
        auto sortnum = ps.get_sortnum();
        auto sortid = ps.get_sortid();
        bucket.pro.resize(sortnum);
        for(int i=0;i<sortnum;i++){
            if(sortid[i].tid == Integer)
                bucket.pro[i] = (token)(new IntegerSortValue);
            else if(sortid[i].tid == Real)
                bucket.pro[i] = (token)(new RealSortValue);
            else if(sortid[i].tid == String)
                bucket.pro[i] = (token)(new StringSortValue);
            else if(sortid[i].tid == dot)
                bucket.pro[i] = (token)(new DotSortValue);
            else
                bucket.pro[i] = (token)(new ProductSortValue(sortid[i].sid));
        }
        bucket.tid = productsort;
        bucket.sid = getsid();

        for(int i=0;i<sortnum;i++){
            Bucket bkt;
            valueindex[i]->getcolor(bkt);
            bucket.pro[i]->setcolor(bkt);
        }

}

void IntegerSortValue::setcolor(const Bucket &bucket) {
    if(bucket.tid == Integer){
        value = bucket.integer;
    }
    else
        throw "IntegerSort's bucket doesn't match in MultiSet.cpp!";
}

void IntegerSortValue::getcolor(Bucket &bucket) {
        bucket.integer = value;
        bucket.tid = Integer;
}

void RealSortValue::setcolor(const Bucket &bucket) {
    if (bucket.tid == Real) {
        value = bucket.real;
    }
    else
        throw "RealSort's bucket doesn't match in MultiSet.cpp!";
}

void RealSortValue::getcolor(Bucket &bucket) {
        bucket.real = value;
        bucket.tid = Real;
}

void StringSortValue::setcolor(const Bucket &bucket) {
    if(bucket.tid == String){
        value = bucket.str;
    }
    else
        throw "StringSort's bucket doesn't match in MultiSet.cpp!";
}

void StringSortValue::getcolor(Bucket &bucket) {
        bucket.str = value;
        bucket.tid = String;
}

size_t Bucket::hash() {
    switch(tid){
        case Integer:
            return (integer+1) * HASH_OFFSET * HASH_OFFSET;
        case Real:
            return (static_cast<size_t >(real)+1) * HASH_OFFSET * HASH_OFFSET * HASH_OFFSET;
        case String: {
//            size_t count = 0;
//            for (int i = 0; i < str.length(); i++) {
//                count += static_cast<size_t >(str[i]);
//            }
//            return count;
            if(str.length() == 0)
                return 1 * HASH_OFFSET;
            size_t str_hashValue = 0;
            for(int i=0;i<str.length();i++)
                str_hashValue += static_cast<size_t>(str[i]) * HASH_OFFSET;
            return  str_hashValue;
        }
        case dot: {
            return 1 * HASH_OFFSET * HASH_OFFSET * HASH_OFFSET;
        }
        case productsort: {
            size_t count=0;
            auto ps = sorttable.find_productsort(sid);
            auto sortnum = ps.get_sortnum();
            count+= pro.size() * HASH_OFFSET * HASH_OFFSET * HASH_OFFSET * HASH_OFFSET * HASH_OFFSET;
            for (int i = 0; i < sortnum; i++) {
                Bucket bkt;
                pro[i]->getcolor(bkt);
                count += bkt.hash();
            }
            return count;
//            Bucket bkt;
//            pro[pro.size()-1]->getcolor(bkt);
//            return bkt.hash();
        }
    }
    throw "error tid in hash() in MultiSet.cpp!";

}

//bool operator==(const Product_t &p1,const Product_t &p2){
//    if(p1.size() != p2.size())
//        return false;
//    for(int i=0;i<p1.size();i++){
//        Bucket bkt1,bkt2;
//        p1[i]->getcolor(bkt1);
//        p2[i]->getcolor(bkt2);
//        if(bkt1 == bkt2)
//            ;
//        else
//            return false;
//    }
//    return true;
//}

bool Equal_product(const Product_t &p1,const Product_t &p2){
    if(p1.size() != p2.size())
        return false;
    for(int i=0;i<p1.size();i++){
        Bucket bkt1,bkt2;
        p1[i]->getcolor(bkt1);
        p2[i]->getcolor(bkt2);
        if(bkt1 == bkt2)
            ;
        else
            return false;
    }
    return true;
}

bool Lessthan_product(const Product_t &p1,const Product_t &p2){
    if(p1.size() != p2.size())
        return p1.size()<p2.size();
//    for(int i=0;i<p1.size();i++){
//        Bucket bkt1,bkt2;
//        p1[i]->getcolor(bkt1);
//        p2[i]->getcolor(bkt2);
//        if(bkt1 < bkt2)
//            return true;
//    }
//    return false;
    int length = p1.size();
    Bucket bkt1,bkt2;
    p1[length-1]->getcolor(bkt1);
    p2[length-1]->getcolor(bkt2);
    return bkt1<bkt2;
}


//
//bool operator<(const Product_t &p1,const Product_t &p2){
//    if(p1.size() != p2.size())
//        return p1.size()<p2.size();
//    for(int i=0;i<p1.size();i++){
//        Bucket bkt1,bkt2;
//        p1[i]->getcolor(bkt1);
//        p2[i]->getcolor(bkt2);
//        if(bkt1 < bkt2)
//            return true;
//    }
//    return false;
//}

//bool operator==(const token &t1,const token &t2){
//    Bucket bkt1,bkt2;
//    t1->getcolor(bkt1);
//    t2->getcolor(bkt2);
//    return bkt1 == bkt2;
//}

bool Bucket::operator==(const Bucket &bkt) {
    switch (tid) {
        case Integer:
            return integer == bkt.integer;
        case Real:
            return real == bkt.real;
        case String:
            return str == bkt.str;
        case dot:
            return tid == bkt.tid;
        case productsort:
            return Equal_product(pro , bkt.pro);
    }
    throw "error tid in operator== in MultiSet.cpp!";
}

bool Bucket::operator<(const Bucket &bkt) {
    switch (tid) {
        case Integer:
            return integer < bkt.integer;
        case Real:
            return real < bkt.real;
        case String:
            return str < bkt.str;
        case dot:
            return tid < bkt.tid;
        case productsort:
            return Lessthan_product(pro , bkt.pro);
    }
    throw "error tid in operator< in MultiSet.cpp!";
}

void Bucket::getVarValue(Bucket &bkt) {
    pro[0]->getcolor(bkt);
    return;
}

//void Bucket::printBkt() {
//    switch (tid) {
//        case productsort:
//            for (int i = 0; i < sorttable.productsort[sid].sortnum; i++) {
//                Bucket tmpbkt;
//                pro[i]->getcolor(tmpbkt);
//                tmpbkt.printBkt();
//                cout << ",";
//            }
//            cout<<endl;
//            break;
//        case Integer:
//            cout << integer;
//            break;
//        case Real:
//            cout << real;
//            break;
//        case String:
//            cout << str;
//            break;
//
//    }
//}



bool MultiSet::generateFromToken(const token &tk) {
//    if(tid != tk->getmytype() || (tid == productsort && sid != tk->getsid()))
//        throw "generateFromToken error! type doesn't match!";
    color_count = 1;
    tid = tk->getmytype();
    if(tid == productsort)
        sid = tk->getsid();
    else
        sid = 0;
    auto iter = mapTokens.find(tk);
    if(iter != mapTokens.end())
        iter->second++;
    else
        mapTokens.emplace(tk, 1);
    return true;
}


bool MultiSet::operator>=(const MultiSet &ms) {
    if(tid != ms.tid || (tid == productsort && sid != ms.sid))
        throw "error! MultiSet >= type doesn't match!";
    auto iter2 = ms.mapTokens.begin();
    while(iter2 != ms.mapTokens.end()){
        auto iter1 = mapTokens.find(iter2->first);
        if(iter1 == mapTokens.end())
            return false;
        if(iter1->second<iter2->second)
            return false;
        iter2++;
    }
    return true;
}

bool MultiSet::operator==(const MultiSet &ms) {
    if(tid != ms.tid || (tid == productsort && sid != ms.sid))
        throw "error! MultiSet == type doesn't match!";
    if(ms.getcolorcount()!=getcolorcount())
        return false;
    auto iter2 = ms.mapTokens.begin();
    while(iter2 != ms.mapTokens.end()){
        auto iter1 = mapTokens.find(iter2->first);
        if(iter1 == mapTokens.end())
            return false;
        if(iter1->second != iter2->second)
            return false;
        iter2++;
    }
    return true;
}

void MultiSet::operator=(const MultiSet &ms) {
    tid = ms.tid;
    sid = ms.sid;
    mapTokens = ms.mapTokens;
    color_count = ms.color_count;
}

bool MultiSet::MINUS(const MultiSet &ms) {
    if(tid != ms.tid || (tid == productsort && sid != ms.sid)){
        cout<<tid<<endl<<ms.tid<<endl;
        throw "error! MultiSet MINUS type doesn't match!";
    }
    auto iter2 = ms.mapTokens.begin();
    while(iter2 != ms.mapTokens.end()){
        auto iter1 = mapTokens.find(iter2->first);
//        if(iter1 == mapTokens.end() || iter1->second<iter2->second)
//            return false;
        if(iter1 == mapTokens.end()) {
            mapTokens.emplace(iter2->first,-iter2->second);
        }
        else {
            iter1->second -= iter2->second;
            if(iter1->second == 0) {
                mapTokens.erase(iter1);
                color_count--;
            }
        }
        iter2++;
    }
    return true;
}
// multisets
bool MultiSet::PLUS(const MultiSet &ms) {
    if(tid != ms.tid || (tid == productsort && sid != ms.sid)){
        throw "error! MultiSet >= type doesn't match!";
    }
    auto iter2 = ms.mapTokens.begin();
    while(iter2 != ms.mapTokens.end()){
        auto iter1 = mapTokens.find(iter2->first);
        if(iter1 == mapTokens.end()) {
            mapTokens.emplace(iter2->first, iter2->second);
            color_count++;
        }
        else
            iter1->second+=iter2->second;
        iter2++;
    }
    return true;
}

hash_t MultiSet::Hash() {
    auto iter = mapTokens.begin();
    if(color_count == 0)
        return 0;
//    auto tk = mapTokens.begin()->first;
//    auto tkc = mapTokens.begin()->second;
    hash_t hv = 0;
    hv = mapTokens.size()*HASH_OFFSET * HASH_OFFSET * HASH_OFFSET;
//    hv = tkc * HASH_OFFSET * HASH_OFFSET * HASH_OFFSET;
    clock_t start,end;

    while(iter != mapTokens.end()){
        hv += iter->second * HASH_OFFSET * HASH_OFFSET;
//        hv += tkc * HASH_OFFSET * HASH_OFFSET;

    Bucket bkt;
//        start = clock();
        iter->first->getcolor(bkt);
//        tk->getcolor(bkt);
//        end = clock();
//        cout<<"getcolor time:"<<end-start<<endl;
//        start = clock();
        hv += bkt.hash();
//        end = clock();
//        cout<<"sub hash time:"<<end-start<<endl;
        iter++;
    }
    return hv;
}


