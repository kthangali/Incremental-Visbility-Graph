// #include "TorchUtils.h"

// /////////////////////////////////////////////////////////
// //////////////////// Start SimpleNN /////////////////////

// SimpleNN::SimpleNN() {
//     // Construct and register two Linear submodules.
//     fc1 = register_module("fc1", torch::nn::Linear(2, 64));
//     fc2 = register_module("fc2", torch::nn::Linear(64, 32));
//     fc3 = register_module("fc3", torch::nn::Linear(32, 1));
// }

// torch::Tensor SimpleNN::forward(torch::Tensor x) {
//     // Use one of many tensor manipulation functions.
//     x = torch::relu(fc1->forward(x.reshape({x.size(0), 2})));
//     // x = torch::dropout(x, /*p=*/0.5, /*train=*/is_training());
//     x = torch::relu(fc2->forward(x));
//     x = fc3->forward(x);
//     return x;
// }

// //////////////////// End SimpleNN /////////////////////
// /////////////////////////////////////////////////////////

// /////////////////////////////////////////////////////////
// //////////////////// Start HF_MimicNN /////////////////////

// HF_MimicNN::HF_MimicNN():
//             m_countTotal(0), m_countBatches(0) {
//     m_snn = new SimpleNN();
//     m_snn->eval(); // Removes dropout randomness
// }

// double HF_MimicNN::getHeuristic(const StateXY &s) { 
//     notImplemented("getHeuristic"); 
//     return -1;    
// }

// vector<double> HF_MimicNN::getBatchHeuristic(const vector<StateXY> &states) {
//     // Convert states to torch tensors
//     std::vector<torch::Tensor> tensors(states.size());
//     for (size_t i = 0; i < states.size(); ++i) {
//         tensors[i] = stateToTensor(states[i]);
//     }
    
//     // Actual computation
//     torch::Tensor input = torch::stack(tensors).to(torch::kFloat32);
//     torch::Tensor pred = m_snn->forward(input);
//     std::vector<double> v(pred.data_ptr<float>(), pred.data_ptr<float>() + pred.numel());

//     // Metrics
//     m_countTotal += states.size();
//     ++m_countBatches;
//     // return v;
//     return m_hf->getBatchHeuristic(states);
// }

// void HF_MimicNN::assertCorrectSettings() {
//     assert(m_snn != nullptr);
// }

// //////////////////// End HF_MimicNN /////////////////////
// /////////////////////////////////////////////////////////