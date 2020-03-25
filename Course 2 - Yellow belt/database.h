#pragma once

#include "date.h"
#include "node.h"
#include <map>
#include <set>
#include <algorithm>
#include <utility>
#include <functional>
#include <string>

using namespace std;

//==================================================================================
class Database {
public:
    void Add(const Date& date, const string& event);    

    vector<string> FindIf(function<bool(const Date & date, const string & even)> pred) const;
    int RemoveIf(function<bool(const Date & date, const string & even)> pred);

    string Last(const Date& date) const;
    void Print(ostream& stream) const;

private:    
    map<Date, pair<vector<string>, set<string>>> storage;
};