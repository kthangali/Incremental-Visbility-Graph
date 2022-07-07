#pragma once
#include "utils.h"
#include <queue>

// Ensures that .customHash() and == are defined
//   and that hash<State> works
template <class State>
void checkStateDefinedProperly() {
    State s;
    s.customHash(); // Checking.customHash() exists
    assert(s == s); // Checking == exists
    auto x = hash<State>{}(s); // Checking that std::hash<State> has been defined
    int y = s.c[0]; // Checking that state has a coordinates array c
    string z = s.getStr(); // Checking getStr() exists
}

////////////////////////////////////////////////
///////////////// Example State ////////////////

// Example State
class StateExample {
public:
    // Required for heuristic templated functions
    array<int, 1> c;

    // Required for hashing
    virtual size_t customHash() const = 0;

    // Required equal operator
    virtual bool operator==(StateExample const& other) const = 0;

    // Required for logging
    virtual string getStr() const = 0;
};

// Making std::hash<StateExample> use s.customHash();
namespace std  {
    template<>
    struct hash<StateExample> {
        size_t operator()(StateExample const& s) const {
            return s.customHash();
        }
    };
}

////////////////////////////////////////////////
///////////////// StateXY //////////////////////

class StateXY {
public:
    array<int, 2> c;

    StateXY() {c[0] = 0; c[1] = 0;};
    StateXY(int x, int y) {c[0] = x; c[1] = y;};

    size_t customHash() const;
    bool operator==(StateXY const& other) const;
    string getStr() const;

    StateXY operator+(StateXY const& other) const;
    bool operator<(StateXY const& other) const;
};

namespace std  {
    template<>
    struct hash<StateXY> {
        size_t operator()(const StateXY& s) const {
            return s.customHash();
        }
    };
}


////////////////////////////////////////////////
//////////////// StateXYTime ///////////////////

class StateXYTime {
public:
    array<int, 3> c;

    StateXYTime() {c[0] = 0; c[1] = 0; c[2] = 0;};
    StateXYTime(int x, int y, int t) {c[0] = x; c[1] = y; c[2] = t;};

    size_t customHash() const;
    bool operator==(StateXYTime const& other) const;
    string getStr() const;

    StateXYTime operator+(StateXYTime const& other) const;
};

namespace std  {
    template<>
    struct hash<StateXYTime> {
        size_t operator()(const StateXYTime& s) const {
            return s.customHash();
        }
    };
}


/////////////////////////////////////////////////////
///////////////// StateXYTheta //////////////////////

class StateXYTheta {
public:
    array<int, 3> c;

    StateXYTheta() {c[0] = 0; c[1] = 0; c[2] = 0;};
    StateXYTheta(int x, int y, int theta) {c[0] = x; c[1] = y; c[2] = theta;};

    size_t customHash() const;
    bool operator==(StateXYTheta const& other) const;
    string getStr() const;

    StateXYTheta operator+(StateXYTheta const& other) const;
};

namespace std  {
    template<>
    struct hash<StateXYTheta> {
        size_t operator()(const StateXYTheta& s) const {
            return s.customHash();
        }
    };
}