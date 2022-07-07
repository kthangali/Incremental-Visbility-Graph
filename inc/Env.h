#pragma once
#include "utils.h"
#include "State.h"

////////// Start Main Abstract Class Env /////

template <class State>
struct Transition {
    double cost;
    bool isValid;
    State s;

    Transition(double cost, bool isValid, State s): cost(cost), isValid(isValid), s(s) {};
};

// Env interface class dictating how other classes will use Env
template <class State>
class Env {
public:
    // virtual vector<Transition<State>> getSuccessors(State& s) = 0;
    virtual void assertCorrectSettings() = 0; // Checks if all settings are correct
    virtual Transition<State> getTransition(const State& s, const State& deltaState) = 0;
    virtual bool isValidState(const State& s) = 0; // Requires checking both internal and external constraints
};

////////// End Main Abstract Class Env /////


//////////////////////////////////
////////// Start Env_2D //////////

template <class State>
class Env2D: public Env<State> {
public:
    Env2D();
    ~Env2D();

    // Required by inheritance
    void assertCorrectSettings();
    Transition<State> getTransition(const State& s, const State& deltaState);
    bool isValidState(const State& s); // Checks bounds and collisions

    // Initialization
    void buildFromObstacleMap(const string& filepath);

    // Collision checking function
    bool isValidWorkspaceState(const StateXY& workspaceState);
    
    // Functions required to be template specified per class
    bool isValidPlanningState(const State& s); // Bound checks
    StateXY stateToWorkspace(const State& s);
    
    //// Variables
    int m_height, m_width;
    vector<vector<bool>> m_occupancy;

    //// Extra stuff here
    State m_lowerbound;
    State m_upperbound;
};
////////// End Env_2D /////


// Ensures that the corresponding functions are defined per State:
// isValidPlanningState, stateToWorkspace, isValidWorkspaceState
template <class State>
void checkEnv2DDefinedProperly() {
    Env2D<State> env;
    State s;
    env.isValidPlanningState(s); // Checking isValidPlanningState exists
    StateXY workspaceS = env.stateToWorkspace(s); // Checking stateToWorkspace exists
    env.isValidWorkspaceState(workspaceS); // Checking isValidWorkspaceState exists
}



//////// StateXY

template <>
inline bool Env2D<StateXY>::isValidPlanningState(const StateXY& s) {
    return true;
}

template <>
inline StateXY Env2D<StateXY>::stateToWorkspace(const StateXY& s) {
    return s;
}

//////// StateXYTheta

template <>
inline bool Env2D<StateXYTheta>::isValidPlanningState(const StateXYTheta& s) {
    return true;
}

template <>
inline StateXY Env2D<StateXYTheta>::stateToWorkspace(const StateXYTheta& s) {
    return StateXY(s.c[0], s.c[1]);
}

//////// StateXYTime

template <>
inline bool Env2D<StateXYTime>::isValidPlanningState(const StateXYTime& s) {
    return true;
}

template <>
inline StateXY Env2D<StateXYTime>::stateToWorkspace(const StateXYTime& s) {
    return StateXY(s.c[0], s.c[1]);
}

#include "Env.hpp"