#include "Profiler.h"

#include <iostream>
#include <filesystem>

utils::Profiler::Profiler()
	: m_sessionName("")
	, m_defaultSaveDir("profiles")
{

}

utils::Profiler& utils::Profiler::GetInstance()
{
	static Profiler profiler;
	return profiler;
}

void utils::Profiler::Begin(const std::string& session_name)
{
	m_sessionName = session_name;
}

void utils::Profiler::End()
{
	auto current_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();

	SaveProfile(m_sessionName + '_' + std::to_string(current_time) + ".json");
}

void utils::Profiler::WriteProfile(const ProfileResult& result)
{
	SpinLock lock;

	m_results.push_back(result);
}

void utils::Profiler::SaveProfile(const std::string& filepath)
{
	SpinLock lock;

	if (!std::filesystem::exists(m_defaultSaveDir) || !std::filesystem::is_directory(m_defaultSaveDir))
	{
		std::filesystem::create_directory(m_defaultSaveDir);
	}

	std::fstream output_file;
	output_file.open(m_defaultSaveDir + '/' + filepath, std::ios::out);
	if (!output_file.is_open())
	{
		std::cout << "Failed to open file in utils::Profiler::SaveProfile\n";
		return;
	}

	output_file << "{\"otherData\": {\"version\":\"Profiler v0.1\",\"session_name\":\"" << m_sessionName << "\"},\"traceEvents\":[";

	bool first_entry = true;
	for (auto& item : m_results)
	{
		if (!first_entry)
		{
			output_file << ",";
		}
		first_entry = false;

		output_file << "{";
		output_file << "\"cat\":\"function\",";
		output_file << "\"dur\":" << item.end - item.start << ',';
		output_file << "\"name\":\"" << item.name << "\",";
		output_file << "\"ph\":\"X\",";
		output_file << "\"pid\":0,";
		output_file << "\"tid\":" << item.thread_id << ",";
		output_file << "\"ts\":" << item.start;
		output_file << "}";
	}

	output_file << "]}";
	output_file.close();
}

void utils::Profiler::SetDefaultSaveDir(const std::string& safe_dir)
{
	m_defaultSaveDir = safe_dir;
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
