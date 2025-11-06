#include "LogReader.h"
#include <fstream>
#include <regex>
#include <iostream>


LogReader::LogReader(const std::string& filename)
    : filename_(filename) {
}

 
bool LogReader::filter_by_keyword(const std::string& keyword, const std::string& line) const {

    if (line.find(keyword) != std::string::npos) {
        return true;
    }
    return false;
}

 
bool LogReader::filter_by_regex(const std::string& pattern, const std::string& line) const {

    std::regex re(pattern);

    if (std::regex_search(line, re)) {
        return true;
    }


    return false;

}

std::vector<std::string> LogReader::readAll() {
    std::lock_guard<std::mutex> guard(mutex);
    std::ifstream file(filename_);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

std::vector<std::string> LogReader::readByParameterRegex(const std::string& pattern) {
    std::lock_guard<std::mutex> guard(mutex);
    std::ifstream file(filename_);
    std::vector<std::string> filtered;
    std::regex re(pattern);
    std::string line;

    while (std::getline(file, line)) {
        if (std::regex_search(line, re)) {
            filtered.push_back(line);
        }
    }

    return filtered;
}

std::vector<std::string> LogReader::readByParameter(const std::string& parameter) {
    std::lock_guard<std::mutex> guard(mutex);
    std::ifstream file(filename_);
    std::vector<std::string> filtered;
    std::string line;
    while (std::getline(file, line)) {
        if (filter_by_keyword(parameter, line)) {
            filtered.push_back(line);

        }
    }
    return filtered;
}

LogReader::~LogReader() {
     //filename_.close();
}
