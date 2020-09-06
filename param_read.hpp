#ifndef PARAM_READ_HPP_
#define PARAM_READ_HPP_

#include <fstream>
#include <bits/stdc++.h>
#include <sstream>
#include <iterator>
#include "util.hpp"

using namespace std;

vector<int> paramToVector(string inst_file){
    ifstream in(inst_file);
    string str;
    vector<int>input;
    for(int i=0;i<4;i++){
        getline(in, str);
        input.push_back(stoi(split(str,' ')[0]));
    }
    in.close();
    return input;
}

vector<vector<string>> inputToVector(string inst_file){
    ifstream in(inst_file);
    string str;
    for(int i=0;i<5;i++){
        getline(in, str);
    }
    vector<vector<string>> res;
    while(getline(in, str)){
        res.push_back(split(str, ','));
    }
    in.close();
    return res;
}

#endif