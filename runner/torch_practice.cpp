// #include "utils.h"
// #include "SimpleTimer.h"
// #include "TorchUtils.h"
// #include "State.h"
// #include <torch/torch.h>
// #include <torch/script.h> // For loading jit model(?)


// #include "Search.h"
// #include "LocalHeuristic.h"


// void testBatchNN() {
//     checkStateDefinedProperly<StateXY>();

//     string mapFilePath = "../data/arena_v1_1.map";
//     // string mapFilePath = "../datasets/maze-map/maze512-1-0.map";
//     // string scenFilePath = "../datasets/maze-scen/maze512-1-0.scen";
//     Env2D<StateXY> menv;
//     menv.buildFromObstacleMap(mapFilePath);

//     AP_SingleDim<StateXY> ap(&menv, 1);

//     SimpleLogger sl;
//     sl.setLogger("../output.txt",  /*keepExisting=*/ false);
//     sl.logString(mapFilePath);

//     StateXY start(20, 4);
//     StateXY goal(14, 12);

//     // StateXY start(380, 345);
//     // StateXY goal(427, 487);

//     DC_ClosedList<StateXY> dc_all;
//     DC_ClosedList<StateXY> dc_focal;
//     SimpleTimer stimer;
    
//     //// Setting up anchor queue
//     HF_Manhattan<StateXY, StateXY> hf1(goal, &menv);
//     PF_Weighted pf1(5);
//     Queue<StateXY>* ancQ = new Queue<StateXY>("Anchor", /*logger=*/ &sl, /*hf=*/ &hf1, /*pf=*/ &pf1,
//         /*dc_check=*/ &dc_all, /*dc_updates=*/ {&dc_all, &dc_focal}, /*ap=*/ &ap);

//     //// Setting up Focal queue with NN heuristic
//     HF_MimicNN hf2;
//     HF_Manhattan<StateXY, StateXY> hf2_underlying(goal, &menv);
//     hf2.m_hf = &hf2_underlying;
//     PF_Weighted pf2(2);
//     BatchQueue<StateXY>* focalQ = new BatchQueue<StateXY>("Focal", /*logger=*/ &sl, /*hf=*/ &hf2, /*pf=*/ &pf2,
//         /*dc_check=*/ &dc_focal, /*dc_updates=*/ {&dc_focal}, /*ap=*/ &ap,
//         /*batchsize=*/ 10, /*expandFullBatch=*/ true);

//     FocalSearch<StateXY> astar; //= FocalSearch<StateXY>();
//     astar.m_expansionLimit = 100000;
//     astar.m_timeLimit = 5000; // INTMAX // In milliseconds
//     astar.m_wFocal = 1.5;
//     astar.setGoal(goal);
//     astar.m_qs.push_back(ancQ);
//     astar.m_qs.push_back(focalQ);
//     astar.m_stimer = &stimer;

//     string status;
//     vector<StateXY> path;
//     double cost;
//     tie(status, path, cost) = astar.runSearch(start);
//     cout << status << " " << cost << "\n" << getPathString<StateXY>(path) << endl;
//     stimer.getTotalTime("SearchLoop");
//     stimer.printAllStats();
//     cout << "Count/Batches: " << hf2.m_countTotal << "/" << hf2.m_countBatches << endl;
// }



// // Define a new Module.
// struct Net : torch::nn::Module {
//   Net() {
//     // Construct and register two Linear submodules.
//     fc1 = register_module("fc1", torch::nn::Linear(2, 64));
//     fc2 = register_module("fc2", torch::nn::Linear(64, 32));
//     fc3 = register_module("fc3", torch::nn::Linear(32, 1));
//   }

//   // Implement the Net's algorithm.
//   torch::Tensor forward(torch::Tensor x) {
//     // Use one of many tensor manipulation functions.
//     x = torch::relu(fc1->forward(x.reshape({x.size(0), 2})));
//     x = torch::dropout(x, /*p=*/0.5, /*train=*/is_training());
//     x = torch::relu(fc2->forward(x));
//     x = fc3->forward(x);
//     // x = torch::log_softmax(fc3->forward(x), /*dim=*/1);
//     return x;
//   }

//   // Use one of many "standard library" modules.
//   torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};
// };


// vector<double> getBatchHeuristic(vector<StateXY>& states, Net* nn) {
//     std::vector<torch::Tensor> tensors;
//     tensors.reserve(states.size());
//     for (StateXY& s : states) {
//         tensors.push_back(stateToTensor(s));
//     }
//     torch::Tensor input = torch::stack(tensors).to(torch::kFloat32);
//     cout << input.sizes() << endl;
//     // torch::Tensor input = torch::rand({4, 2}).to(torch::kFloat32);
//     torch::Tensor pred = nn->forward(input);
//     cout << pred << endl;
//     std::vector<double> v(pred.data_ptr<float>(), pred.data_ptr<float>() + pred.numel());
//     return v;
// }

// void nnTimingTest(int numTotalExamples, int batchSize) {
//     SimpleTimer stimer;
//     StateXY s(1, 2);
//     int numTrials = numTotalExamples/batchSize;

//     stimer.start("Total");
//     stimer.start("InitNN");
//     Net* nn = new Net();
//     nn->eval();
//     stimer.stop("InitNN");

//     for (int n = 0; n < numTrials; n++) {
//         std::vector<torch::Tensor> tensors(batchSize);
//         stimer.start("StateToTensor");
//         for (size_t i = 0; i < batchSize; ++i) {
//             tensors[i] = stateToTensor(s);
//         }
//         stimer.stop("StateToTensor");

//         stimer.start("Stack");
//         torch::Tensor input = torch::stack(tensors).to(torch::kFloat32);
//         stimer.stop("Stack");

//         stimer.start("NN");
//         torch::Tensor pred = nn->forward(input);
//         stimer.stop("NN");

//         stimer.start("TensorToVec");
//         std::vector<double> v(pred.data_ptr<float>(), pred.data_ptr<float>() + pred.numel());
//         stimer.stop("TensorToVec");
//     }
//     stimer.stop("Total");
//     stimer.printAllStats();
//     cout << "----------------------" << endl;

//     delete nn;
// }

// int main(int argc, char** argv) {
//     // This is needed as the first time it is created takes much longer than other calls
//     Net* nn = new Net();
//     delete nn;

//     nnTimingTest(100000, 1);
//     nnTimingTest(100000, 5);
//     nnTimingTest(100000, 25);
//     nnTimingTest(100000, 125);

//     // testBatchNN();
    
//     // torch::manual_seed(0);
//     // Net* nn = new Net();
//     // nn->eval();

//     // State tmp(2,3);
//     // vector<State> v = {tmp, tmp};
//     // auto fvals = getBatchHeuristic(v, nn);

//     // std::cout << test(tmp) << std::endl;
//     // torch::Tensor x = torch::rand(4);
//     // int* input = x.data<float>();

//     // auto ten = torch::ones({ 3, 4 }); //.to(torch::kInt32);
// 	// std::vector<float> v(ten.data_ptr<float>(), ten.data_ptr<float>() + ten.numel());

//     // vector<int> v(std::begin(x), std::stop(x));
//     // cout << fvals << endl;
// }