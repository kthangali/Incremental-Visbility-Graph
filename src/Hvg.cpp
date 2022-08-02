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
    for (const StateXY& ac : actions) { 
        Transition<StateXY> t = m_ap->m_env->getTransition(parentState, ac); //check if transition is valid 
        if(!t.isValid) {continue;} //skip if transition is invalid 
        HVGNode child = HVGNode(parentNode, 0, -1, t.s, false, false, q_scan_x, q_scan_y, q_vg); //create child node for corresponding action 
        shared_ptr<HVGNode> child_ptr = make_shared<HVGNode>(child); //convert child into pointer
        
        //generate new scans
        scan(child_ptr, env); //collect any new nodes
        //add all new nodes to overall scans
        for(auto n : child_ptr->scan_x)
        {
            q_scan_x.insert(n);
        }
        for(auto n : child_ptr->scan_y)
        {
            q_scan_y.insert(n);
        }

        //get new visibility graph with updated scans 
        child_ptr->vg_nodes = getVG();
        child_ptr->vg_nodes.insert(child.s); //insert goal into the graph 
        
        //get g value 
        child_ptr->g = shortPathFromVG(child.vg_nodes, start, child.s, false);

        children.push_back(child_ptr);
    }
    return children;
}

// overridden expand function 
tuple<vector<shared_ptr<Node<StateXY>>>, vector<shared_ptr<Node<StateXY>>> > HVGQueue::expand(double ancFThresh) {
    assert(canExpand(ancFThresh)); // This also calls prepareForExpand()

    QNodeT qn = m_pq.top(); // Get top node  
    m_pq.pop(); // Remove from queue 
    if (m_logger != nullptr) // Save to logger if needed
        m_logger->logExpansion(qn.getStr(), m_name);
    for (DuplicityChecker<StateXY>* dc: m_dc_updates) { // Updates Duplicity Checkers
        dc->updateDuplicity(qn);
    }
    //casting from NodeT to HVGNode
    shared_ptr<HVGNode> qn_HVG;
    //creating new HVGNode with same state as qn_HVG

    HVGNode temp = HVGNode(qn.n->parent, 0, -1, qn.n->s, false, false, q_scan_x, q_scan_y, q_vg);
    //debugging
    expanded_states.insert(temp.s);
    qn_HVG = make_shared<HVGNode>(temp); 

    scan(qn_HVG, m_ap->m_env);
    //add any new scan nodes to overall scan
    for(auto x : qn_HVG->scan_x)
    {
        q_scan_x.insert(x);
    }
    for(auto y : qn_HVG->scan_y)
    {
        q_scan_y.insert(y);
    }

    //add any new vg nodes to overall vg 
    qn_HVG->vg_nodes = getVG(); 
    for (auto vg_node : qn_HVG->vg_nodes)
    {
        q_vg.insert(vg_node);
    }

    //call scan on qn_HVG to get its scans 
    vector<shared_ptr<NodeT>> expanded = {qn.n}; //set of expanded nodes

    vector<shared_ptr<HVGNode>> children = getChildren(qn_HVG, qn_HVG->s, m_ap->m_env); 

    //put into dummy vector to match return type 
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
    set <StateXY> scan_x = node->scan_x; 
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
set<StateXY> HVGQueue::getVG()
{
    //q_scan_x and q_scan_y contain all the previous scans + scans from node
    set<StateXY> scans_x = q_scan_x;
    set<StateXY> scans_y = q_scan_y;
    //can just loop over x because w only care about what's in both of the scans
    for(auto itr_x : scans_x)  //itr is a StateXY 
    {
        for(auto itr_y : scans_y)
        {
            if(itr_x == itr_y)
            {
                q_vg.insert(itr_y);
            }
        }
    }
    return q_vg; 
}


//brute force over vg to find shortest path from start to goal 
double HVGQueue::shortPathFromVG(set<StateXY> vg_temp, StateXY start, StateXY goal, bool goalFound)
{
    set<StateXY> vg = vg_temp;
    vg.insert(start); //insert the start node into the vg (does nothing if it's there already)
    vg.insert(goal);
    if(start == goal){return 0;} //start is already goal, no path 
    double smallest;
    double length;
    shared_ptr<StateXY> p;
    // get valid edges over vg
    int x0, y0, x1, y1;
    set<StateXY> new_nodes;
    set<StateXY> old_nodes;
    bool valid;

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
    old_nodes.insert(start); //we always want the path from start 
    paths.insert({start, make_tuple(nullptr, 0)}); //length from start to start is 0
    //adding any new valid edges from old nodes to new nodes 
    for (auto end : new_nodes) 
    {
        smallest = INT_MAX;
        p = nullptr;
        for(auto s : old_nodes)
        {
            //check validity of edge from s to end 
            valid = collisionCheck(s, end);
            if(valid)
            {
                x1 = end.c[0];
                y1 = end.c[1];
                x0 = s.c[0];
                y0 = s.c[1];
                int in = pow(x1 - x0, 2) + pow(y1 - y0, 2);
                double sq = sqrt(in);
                length = sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
                if(length + get<1>(paths.at(s)) < smallest)
                {
                    smallest = length + get<1>(paths.at(s));
                    p = make_shared<StateXY>(s);

                }
            }
        }
        paths.insert({end, make_tuple(p, smallest)});
    }
    
    //check all edges from each vg node to new goal point 
    //finding shortest path to goal 
    x1 = goal.c[0];
    y1 = goal.c[1];
    smallest = INT_MAX;
    // p = nullptr;
    for (auto curr : vg)
    {
        valid = collisionCheck(curr, goal);
        if(valid)
        {
            x0 = curr.c[0];
            y0 = curr.c[1];
            length = sqrt(pow(x1-x0,2) + pow(y1-y0,2));
            //add length from curr to goal to length of start to curr 
            double temp = get<1>(paths.at(curr)) + length;
            if(temp < smallest)
            {
                smallest = temp;
                // p = make_shared<StateXY>(curr);
            }

        }
    }
    bool in_vg = false;
    for(auto node : vg)
    {
        if(node.c[0] == goal.c[0] && node.c[1] == goal.c[1])
        {
            in_vg = true;
        }
    }
    //remove all edges to new node from paths, unless its a vg node 
    if(in_vg && goalFound == false)
    {
        paths.erase(goal);
    }
    bool c = checkAngledEdge(1, 6, 8, 2);
    return smallest;
}


//returns true if (x,y) is valid on the edge from (startX, startY) to (endX, endY)
bool HVGQueue::validityCheck(int x, int y, int startX, int startY, int endX, int endY)
{
        int temp_x = x;
        int temp_y = y; 
        if(!m_ap->m_env->isValidState(StateXY(temp_x, temp_y)))
        {
            //if (x,y) is within bounds and not equal to the start or end states 
            if(make_tuple(startX, startY) != make_tuple(temp_x, temp_y)
                && make_tuple(endX, endY) != make_tuple(temp_x, temp_y))
                {
                    return false;
                }
        }
        return true;
}

//returns true if the edge from (startX, startY) to (endX, endY) is an edge along
// the side of an obstacle
bool HVGQueue::isObstacleSide(int startX, int startY, int endX, int endY)
{
    if(startX == endX) //vertical
    {
        for(int k = min(startY, endY); k <= max(startY, endY); k++)
        {
            if(m_ap->m_env->isValidState(StateXY(startX, k))) //some valid state in between
            {
                return false;
            }
        }
        //startY to endY is all invalid 
        //check left side
        for(int k = min(startY, endY); k <= max(startY, endY); k++)
        {
            if(!m_ap->m_env->isValidState(StateXY(startX - 1, k)))
            {
                //check the right side
                for(int k = min(startY, endY); k <= max(startY, endY); k++)
                {
                    if(!m_ap->m_env->isValidState(StateXY(startX + 1, k)))
                    {
                        return false;
                    }
                }
                return true;
            }
        }
        return true;
    }
    else
    {
        for(int k = min(startX, endX); k <= max(startX, endX); k++)
        {
            if(m_ap->m_env->isValidState(StateXY(k, startY))) //some valid state in between
            {
                return false;
            }
        }
        //check down
        for(int k = min(startX, endX); k <= max(startX, endX); k++)
        {
            if(!m_ap->m_env->isValidState(StateXY(k , startY + 1)))
            {
                //check the up side
                for(int k = min(startX, endX); k <= max(startX, endX); k++)
                {
                    if(!m_ap->m_env->isValidState(StateXY(k, startY - 1)))
                    {
                        return false;
                    }
                }
                return true;
            }
        }
        return true;

    }
    return true;
}

bool HVGQueue::checkAngledEdge(int startX, int startY, int endX, int endY)
{
    double temp_x;
    double temp_y;
    double dX;
    double dY;
    double x_dirs[4] = {-0.5, -0.5, 0.5, 0.5};
    double y_dirs[4] = {0.5, -0.5, 0.5, -0.5};
    bool valid = true;

    for(int i = 0; i < 4; i++)
    {
        dX = x_dirs[i];
        dY = y_dirs[i];
        valid = true;
        for(double k = 0.025; k < 1; k += 0.025)
        {
            temp_x = (startX + dX) * (k) + (endX + dX) * (1-k); 
            temp_y = (startY + dY) * (k) + (endY + dY) * (1-k);
            if(temp_x - int(temp_x) == 0.5 && temp_y - int(temp_y) == 0.5)
            {
                continue;
            }
            else
            {
                if(!validityCheck(round(temp_x), round(temp_y), startX, startY, endX, endY))
                {
                    valid = false;
                    break;
                }
            } 
        }
        if(valid == true) {return true;}
    }
    //none of the edges returned true
    return false;
}

bool HVGQueue::collisionCheck(StateXY start, StateXY end)
{
    //if x of start and goal is the same or y of start and goal is the same then 
    //we're checking a straight edge - check both sides 
    //otherwise collision check normally 
    int startX = start.c[0];
    int startY = start.c[1];
    int endX = end.c[0];
    int endY = end.c[1];
    double temp_x;
    double temp_y;
    bool valid;
    if(startX == endX) //vertical edge, try rounding left and right
    {
        valid = true; 
        //check current column
        temp_x = startX;
        if(isObstacleSide(startX, startY, endX, endY))
        {
            return true;
        }
        for(double k = min(startY, endY) + 1; k < max(startY, endY); k++)
        {
            temp_y = k;
            if(!m_ap->m_env->isValidState(StateXY(temp_x, temp_y)))
            {
                valid = false;
                break;
            }    
        }
    }

    else if(startY == endY) //horizontal edge, check up and down
    {
        valid = true; 
        //check the same row
        temp_y = startY;
        if(isObstacleSide(startX, startY, endX, endY))
        {
            return true;
        }
        for(double k = min(startX, endX) + 1; k < max(startX, endX); k++)
        {
            temp_x = k;
            if(!m_ap->m_env->isValidState(StateXY(temp_x, temp_y)))
            {
                valid = false;
                break;
            }    
        }
    }
    else
    {
        valid = checkAngledEdge(startX, startY, endX, endY);
    }
    return valid;
    
}

vector<StateXY> HVGQueue::getHVGPath(StateXY goal)
    {
        vector<StateXY> path; 
        path.push_back(goal);
        StateXY t = goal;
        shared_ptr<StateXY> temp = get<0>(paths.at(goal));
        while(get<0>(paths.at(goal)) != nullptr)
        {
            path.push_back(*get<0>(paths.at(goal)));
            goal = *get<0>(paths.at(goal));
        }
        
        reverse(path.begin(), path.end());
        return path; 

    }