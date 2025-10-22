#pragma once
#include <iostream>
#include <string>
#include "LogLevel.h"


struct LogData
{
	std::string msg;
	std::string type;
	std::string location;
	LogLevel level;
	std::thread::id thr_id;

};
