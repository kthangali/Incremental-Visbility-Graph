#include "DuplicityChecker.h"
#include "HeuristicFunction.h"
#include "Env.h"
#include "Queue.h"
#include "Search.h"
#include "SimpleLogger.h"
#include "SimpleTimer.h"
#include "PriorityFunction.h"
#include <thread> // For just the SimpleTimer test

void testSimpleTimer() {
    SimpleTimer st;
    for (int i = 0; i < 5; i++) {
        st.start("Test");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        st.stop("Test");
    }
    cout << st.getTotalTime("Test") << endl;
    cout << st.getAveTime("Test") << endl;
}



int main(int argc, char* argv[]) {
    testSimpleTimer();
}