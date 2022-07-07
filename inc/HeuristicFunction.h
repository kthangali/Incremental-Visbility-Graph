#pragma once
#include "utils.h"
#include "State.h"

#include "Env.h" // For HF_LocalTrue

///////////////////////////////////////////////////////////////////////////
/////////////////////// Example Heuristic Function ////////////////////////

// Example Heuristic Function
// class HeuristicFunctionExample {
// public:
//     HeuristicFunctionExample(){};
//     HeuristicFunctionExample(const StateExample& goalState){}; //: m_goalState(goalState){};
//     virtual double getHeuristic(const StateExample& s);
//     virtual vector<double> getBatchHeuristic(vector<StateExample>& s);
//     virtual void assertCorrectSettings(){}; // Checks if all settings are correct

//     // StateExample m_goalState;
// };

///////////////////////////////////////////////////////////////////////////
/////////////////////// Template/Interface Heuristic Function ////////////////////////

// Template/Interface Heuristic
template <class State>
class HF_Template {
public:
    // HF_Template(){};
    // HF_Template(const State& goalState): m_goalState(goalState){};
    virtual double getHeuristic(const State& s) = 0;
    virtual vector<double> getBatchHeuristic(const vector<State>& s);
    virtual void assertCorrectSettings() = 0;

    // State m_goalState;
};


// Zero Heuristic functions
template <class State>
class HF_Zero: public HF_Template<State> {
public:
    HF_Zero(){};
    double getHeuristic(const State& s); // Always returns 0
    void assertCorrectSettings(){};
};

// Returns manhattan distance in GoalStateClass distance
template <class State, class GoalStateClass>
class HF_Manhattan: public HF_Template<State> {
public:
    // HF_Manhattan(){};
    HF_Manhattan(const GoalStateClass& goal, Env2D<State>* env): 
                                    m_goal(goal), m_env(env){};
    double getHeuristic(const State& s);
    void assertCorrectSettings(){};

    GoalStateClass m_goal;
    Env2D<State>* m_env;
};
/////////////////////
// Note: Not the following one, which only works if the goal is the same class type as state
// Manhattan Heuristic
// template <class State>
// class HF_Manhattan: public HF_Template<State> {
// public:
//     State m_goalState; // To access inheritted variable
//     HF_Manhattan(){};
//     HF_Manhattan(const State& goalState): m_goalState(goalState){};
//     double getHeuristic(const State& s);
//     void assertCorrectSettings(){};
// };

// // Partial Manhattan Heuristic
// template <class State>
// class HF_SubManhattan: public HF_Template<State> {
// public:
//     using HF_Template<State>::m_goalState; // To access inheritted variable
//     HF_SubManhattan(): index(0){};
//     HF_SubManhattan(const State& goalState, int index): HF_Template<State>(goalState), index(index) {};
//     double getHeuristic(const State& s);
//     // vector<double> getBatchHeuristic(vector<State>& s){return HF_Template::getBatchHeuristic(s);};
//     void assertCorrectSettings(){};

//     int index;
// };

// // Euclidean Manhattan Heuristic
// template <class State>
// class HF_Euclidian: public HF_Template<State> {
// public:
//     using HF_Template<State>::m_goalState; // To access inheritted variable
//     HF_Euclidian(const State& goalState): HF_Template<State>(goalState){};
//     double getHeuristic(const State& s);
//     // vector<double> getBatchHeuristic(vector<State>& s){return HF_Template::getBatchHeuristic(s);};
//     void assertCorrectSettings(){};
// };

// // Specialied 2D Heuristic
// // class HF_StateXY_FakeNN: public HF_Template<StateXY> {
// // public:
// //     using HF_Template<StateXY>::m_goalState; // To access inheritted variable
// //     HF_StateXY_FakeNN();
// //     HF_StateXY_FakeNN(const StateXY& goalState): HF_Template<StateXY>(goalState){};
// //     double getHeuristic(const StateXY& s);
// //     // vector<double> getBatchHeuristic(vector<StateXY>& s);
// //     void assertCorrectSettings(){};
// // };

// template <class State>
// class HF_ManhattanWorkspace: public HF_Template<State> {
// public:
//     HF_ManhattanWorkspace(const StateXY& workspaceGoalState, Env2D<State>* env): m_env(env), 
//                                                 m_workspaceGoalState(workspaceGoalState){};
//     double getHeuristic(const State& s);
//     void assertCorrectSettings(){assert(m_env);};

//     Env2D<State>* m_env;
//     StateXY m_workspaceGoalState;
// };

#include "HeuristicFunction.hpp"