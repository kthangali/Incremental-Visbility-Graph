#pragma once
#include "utils.h"
#include <queue>

#include "Node.h"
#include "HeuristicFunction.h"
#include "DuplicityChecker.h"
#include "Env.h"
#include "SimpleLogger.h"
#include "PriorityFunction.h"

#include "ActionPrimitives.h"

//////////////////////////////////////////////////////////////////
///////////////////////// Queue Below ////////////////////////////

template <class State>
class Queue {
public:
    using QNodeT = QNode<State>;
    using NodeT = Node<State>;
    
    // Queue();
    // Queue(string qName);
    Queue(const string& qName, SimpleLogger* logger, 
            HF_Template<State>* hf, 
            PriorityFunction* pf, 
            DuplicityChecker<State>* dc_check, 
            vector<DuplicityChecker<State>*> dc_updates,
            AP_Template<State>* ap);
    // Fake named constructor to use later on:
    // new Queue<StateXY>(/*name=*/ , /*logger=*/ , /*hf=*/ , /*pf=*/ ,
    //     /*dc_check=*/ , /*dc_updates=*/ , /*ap=*/ );

    ~Queue();

    virtual void insert(shared_ptr<NodeT>& n);
    virtual tuple<vector<shared_ptr<NodeT>>, vector<shared_ptr<NodeT>> > expand(double ancFThresh); // Returns children, expanded
    virtual bool canExpand(double ancFThresh);
    virtual shared_ptr<NodeT> getTop();
    virtual void updateNodeWithAnchorStats(shared_ptr<NodeT>& n); // Should only be called by anchor queue
    virtual void prepareForExpand();
    virtual void assertCorrectSettings(); // Checks if all settings are correct
    virtual void reset(); // Resets search
    virtual tuple<string, string> reportStats(); // First string is header, second are values

    string m_name;
    SimpleLogger* m_logger;
    HF_Template<State>* m_hf;
    PriorityFunction* m_pf;
    DuplicityChecker<State>* m_dc_check;
    vector<DuplicityChecker<State>*> m_dc_updates;
    AP_Template<State>* m_ap;


    // bool m_admissibleQ;
protected:
    std::priority_queue<QNodeT, vector<QNodeT>, QNodeComparator<State> > m_pq; 
};

template <class State>
class QueueMultiDC: public Queue<State> {
public:
    using QNodeT = QNode<State>;
    using NodeT = Node<State>;

    using Queue<State>::m_pf;
    using Queue<State>::m_dc_check;
    using Queue<State>::m_dc_updates;
    using Queue<State>::m_hf;
    using Queue<State>::m_pq;
    using Queue<State>::m_logger;
    using Queue<State>::m_name;
    using Queue<State>::m_ap;

    QueueMultiDC(const string& qName, SimpleLogger* logger, 
            HF_Template<State>* hf, 
            PriorityFunction* pf, vector<DuplicityChecker<State>*> dc_checks, 
            vector<DuplicityChecker<State>*> dc_updates,
            AP_Template<State>* ap);
    // Fake named constructor to use later on:
    // new QueueMultiDC<StateXY>(/*name=*/ , /*logger=*/ , /*hf=*/ , /*pf=*/ ,
    //     /*dc_checks=*/ , /*dc_updates=*/ , /*ap=*/ );

    void prepareForExpand();
    void assertCorrectSettings();

    vector<DuplicityChecker<State>*> m_dc_checks;
};

//////////////////////////////////////////////////////////////////
///////////////////////// BatchQueue Below ////////////////////////////

// Expands nodes in batches
template <class State>
class BatchQueue: public Queue<State> {
public:
    using QNodeT = QNode<State>;
    using NodeT = Node<State>;

    using Queue<State>::m_pf;
    using Queue<State>::m_dc_check;
    using Queue<State>::m_hf;
    using Queue<State>::m_pq;

    // BatchQueue();
    BatchQueue(const string& qName, SimpleLogger* logger, HF_Template<State>* hf, 
            PriorityFunction* pf, DuplicityChecker<State>* dc_check, 
            vector<DuplicityChecker<State>*> dc_updates,
            AP_Template<State>* ap, int batchsize, bool expandFullBatch);
    // Fake named constructor to use later on:
    // new BatchQueue<StateXY>(/*name=*/ , /*logger=*/ , /*hf=*/ , /*pf=*/ ,
    //     /*dc_check=*/ , /*dc_updates=*/ , /*ap=*/ , 
    //     /*batchsize=*/ , /*expandFullBatch=*/ );

    ~BatchQueue();

    //// Inherited
    void insert(shared_ptr<NodeT>& n);
    bool canExpand(double ancFThresh);
    void prepareForExpand();
    void assertCorrectSettings();

    int m_batchsize;
    bool m_expandFullBatch;

private:
    double m_ancFThresh;
    std::priority_queue<QNodeT, vector<QNodeT>, QNodeComparator<State> > m_openpq;
    vector<shared_ptr<NodeT>> m_computeSlowHF_waitlist;
};

//////////////////////////////////////////////////////////////////
///////////////////////// FocalQueue Below ////////////////////////////

// Focal queue that only adds nodes if they satisfy threshold
//   As opposed to Queue which will add everything and will check the top node's ancF value
//   I.e. Queue will result in EES style canExpand(), while FocalQueue will return false if bound is not met(?) TODO: Double check
template <class State>
class FocalQueue: public Queue<State> {
public:
    using QNodeT = QNode<State>;
    using NodeT = Node<State>;

    using Queue<State>::m_pf;
    using Queue<State>::m_dc_check;
    using Queue<State>::m_hf;
    using Queue<State>::m_pq;

    FocalQueue();
    FocalQueue(const string& qName);
    ~FocalQueue();

    // Inherited
    void insert(shared_ptr<NodeT>& n);
    bool canExpand(double ancFThresh);
    void prepareForExpand();
    void assertCorrectSettings();

private:
    double m_ancFThresh;
    std::priority_queue<QNodeT, vector<QNodeT>, QNodeComparator<State> > m_openpq;
};


#include "Queue.hpp"