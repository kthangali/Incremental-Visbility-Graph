#pragma once
#include "utils.h"
#include <algorithm>  // For reverse
#include <set> 
#include "State.h"
using namespace std;

/////////////// Node (Old version) /////////////
// Old version: Note that State s is a fixed type which makes it impossible to generalize
// struct Node {
//     Node* parent;
//     double g;
//     double ancH; // May not be necessary
//     double ancF;
//     State s;
//     bool expandedAd;
//     bool expandedInAd;

//     Node(Node* parent, double g, double ancH, double ancF, State s, bool expandedAd, bool expandedInAd):
//         parent(parent), g(g), ancH(ancH), ancF(ancF), s(s), expandedAd(expandedAd), expandedInAd(expandedInAd){};
// };

// Node used in search queue logic
template <class State>
struct Node {
    // Node* parent; // Not using raw pointers
    shared_ptr<Node<State>> parent; // Used to prevent memory leaks
    double g;
    State s;
    
    double ancF;
    bool expandedAd;
    bool expandedInAd;
    Node() {};
    virtual ~Node(){}; //change to a polymorphic type 
    // Node(): {};
    Node(const shared_ptr<Node<State>>& parent, double g, double ancF, State s, bool expandedAd, bool expandedInAd):
        parent(parent), g(g), ancF(ancF), s(s), expandedAd(expandedAd), expandedInAd(expandedInAd){};
};

// template <class State>
struct HVGNode : public Node<StateXY>{
    // public: 
        set<StateXY> scan_x; 
        set<StateXY> scan_y; 
        set<StateXY> vg_nodes;
        
        using Node::g; 
        using Node::s;
        using Node::ancF; 
        using Node::expandedAd;
        using Node::expandedInAd;
        using Node::parent;
        HVGNode(const shared_ptr<Node<StateXY>>& input_parent, double input_g, double ancF, StateXY input_s, bool input_expandedAd, bool input_expandedInAd,
        set<StateXY> scan_x, set<StateXY> scan_y, set<StateXY> vg_nodes):
        scan_x(scan_x), scan_y(scan_y), vg_nodes(vg_nodes){s = input_s; parent = input_parent; g = input_g; expandedAd = input_expandedAd; expandedInAd = input_expandedInAd;};


        // HVGNode(set<StateXY> scan_x, set<StateXY> scan_y, double g, set<StateXY> vg_nodes, StateXY input_s):
        // scan_x(scan_x), scan_y(scan_y), g(g), vg_nodes(vg_nodes){s = input_s;};

};

// Helpful util function
template <class State>
vector<State> getPathFromNode(shared_ptr<Node<State>> n) {
    vector<State> path;
    while (n->parent != nullptr) {
        path.push_back(n->s);
        n = n->parent;
    }
    path.push_back(n->s); // Add initial root node without a parent
    std::reverse(path.begin(),path.end());
    return path;
};