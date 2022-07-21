//////////////////////////////////////////////////////////////////
///////////////////////// Main Queue Abstract Class Below ////////////////
//////////////////////////////////////////////////////////////////
template <class State>
void Search<State>::assertStartCorrectly() {
    // for (DuplicityChecker* dc: m_dcs) {
    //     dc->assertCorrectSettings();
    // }
    assert(m_qs.size() > 0);
    for (Queue<State>*& q: m_qs) {
        q->assertCorrectSettings();
    }

    assert(m_fthresh == 0);
}

// Example runSearch()
template <class State>
tuple<string, vector<State>, double> Search<State>::runSearch(State& startState) {
    assertStartCorrectly();
    shared_ptr<NodeT> startNode = make_shared<NodeT>(nullptr, 0, 0, startState, false, false);
    m_qs[0]->updateNodeWithAnchorStats(startNode);
    m_qs[0]->insert(startNode);

    Queue<State>* q;
    while (m_qs[0]->canExpand(DOUBLEMAX)) {
        q = chooseQueue();
        vector<shared_ptr<NodeT>> children, expanded;
        tie(children, expanded) = q->expand(m_fthresh);

        // For all expanded nodes
        for (const shared_ptr<NodeT> &n : expanded) {
            // Check goal condition
            if (isGoal(n->s)) {
                m_solution_path = getPathFromNode(n);
                m_solution_cost = n->g;
                m_solution_status = "Found solution";
                return make_tuple(m_solution_status, m_solution_path, m_solution_cost);
            }
        }

        // For all children nodes
        //children is empty at some point
        for(shared_ptr<NodeT> &n : children) {
            // Update all children with anchor stats
            m_qs[0]->updateNodeWithAnchorStats(n);

            // Insert child nodes into all queues
            for (Queue<State>*& q : m_qs) {
                q->insert(n);
                // Note: Inserting expanded nodes into duplicity checkers handled in Queues
            }
        }

        // Terminate early if needed
        if (terminateSearch()) {
            m_solution_path = vector<State>();
            m_solution_cost = -1;
            m_solution_status = "Reached time/expansion limit";
            return make_tuple(m_solution_status, m_solution_path, m_solution_cost);
        }
    }
    m_solution_path = vector<State>();
    m_solution_cost = -1;
    m_solution_status = "Emptied anchor";
    return make_tuple(m_solution_status, m_solution_path, m_solution_cost);
}


//////////////////////////////////////////////////////////////////
//////////////////// BasicAStartSearch Below /////////////////////
//////////////////////////////////////////////////////////////////
template <class State>
BasicAStarSearch<State>::BasicAStarSearch(): Search<State>(),
        m_expansionLimit(0), m_timeLimit(0), m_stimer(nullptr), m_env2D(nullptr) {

}
template <class State>
BasicAStarSearch<State>::~BasicAStarSearch() {

}

template <class State>
Queue<State>* BasicAStarSearch<State>::chooseQueue() {
    m_fthresh = DOUBLEMAX;
    return m_qs[0];
}

template <class State>
void BasicAStarSearch<State>::setGoal(const State& s) {
    m_goalState = s;
}

template <class State>
void BasicAStarSearch<State>::setWorkspaceGoal(const StateXY& s, Env2D<State>* env2D) {
    m_goalWorkspaceState = s;
    m_env2D = env2D;
}

template <class State>
tuple<string, vector<State>, double> BasicAStarSearch<State>::runSearch(State& startState) {
    m_numExpanded = 0;
    m_stimer->start("SearchLoop");
    return Search<State>::runSearch(startState);
}

// Side effects: Increments m_numExpanded, so assumes this is only called once in the search loop!
template <class State>
bool BasicAStarSearch<State>::isGoal(State& s) {
    m_numExpanded++; // Assumes this is only called once in the search loop!
    if (m_env2D) { // If env is set, check workspace goal
        return m_goalWorkspaceState == m_env2D->stateToWorkspace(s);
    }
    return m_goalState == s; // Else check search-space goals
}

template <class State>
bool BasicAStarSearch<State>::terminateSearch() {
    m_stimer->stop("SearchLoop");
    if (m_stimer->getTotalTime("SearchLoop") >= m_timeLimit || 
            m_numExpanded >= m_expansionLimit) {
        cout << "Num nodes expanded: " << m_numExpanded << endl;
        return true;
    }
    m_stimer->start("SearchLoop");
    return false;
}

template <class State>
void BasicAStarSearch<State>::assertStartCorrectly() {
    Search<State>::assertStartCorrectly();
    assert(m_stimer != nullptr);
    assert(m_expansionLimit > 0);
    assert(m_timeLimit > 0);
}

template <class State>
void BasicAStarSearch<State>::reportStats(const string& fileName, int seed) {
    cout << "Num expanded: " << m_numExpanded << endl;
    m_stimer->printAllStats();

    string header, stats;
    header = "seed,solution_status,solution_cost,run_time,nodes_expanded,";
    stats = to_string(seed) + "," + m_solution_status + ","
        + to_string(m_solution_cost) + "," + to_string(m_stimer->getTotalTime("SearchLoop")) + ","
        + to_string(m_numExpanded) + ",";

    for (Queue<State>* a_q : m_qs) {
        string qHeader, qStats;
        tie(qHeader, qStats) = a_q->reportStats();
        header += qHeader;
        stats += qStats;
    }


    std::ifstream infile(fileName);
    bool exist = infile.good();
    infile.close();
    if (!exist) { // If it doesn't exist, create header
        std::ofstream addHeads(fileName);
        addHeads << header << endl;
        addHeads.close();
    }

    // Append stats line
    std::ofstream addStats(fileName, std::ios::app);
    addStats << stats << endl;
    addStats.close();
}


//////////////////////////////////////////////////////////////////
///////////////////////// Focal Search Below ////////////////////////////
//////////////////////////////////////////////////////////////////

template <class State>
FocalSearch<State>::FocalSearch(): BasicAStarSearch<State>() {

}

template <class State>
FocalSearch<State>::~FocalSearch() {

}

template <class State>
Queue<State>* FocalSearch<State>::chooseQueue() {
    m_fthresh = max(m_fthresh, m_qs[0]->getTop()->ancF * m_wFocal); /// TODO: Check this logic with Max
    if (m_qs[1]->canExpand(m_fthresh)) {
        return m_qs[1];
    }
    return m_qs[0];
}

template <class State>
void FocalSearch<State>::assertStartCorrectly() {
    assert(m_wFocal >= 1);
    BasicAStarSearch<State>::assertStartCorrectly();
}
