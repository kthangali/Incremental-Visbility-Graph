#include "Hvg.h"
using namespace std;


//constructor 


HVGQueue::HVGQueue(const string& qName, SimpleLogger* logger, 
            HF_Template<StateXY>* hf, 
            PriorityFunction* pf,
            DuplicityChecker<StateXY>* dc_check, 
            vector<DuplicityChecker<StateXY>*> dc_updates,
            AP_Template<StateXY>* ap):
        Queue(qName, logger, hf, pf, dc_check,
        dc_updates, ap){};
//new method of calculating g-value 
//perform scanning logic over node to generate visibility graph and then perform 
//A* search from start to node to find shortest euclidean distance, this distance becomes the g-value of the node 

//write scanning/getVG/shortPathFromVG functions here 
//update expand function to set g-value to be distance of path returned by shortPathFromVG 


//initialize dX and dY to get through up/down/left/right

//takes in parentNode/state and environment 
//returns all the valid child nodes of parentNode with respective scans
vector<HVGNode> HVGQueue::getChildren(shared_ptr<HVGNode> parentNode, StateXY& parentState, Env<StateXY>* env)
{   
    //pass in action primitive and call ap->getActions
    vector<StateXY> actions = m_ap->getActions(parentState); //generate action space 
    vector<HVGNode> children; 
    for (const StateXY& ac : actions) { 
        Transition<StateXY> t = env->getTransition(ac, parentState);
        if(!t.isValid) {continue;} //skip if transition is invalid 
        HVGNode child = HVGNode(parentNode->scan_x, parentNode->scan_y, 0, set<StateXY>()); //create child node for corresponding action 
        child.scan_x = parentNode->scan_x; //copy over scans 
        child.scan_y = parentNode->scan_y;
        shared_ptr<HVGNode> child_ptr = make_shared<HVGNode>(child); //convert child into pointer
        scan(child_ptr, env); //update scans
        child.vg_nodes = getVG(child); //generate child's vg graph 
        int g = shortPathFromVG(child.vg_nodes, child.s, child.s); //figure out how to access start
        
        child.g = g;
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
    // shared_ptr<HVGNode*> qn_HVG = dynamic_pointer_cast<HVGNode*>(qn.n); //cast qn.n to an HVG Node pointer 
    shared_ptr<HVGNode> qn_HVG = dynamic_pointer_cast<HVGNode>(qn.n);
    scan(qn_HVG, m_ap->m_env);

    //call scan on qn_HVG to get its scans 
    vector<shared_ptr<NodeT>> expanded = {qn.n}; //set of expanded nodes
    // vector<HVGNode*> expanded = {qn.n};
    // vector<HVGNode*> expanded = {qn.n};
    // HVGNode* parent = qn.n; 
    vector<HVGNode> children = getChildren(qn_HVG, qn_HVG->s, m_ap->m_env); 
    
    return std::make_tuple(children, expanded);
}

void HVGQueue::scan(shared_ptr<HVGNode> node, Env<StateXY>* e) //modify this to take in a q node? 
{
    HVGNode node_obj = *node;
    set<StateXY> scan_x = node_obj.scan_x;  //set these to be empty sets at first 
    set <StateXY> scan_y = node_obj.scan_y;
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
            int currPose_x = node_obj.s.c[0]; 
            int currPose_y = node_obj.s.c[1];
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
    node_obj.scan_x = scan_x;
    node_obj.scan_y = scan_y;
    node = make_shared<HVGNode>(node_obj);
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
    return 5;

}