// #include "HeuristicFunction.h"

//////////////////// Template Class //////////////////////
// Template getBatchHeuristic()
template <class State>
vector<double> HF_Template<State>::getBatchHeuristic(const vector<State>& states) {
    vector<double> ans;
    ans.reserve(states.size());
    for (const State& s: states) {
        ans.push_back(getHeuristic(s));
    }
    return ans;
}

// Zero Heuristic
template <class State>
double HF_Zero<State>::getHeuristic(const State& s) {
    return 0;
}

// Manhattan Heuristic
// template <class State>
// double HF_Manhattan<State>::getHeuristic(const State& s) {
//     double ans = 0;
//     for (size_t i = 0; i < s.c.size(); ++i) { // Go through all coordinates in state
//         ans += abs(s.c[i] - m_goalState.c[i]);
//     }
//     return ans;
// }
template <class State, class GoalStateClass>
double HF_Manhattan<State, GoalStateClass>::getHeuristic(const State& s) {
    double ans = 0;
    GoalStateClass curState = m_env->stateToWorkspace(s);
    for (size_t i = 0; i < curState.c.size(); ++i) { // Go through all coordinates in state
        ans += abs(curState.c[i] - m_goal.c[i]);
    }
    return ans;
}

// // SubManhattan Heuristic
// template <class State>
// double HF_SubManhattan<State>::getHeuristic(const State& s) {
//     return abs(s.c[index] - m_goalState.c[index]);
// }

// // Euclidean Heuristic
// template <class State>
// double HF_Euclidian<State>::getHeuristic(const State& s) {
//     double ans = 0;
//     for (size_t i = 0; i < s.c.size(); ++i) { // Go through all coordinates in state
//         ans += pow(s.c[i] - m_goalState.c[i], 2);
//     }
//     return sqrt(ans);
// }

// // Workspace
// template <class State>
// double HF_ManhattanWorkspace<State>::getHeuristic(const State& s) {
//     double ans = 0;
//     StateXY curWState = m_env->stateToWorkspace(s);
//     for (size_t i = 0; i < curWState.c.size(); ++i) { // Go through all coordinates in state
//         ans += abs(curWState.c[i] - m_workspaceGoalState.c[i]);
//     }
//     return ans;
// }


// //////////////////// Main Abstract Class //////////////////////
// double HeuristicFunction::getHeuristic(const State& s) {
//     throw runtime_error("HeuristicFunction::getHeuristic: Not defined");
// }

// vector<double> HeuristicFunction::getBatchHeuristic(vector<State>& states) {
//     vector<double> ans;
//     ans.reserve(states.size());
//     for (const State& s: states) {
//         ans.push_back(getHeuristic(s));
//     }
//     return ans;
// }

// //////////////////// Zero Heuristic //////////////////////
// double HF_Zero::getHeuristic(const State& s) {
//     return 0;
// }

// //////////////////// Manhattan Heuristic //////////////////////
// double HF_Manhattan::getHeuristic(const State& s) {
//     return abs(s.c[0] - m_goalState.c[0]) + abs(s.c[1] - m_goalState.c[1]);
// }

// //////////////////// SubManhattan Heuristic //////////////////////
// double HF_SubManhattan::getHeuristic(const State& s) {
//     return abs(s.c[index] - m_goalState.c[index]);
// }

// //////////////////// Euclidean Heuristic //////////////////////
// double HF_Euclidian::getHeuristic(const State& s) {
//     return sqrt(pow(s.c[0] - m_goalState.c[0], 2) + pow(s.c[1] - m_goalState.c[1], 2));
// }


//////////////////////////////////////////////////////////////////
//////////////////// NN Heuristic //////////////////////

// // Constructs 
// HF_Batch::HF_Batch(const State& goalState): HeuristicFunction(goalState), m_countTotal(0), m_countBatches(0) {
//     m_hf = new HF_Manhattan(goalState);
// }

// // A very naive batch version
// vector<double> HF_Batch::getBatchHeuristic(vector<State>& states) {
//     vector<double> ans;
//     ans.reserve(states.size());
//     for (const State& s: states) {
//         ans.push_back(m_hf->getHeuristic(s));
//         ++m_countTotal;
//     }
//     ++m_countBatches;
//     return ans;
// }

