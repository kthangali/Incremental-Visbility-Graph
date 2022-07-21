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



//takes in parentNode/state and environment 
//returns all the valid child nodes of parentNode with respective scans
vector<shared_ptr<HVGNode>> HVGQueue::getChildren(shared_ptr<HVGNode> parentNode, StateXY& parentState, Env<StateXY>* env)
{   
    vector<StateXY> actions = m_ap->getActions(parentState); //generate action space 
    vector<shared_ptr<HVGNode>> children; 
    for (const StateXY& ac : actions) { //loop over action space
        Transition<StateXY> t = m_ap->m_env->getTransition(qn.n->s, ac); //check if transition is valid 
        if(!t.isValid) {continue;} //skip if transition is invalid 
        HVGNode child = HVGNode(parentNode->scan_x, parentNode->scan_y, 0, set<StateXY>(), t.s); //create child node for corresponding action 
        shared_ptr<HVGNode> child_ptr = make_shared<HVGNode>(child); //convert child into pointer

        //copy over parent scans and generate new scans
        child_ptr->scan_x = parentNode->scan_x;
        child_ptr->scan_y = parentNode->scan_y;
        scan(child_ptr, env); //update scans
    
        child_ptr->vg_nodes = getVG(child);
        child_ptr->vg_nodes.insert(child.s); //insert goal into the graph 
        double g = shortPathFromVG(child.vg_nodes, start, child.s); 
        child_ptr->g = g;
        //set child's g value 
        children.push_back(child_ptr);
    }
    return children;
}

//overridden expand function 
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
    temp.s = qn.n->s;
    qn_HVG = make_shared<HVGNode>(temp);
    qn_HVG->s = StateXY(qn.n->s.c[0], qn.n->s.c[1]);
    // qn_HVG = dynamic_pointer_cast<HVGNode>(qn.n);
    scan(qn_HVG, m_ap->m_env);

    //call scan on qn_HVG to get its scans 
    vector<shared_ptr<NodeT>> expanded = {qn.n}; //set of expanded nodes

    vector<shared_ptr<HVGNode>> children = getChildren(qn_HVG, qn_HVG->s, qn, m_ap->m_env); 
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
    vg.insert(start); //insert the start node into the vg (does nothing if it's there already)
    if(start == goal){return 0;} //start is already goal, no path 
    double smallest = INT_MAX;
    // //get valid edges over vg
    int x0;
    int y0;
    int x1;
    int y1;
    set<StateXY> new_nodes;
    set<StateXY> old_nodes;
    for (auto node : vg)
    {
        if(paths.find(node) == paths.end()) //node is new
        {
            new_nodes.insert(node);
        }
        else
        {
            old_nodes.insert(node);
        }
    }
    
    for (auto end : new_nodes) //loop over all new nodes
    {
        for(auto s : old_nodes)
        {
            //check validity of edge from s to end 
            bool valid = true;
            for(double k = 0.1; k <= 1; k = k + 0.1)
            {
                x0 = s.c[0];
                y0 = s.c[1];
                int temp_x = min(x0,end.c[0]) + k * (abs(x0 - end.c[0]));
                int temp_y = min(y0, end.c[1]) + k * (abs(y0 - end.c[1]));
                if(!m_ap->m_env->isValidState(StateXY(temp_x, temp_y)))
                {
                    valid = false;
                    break;
                }
            }
            //if edge is valid, check if length from start to s + length from s to end is smallest so far 
            if(valid)
            {
                x1 = end.c[0];
                y1 = end.c[1];
                int length = sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
                if(length + paths.at(s) < smallest)
                {
                    smallest = length;
                }
            }
        }
        paths.insert({end, smallest});
    }
    
    //check all edges from each vg node to new goal point 
    x1 = goal.c[0];
    y1 = goal.c[1];
    for (auto curr : vg)
    {
        bool valid = true;
        for(double k = 0.1; k <= 1; k = k + 0.1)
        {
            int temp_x = min(curr.c[0], x1) + k * (abs(x1 - curr.c[0]));
            int temp_y = min(curr.c[1], y1) + k * (abs(y1 - curr.c[1]));
            if(!m_ap->m_env->isValidState(StateXY(temp_x, temp_y)))
            {
                valid = false;
                break;
            }
        }
        //think we also need to check edges between nodes aside from start 
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