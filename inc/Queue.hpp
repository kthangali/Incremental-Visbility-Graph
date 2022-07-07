#include "Queue.h"
#include "Env.h"
#include "Node.h"
#include "State.h"
#include<set> 
#include<vector>

//////////////////////////////////////////////////////////////////
///////////////////////// Queue Below ////////////////////////////

using namespace std; 
template <class State>
Queue<State>::Queue(const string& qName, SimpleLogger* logger, 
            HF_Template<State>* hf, 
            PriorityFunction* pf,
            DuplicityChecker<State>* dc_check, 
            vector<DuplicityChecker<State>*> dc_updates,
            AP_Template<State>* ap):
        m_name(qName), m_logger(logger), m_hf(hf), m_pf(pf), m_dc_check(dc_check),
        m_dc_updates(dc_updates), m_ap(ap) {
}

template <class State>
Queue<State>::~Queue() {
    
}

// Regular A* insert
template <class State>
void Queue<State>::insert(shared_ptr<NodeT>& n) {
    double h = m_hf->getHeuristic(n->s);
    double f = m_pf->getPriority(n->g, h);
    QNodeT qn(f, n);
    m_pq.push(qn);
}

//new method of calculating g-value 
//perform scanning logic over node to generate visibility graph and then perform 
//A* search from start to node to find shortest euclidean distance, this distance becomes the g-value of the node 

//write scanning/getVG/shortPathFromVG functions here 
//update expand function to set g-value to be distance of path returned by shortPathFromVG 


//initialize dX and dY to get through up/down/left/right

void scan(Node<StateXY>* node, set<StateXY> scan_x, set<StateXY> scan_y) // Env<State>* e)
{
    int dX[4] = {-1,1, 0, 0}; 
    int dY[4] = {0, 0, -1, 1};
    for(int i = 0; i < 4; i++)
    {
        int x_move = dX[i];
        int y_move = dY[i];
        bool obstacle_hit = false; 
        Node<StateXY>* curr = node;  
        while(obstacle_hit == false)
        {
            //current x and y positions 
            int currPose_x = node->s.c[0]; 
            int currPose_y = node->s.c[1];
            int new_x = currPose_x + x_move;
            int new_y = currPose_y + y_move;
            //create new state XY with new_x and new_y
            StateXY newState = StateXY(new_x, new_y);
            //obstacle_hit = e->isValidState(newState);
            obstacle_hit = false;
            if(obstacle_hit)
            {
                    if(y_move == 0)
                    {
                        // scan_x.insert(newState);
                    }
                    else
                    {
                        // scan_y.insert(newState);
                    }

            }

        }
    }
}

//Input: set of scans in x direction and set of scans in y direction 
//Output: set of stateXY objects present in both, indicating obstacle corner 
set<StateXY> getVG(set<StateXY> scans_x, set<StateXY> scans_y)
{
    set<StateXY> VG {}; 
    for(auto itr : scans_x)
    {
        if(scans_y.count(itr) != 0){
            VG.insert(itr);
        }
    }
    return VG; 
}


//run A* search over vg to find shortest path and return its length to be used as g value 
int shortPathFromVG(set<StateXY> vg)
{


    return 0;
}
// Regular A* expand
//this one needs to be modified (how g value is calculated)
template <class State>
tuple<vector<shared_ptr<Node<State>>>, vector<shared_ptr<Node<State>>> > Queue<State>::expand(double ancFThresh) {
    assert(canExpand(ancFThresh)); // This also calls prepareForExpand()

    QNodeT qn = m_pq.top(); // Get top node
    m_pq.pop(); // Remove from queue
    if (m_logger != nullptr) // Save to logger if needed
        m_logger->logExpansion(qn.getStr(), m_name);
    for (DuplicityChecker<State>* dc: m_dc_updates) { // Updates Duplicity Checkers
        dc->updateDuplicity(qn);
    }
    vector<shared_ptr<NodeT>> expanded = {qn.n};

    // Get transition data and create children
    //can we use this same create children function for HVG implementation 
    vector<Transition<State> > transitions = m_ap->getSuccessors(qn.n->s);
    vector<shared_ptr<NodeT>> children;
    for (const Transition<State>& tran : transitions) {
        if (!tran.isValid)
            continue;

        shared_ptr<NodeT> child = make_shared<NodeT>(qn.n, qn.n->g + tran.cost, -1, tran.s, false, false);
        children.push_back(child);
    }
    
    return std::make_tuple(children, expanded);
}

// Side effects: Calls prepareForExpand()
template <class State>
bool Queue<State>::canExpand(double ancFThresh) {
    prepareForExpand();
    if (!m_pq.empty() && m_pq.top().n->ancF <= ancFThresh) {
        return true;
    }
    return false;
}

// Returns top node, intended use is only for getting min f value
template <class State>
shared_ptr<Node<State>> Queue<State>::getTop() {
    prepareForExpand();
    assert(!m_pq.empty());
    return m_pq.top().n;
}

// Updates ancH and ancF values using this Q's hf & pf
template <class State>
void Queue<State>::updateNodeWithAnchorStats(shared_ptr<NodeT>& n) {
    double ancH = m_hf->getHeuristic(n->s);
    n->ancF = m_pf->getPriority(n->g, ancH);
}

// Side effects: Removes duplicates and previously expanded nodes
//   Ensures that top node is ready to expand
template <class State>
void Queue<State>::prepareForExpand() {
    while (!m_pq.empty()) {
        if (m_dc_check->getDuplicityB(m_pq.top())) { // If it is a duplicate
            m_pq.pop();
        }
        else { // The top node is now ready to expand
            break;
        }
    }
}

// Makes sure settings are correct
template <class State>
void Queue<State>::assertCorrectSettings() {
    // assert(m_logger != nullptr);
    if (m_logger == nullptr) {
        cout << "Warning: Queue " << m_name << " has no logger" << endl;
    }
    m_hf->assertCorrectSettings();
    m_pf->assertCorrectSettings();

    bool dcCheckInUpdatesList = false;
    for (DuplicityChecker<State>* dc: m_dc_updates) {
        dc->assertCorrectSettings();
        dcCheckInUpdatesList = dcCheckInUpdatesList || (dc == m_dc_check); // If true stays true, else gets updated by rhs
    }

    assert(dcCheckInUpdatesList);
    m_ap->assertCorrectSettings();
}

// Resets Queue by removing states from priority queue
template <class State>
void Queue<State>::reset() {
    while (!m_pq.empty()) {
        m_pq.pop();
    }
}

// Returns header string and value string
//   Note: Both comma seperated for csv usage
template <class State>
tuple<string, string> Queue<State>::reportStats() {
    string pf_header, pf_stats;
    tie(pf_header, pf_stats) = m_pf->reportStats();
    string header = "queue_name," + pf_header;
    string value = m_name + "," + pf_stats;
    return make_tuple(header, value);
}

///////////////////////////////////////////////////////////////////////
///////////////////////// QueueMultiDC Below ////////////////////////////
template <class State>
QueueMultiDC<State>::QueueMultiDC(const string& qName, SimpleLogger* logger, 
            HF_Template<State>* hf, PriorityFunction* pf,
            vector<DuplicityChecker<State>*> dc_checks, 
            vector<DuplicityChecker<State>*> dc_updates,
            AP_Template<State>* ap):
            Queue<State>(qName, logger, hf, pf, dc_checks[0], dc_updates, ap), m_dc_checks(dc_checks) {
}


// Side effects: Removes duplicates and previously expanded nodes
//   Ensures that top node is ready to expand
template <class State>
void QueueMultiDC<State>::prepareForExpand() {
    while (!m_pq.empty()) {
        bool isDuplicate = false;
        for (DuplicityChecker<State>* dc: m_dc_checks) {
            if (dc->getDuplicityB(m_pq.top())) {
                isDuplicate = true;
                break;
            }
        }
        if (isDuplicate) {
            m_pq.pop();
        }
        else {
            break;
        }
    }
}

// Makes sure settings are correct
template <class State>
void QueueMultiDC<State>::assertCorrectSettings() {
    if (m_logger == nullptr) {
        cout << "Warning: Queue " << m_name << " has no logger" << endl;
    }
    m_hf->assertCorrectSettings();
    m_pf->assertCorrectSettings();

    for (DuplicityChecker<State>* dc: m_dc_updates) {
        dc->assertCorrectSettings();
    }
    for (DuplicityChecker<State>* dc: m_dc_checks) {
        dc->assertCorrectSettings();
    }

    m_ap->assertCorrectSettings();
}

///////////////////////////////////////////////////////////////////////
///////////////////////// BatchQueue Below ////////////////////////////

// template <class State>
// BatchQueue<State>::BatchQueue(string qName): Queue<State>(qName), m_ancFThresh(0) {
// }

template <class State>
BatchQueue<State>::BatchQueue(const string& qName, SimpleLogger* logger, 
            HF_Template<State>* hf, PriorityFunction* pf,
            DuplicityChecker<State>* dc_check, 
            vector<DuplicityChecker<State>*> dc_updates,
            AP_Template<State>* ap, int batchsize, bool expandFullBatch):
        Queue<State>(qName, logger, hf, pf, dc_check, dc_updates, ap), 
        m_batchsize(batchsize), m_expandFullBatch(expandFullBatch) {
}

template <class State>
void BatchQueue<State>::insert(shared_ptr<NodeT>& n) {
    QNodeT qn(n->ancF, n); // m_openpq is sorted on anchorF value
    m_openpq.push(qn); // Push to m_openpq, not m_pq
}

// Side effects: Updates m_ancFThresh, m_pq, m_openpq
template <class State>
bool BatchQueue<State>::canExpand(double ancFThresh) {
    m_ancFThresh = max(m_ancFThresh, ancFThresh);
    prepareForExpand();
    if (m_expandFullBatch) {
        return m_pq.size() >= m_batchsize;
    }
    else {
        return m_pq.size() > 0;
    }
}

// Moves QNodes from m_openpqm to waitlist, computes batch heuristic and adds to m_pq if needed
template <class State>
void BatchQueue<State>::prepareForExpand() {
    // cout << "Before " << m_pq.size() << " " << m_computeSlowHF_waitlist.size() << " " << m_openpq.size() << endl;
    while (!m_openpq.empty()) {
        if (m_dc_check->getDuplicityB(m_openpq.top())) {
            m_openpq.pop();
        }
        else { // The top node in m_openpq is ready to expand
            //// If this node satisfies threshold bound, put it into pq and continue.
            ////   If not, all other nodes have higher ancF and we don't need to check anymore
            shared_ptr<NodeT> const n = m_openpq.top().n;
            if (n->ancF <= m_ancFThresh) {
                m_computeSlowHF_waitlist.push_back(n); // Put Node from m_openpq to m_computeSlowHF_waitlist
                m_openpq.pop(); // Remove QNode from m_openpq
            }
            else {
                break;
            }
        }
    }

    //// Compute heuristic of elements in waitlist in batches and move them to m_pq (and remove from waitlist)
    while ((!m_expandFullBatch && m_computeSlowHF_waitlist.size() > 0) || // If we can expand something
                m_expandFullBatch && m_computeSlowHF_waitlist.size() >= m_batchsize) { // If we can expand a full batch
        
        //// Get a maximum of m_batchsize states to compute heuristic on
        vector<State> states;
        for (int i = 0; i < m_computeSlowHF_waitlist.size() && i < m_batchsize; ++i) {
            states.push_back(m_computeSlowHF_waitlist[i]->s);
        }
        
        // Get the batch values, calls the (slow) batch heuristic
        vector<double> hvals = m_hf->getBatchHeuristic(states);

        //// Insert into m_pq
        for (int i = 0; i < m_computeSlowHF_waitlist.size() && i < m_batchsize; ++i) {
            double f = m_pf->getPriority(m_computeSlowHF_waitlist[i]->g, hvals[i]); // Compute f value
            m_pq.push(QNodeT(f, m_computeSlowHF_waitlist[i])); // Push nodes into m_pq
        }

        // Erase the elements from m_computeSlowHF_waitlist
        m_computeSlowHF_waitlist.erase(m_computeSlowHF_waitlist.begin(), m_computeSlowHF_waitlist.begin()+states.size());
    }
    // cout << "After: " << m_pq.size() << " " << m_computeSlowHF_waitlist.size() << " " << m_openpq.size() << endl;

    Queue<State>::prepareForExpand();
}

template <class State>
void BatchQueue<State>::assertCorrectSettings() {
    assert(m_ancFThresh == 0);
    assert(m_batchsize > 0);
    Queue<State>::assertCorrectSettings();
}


//////////////////////////////////////////////////////////////////
///////////////////////// FocalQueue Below ////////////////////////////

template <class State>
FocalQueue<State>::FocalQueue(const string& qName): Queue<State>(qName), m_ancFThresh(0) {

}
template <class State>
FocalQueue<State>::~FocalQueue() {

}

template <class State>
void FocalQueue<State>::insert(shared_ptr<NodeT>& n) {
    QNodeT qn(n->ancF, n); // m_openpq is sorted on anchorF value
    m_openpq.push(qn); // Push to m_openpq, not m_pq
}

// Side effects: Updates m_ancFThresh, m_pq, m_openpq
template <class State>
bool FocalQueue<State>::canExpand(double ancFThresh) {
    m_ancFThresh = max(m_ancFThresh, ancFThresh);
    prepareForExpand();
    return !m_pq.empty();
}

// Moves QNodes from m_openpq to m_pq if satisfies m_ancFThresh
template <class State>
void FocalQueue<State>::prepareForExpand() {
    while (!m_openpq.empty()) {
        if (m_dc_check->getDuplicityB(m_openpq.top())) {
            m_openpq.pop();
        }
        else { // The top node in m_openpq is ready to expand
            //// If this node satisfies threshold bound, put it into pq and continue.
            ////   If not, all other nodes have higher ancF and we don't need to check anymore
            shared_ptr<NodeT> const n = m_openpq.top().n;
            if (n->ancF <= m_ancFThresh) {
                double h = m_hf->getHeuristic(n->s);
                double f = m_pf->getPriority(n->g, h); // Compute focal f value
                m_pq.push(QNodeT(f, n)); // Put Node from m_openpq to m_pq
                m_openpq.pop(); // Remove QNode from m_openpq
            }
            else {
                break;
            }
        }
    }

    // Remove duplicates from m_pq
    while (!m_pq.empty() && m_dc_check->getDuplicityB(m_pq.top())) {
        m_pq.pop();
    }
}

template <class State>
void FocalQueue<State>::assertCorrectSettings() {
    assert(m_ancFThresh == 0);
    Queue<State>::assertCorrectSettings();
}