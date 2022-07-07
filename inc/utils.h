#pragma once
#include <string>
#include <tuple>
#include <list>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <iostream>  // std::cout, std::endl
#include <iomanip>   // std::setprecision
#include <exception>
#include <assert.h>
#include <math.h>
#include <limits> // numeric_limits
#include <algorithm>
#include <memory> // 

using std::string;
using std::to_string;
using std::tuple;
using std::tie;
using std::make_tuple;
using std::vector;
using std::array;
using std::list;
using std::hash;
using std::unordered_set;
using std::cout;
using std::endl;
using std::runtime_error;
using std::max;
using std::min;
using std::shared_ptr;
using std::make_shared;

const double DOUBLEMAX = std::numeric_limits<double>::max();
const int INTMAX = std::numeric_limits<int>::max();


/////////////////////////////////////////////////////
//////////////// Start useful functions //////////////////

inline bool equalDb(double x, double y) {
    return abs(x - y) < 1e-8;
}

inline void notImplemented(string message) {
    throw runtime_error(message + ": Not implemented\n");
}

template <class State>
string getPathString(vector<State>& path) {
    string ans = "";
    for (State& s: path) {
        ans += s.getStr() + "->";
    }
    return ans;
}
///////////////// End useful stuff ///////////////////
/////////////////////////////////////////////////////