#pragma once

/**
 * !!!
 * TO SEE PROFILER OUTPUT
 * OPEN chrome://tracing AND DRAG OUTPUT JSON FILE THERE
 * !!!
 * 
 * chrome://tracing should be available in all chromium based browsers
 */

#include <chrono>
#include <string>
#include <thread>
#include <fstream>
#include <vector>

#include "SpinLock.h"

#define COMBINE_HELPER(X,Y) X##Y
#define COMBINE(X,Y) COMBINE_HELPER(X,Y)

#define PROFILE_SCOPE(name) utils::ProfileTimer COMBINE(timer, __LINE__)(name)
#define PROFILE_FUNC() utils::ProfileTimer COMBINE(timer, __LINE__)(__FUNCTION__)
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
	std::vector<ProfileResult> m_results;
	std::string m_defaultSaveDir;

public:
	Profiler();

	static Profiler& GetInstance();

	void Begin(const std::string& session_name);
	void End();

	void WriteProfile(const ProfileResult& result);
	void SaveProfile(const std::string& filepath);

	void SetDefaultSaveDir(const std::string& safe_dir);

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
