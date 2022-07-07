#include "SimpleTimer.h"

SimpleTimer::SimpleTimer() {

}

SimpleTimer::~SimpleTimer() {

}

void SimpleTimer::start(const string& key) {
    if (keyToItem.find(key) == keyToItem.end()) {
        keyToItem[key] = TimerItem();
    }
    keyToItem[key].startTime = std::chrono::steady_clock::now();
}

void SimpleTimer::stop(const string& key) {
    // https://stackoverflow.com/questions/22387586/measuring-execution-time-of-a-function-in-c
    auto duration = std::chrono::steady_clock::now() - keyToItem[key].startTime;
    keyToItem[key].totalTime += std::chrono::duration<double, std::milli>(duration).count()/1000;
    keyToItem[key].count += 1;
}

double SimpleTimer::getTotalTime(const string& key) {
    return keyToItem[key].totalTime;
}

double SimpleTimer::getAveTime(const string& key) {
    return keyToItem[key].totalTime/keyToItem[key].count;
}

void SimpleTimer::printAllStats() {
    for (auto& keyItemPair: keyToItem) {
        TimerItem& stats = keyItemPair.second;
        cout << keyItemPair.first << endl;
        cout << "  Total: " << std::fixed << std::setprecision(3) << stats.totalTime; // << endl;
        cout << "  Average: " << std::fixed << std::setprecision(3) << stats.totalTime/stats.count; // << endl;
        cout << "  Count: " << stats.count << endl;
    }
}