#pragma once
#include "utils.h"
// #include "Queue.h"
#include "QNode.h"


// Abstract class/interface
template <class State>
class DuplicityChecker {
public:
    using QNodeT = QNode<State>;
    void virtual updateDuplicity(const QNodeT& qn) = 0; // Updates duplicity
    bool virtual getDuplicityB(const QNodeT& qn) = 0; // Returns boolean
    double virtual getDuplicityD(const QNodeT& qn); // Returns double
    virtual void assertCorrectSettings(){}; // Checks if all settings are correct
    virtual void reset(){}; // Clears Duplicity Checker
};

// Closed list that just checks states
template <class State>
class DC_ClosedList : public DuplicityChecker<State> {
public:
    using QNodeT = QNode<State>;
    DC_ClosedList();
    ~DC_ClosedList();

    void updateDuplicity(const QNodeT& qn);
    bool getDuplicityB(const QNodeT& qn);
    void assertCorrectSettings();
    void reset();

private:
    unordered_set<QNodeT, QNodeHasher<State>, QNodeEqual<State>> seenStates;
};

// Closed list that checks f values
template <class State>
class DC_ClosedListReopen : public DuplicityChecker<State> {
public:
    using QNodeT = QNode<State>;
    DC_ClosedListReopen();
    ~DC_ClosedListReopen();

    void updateDuplicity(const QNodeT& qn);
    bool getDuplicityB(const QNodeT& qn);
    void assertCorrectSettings();
    void reset();

private:
    unordered_set<QNodeT, QNodeHasher<State>, QNodeEqual<State>> seenStates;
};

#include "DuplicityChecker.hpp"