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
#include <unordered_map>    

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
    double shortPathFromVG(set<StateXY> vg, StateXY start, StateXY goal, bool goalFound);
    vector<shared_ptr<HVGNode>> getChildren(shared_ptr<HVGNode> parentNode, StateXY& parentState, Env<StateXY>* env);
    bool collisionCheck(StateXY start, StateXY end);
    bool validityCheck(int x, int y, int startX, int startY, int endX, int endY);
    StateXY start;
    
    //inherited
    // template<class StateXY> 
    // using Queue<StateXY>::expand; 
    tuple<vector<shared_ptr<Node<StateXY>>>, vector<shared_ptr<Node<StateXY>>> > expand(double ancFThresh);

    using Queue<StateXY>::Queue;
    using Queue<StateXY>::assertCorrectSettings;
    using Queue<StateXY>::m_name;
    using Queue<StateXY>::m_logger;
    using Queue<StateXY>::m_hf;
    using Queue<StateXY>::m_pf;
    using Queue<StateXY>::m_dc_check;
    using Queue<StateXY>::m_dc_updates;
    using Queue<StateXY>::m_ap;
    using Queue<StateXY>::updateNodeWithAnchorStats;

    set<StateXY> q_vg; 
    set<StateXY> q_scan_x; 
    set<StateXY> q_scan_y; 
    vector<StateXY> getHVGPath(StateXY goal);


    unordered_map<StateXY, tuple<shared_ptr<StateXY>, double>> paths;
};

