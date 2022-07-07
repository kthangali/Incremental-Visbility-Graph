// Example program
#include "utils.h"
#include <queue> // priority queue

using namespace std;

// Example State
class ExampleState {
public:
    virtual size_t customHash() const = 0;
    virtual bool operator==(ExampleState& other) const = 0;
};

/////////////////////////////////////////
////////////// StateXY1 //////////////////

// StateXY1
class StateXY1 {
public:
    int c[2];

    StateXY1() {c[0] = 0; c[1] = 0;};
    StateXY1(int x, int y) {c[0] = x; c[1] = y;};

    size_t customHash() const {
        // size_t res = 17;
        // res = res * 31 + hash<int>()( c[0] );
        // res = res * 31 + hash<int>()( c[1] );
        // return res;
        return 17;
    }
    bool operator==(StateXY1& other) const {
        return c[0] == other.c[0] && c[1] == other.c[1];
    };
};

namespace std 
{
    template<>
    struct hash<StateXY1>
    {
        size_t operator()(StateXY1 const& s) const
        {
            return s.customHash();
        }
    };
}


class XY_HF_Euclidean {
public:
    XY_HF_Euclidean(){};
    XY_HF_Euclidean(const StateXY1& goalState): m_goalState(goalState){};
    double getHeuristic(const StateXY1& s) {return 1;};

    StateXY1 m_goalState;
};

/////////////////////////////////////////
////////////// StateXY1Theta //////////////////

class StateXY1Theta {
public:
    int c[3];
    double blah;

    StateXY1Theta() {c[0] = 0; c[1] = 0; c[2] = 0;};
    StateXY1Theta(int x, int y, int theta) {c[0] = x; c[1] = y; c[2] = theta;};

    size_t customHash() {
        return 17;
    }
    bool operator==(StateXY1Theta& other) const {
        return c[0] == other.c[0] && c[1] == other.c[1] && c[2] == other.c[2];
    };
};

class XYTheta_HF_Euclidean {
public:
    XYTheta_HF_Euclidean(){};
    XYTheta_HF_Euclidean(const StateXY1Theta& goalState): m_goalState(goalState){};
    double getHeuristic(const StateXY1Theta& s) {return 1;};

    StateXY1Theta m_goalState;
};

// template <class AStateType>
// class XYTheta_HF_Euclidean2 {
// public:
//     XYTheta_HF_Euclidean2(const StateXY1Theta& goalState): m_goalState(goalState){};
//     double getHeuristic(const StateXY1Theta& s) {
//         return 
//         // return 1;
//     };

//     // StateXY1Theta m_goalState;
//     AStateType m_goalState;
// };

// template <>
// class XYTheta_HF_Euclidean2 <StateXY1Theta> {
// public:
//     XYTheta_HF_Euclidean2(const StateXY1Theta& goalState): m_goalState(goalState){};
//     double getHeuristic(const StateXY1Theta& s) {
//         return m_goalState.blah;
//     };

//     StateXY1Theta m_goalState;
// };

///////////////////////////////////////////////////
///////////// TestQ //////////////////

template <class AStateType> 
class TestNode {
public:
    TestNode* parent;
    double g;
    AStateType s;
};

template <class AStateType>
class QTestNode {
public:
    double f;
    TestNode<AStateType>* n;
};

template <class AStateType>
struct QTestNodeComparator {
    bool operator()(const QTestNode<AStateType>& p1, const QTestNode<AStateType>& p2) {
        return p1.f > p2.f; // Note need > for min queue
    }
};

template <class AStateType>
struct QTestNodeHasher { // TODO: Make this based on StateHasher
    size_t operator()(const QTestNode<AStateType>& k) const { // Based on k's underlying state
        return k.n->s.customHash();
    }
};

template <class AStateType>
struct QTestNodeEqual { // TODO: Make this based on StateEqual
    bool operator()(const QTestNode<AStateType>& lhs, const QTestNode<AStateType>& rhs) const {
        return lhs.n->s == rhs.n->s;
    }
};



template <class AStateType, class HF> 
class TestQ {
public:
    using QNodeType = QTestNode<AStateType>;
    TestQ(){};

    HF* xyhf;
    // unordered_set<TestNode<AStateType>> closedList;
    std::priority_queue<QNodeType, vector<QNodeType>, QTestNodeComparator<AStateType>> m_pq;
    void insert(TestNode<AStateType>* n) {
        n->s;
        xyhf->getHeuristic(n->s);
    }
};

template <class AStateType, class HF>
void testImp() {
    AStateType s;
    HF hf(s);
    TestQ<AStateType, HF> tq;
    tq.xyhf = &hf;
}

int main() {
    // StateXY1 s;
    // XY_HF_Euclidean hf(s);
    // TestQ<StateXY1, XY_HF_Euclidean> tq;

    StateXY1Theta s;
    XYTheta_HF_Euclidean hf(s);
    TestQ<StateXY1, XYTheta_HF_Euclidean> tq;

    tq.xyhf = &hf;
    // TestNode<StateXY1Theta> 
    // tq.insert()

    cout << "TEST" << endl;
}
