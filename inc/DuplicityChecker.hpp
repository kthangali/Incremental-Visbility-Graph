//////////////////////////////////////////////////////////////////
////////////////////// DuplicityChecker Below ///////////////////////
template <class State>
double DuplicityChecker<State>::getDuplicityD(const QNodeT& qn) {
    notImplemented("DuplicityChecker::getDuplicityD");
    return -1;
}


//////////////////////////////////////////////////////////////////
////////////////////// DC_ClosedList Below ///////////////////////
template <class State>
DC_ClosedList<State>::DC_ClosedList() {

}

template <class State>
DC_ClosedList<State>::~DC_ClosedList() {
    seenStates.clear();
}

template <class State>
void DC_ClosedList<State>::updateDuplicity(const QNodeT& qn) {
    seenStates.insert(qn);
}

// Returns true if duplicate, false otherwise
template <class State>
bool DC_ClosedList<State>::getDuplicityB(const QNodeT& qn) {
    auto search = seenStates.find(qn);
    return search != seenStates.end();
}

template <class State>
void DC_ClosedList<State>::assertCorrectSettings() {
    // Everything is fine by default
}

template <class State>
void DC_ClosedList<State>::reset() {
    seenStates.clear();
}


//////////////////////////////////////////////////////////////////
////////////////////// DC_ClosedListReopen Below ///////////////////////
template <class State>
DC_ClosedListReopen<State>::DC_ClosedListReopen() {
}

template <class State>
DC_ClosedListReopen<State>::~DC_ClosedListReopen() {
}

template <class State>
void DC_ClosedListReopen<State>::updateDuplicity(const QNodeT& qn) {
    auto insertion_result = seenStates.insert(qn);
    if (!insertion_result.second) { // If failed to insert because existing element
        if (insertion_result.first->f > qn.f) { // If new qn is better (lower f)
            seenStates.erase(insertion_result.first); // Removes old element
            seenStates.insert(qn); // Inserts new one
        }
    }
}

// Returns true if duplicate, false otherwise
template <class State>
bool DC_ClosedListReopen<State>::getDuplicityB(const QNodeT& qn) {
    auto search = seenStates.find(qn);
    if (search != seenStates.end()) {
        return search->f <= qn.f;
    }
    return false;
}

template <class State>
void DC_ClosedListReopen<State>::assertCorrectSettings() {
    // Everything is fine by default
}

template <class State>
void DC_ClosedListReopen<State>::reset() {
    seenStates.clear();
}