#include "Macros.h"
#include "Logger.h"

//std::unique_ptr<Logger> Logger::instance = nullptr;

Logger::Logger(const LogLevel& level, const std::string& path, const unsigned int amount, const bool do_flush)
    : queue(DEFAULT_SIZE), local_level{ level}, output_path{ path }, amount{ amount }, end(DEFAULT_END), do_flush(do_flush) {
    fileInit(this->amount);

    thrd = std::thread([this]() {
        LogData* msg = nullptr;
        while (!end || !queue.empty()) {
            while (queue.pop(msg)) {
                if (msg) {
                    flushMessage(*msg, false);
                    delete msg;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        });

}

Logger& Logger::getInstance(const LogLevel& level, const std::string& path, const unsigned int amount, const bool do_flush) {

    static Logger instance(level, path, amount, do_flush);
    return instance;
}

void Logger::fileInit(const unsigned int amount)
{
    std::string log_dir{ "Logs" };

    bool error = 0;
    if (std::filesystem::is_directory(output_path))
    {
        log_dir = output_path + "/Logs";
    }
    else if (output_path != "")error = 1;

    if (std::filesystem::is_directory(log_dir))
    {
        size_t count = std::distance(std::filesystem::directory_iterator{ log_dir }, {});
        if (count >= amount)
        {
            for (const auto& file : std::filesystem::directory_iterator(log_dir))
            {
                std::filesystem::remove(file);

                count = std::distance(std::filesystem::directory_iterator{ log_dir }, {});
                if (count < amount) break;
            }
        }
    }
    else
    {
        std::filesystem::create_directory(log_dir);
    }

    std::string buff_name = log_dir + "/log_";
    buff_name += std::format("{:%d-%m-%y-%H_%M_%S}", std::chrono::system_clock::now()) + ".txt";

    file = std::ofstream{ buff_name };

    output_path = buff_name;

    if (error)
        log("invalid output path, default will be used", "[WARNING]", FUNCTION_NAME, local_level, std::this_thread::get_id());
}

void Logger::shutDown() {
    end = true;
    if (thrd.joinable())
        thrd.join();
    file.close();
}

Logger::~Logger() {
    shutDown();
}

void Logger::setOutputPath(const std::string& path)
{

    std::unique_lock lock{ mutex };
    output_path = path;
}

const std::string& Logger::getOutputPath() const
{

    std::shared_lock lock{ mutex };
    return output_path;
}

void Logger::setLevel(LogLevel level)
{
    std::unique_lock loc{ mutex };
    local_level = level;
}

const LogLevel& Logger::getLevel() const
{
    std::shared_lock lock{ mutex };
    return local_level;
}

void Logger::setFlush(const bool if_flush) {
    do_flush = if_flush;
}

bool Logger::blockLog(LogLevel level)
{
    return static_cast<std::underlying_type<LogLevel>::type>(level) > \
        static_cast<std::underlying_type<LogLevel>::type>(local_level);
}

std::string Logger::toString(LogLevel level) {
    switch (level) {
    case LogLevel::TRACE:   return "TRACE";
    case LogLevel::DEBUG: return "DEBUG";
    case LogLevel::PROD:    return "PROD";
    default:                return "NONE";
    }
}

void  Logger::write_log_to_file(const LogData& data) {
    std::string file_output;

    file_output += std::format("{:%H:%M:%S-%d.%m.%y}", std::chrono::system_clock::now());
    file_output += '\t';
    file_output += data.type;
    file_output += '\t';
    file_output += toString(data.level);
    file_output += '\t';
    file_output += data.location;
    file_output += '\t';
    file_output += "(thread ";
    file_output += std::to_string(std::hash<std::thread::id>{}(data.thr_id));
    file_output += ")";
    file_output += '\t';
    file_output += data.msg;
    file_output += '\n';

    file << file_output;
    file.flush();
}

void write_log_to_console(const LogData& data) {
    std::string console_output;

    console_output += std::format("{:%H:%M:%S-%d.%m.%y}", std::chrono::system_clock::now());
    console_output += '\t';

    auto it = colored.find(data.type);
    if (it != colored.end())
        console_output += it->second + data.type + DEFAULT_COLOR;
    else
        console_output += data.type;

    console_output += '\t';
    console_output += data.location;
    console_output += '\t';
    console_output += "(thread ";
    console_output += std::to_string(std::hash<std::thread::id>{}(data.thr_id));
    console_output += ")";
    console_output += '\t';
    console_output += data.msg;
    console_output += '\n';

    std::cout<< console_output;

}

void Logger::flushMessage(const LogData& data, bool if_flush)
{
    if (blockLog(data.level))
        return;

    if (static_cast<int>(local_level) == 0) return;


    

    if (do_flush)  write_log_to_console(data);
    write_log_to_file(data);
   
}

void Logger::operator+=(const LogData& data) {
    log(data);
}

void Logger::log(const LogData& data) {
    LogData* msg = new LogData{ data };
    while (!queue.push(msg)) {
        std::this_thread::yield();
    }
}
void Logger::log(const std::string& str, const std::string& type, const std::string& location,
    const LogLevel& level, std::thread::id id = std::this_thread::get_id())
    //void* ptr_this = nullptr)
{
    LogData* msg = new LogData{ str, type, location, level, id };//, ptr_this };
    while (!queue.push(msg)) {
        std::this_thread::yield();
    }
}


void Logger::logError(const std::string& msg)
{
    log(msg, "[ERROR]", LOG_GET_FUNC(), local_level);
}

void Logger::logWarning(const std::string& msg)
{
    log(msg, "[WARNING]", LOG_GET_FUNC(), local_level);
}

void Logger::logInfo(const std::string& msg)
{
    log(msg, "[INFO]", LOG_GET_FUNC(), local_level);
}

void Logger::logFuncStart() {

    if (static_cast<int>(local_level) >= 2)
        log("Function is started", "[INFO]", LOG_GET_FUNC(), local_level);
}

void Logger::logFuncEnd()
{
    if (static_cast<int>(local_level) >= 2)
        log("Function is successfully executed", "[INFO]", LOG_GET_FUNC(), local_level);
}