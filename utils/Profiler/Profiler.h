#pragma once

#include <chrono>
#include <string>
#include <thread>
#include <fstream>

#include "SpinLock.h"

#define COMBINE_HELPER(X,Y) X##Y
#define COMBINE(X,Y) COMBINE_HELPER(X,Y)

#define PROFILE_SCOPE(name) utils::ProfileTimer COMBINE(timer, __LINE__)(name)
#define PROFILE_FUNC() utils::ProfileTimer COMBINE(timer, __LINE__)(__FUNCSIG__)
#define PROFILE_BEGIN(name) utils::Profiler::GetInstance().Begin(name)
#define PROFILE_END() utils::Profiler::GetInstance().End()

namespace utils
{

struct ProfileResult
{
	std::string name;
	std::thread::id thread_id;
	long long start = 0;
	long long end = 0;
};

class Profiler
{
private:
	std::string m_sessionName;
	std::ofstream m_output;
	bool m_firstEntry;

	SpinLock m_lock;
	
public:
	Profiler();

	static Profiler& GetInstance();

	void Begin(const std::string& session_name, const std::string& filepath = "profiler_output.json");

	void End();

	void WriteProfile(const ProfileResult& result);
};

class ProfileTimer
{
private:
	std::string m_name;
	bool m_stopped;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start;

public:
	ProfileTimer(const std::string& name);

	~ProfileTimer();

	void Stop();

};

}
