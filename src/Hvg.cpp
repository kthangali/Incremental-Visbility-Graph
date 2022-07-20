#include "Hvg.h"
// using namespace std;


//constructor (same as Queue's constructor)
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
vector<shared_ptr<HVGNode>> HVGQueue::getChildren(shared_ptr<HVGNode> parentNode, StateXY& parentState, Env<StateXY>* env)
{   
    //pass in action primitive and call ap->getActions
    vector<StateXY> actions = m_ap->getActions(parentState); //generate action space 
    vector<shared_ptr<HVGNode>> children; 
    for (const StateXY& ac : actions) { //loop over delta x and delta y 
        // StateXY newState = StateXY(parentState.c[0] + ac.c[0], parentState.c[1] + ac.c[1]); //create new state with x,y changes
        Transition<StateXY> t = env->getTransition(parentState, ac); //check if transition is valid 
        if(!t.isValid) {continue;} //skip if transition is invalid 
        HVGNode child = HVGNode(parentNode->scan_x, parentNode->scan_y, 0, set<StateXY>(), t.s); //create child node for corresponding action 
        child.scan_x = parentNode->scan_x; //copy over scans 
        child.scan_y = parentNode->scan_y;
        shared_ptr<HVGNode> child_ptr = make_shared<HVGNode>(child); //convert child into pointer
        scan(child_ptr, env); //update scans
        child.vg_nodes = getVG(child); //generate child's vg graph 
        child.vg_nodes.insert(child.s); //insert goal into the graph 
        double g = shortPathFromVG(child.vg_nodes, start, child.s); 
        
        child.g = g;
        //set child's g value 
        children.push_back(child_ptr);
    }
    return children;
}

//overridden expand function 
//not getting called, likely because it doesn't have the exact same header as queue? 
//logic of creating a new HVG node and then re-scanning is suboptimal but casting wasn't working 
// template <class StateXY>
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
    //casting from NodeT to HVGNode
    shared_ptr<HVGNode> qn_HVG;
    //creating new HVGNode with same state as qn_HVG
    StateXY s = qn.n->s;
    // HVGNode temp; = nullptr;
    HVGNode temp = HVGNode(set<StateXY>(), set<StateXY>(), 0, set<StateXY>(), qn.n->s);
    qn_HVG = make_shared<HVGNode>(temp);
    // qn_HVG = dynamic_pointer_cast<HVGNode>(qn.n);
    scan(qn_HVG, m_ap->m_env);

    //call scan on qn_HVG to get its scans 
    vector<shared_ptr<NodeT>> expanded = {qn.n}; //set of expanded nodes

    vector<shared_ptr<HVGNode>> children = getChildren(qn_HVG, qn_HVG->s, m_ap->m_env); 
    //match return type (HVGNodes are Nodes)
    vector<shared_ptr<Node<StateXY>>> dummy_children;
    for(auto c: children)
    {
        dummy_children.push_back(c);
    }
    
    return std::make_tuple(dummy_children, expanded);
}

//scans in x and y directions of node and adds any hit obstacle coordinates to node's scan lists 
void HVGQueue::scan(shared_ptr<HVGNode> node, Env<StateXY>* e)
{
    // HVGNode node_obj = node;
    // set<StateXY> scan_x = node->scan_x;  
    set <StateXY> scan_x = node->scan_x;  //set these to be empty sets at first 
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
        int currPose_x = node->s.c[0]; 
        int currPose_y = node->s.c[1];
        while(obstacle_hit == false)
        {
            //current x and y positions 
            int new_x = currPose_x + x_move;
            int new_y = currPose_y + y_move;
            //create new state XY with new_x and new_y
            StateXY newState = StateXY(new_x, new_y);
            //collision check the new state 
            obstacle_hit = !(e->isValidState(newState)); //should be true if state is not valid
            if(obstacle_hit)
            {
                    //add to appropriate list of partial scans 
                    if(y_move == 0) //if we're moving in the x direction
                    {
                        scan_x.insert(newState);
                    }
                    else
                    {
                        scan_y.insert(newState);
                    }
            }
            currPose_x = new_x;
            currPose_y = new_y;

        }
    }
    //re-copy over modified scans 
    node->scan_x = scan_x;
    node->scan_y = scan_y;
    // node = make_shared<HVGNode>(node_obj);
}


//Input: set of scans in x direction and set of scans in y direction 
//Output: set of stateXY objects present in both, indicating obstacle corner 
set<StateXY> HVGQueue::getVG(HVGNode node)
{
    set<StateXY> scans_x = node.scan_x;
    set<StateXY> scans_y = node.scan_y;
    set<tuple<int,int>> y_coords{}; //create empty set to store just x and y coordinates of y scans 
    for(auto st : scans_y)
    {
        array<int, 2> c = st.c;
        y_coords.insert(std::make_tuple(c[0], c[1]));
    }
    set<StateXY> VG {}; 
    for(auto itr : scans_x)  //itr is a StateXY 
    {
        array<int,2> itr_coords = itr.c;
        tuple<int, int> temp = std::make_tuple(itr_coords[0], itr_coords[1]);
        //if the state is in both scans_x and scans_y
        if(y_coords.count(temp) != 0)
        { //if there's a set of coordinates in both
            VG.insert(itr); //add the corresponding state 
        }
    return VG; 
    }
}


//brute force over vg to find shortest path from start to goal 
double HVGQueue::shortPathFromVG(set<StateXY> vg, StateXY start, StateXY goal)
{
    vg.insert(start); //insert the start node into the vg 
    if(start == goal){return 0;}
    double smallest = INT_MAX;
    // //get valid edges over vg
    int x0 = start.c[0];
    int y0 = start.c[1];
    int x1;
    int y1;
    //get all edges from start and add to hashmap if not already there
    for (auto end : vg)
    {
        //if the path from start to end isn't already in the hashmap
        if(paths.find(end) == paths.end())
        {
            // Transition<StateXY> t = m_ap->m_env->getTransition(start, end);
            bool valid = true;
            for(double k = 0.1; k <= 1; k = k + 0.1)
            {
                int temp_x = min(x0,end.c[0]) + k * (abs(x0 - end.c[0]));
                int temp_y = min(y0, end.c[1]) + k * (abs(y0 - end.c[1]));
                // int temp_x = x0 * k + end.c[0] * (k - 1);
                // int temp_y = y0 * k + end.c[1] * (k - 1);
                if(!m_ap->m_env->isValidState(StateXY(temp_x, temp_y)))
                {
                    valid = false;
                    break;
                }
            }
            if(valid)
            {
                x1 = end.c[0];
                y1 = end.c[1];
                int length = sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
                paths.insert({end,length});
            }
        }
    }
    //check all edges from each vg node to new goal point 
    x1 = goal.c[0];
    y1 = goal.c[1];
    for (auto curr : vg)
    {
        // Transition<StateXY> t = m_ap->m_env->getTransition(curr, goal);
        bool valid = true;
        for(double k = 0.1; k <= 1; k = k + 0.1)
        {
            // int temp_x = curr.c[0] * k + x1 * (k - 1);
            int temp_x = min(curr.c[0], x1) + k * (abs(x1 - curr.c[0]));
            // int temp_y = curr.c[1] * k + y1 * (k - 1);
            int temp_y = min(curr.c[1], y1) + k * (abs(y1 - curr.c[1]));
            if(!m_ap->m_env->isValidState(StateXY(temp_x, temp_y)))
            {
                valid = false;
                break;
            }
        }
        //if there is a valid edge from the node to goal 
        if(valid)
        {
            x0 = curr.c[0];
            y0 = curr.c[1];
            double len = sqrt(pow(x1-x0,2) + pow(y1-y0,2));
            //add length from curr to goal to length of start to curr 
            double temp = paths.at(curr) + len;
            if(temp < smallest)
            {
                smallest = temp;
            }

        }
    }
    return smallest;
}