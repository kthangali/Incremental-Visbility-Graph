/////////////////////////////////////////////////
//////////////////// Env2D //////////////////////

template <class State>
Env2D<State>::Env2D() {
}

template <class State>
Env2D<State>::~Env2D() {
}

template <class State>
void Env2D<State>::buildFromObstacleMap(const string& filepath) {
    std::FILE *f = fopen(filepath.c_str(), "r");
	if (f) {
    }
    else {
        printf("Opening file failed! \n");
        throw std::runtime_error("Opening map file failed!");
    }

    if (fscanf(f, "type octile\nheight %d\nwidth %d\nmap\n", &m_height, &m_width) != 2) {
        throw runtime_error("Invalid End2D<State>::buildFromObstacleMap parsing map metadata");
    }
    
    ////// Go through file and add to m_occupancy
    m_occupancy.resize(m_height, std::vector<bool>(m_width, false)); // (H,W)

    double cx, cy, cz;
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            char c;
            do {
                if (fscanf(f, "%c", &c) != 1) {
                    throw runtime_error("Invalid End2D<State>::buildFromObstacleMap parsing individual map data");
                }
            } while (isspace(c));
            if (!(c == '.')) { // Anything that is not a "." is an obstacle
                m_occupancy[y][x] = true;
            }
        }
    }
    fclose(f);
}

template <class State>
void Env2D<State>::assertCorrectSettings() {
    assert(m_height > 0 && m_width > 0);
    assert(m_occupancy.size() == m_height);
    assert(m_occupancy[0].size() == m_width);
}

////////////////////////////////////////////////////////////////////////////
//////////////////////////// New Env Functions here ////////////////////////

// Checks if state satisfies bound limits and workspace limits
template <class State>
bool Env2D<State>::isValidState(const State& s) {
    bool valid = isValidPlanningState(s);
    if (valid) { // If state is valid, compute workspace state and check if that is valid
        // Collision checking logic will go here
        StateXY workspaceState = stateToWorkspace(s);
        valid = isValidWorkspaceState(workspaceState);
    }
    return valid;
}

// Returns transition corresponding to s + deltaState
// Note: Collision checking logic goes here
template <class State>
Transition<State> Env2D<State>::getTransition(const State& s, const State& deltaState) {
    int cost = 1;
    State nextS = s + deltaState;
    bool valid = isValidState(nextS);
    return Transition<State>(cost, valid, nextS);
}

// Checks if state satisfies bound limits
// Note: Must be template specified per State
template <class State>
bool Env2D<State>::isValidPlanningState(const State& s) {
    notImplemented("Env2D<State>::isValidPlanningState with current State");
    return false;
}

// Converts (planning) state to workspace state
// Note: Must be template specified per (planning) State
template <class State>
StateXY Env2D<State>::stateToWorkspace(const State& s) {
    notImplemented("Env2D<State>::stateToWorkspace with current State");
    return StateXY(0,0);
}

// Checks if the WorkspaceState is valid (collision free)
template <class State>
bool Env2D<State>::isValidWorkspaceState(const StateXY& s) {
    return s.c[0] >= 0 && s.c[0] < m_width && s.c[1] > 0 && s.c[1] < m_height && 
            !m_occupancy[s.c[1]][s.c[0]];
}
