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
        Transition<StateXY> t = m_ap->m_env->getTransition(parentState, ac); //check if transition is valid 
        if(!t.isValid) {continue;} //skip if transition is invalid 
        // HVGNode child = HVGNode(parentNode, 0, -1, t.s, false, false, parentNode->scan_x, parentNode->scan_y, parentNode->vg_nodes); //create child node for corresponding action 
        HVGNode child = HVGNode(parentNode, 0, -1, t.s, false, false, q_scan_x, q_scan_y, q_vg); //create child node for corresponding action 
        shared_ptr<HVGNode> child_ptr = make_shared<HVGNode>(child); //convert child into pointer
        
        //generate new scans
        scan(child_ptr, env); //update scans
        for(auto n : child_ptr->scan_x)
        {
            q_scan_x.insert(n);
        }
        for(auto n : child_ptr->scan_y)
        {
            q_scan_y.insert(n);
        }

        //should keep vg nodes of parent node since we're using previous scans 
        child_ptr->vg_nodes = getVG(child);
        child_ptr->vg_nodes.insert(child.s); //insert goal into the graph 
        double g = shortPathFromVG(child.vg_nodes, start, child.s, false); 
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
    //casting from NodeT to HVGNode
    shared_ptr<HVGNode> qn_HVG;
    //creating new HVGNode with same state as qn_HVG

    HVGNode temp = HVGNode(qn.n->parent, 0, -1, qn.n->s, false, false, q_scan_x, q_scan_y,set<StateXY>());
    qn_HVG = make_shared<HVGNode>(temp); 
    // qn_HVG->s = StateXY(qn.n->s.c[0], qn.n->s.c[1]);

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
    qn_HVG->vg_nodes = getVG(*qn_HVG); 
    for (auto vg_node : qn_HVG->vg_nodes)
    {
        q_vg.insert(vg_node);
    }

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
set<StateXY> HVGQueue::getVG(HVGNode node)
{
    set<StateXY> scans_x = q_scan_x;
    set<StateXY> scans_y = q_scan_y;
    set<tuple<int,int>> y_coords{}; //create empty set to store just x and y coordinates of y scans 
    for(auto itr_x : scans_x)  //itr is a StateXY 
    {
        for(auto itr_y : scans_y)
        {
            if(itr_y.c[0] == itr_x.c[0] && itr_y.c[1] == itr_x.c[1])
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
    //checking for / adding new edges 
    for (auto end : new_nodes) //loop over all new nodes
    {
        smallest = INT_MAX;
        p = nullptr;
        for(auto s : old_nodes)
        {
            //check validity of edge from s to end 
            if(s == end){continue;}
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
    if(in_vg && goalFound == false)
    {
        paths.erase(goal);
    }
    bool t = collisionCheck(StateXY(21,9), StateXY(4,9));
    return smallest;
}

bool HVGQueue::validityCheck(int x, int y, int startX, int startY, int endX, int endY)
{
        int temp_x = x;
        int temp_y = y; 
        if(!m_ap->m_env->isValidState(StateXY(temp_x, temp_y)))
        {
            if(startX <= temp_x && temp_x <= endX && 
                min(startY, endY) <= temp_y && 
                temp_y <= max(startY, endY)
                && make_tuple(startX, startY) != make_tuple(temp_x, temp_y)
                && make_tuple(endX, endY) != make_tuple(temp_x, temp_y))
                {
                    return false;
                }
        }
        return true;
}

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
        for(double k = min(startX, endX) + 1; k < max(startX, endX); k++)
        {
            temp_x = k;
            temp_y = round(min(startY, endY) + k * (abs(startY - endY)));
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
        valid = true;
        //put in terms of left to right 
        if(endX < startX) 
        {
            int tempStart = startX;
            int tempEnd = startY;
            startX = endX;
            startY = endY;
            endX = tempStart;
            endY = tempEnd;
        }
        double m = abs(float(endY - startY) / float(endX - startX));
        for(double k = 0.1; k <= 1; k += 0.1)
        {
            temp_x = startX + k * abs(endX - startX);
            if(endY < startY) //positive slope
            {
                temp_y = round(startY - (temp_x - startX) * m);
            }
            else //negative slope
            {
                temp_y = round(startY + (temp_x - startX) * m);
            }
            if(temp_x - int(temp_x) == 0.5)
            {
                if(!validityCheck(round(temp_x), temp_y, startX, startY, endX, endY) && 
                !validityCheck(int(temp_x), temp_y, startX, startY, endX, endY))
                {
                    valid = false;
                    break;
                }
            }
            else
            {
                if(!validityCheck(round(temp_x), temp_y, startX, startY, endX, endY))
                {
                    return false;
                }
            }
        }
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