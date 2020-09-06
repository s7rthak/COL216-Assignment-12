
#include "param_read.hpp"
#include <bits/stdc++.h>

using namespace std;

int eps = 1e-5;

class CacheBlock{
    public:
        int index = -1, tag;
        bool valid = 0, dirty = 0;
        int data = 0;
        int last_access = -1;
        bool page_ref = 0;
        CacheBlock(int i){
            index = i;
        }
};

class CacheAssoc{
    public:
        int assoc_size;
        vector<CacheBlock> members;
        CacheAssoc(int a){
            assoc_size = a;
            // members.assign(assoc_size, CacheBlock(u));
            // for(int i=u;i<d;i++){
            //     members[i-u] = CacheBlock(i);
            // }
        }
        vector<CacheBlock> t1;
        vector<CacheBlock> t2;
        vector<CacheBlock> b1;
        vector<CacheBlock> b2;
        int t1_head = 0;
        int t2_head = 0;
};

class CacheDir{
    public:
        int assoc = 1;
        int block_size, num_blocks;
        int cur_inst = -1;
        vector<CacheAssoc> cache;
        CacheDir(int a, int bl, int bs){
            assoc = a;
            block_size = bs;
            num_blocks = bl;
            for(int i=0;i<bl;i++){
                cache.push_back(CacheAssoc(assoc));
            }
        }
        int p = 0;
};

int indexOf(vector<CacheBlock> &search, int data){
    for(int i=0;i<search.size();i++){
        if(search[i].index == data)
            return i;
    }
    return -1;
}

void replace(CacheAssoc &myAssoc, CacheDir &myCache){
    bool found = 0;
    while(!found){
        if(myAssoc.t1.size() >= max(1,myCache.p)){
            if(!myAssoc.t1[myAssoc.t1_head].page_ref){
                found = 1;
                myAssoc.b1.push_back(myAssoc.t1[myAssoc.t1_head]);
                // cout<<myAssoc.b1.size()<<"\n";
                myAssoc.t1.erase(myAssoc.t1.begin() + myAssoc.t1_head);
                if(myAssoc.t1.size() != 0)
                    myAssoc.t1_head = myAssoc.t1_head % myAssoc.t1.size();
                // cout<<"hello\n";
            }else{
                myAssoc.t1[myAssoc.t1_head].page_ref = 0;
                // if(myAssoc.t2.size() != 0)
                myAssoc.t2.insert(myAssoc.t2.begin() + myAssoc.t2_head, myAssoc.t1[myAssoc.t1_head]);
                // else
                //     myAssoc.t2.push_back(myAssoc.t1[myAssoc.t1_head]);
                myAssoc.t2_head++;
                myAssoc.t2_head = myAssoc.t2_head % myAssoc.t2.size();
                myAssoc.t1.erase(myAssoc.t1.begin() + myAssoc.t1_head);
                if(myAssoc.t1.size() != 0)
                    myAssoc.t1_head = myAssoc.t1_head % myAssoc.t1.size();
            }
        }else{
            if(!myAssoc.t2[myAssoc.t2_head].page_ref){
                found = 1;
                myAssoc.b2.push_back(myAssoc.t2[myAssoc.t2_head]);
                myAssoc.t2.erase(myAssoc.t2.begin() + myAssoc.t2_head);
                if(myAssoc.t2.size() != 0)
                    myAssoc.t2_head = myAssoc.t2_head % myAssoc.t2.size();
            }else{
                myAssoc.t2[myAssoc.t2_head].page_ref = 0;
                myAssoc.t2_head++;
                if(myAssoc.t2.size() != 0)
                    myAssoc.t2_head = myAssoc.t2_head % myAssoc.t2.size();
            }
        }
    }
}

void CAR_read(int check, int assoc_to_check, CacheDir &myCache){
    myCache.cur_inst++;
    CacheAssoc &cur_set = myCache.cache[assoc_to_check];
    int i1 = indexOf(cur_set.t1, check), i2 = indexOf(cur_set.t2, check);
    if(i1 >= 0 || i2 >= 0){             // cache hit
        if(i1 >= 0){
            cur_set.t1[i1].last_access = myCache.cur_inst;
            cur_set.t1[i1].page_ref = 1;
        }else{
            cur_set.t2[i2].last_access = myCache.cur_inst;
            cur_set.t2[i2].page_ref = 1;
        }
    }else{                              // cache miss      
        int j1 = indexOf(cur_set.b1, check), j2 = indexOf(cur_set.b2, check);
        if(cur_set.t1.size() + cur_set.t2.size() == cur_set.assoc_size){
            replace(cur_set, myCache);
            // cout<<cur_set.b1.size()<<"\n";
            int j1 = indexOf(cur_set.b1, check), j2 = indexOf(cur_set.b2, check);
            if(j1 < 0 && j2 < 0 && cur_set.b1.size() + cur_set.t1.size() == cur_set.assoc_size){
                cur_set.b1.erase(cur_set.b1.begin());
            }else if(cur_set.t1.size() + cur_set.t2.size() + cur_set.b1.size() + cur_set.b2.size() == 2 * cur_set.assoc_size && j1 < 0 && j2 < 0){
                cur_set.b2.erase(cur_set.b2.begin());
            }
        }
        if(j1 < 0 && j2 < 0){
            cur_set.t1.insert(cur_set.t1.begin() + cur_set.t1_head++, CacheBlock(check));
            cur_set.t1[cur_set.t1_head-1].last_access = myCache.cur_inst;
            cur_set.t1_head = cur_set.t1_head % cur_set.t1.size();
        } else if(j1 >= 0){
            myCache.p = min(myCache.p + max(1, (int)(cur_set.b2.size()/(cur_set.b1.size() + eps))), cur_set.assoc_size);
            cur_set.t2.insert(cur_set.t2.begin() + cur_set.t2_head++, CacheBlock(check));
            cur_set.t2[cur_set.t2_head-1].last_access = myCache.cur_inst;
            cur_set.t2_head = cur_set.t2_head % cur_set.t2.size();
        } else if(j2 >= 0){
            myCache.p = max(myCache.p - max(1, (int)(cur_set.b1.size()/(cur_set.b2.size() + eps))), 0);
            cur_set.t2.insert(cur_set.t2.begin() + cur_set.t2_head++, CacheBlock(check));
            cur_set.t2[cur_set.t2_head-1].last_access = myCache.cur_inst;
            cur_set.t2_head = cur_set.t2_head % cur_set.t2.size();
        }
    }
}

void CAR_write(int check, int assoc_to_check, CacheDir &myCache, int what){
    myCache.cur_inst++;
    CacheAssoc &cur_set = myCache.cache[assoc_to_check];
    int i1 = indexOf(cur_set.t1, check), i2 = indexOf(cur_set.t2, check);
    if(i1 >= 0 || i2 >= 0){             // cache hit
        if(i1 >= 0){
            cur_set.t1[i1].last_access = myCache.cur_inst;
            cur_set.t1[i1].page_ref = 1;
            cur_set.t1[i1].dirty = 1;
            cur_set.t1[i1].data = what;
        }else{
            cur_set.t2[i2].last_access = myCache.cur_inst;
            cur_set.t2[i2].page_ref = 1;
            cur_set.t1[i1].dirty = 1;
            cur_set.t1[i1].data = what;
        }
    }else{                              // cache miss      
        int j1 = indexOf(cur_set.b1, check), j2 = indexOf(cur_set.b2, check);
        if(cur_set.t1.size() + cur_set.t2.size() == cur_set.assoc_size){
            replace(cur_set, myCache);
            int j1 = indexOf(cur_set.b1, check), j2 = indexOf(cur_set.b2, check);
            if(j1 < 0 && j2 < 0 && cur_set.b1.size() + cur_set.t1.size() == cur_set.assoc_size){
                cur_set.b1.erase(cur_set.b1.begin());
            }else if(cur_set.t1.size() + cur_set.t2.size() + cur_set.b1.size() + cur_set.b2.size() == 2 * cur_set.assoc_size && j1 < 0 && j2 < 0){
                cur_set.b2.erase(cur_set.b2.begin());
            }
        }
        if(j1 < 0 && j2 < 0){
            cur_set.t1.insert(cur_set.t1.begin() + cur_set.t1_head++, CacheBlock(check));
            cur_set.t1[cur_set.t1_head-1].last_access = myCache.cur_inst;
            cur_set.t1[cur_set.t1_head-1].data = what;
            cur_set.t1[cur_set.t1_head-1].dirty = 1;
            cur_set.t1_head = cur_set.t1_head % cur_set.t1.size();
        } else if(j1 >= 0){
            myCache.p = min(myCache.p + max(1, (int)(cur_set.b2.size()/(cur_set.b1.size() + eps))), cur_set.assoc_size);
            cur_set.t2.insert(cur_set.t2.begin() + cur_set.t2_head++, CacheBlock(check));
            cur_set.t2[cur_set.t2_head-1].last_access = myCache.cur_inst;
            cur_set.t2[cur_set.t2_head-1].data = what;
            cur_set.t2[cur_set.t2_head-1].dirty = 1;
            cur_set.t2_head = cur_set.t2_head % cur_set.t2.size();
        } else if(j2 >= 0){
            myCache.p = max(myCache.p - max(1, (int)(cur_set.b1.size()/(cur_set.b2.size() + eps))), 0);
            cur_set.t2.insert(cur_set.t2.begin() + cur_set.t2_head++, CacheBlock(check));
            cur_set.t2[cur_set.t2_head-1].last_access = myCache.cur_inst;
            cur_set.t2[cur_set.t2_head-1].data = what;
            cur_set.t2[cur_set.t2_head-1].dirty = 1;
            cur_set.t2_head = cur_set.t2_head % cur_set.t2.size();
        }
    }
}

int main(int argc, char *argv[]){
    // cout<<"h";
    vector<int> p = paramToVector(argv[1]);
    vector<vector<string>> mem_acc = inputToVector(argv[1]);
    int cache_size = p[0];
    int block_size = p[1];
    int cache_assoc = p[2];
    int T = p[3];
    CacheDir myCache = CacheDir(cache_assoc, cache_size/block_size, block_size);
    int index_size = cache_size/block_size;
    for(int i=0;i<mem_acc.size();i++){
        int acc = stoi(mem_acc[i][0]);
        int to_check = (acc%index_size)/cache_assoc;
        if(mem_acc[i][1] == "R"){
            CAR_read(acc,to_check,myCache);
        }else{
            CAR_write(acc,to_check,myCache,stoi(mem_acc[i][2]));
        }
    }
    // replace(myCache.cache[0], myCache);
    // cout<<myCache.cache[0].t1_head;
    for(int i=0;i<index_size/cache_assoc;i++){
        CacheAssoc &to_print = myCache.cache[i];
        cout<<"--- T1 of Assoc-" + to_string(i) + " ---\n";
        for(int j=0;j<to_print.t1.size();j++){
            cout<<to_print.t1[j].index<<" "<<to_print.t1[j].data<<"\n";
        }
        cout<<"--- T2 of Assoc-" + to_string(i) + " ---\n";
        for(int j=0;j<to_print.t2.size();j++){
            cout<<to_print.t2[j].index<<" "<<to_print.t2[j].data<<"\n";
        }
        cout<<"--- B1 of Assoc-" + to_string(i) + " ---\n";
        for(int j=0;j<to_print.b1.size();j++){
            cout<<to_print.b1[j].index<<" "<<to_print.b1[j].data<<"\n";
        }
        cout<<"--- B2 of Assoc-" + to_string(i) + " ---\n";
        for(int j=0;j<to_print.b2.size();j++){
            cout<<to_print.b2[j].index<<" "<<to_print.b2[j].data<<"\n";
        }
        cout<<"\n";
    }
    // cout<<myCache.p<<"\n";
}