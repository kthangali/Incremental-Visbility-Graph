#pragma once
#include "utils.h"
#include "Queue.h"
#include "DuplicityChecker.h"
#include "Env.h"
#include "SimpleTimer.h"

////////////////// Main Search Abstract Class Below ///////////
template <class State>
class Search {
public:
    using NodeT = Node<State>;
    Search(): m_fthresh(0){};
    ~Search(){};

    virtual Queue<State>* chooseQueue() = 0;
    virtual bool isGoal(State& s) = 0;
    virtual tuple<string, vector<State>, double> runSearch(State& startState);
    virtual bool terminateSearch() = 0; // E.g. timeout, expansion out
    virtual void assertStartCorrectly(); // Make sure all settings and variables are good since we are not having constructors
    // virtual void reportStats(const string& fileName) = 0; // Report/save stats

    // vector<Queue<State>*> m_qs;
    vector<Queue<State>*> m_qs;

    double m_fthresh;

    // Variables for search results
    vector<State> m_solution_path;
    double m_solution_cost;
    string m_solution_status;
};


/////////////////// Basic A* Class Below ////////////
template <class State>
class BasicAStarSearch: public Search<State> {
public:
    using Search<State>::m_fthresh;
    using Search<State>::m_qs;
    using Search<State>::m_solution_status;
    using Search<State>::m_solution_cost;

    BasicAStarSearch();
    ~BasicAStarSearch();

    //// Required for inheriting Search
    Queue<State>* chooseQueue();
    bool isGoal(State& s);
    tuple<string, vector<State>, double> runSearch(State& startState);
    bool terminateSearch();
    void assertStartCorrectly();
    void reportStats(const string& fileName, int seed);

    ///// Extra stuff
    void setGoal(const State& s);
    void setWorkspaceGoal(const StateXY& s, Env2D<State>* env2D);

    State m_goalState;
    StateXY m_goalWorkspaceState;
    Env2D<State>* m_env2D;

    SimpleTimer* m_stimer;
    int m_numExpanded;

    // Termination critera
    int m_expansionLimit;
    int m_timeLimit; // In seconds
};


/////////////////// Focal Search Below //////////////
template <class State>
class FocalSearch: public BasicAStarSearch<State> {
public:
    using BasicAStarSearch<State>::m_fthresh;
    using BasicAStarSearch<State>::m_qs;

    FocalSearch();
    ~FocalSearch();

    //// Inherited
    void assertStartCorrectly();
    Queue<State>* chooseQueue();

    //// Extra stuff
    double m_wFocal; // Focal sub-optimality
    // bool m_EES_style; // Changes how chooseQueue works
};

#include "Search.hpp"
