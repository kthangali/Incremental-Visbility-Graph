#include "Hvg.h"

//new method of calculating g-value 
//perform scanning logic over node to generate visibility graph and then perform 
//A* search from start to node to find shortest euclidean distance, this distance becomes the g-value of the node 

//write scanning/getVG/shortPathFromVG functions here 
//update expand function to set g-value to be distance of path returned by shortPathFromVG 


//initialize dX and dY to get through up/down/left/right

vector<HVGNode<StateXY>> HVGQueue::getChildren(HVGNode<StateXY>* parentNode, StateXY& parentState, Env<StateXY>* env)
{   
    //pass in action primitive and call ap->getActions
    vector<StateXY> actions = m_ap->getActions(parentState);
    vector<HVGNode<StateXY>> children; 
    for (const StateXY& ac : actions) { 
        Transition<StateXY> t = env->getTransition(ac, parentState);
        if(!t.isValid) {continue;} //skip if transition is invalid 
        HVGNode<StateXY> child = HVGNode<StateXY>(); //create child node for corresponding action 
        child.scan_x = parentNode->scan_x; //copy over scans 
        child.scan_y = parentNode->scan_y;
        scan(child, env); //add any obstacles to previous scans 
        child.vg_nodes = getVG(child); //generate child's vg graph 
        //search over child here using A* function to get its g value 
        //set child's g value 
        children.push_back(child);
    }
    return children;
}

template <class StateXY>
tuple<vector<shared_ptr<Node<StateXY>>>, vector<shared_ptr<Node<StateXY>>> > HVGQueue::expand(double ancFThresh) {
    assert(canExpand(ancFThresh)); // This also calls prepareForExpand()

    QNodeT qn = m_pq.top(); // Get top node  
    m_pq.pop(); // Remove from queue 
    if (m_logger != nullptr) // Save to logger if needed
        m_logger->logExpansion(qn.getStr(), m_name);
    for (DuplicityChecker<StateXY>* dc: m_dc_updates) { // Updates Duplicity Checkers
        dc->updateDuplicity(qn);
    }
    vector<shared_ptr<HVGNode<StateXY>>> expanded = {qn.n}; //set of expanded nodes
    //qn.n is an hvg node 
    vector<HVGNode<StateXY>> children; 
    //children = getChildren(qn.n,qn.n.s, m_ap->env) //how to get environment to be passed in 
    // vector<HVGNode*> expanded = {qn.n};
    // HVGNode* parent = qn.n; 
    return std::make_tuple(children, expanded);
}

void HVGQueue::scan(HVGNode<StateXY> node, Env<StateXY>* e) //modify this to take in a q node? 
{
    set<StateXY> scan_x = node.scan_x;  //set these to be empty sets at first 
    set <StateXY> scan_y = node.scan_y;
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
            int currPose_x = node.s.c[0]; 
            int currPose_y = node.s.c[1];
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
    node.scan_x = scan_x;
    node.scan_y = scan_y;
}



//Input: set of scans in x direction and set of scans in y direction 
//Output: set of stateXY objects present in both, indicating obstacle corner 
set<StateXY> HVGQueue::getVG(HVGNode<StateXY> node)
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