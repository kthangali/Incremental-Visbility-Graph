#include <boost/program_options.hpp>
#include <boost/filesystem.hpp> // For create_directories
#include <iomanip> // For std::put_time
#include <regex> // For regex and split logic

#include "utils.h"
#include "State.h"
#include "Search.h"
#include "Hvg.h"

// Create log directory and return the path
string createLogDir(string& results_path, string time_str, string suffix) {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
	if (time_str == "") {
		std::ostringstream oss;
		oss << std::put_time(&tm, "%m%d_%H%M%S");
		time_str = oss.str();
	}

	string the_dir = results_path + time_str + "_" + suffix;
    boost::filesystem::create_directories(the_dir);
    return the_dir;
}


// Splits string based on deliminator
vector<string> split(const string& str, const string& delim) {   
    // https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c/64886763#64886763
    const std::regex ws_re(delim);
    return { std::sregex_token_iterator(str.begin(), str.end(), ws_re, -1), std::sregex_token_iterator() };
}

// Loads StateXY from string
StateXY loadStateXY(const string& str) {
    vector<string> tokens = split(str, ",");
    StateXY ans(0,0);
    int count = 0;
    for (int i = 0; i < tokens.size(); ++i) {
        ans.c[i] = std::stoi(tokens[i]);
    }
    return ans;
}

// Gets a State that corresponds to input StateXY
template <class State>
State loadStateFromStateXY(const StateXY& s) {
    State ans;
    for (int i = 0; i < s.c.size(); ++i) {
        ans.c[i] = s.c[i];
    }
    return ans;
}


template <class State, class AP_Class>
void RunSearchInstance(int argc, char** argv) {
    namespace po = boost::program_options;
	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()
		// batch running
		("seed", po::value<int>()->default_value(0), "random seed")
		("batchFolder", po::value<string>()->default_value(".."), "Folder to save outputs")

		// params for the input instance and experiment settings
		("map", po::value<string>()->required(), "input file for map")
		// ("scen", po::value<string>()->required(), "scene input file for start & goals")
		("timeout", po::value<double>()->default_value(60), "timeout time (seconds)")
		("start", po::value<string>()->required(), "start StateXY")
        ("goal", po::value<string>()->required(), "goal StateXY")
        ;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

    const int SEED = vm["seed"].as<int>();
    const string BATCHFOLDER = vm["batchFolder"].as<string>();
    if (BATCHFOLDER != "..") // Create batchfolder if it doesn't exist and is not the root
        boost::filesystem::create_directories(BATCHFOLDER);

    string mapFilePath = vm["map"].as<string>();
    Env2D<State>* env = new Env2D<State>();
    env->buildFromObstacleMap(mapFilePath);


    StateXY tmp = loadStateXY(vm["start"].as<string>());
    State start = loadStateFromStateXY<State>(tmp);
    StateXY workspaceGoal = loadStateXY(vm["goal"].as<string>());

    AP_Class* ap = new AP_Class(env, 1); // Requires AP_Class to take in a step size
    // Likely should change to AP_Class being an input that takes in menv afterwards

    SimpleLogger* sl = new SimpleLogger();
    sl->setLogger(BATCHFOLDER + "/output.txt", /*keepExisting=*/ false);
    sl->logString(mapFilePath);

    SimpleTimer* stimer = new SimpleTimer();
    DC_ClosedList<State>* dc_all = new DC_ClosedList<State>();
    
    HF_Manhattan<State, StateXY>* hf1 = new HF_Manhattan<State, StateXY>(workspaceGoal, env);
    PF_Weighted* pf1 = new PF_Weighted(1);

   

    // Queue<State>* ancQ = new Queue<State>(/*name=*/ "Anchor", /*logger=*/ sl, /*hf=*/ hf1, 
    //     /*pf=*/ pf1, /*dc_check=*/ dc_all, /*dc_updates=*/ {dc_all}, /*ap=*/ ap);
    // Note that cannot use {dc_all} array initialization for make_shared<>()

    HVGQueue* ancQ = new HVGQueue(/*name=*/ "Anchor", /*logger=*/ sl, /*hf=*/ hf1, 
        /*pf=*/ pf1, /*dc_check=*/ dc_all, /*dc_updates=*/ {dc_all}, /*ap=*/ ap);
    // //add start state to ancQ
    ancQ->start = start;
    ancQ->q_vg = set<StateXY>();
    BasicAStarSearch<State> astar;
    astar.m_expansionLimit = 10000;
    astar.m_timeLimit = 5000; // INTMAX // In milliseconds
    // astar.setGoal(goal);
    astar.setWorkspaceGoal(workspaceGoal, env);
    astar.m_qs.push_back(ancQ);
    // cout << "pushed queue";
    astar.m_stimer = stimer;

    string status;
    vector<State> path;
    double cost;
    tie(status, path, cost) = astar.runSearch(start);
    cout << status << " " << cost << endl; 
    cout << "Grid Path" << endl;
    cout << getPathString(path) << endl;    
    double dummy = ancQ->shortPathFromVG(ancQ->q_vg, start, workspaceGoal, true);


    vector<StateXY> HVGPath = ancQ->getHVGPath(workspaceGoal);
    cout << "HVG Path" << endl;
    cout << getPathString(HVGPath) << endl;
    astar.reportStats(BATCHFOLDER + "/output_stats.csv", SEED);
}

int main(int argc, char** argv) {
    // testBatchNN();
    RunSearchInstance<StateXY, AP_SingleDim<StateXY> >(argc, argv);
    //comment bottom 2 out for testing 
    // RunSearchInstance<StateXYTime, AP_StateXYTime >(argc, argv);
    // RunSearchInstance<StateXYTheta, AP_StateXYTheta >(argc, argv);
}