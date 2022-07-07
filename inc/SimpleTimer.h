#pragma once
#include "utils.h"
#include <map>
#include <chrono>

struct TimerItem {
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    double totalTime; // In seconds
    int count; // So we can divide totalTime by count to get average time

    TimerItem(): count(0), totalTime(0) {};
};

class SimpleTimer {
public:
    SimpleTimer();
    ~SimpleTimer();

    void start(const string& key);
    void stop(const string& key);
    double getTotalTime(const string& key);
    double getAveTime(const string& key);
    void printAllStats();

private:
    std::unordered_map<string, TimerItem> keyToItem;
};