#include "Queue.h"
#include "State.h"
#include "Node.h"
#include "utils.h"
#include "ActionPrimitives.h"
#include <queue>
#include "DuplicityChecker.h"
#include "HeuristicFunction.h"
#include "SimpleLogger.h"
#include "PriorityFunction.h"
#include "Env.h"
#include <vector>
#include <set>

class HVGQueue : public Queue<StateXY> {
public:
    HVGQueue(const string& qName, SimpleLogger* logger, 
            HF_Template<StateXY>* hf, 
            PriorityFunction* pf, 
            DuplicityChecker<StateXY>* dc_check, 
            vector<DuplicityChecker<StateXY>*> dc_updates,
            AP_Template<StateXY>* ap);


    //new functions
    void scan(shared_ptr<HVGNode> node, Env<StateXY>* e);
    set<StateXY> getVG(HVGNode node);
    int shortPathFromVG(set<StateXY> vg, StateXY start, StateXY goal);
    vector<HVGNode> getChildren(shared_ptr<HVGNode> parentNode, StateXY& parentState, Env<StateXY>* env);
    
    //inherited and modified 
    template<class StateXY> 
    tuple<vector<shared_ptr<Node<StateXY>>>, vector<shared_ptr<Node<StateXY>>> > expand(double ancFThresh);
    // using Queue<StateXY>::m_ap;
    // using Queue<StateXY>::m_name;
    // using Queue<StateXY>::m_logger;
    // using Queue<StateXY>::m_pf;
    using Queue<StateXY>::Queue;
    // using Queue<StateXY>::m_dc_updates;
    string m_name;
    SimpleLogger* m_logger;
    HF_Template<StateXY>* m_hf;
    PriorityFunction* m_pf;
    DuplicityChecker<StateXY>* m_dc_check;
    vector<DuplicityChecker<StateXY>*> m_dc_updates;
    AP_Template<StateXY>* m_ap;
};