#pragma once
#include <string>
#include <vector>
#include <mutex>

class LogReader {
    public:
        explicit LogReader(const std::string& filename);

        std::vector<std::string> readByParameterRegex(const std::string& pattern);

        bool filter_by_keyword(const std::string& keyword, const std::string& line) const;


        bool filter_by_regex(const std::string& pattern, const std::string& line) const;

        std::vector<std::string> readAll();

        std::vector<std::string> readByParameter(const std::string& parameter);

        ~LogReader();

    private:
        std::string filename_;
        std::mutex mutex;
};
