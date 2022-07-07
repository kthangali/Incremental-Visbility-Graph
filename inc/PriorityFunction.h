#pragma once
#include "utils.h"

// Abstract Priority Function class
class PriorityFunction {
public:
    virtual double getPriority(double g, double h) = 0;
    virtual void assertCorrectSettings() = 0; // Checks if all settings are correct
    virtual tuple<string, string> reportStats() = 0;
};

// Basic weighted heuristic function
class PF_Weighted: public PriorityFunction {
public:
    PF_Weighted(double w): m_w(w){};

    //// Required for inheritance
    double getPriority(double g, double h);
    void assertCorrectSettings();
    tuple<string, string> reportStats();

    double m_w;
};