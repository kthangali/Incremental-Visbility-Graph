#include "ActionPrimitives.h"

//////////////////////////////////////////////////////////////
/////// AP_StateXYTime action primitives class ///////////////

AP_StateXYTime::AP_StateXYTime(Env<StateXYTime>* env, int stepSize):
                    AP_Template<StateXYTime>(env), m_stepSize(stepSize) {
    m_actions.push_back(StateXYTime(0, m_stepSize, 1));
    m_actions.push_back(StateXYTime(0, -m_stepSize, 1));
    m_actions.push_back(StateXYTime(m_stepSize, 0, 1));
    m_actions.push_back(StateXYTime(-m_stepSize, 0, 1));
}

// Returns fixed action set
vector<StateXYTime> AP_StateXYTime::getActions(const StateXYTime& s) {
    return m_actions;
}


//////////////////////////////////////////////////////////////
/////// AP_StateXYTime action primitives class ///////////////

AP_StateXYTheta::AP_StateXYTheta(Env<StateXYTheta>* env, int stepSize):
                    AP_Template<StateXYTheta>(env), m_stepSize(stepSize) {
    /* Actions allow movement only in the positive theta direction, and rotating to other theta
    theta = 0:  (1,0,0), (0,0,1), (0,0,-1)
    theta = 1:  (0,1,0), (0,0,1), (0,0,-1)
    theta = 2:  (-1,0,0), (0,0,1), (0,0,-1)
    theta = 3:  (0,-1,0), (0,0,1), (0,0,-1)
    */

    vector<StateXYTheta> firstVals;
    firstVals.push_back(StateXYTheta(m_stepSize, 0, 0));
    firstVals.push_back(StateXYTheta(0, m_stepSize, 0));
    firstVals.push_back(StateXYTheta(-m_stepSize, 0, 0));
    firstVals.push_back(StateXYTheta(0, -m_stepSize, 0));

    for (int theta = 0; theta < 4; ++theta) {
        m_actions[theta] = {firstVals[theta], StateXYTheta(0,0,1), StateXYTheta(0,0,-1)};
    }
}

// Returns fixed action set
vector<StateXYTheta> AP_StateXYTheta::getActions(const StateXYTheta& s) {
    return m_actions[s.c[2]]; // Return actions based on theta value
}