#include "SimpleLogger.h"

SimpleLogger::SimpleLogger() {

}

SimpleLogger::SimpleLogger(const string& filename, bool keepExisting) {
    setLogger(filename, keepExisting);
}

SimpleLogger::~SimpleLogger() {
    reset();
}

// Sets up logger to write to the filename
// keepExisting=True results in appending to existing file, false results in replacing
void SimpleLogger::setLogger(const string& filename, bool keepExisting) {
    reset();
    if (filename != "") {
        // Open/create file, appending or replacing as needed
        if (keepExisting) {
            m_log_fstream.open(filename, std::ios::app); // Appends
        } else {
            m_log_fstream.open(filename, std::ios::trunc); // Replaces
        }
        if (!m_log_fstream.is_open()) {
            throw std::runtime_error("Cannot open file");
        }
    }
}

// Side effects: Writes to log file
void SimpleLogger::logExpansion(const string& nodeString, const string& qName) {
    if (m_log_fstream) {
        m_log_fstream << nodeString << " " << qName << std::endl;
    }
}

// Side effects: Writes to log file
void SimpleLogger::logString(const string& s, bool newLine) {
    if (m_log_fstream) {
        m_log_fstream << s;
        if (newLine) {
            m_log_fstream << std::endl;
        }
    }
}

// Closes filestream if open
void SimpleLogger::reset() {
    if (m_log_fstream.is_open()) {
        m_log_fstream.close();
    }
}