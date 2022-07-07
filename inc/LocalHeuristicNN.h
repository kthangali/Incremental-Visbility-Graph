#pragma once

#include "utils.h"
#include "HeuristicFunction.h"
// #include "Queue.h"
#include <unordered_map>
// #include <torch/script.h>  // For loading model

template <class State>
class HF_LocalNN: public HF_Template<State> {
public:
    HF_LocalNN(const string& filepath, double k_reduce,
            int window, Env2D<State>* m_env);
    double getHeuristic(const State& s);
    void assertCorrectSettings();

    // Helper functions
    // torch::Tensor stateToTensor(const State& s);

    double k_reduce;
    int window;
    std::unordered_map<State, double> m_stored_values;
    // torch::jit::script::Module m_model;

    //// Variables for logging data
    Env2D<State>* m_env;
};


template <class State>
void HF_LocalNN<State>::HF_LocalNN(const string& filepath, double k_reduce,
            int window, Env2D<State>* m_env):
        k_reduce(k_reduce), window(window), m_env(m_env) {

    try {
        // Deserialize the ScriptModule from a file using torch::jit::load().
        // m_model = torch::jit::load(filepath);
        cout << "Loaded module fine!" << endl;
    }
    catch (const c10::Error& e) {
        throw runtime_error("error loading the model");
    }
};

template <class State>
void HF_LocalNN<State>::assertCorrectSettings() {
	assert(k_reduce >= 0);
	assert(window >= 0);
};

template <class State>
double HF_LocalNN<State>::getHeuristic(const State& s) {
    auto search = m_stored_values.find(start_s);
   	if (search != m_stored_values.end()) {
		return search->second;
	}

    // torch::Tensor obsTensor;
    vector<vector<double>> hval;
	hval.resize(window, std::vector<double>(window, -1)); // (H,W)

	// This loop requires knowing how env2D works internally via constructing
	//   neigbhoring states
	for (int y = 0; y < 2*window+1; ++y) {
    	for (int x = 0; x < 2*window+1; ++x) {
			// StateXY curState = StateXY(x, y);
            StateXYTheta curState = StateXYTheta(x, y, 0);
			obsTensor[y][x] = m_hf->getHeuristic(curState);
        }
    }
	sl.log2DVector(hval);


    m_model.forward()
}

template <class State>
// torch::Tensor HF_LocalNN<State>::stateToTensor(const State& s) {

}


