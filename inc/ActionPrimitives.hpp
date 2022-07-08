
// Returns a list of transitions
// First getActions(s) and then collision checks with env to get validity and cost
template <class State>
vector<Transition<State>> AP_Template<State>::getSuccessors(State& s) {
    vector<State> actions = getActions(s);
    vector<Transition<State>> children;
    for (const State& ac : actions) {
        children.push_back(m_env->getTransition(s, ac));
    }
    return children;
}




//////////////////////////////////////////////////////////////
///// Single dimension action primitives class ///////////////

// Single dimension action primitives
template <class State>
AP_SingleDim<State>::AP_SingleDim(Env<State>* env, int stepSize):
                AP_Template<State>(env), m_stepSize(stepSize) {
    State s;
    for (int i = 0; i < s.c.size(); ++i) {
        s.c[i] = m_stepSize; // Moving in + direction
        m_actions.push_back(s);
        s.c[i] = -m_stepSize; // Moving in - direction
        m_actions.push_back(s);
        s.c[i] = 0;
    }
}

// Returns fixed action set
template <class State>
vector<State> AP_SingleDim<State>::getActions(const State& s) {
    return m_actions;
}


