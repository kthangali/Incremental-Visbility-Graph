#include "Queue.h"
#include "State.h"
#include "Node.h"
#include "utils.h"
#include "ActionPrimitives.h"
#include <vector>
#include <set>

class HVGQueue : public Queue<StateXY> {
public:


    HVGQueue();
    //new functions
    void scan(HVGNode node, Env<StateXY>* e);
    set<StateXY> getVG(HVGNode node);
    int shortPathFromVG(set<StateXY> vg, StateXY start, StateXY goal);
    vector<HVGNode> getChildren(HVGNode* parentNode, StateXY& parentState, Env<StateXY>* env);


    //inherited and modified 
    template<class StateXY> 
    tuple<vector<shared_ptr<Node<StateXY>>>, vector<shared_ptr<Node<StateXY>>> > expand(double ancFThresh);
    using Queue<StateXY>::m_ap;
};