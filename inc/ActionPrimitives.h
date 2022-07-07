#pragma once
#include "utils.h"
#include "State.h"
#include "Env.h"

//////////////////////////////////////////////////////////////
//////////// Template ActionPrimitives class /////////////////

template <class State> 
class AP_Template {
public:
    AP_Template(Env<State>* env): m_env(env){};
    virtual vector<State> getActions(const State& s) = 0;
    virtual vector<Transition<State>> getSuccessors(State& s);
    virtual void assertCorrectSettings() = 0;
    
    Env<State>* m_env;
};


// Single dimension action primitives
template <class State>
class AP_SingleDim: public AP_Template<State> {
public:
    using AP_Template<State>::m_env;

    AP_SingleDim(Env<State>* env, int stepSize);
    vector<State> getActions(const State& s);
    void assertCorrectSettings(){assert(m_stepSize>0);};

    int m_stepSize; // +- what size

private:
    vector<State> m_actions; // These can be precomputed as not dependent on input state
};

// StateXYTime specific
class AP_StateXYTime: public AP_Template<StateXYTime> {
public:
    using AP_Template<StateXYTime>::m_env;

    AP_StateXYTime(Env<StateXYTime>* env, int stepSize);
    vector<StateXYTime> getActions(const StateXYTime& s);
    void assertCorrectSettings(){assert(m_stepSize>0);};

    int m_stepSize; // +- what size
private:
    vector<StateXYTime> m_actions; // These can be precomputed as not dependent on input state
};

// StateXYTheta specific
class AP_StateXYTheta: public AP_Template<StateXYTheta> {
public:
    using AP_Template<StateXYTheta>::m_env;

    AP_StateXYTheta(Env<StateXYTheta>* env, int stepSize);
    vector<StateXYTheta> getActions(const StateXYTheta& s);
    void assertCorrectSettings(){assert(m_stepSize>0);};

    int m_stepSize; // +- what size
private:
    array< vector<StateXYTheta>, 4> m_actions; // These can be precomputed as only dependent on input theta
};

#include "ActionPrimitives.hpp"