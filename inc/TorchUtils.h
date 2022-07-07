// #pragma once
// #include "utils.h"
// #include "HeuristicFunction.h"
// #include <torch/torch.h>

// // Converts state into tensor of shape (2)
// // inline torch::Tensor stateToTensor(const State& s) {
// //     return torch::tensor({s.c[0], s.c[1]});
// // }
// inline torch::Tensor stateToTensor(const StateXY& s) {
//     return torch::tensor({s.c[0], s.c[1]});
// }

// struct SimpleNN : torch::nn::Module {
//     SimpleNN();
//     // Implement the Net's algorithm.
//     torch::Tensor forward(torch::Tensor x);

//     // Use one of many "standard library" modules.
//     torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};
// };

// // MimicNN Heuristic
// class HF_MimicNN : public HF_Template<StateXY> {
// // class HF_MimicNN {
// public:
//     HF_MimicNN();
//     double getHeuristic(const StateXY &s);
//     vector<double> getBatchHeuristic(const vector<StateXY> &s);
//     void assertCorrectSettings();

//     int m_countTotal;
//     int m_countBatches;
//     HF_Template<StateXY>* m_hf;
//     // HeuristicFunction<StateXY>* m_hf; // Actual heuristic that will be returned

// private:
//     SimpleNN* m_snn; // Dummy nn to simular forward pass time
// };