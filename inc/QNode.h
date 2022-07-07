#pragma once
#include "Node.h"

////////////// Start QNode ///////////////

// QNode's are placed into Queue's
//   Need this as one Node can be in multiple Q's with different f-values
template <class State>
struct QNode {
    double f;
    shared_ptr<Node<State>> n;

    QNode(double f, shared_ptr<Node<State>>& n): f(f), n(n){};
    string getStr() const {return n->s.getStr();};
};

template <class State>
struct QNodeComparator {
    bool operator()(const QNode<State>& p1, const QNode<State>& p2) {
        return p1.f > p2.f; // Note need > for min queue
    }
};

template <class State>
struct QNodeHasher { // TODO: Make this based on StateHasher
    size_t operator()(const QNode<State>& k) const { // Based on k's underlying state
        return hash<State>{}(k.n->s);
    }
};

template <class State>
struct QNodeEqual { // TODO: Make this based on StateEqual
    bool operator()(const QNode<State>& lhs, const QNode<State>& rhs) const {
        return lhs.n->s == rhs.n->s;
    }
};
////////////// End QNode ///////////////