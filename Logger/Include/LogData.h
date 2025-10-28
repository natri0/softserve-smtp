#pragma once
#include <iostream>
#include <string>
#include <thread>
#include "LogLevel.h"


struct LogData {
private:
    std::string msg;
    std::string type;
    std::string location;
    LogLevel level;
    std::thread::id thr_id;
    //void* object_ptr;
public:
    LogData(const std::string& m, const std::string& t, const std::string& loc,
        const LogLevel& l, std::thread::id id) //void* ptr = nullptr)
        : msg(m), type(t), location(loc), level(l), thr_id(id) { //, object_ptr(ptr) {
    }

    template<typename T>
    LogData& operator<<(const T& value) {
        std::ostringstream oss;
        oss << value;
        msg += oss.str();
        return *this;
    }

    LogData& ref()
    {
        return *this;
    }


};
