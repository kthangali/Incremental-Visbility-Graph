#pragma once

#include "utils.h"
#include "HeuristicFunction.h"
#include "Queue.h"
#include <unordered_map>

// Local Heuristic function
//   Warning: Issue with logEnvAndH, not truly State invariant
//   and will fail if State != StateXYTheta
template <class State>
class HF_LocalTrue: public HF_Template<State> {
public:
    HF_LocalTrue(Queue<State>* copyq, double k_reduce,
         int window, Env2D<State>* m_env);
    double getHeuristic(const State& s);
    void assertCorrectSettings();

    SimpleLogger* m_sl;
    Queue<State>* m_copyq;
    double k_reduce;
    int window;
    std::unordered_map<State, double> m_stored_values;

    //// Variables for logging data
    Env2D<State>* m_env;
    SimpleLogger* m_sl2;

    void logEnvAndH(const string& mapFilepath, const string& hvalsFilepath);

private:
    void resetSearch();
    
    Queue<State>* m_q;
    HF_Template<State>* m_hf;

    void logH(const State& s, double oldVal, double newVal);
    void logData(const State& s, double newVal);
};


//////////////////// LocalTrue Heuristic /////////////////////
template <class State>
HF_LocalTrue<State>::HF_LocalTrue(Queue<State>* m_copyq, double k_reduce, int window, 
			Env2D<State>* env):
		k_reduce(k_reduce), window(window), m_sl(nullptr),
		m_env(env), m_sl2(nullptr) {
	m_hf = m_copyq->m_hf;

	DC_ClosedList<State>* internal_dc_check = new DC_ClosedList<State>();
	m_q = new QueueMultiDC<State>(/*name=*/ "LocalTrue_InternalQ", 
		/*logger=*/ new SimpleLogger(),  /*hf=*/ m_hf, /*pf=*/ make_shared<PF_Weighted>(1), 
		/*dc_check=*/ {internal_dc_check}, /*dc_updates=*/ {internal_dc_check},
		/*ap=*/ m_copyq->m_ap);
	m_q->assertCorrectSettings(); 
}

template <class State>
void HF_LocalTrue<State>::assertCorrectSettings() {
	assert(k_reduce >= 0);
	assert(window >= 0);
};

template <class State>
void HF_LocalTrue<State>::resetSearch() {
	m_q->m_dc_updates[0]->reset();
	m_q->reset();
}

template <class State>
double HF_LocalTrue<State>::getHeuristic(const State& start_s) {
    //// Do a small search here(!)
    /* Create node class
    Create start node
    Go through neighbhors
    */

   	auto search = m_stored_values.find(start_s);
   	if (search != m_stored_values.end()) {
		return search->second;
	}

   	using NodeT = Node<State>;
   	resetSearch();
   	double startH = m_hf->getHeuristic(start_s);
	if (startH - k_reduce <= 0) {
		return startH;
	}
	double borderDist = INTMAX/2;

	// NodeT start(nullptr, 0, 0, start_s, false, false);
	// m_q->insert(&start);
	shared_ptr<NodeT> start = make_shared<NodeT>(nullptr, 0, 0, start_s, false, false);
	m_q->insert(start);
	
	while (m_q->canExpand(10000)) {
		vector<shared_ptr<NodeT>> children, expanded;
		tie(children, expanded) = m_q->expand(10000);
		for (const shared_ptr<NodeT> &n: expanded) {
			double curH = m_hf->getHeuristic(n->s);
			if (curH <= startH - k_reduce) {
				// cout << n->g + (startH - k_reduce) << "  " << startH << endl;
				logH(start_s, startH, n->g + (startH - k_reduce));
				m_stored_values[start_s] = n->g + (startH - k_reduce);
				return n->g + (startH - k_reduce);
			}
		}

		for (shared_ptr<NodeT> &n: children) {
			int delX = abs(n->s.c[0] - start_s.c[0]);
			int delY = abs(n->s.c[1] - start_s.c[1]);
			if (max(delX, delY) <= window) {
				m_q->insert(n);
			}
			else {
				double curH = m_hf->getHeuristic(n->s);
				double tmp = n->g + curH - (startH - k_reduce);
				borderDist = min(borderDist, n->g + curH - (startH - k_reduce));
			}
		}
	}
	// cout << window << "  " << borderDist << endl;
	if (borderDist < INTMAX/2 - 1) {
		logH(start_s, startH, borderDist + (startH - k_reduce));
	}
	m_stored_values[start_s] = borderDist + (startH - k_reduce);
	return borderDist + (startH - k_reduce);
}

template <class State>
void HF_LocalTrue<State>::logH(const State& s, double oldVal, double newVal) {
	if (m_sl) {
		string logStr = s.getStr() + " " + to_string(newVal - oldVal) + " LHImprovement";
		m_sl->logString(logStr);
	}
	if (m_sl2) {
		logData(s, newVal);
	}
}


template <class State>
void HF_LocalTrue<State>::logData(const State& s, double newVal) {
	assert(m_env);
	assert(m_sl2);
	
	m_sl2->logString(s.getStr() + " " + to_string(newVal));
}

template <class State>
void HF_LocalTrue<State>::logEnvAndH(const string& mapFilepath, const string& hvalsFilepath) {
	assert(m_env);
	assert(m_sl2);

	SimpleLogger sl(hvalsFilepath, /*keepExisting=*/ false);
	vector<vector<double>> hval;
	hval.resize(m_env->m_height, std::vector<double>(m_env->m_width, -1)); // (H,W)

	// This loop requires knowing how env2D works internally via constructing
	//   neigbhoring states
	for (int y = 0; y < m_env->m_height; ++y) {
    	for (int x = 0; x < m_env->m_width; ++x) {
			// StateXY curState = StateXY(x, y);
            StateXYTheta curState = StateXYTheta(x, y, 0);
			hval[y][x] = m_hf->getHeuristic(curState);
        }
    }
	sl.log2DVector(hval);
	
	m_sl2->logString(mapFilepath); // Where to find map info
	m_sl2->logString(hvalsFilepath); // Where to find hvals info
	m_sl2->logString("");
}