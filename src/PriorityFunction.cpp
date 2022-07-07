#include "PriorityFunction.h"

////////////////////////////////////////////////
//////// Weighted Heuristic ////////////////////

double PF_Weighted::getPriority(double g, double h) {
    return g + m_w*h;
}
// w is either 0 or >= 1
void PF_Weighted::assertCorrectSettings() {
    assert(m_w == 0 || m_w >= 1);
}

tuple<string, string> PF_Weighted::reportStats() {
    string header = "pf_w,";
    string value = to_string(m_w) + ",";
    return make_tuple(header, value);
}