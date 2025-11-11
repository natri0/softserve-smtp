#include "Profiler.h"

utils::Profiler::Profiler()
	: m_sessionName("")
	, m_firstEntry(true)
{

}

utils::Profiler& utils::Profiler::GetInstance()
{
	static Profiler profiler;
	return profiler;
}

void utils::Profiler::Begin(const std::string& session_name, const std::string& filepath /*= "profiler_output.json"*/)
{
	m_firstEntry = true;
	m_sessionName = session_name;
	m_output.open(filepath);
	if (!m_output.is_open())
	{
		// log error
		//return;
	}

	m_output << "{\"otherData\": {\"version\":\"Profiler v0.1\",\"session_name\":\"" << session_name << "\"},\"traceEvents\":[";
}

void utils::Profiler::End()
{
	m_output << "]}";
	m_output.close();
	m_firstEntry = true;
}

void utils::Profiler::WriteProfile(const ProfileResult& result)
{
	m_lock.lock();

	if (!m_firstEntry)
		m_output << ",";

	m_output << "{";
	m_output << "\"cat\":\"function\",";
	m_output << "\"dur\":" << result.end - result.start << ',';
	m_output << "\"name\":\"" << result.name << "\",";
	m_output << "\"ph\":\"X\",";
	m_output << "\"pid\":0,";
	m_output << "\"tid\":" << result.thread_id << ",";
	m_output << "\"ts\":" << result.start;
	m_output << "}";

	m_firstEntry = false;

	m_lock.unlock();
}

utils::ProfileTimer::ProfileTimer(const std::string& name) 
	: m_name(name)
	, m_stopped(false)
{
	m_start = std::chrono::high_resolution_clock::now();
}

utils::ProfileTimer::~ProfileTimer()
{
	if (!m_stopped)
		Stop();
}

void utils::ProfileTimer::Stop()
{
	auto end = std::chrono::high_resolution_clock::now();

	ProfileResult result;
	result.start = std::chrono::time_point_cast<std::chrono::microseconds>(m_start)
		.time_since_epoch()
		.count();
	result.end = std::chrono::time_point_cast<std::chrono::microseconds>(end)
		.time_since_epoch()
		.count();
	result.thread_id = std::this_thread::get_id();
	result.name = m_name;

	Profiler::GetInstance().WriteProfile(result);

	m_stopped = true;
}
