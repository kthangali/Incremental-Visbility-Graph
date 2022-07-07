#pragma once
#include "utils.h"
#include "Node.h"

#include <fstream> // For reading/writing files

class SimpleLogger {
public:
    SimpleLogger();
    SimpleLogger(const string& filename, bool keepExisting);
    ~SimpleLogger();

    void setLogger(const string& filename, bool keepExisting);
    void logExpansion(const string& nodeString, const string& qName);
    void logString(const string& s, bool newLine=true);

    template <class T>
    void log2DVector(const vector<vector<T> >& data);

private:
    void reset();
    std::ofstream m_log_fstream;
};


template <class T>
void SimpleLogger::log2DVector(const vector<vector<T> >& data) {
    // Want to output x,y 
    string sizeString = to_string(data[0].size()) + "," + to_string(data.size());
    logString(sizeString);

    for (int y = 0; y < data.size(); ++y) {
    	for (int x = 0; x < data[y].size(); ++x) {
			logString(to_string(data[y][x])+",", false);
        }
		logString(""); // Add new line
    }
}