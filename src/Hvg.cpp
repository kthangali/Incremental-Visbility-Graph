#include "Hvg.h"

//new method of calculating g-value 
//perform scanning logic over node to generate visibility graph and then perform 
//A* search from start to node to find shortest euclidean distance, this distance becomes the g-value of the node 

//write scanning/getVG/shortPathFromVG functions here 
//update expand function to set g-value to be distance of path returned by shortPathFromVG 


//initialize dX and dY to get through up/down/left/right

template <class StateXY> 
vector<HVGNode> HVGQueue::getChildren(HVGNode* parentNode, StateXY& parentState, Env<StateXY>* env)
{
    vector<StateXY> actions = getActions(parentState);
    vector<HVGNode> children; 
    for (const StateXY& ac : actions) { 
        HVGNode child = HVGNode();
        child.scan_x = parentNode->scan_x;
        child.scan_y = parentNode->scan_y;
        scan(child, env);
        child.vg_nodes = getVG(child);
        //search over child here using A* function to get its g value
        Transition<StateXY> t = env->getTransition(ac, parentState);
        if(t.isValid)
        {
            children.push_back(child);
        }
    }
    return children;
}

template <class StateXY>
tuple<vector<shared_ptr<Node<StateXY>>>, vector<shared_ptr<Node<StateXY>>> > HVGQueue::expand(double ancFThresh) {
    assert(canExpand(ancFThresh)); // This also calls prepareForExpand()

    HVGNode qn = m_pq.top(); // Get top node
    m_pq.pop(); // Remove from queue
    if (m_logger != nullptr) // Save to logger if needed
        m_logger->logExpansion(qn.getStr(), m_name);
    for (DuplicityChecker<StateXY>* dc: m_dc_updates) { // Updates Duplicity Checkers
        dc->updateDuplicity(qn);
    }
    vector<shared_ptr<NodeT>> expanded = {qn.n};

    // Get transition data and create children
    //can we use this same create children function for HVG implementation 
    // vector<Transition<StateXY> > transitions = m_ap->getSuccessors(qn.n->s);
    // vector<shared_ptr<NodeT>> children;
    // // vector<shared_ptr<HVGNode>> children;
    // for (const Transition<StateXY>& tran : transitions) {
    //     if (!tran.isValid)
    //         continue;
    //     //do scanning logic over child 
    //     //get child's hvg graph and shortest path 
    //     //set g value to be length of shortest path
    //     //then push onto children 
    //     shared_ptr<NodeT> child = make_shared<NodeT>(qn.n, qn.n->g + tran.cost, -1, tran.s, false, false);
    //     children.push_back(child);
    // }
    vector<HVGNode> children = getChildren
    
    return std::make_tuple(children, expanded);
}

void HVGQueue::scan(HVGNode* node, Env<StateXY>* e)
{
    set<StateXY> scan_x = node->scan_x; 
    set <StateXY> scan_y = node->scan_y;
    int dX[4] = {-1,1, 0, 0}; 
    int dY[4] = {0, 0, -1, 1};
    //loop over directions 
    for(int i = 0; i < 4; i++)
    {
        int x_move = dX[i];
        int y_move = dY[i];
        bool obstacle_hit = false; 
        //until we hit an obstacle
        while(obstacle_hit == false)
        {
            //current x and y positions 
            int currPose_x = node->s.c[0]; 
            int currPose_y = node->s.c[1];
            int new_x = currPose_x + x_move;
            int new_y = currPose_y + y_move;
            //create new state XY with new_x and new_y
            StateXY newState = StateXY(new_x, new_y);
            //collision check the new state 
            obstacle_hit = e->isValidState(newState);
            if(obstacle_hit)
            {
                    //add to appropriate list of partial scans 
                    if(y_move == 0)
                    {
                        scan_x.insert(newState);
                    }
                    else
                    {
                        scan_y.insert(newState);
                    }

            }

        }
    }
    //re-copy over modified scans 
    node->scan_x = scan_x;
    node->scan_y = scan_y;
}



//Input: set of scans in x direction and set of scans in y direction 
//Output: set of stateXY objects present in both, indicating obstacle corner 
set<StateXY> HVGQueue::getVG(HVGNode node)
{
    set<StateXY> scans_x = node.scan_x;
    set<StateXY> scans_y = node.scan_y;
    set<StateXY> VG {}; 
    for(auto itr : scans_x) 
    {
        //if the state is in both scans_x and scans_y
        if(scans_y.count(itr) != 0){
            VG.insert(itr);
        }
    }
    return VG; 
}


//run A* search over vg to find shortest path and return its length to be used as g value 
//leave for last
int HVGQueue::shortPathFromVG(set<StateXY> vg, StateXY start, StateXY goal)
{
    //declare adj list
    for(auto node: vg)
    {
        //add to adj list 
        //loop over other vg nodes 
        //check valid edge, add to list if valid 
        //store edge values as distances between nodes 
        //checking validity of edge - Env.getTransition(s1, s2).isValid == true?
    }
    //do A* over adj list to find shortest path from start to goal 

}