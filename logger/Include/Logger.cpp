#include "Logger.h"

#include <utility>
#include "LogData.h"
#include "Formatter.h"

Logger::Logger(LogLevel level, std::string path, std::uint32_t amount, bool do_flush, std::string format)
    : queue(DEFAULT_SIZE), output_path{ std::move(path) }, amount{ amount }, end(DEFAULT_END), do_flush(do_flush), local_level{ level }, format(std::move(format)) {
    fileInit(this->amount);

    thrd = std::thread([this]() {
        LogData* msg = nullptr;
        while (!end || !queue.empty()) {
            while (queue.pop(msg)) {
                if (msg) {
                    flushMessage(*msg);
                    delete msg;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        });

    if (format == "") {
        this->format = chooseFormat(local_level);
    }

}

Logger& Logger::getInstance(LogLevel level, std::string_view path, std::uint32_t amount, bool do_flush, std::string_view format) {

    static Logger instance(level, std::string(path), amount, do_flush, std::string(format));
    return instance;
}

void Logger::fileInit(std::uint32_t amount)
{
    std::string log_dir{ "Logs" };

    bool error = false;
    if (std::filesystem::is_directory(output_path))
    {
        log_dir = output_path + "/Logs";
    }
    else if (!output_path.empty()) {
        error = true;
    }

    if (std::filesystem::is_directory(log_dir))
    {
        size_t count = std::distance(std::filesystem::directory_iterator{ log_dir }, {});
        if (count >= amount)
        {
            for (const auto& file : std::filesystem::directory_iterator(log_dir))
            {
                std::filesystem::remove(file);

                count = std::distance(std::filesystem::directory_iterator{ log_dir }, {});
                if (count < amount) {
                    break;
                }
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

    output_path = std::move(buff_name);

    if (error) {
        log(std::format("{:%d-%m-%y-%H_%M_%S}", std::chrono::system_clock::now()), "invalid output path, default will be used", "[WARNING]", LOG_GET_FUNC(), local_level, std::this_thread::get_id());
    }
}

void Logger::shutDown() {
    end = true;
    if (thrd.joinable()) {
        thrd.join();
    }
    file.close();
}

Logger::~Logger() {
    shutDown();
}


void Logger::setFormat(std::string format) {
    std::unique_lock lock(mutex);
    this->format = std::move(format);
}

std::string Logger::getFormat() const {
    std::shared_lock lock(mutex);
    return format;
}

std::string Logger::chooseFormat(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Prod:
        return FORMAT_PROD;
    case LogLevel::Debug:
        return FORMAT_DEBUG;
    case LogLevel::Trace:
        return FORMAT_TRACE;
    default:
        return FORMAT_NO;
    }
}

void Logger::setOutputPath(std::string path)
{

    std::unique_lock lock{ mutex };
    output_path = std::move(path);
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

void Logger::setFlush(bool if_flush) {
    do_flush = if_flush;
}


bool Logger::blockLog(LogLevel level)
{
    return static_cast<std::underlying_type<LogLevel>::type>(level) > \
        static_cast<std::underlying_type<LogLevel>::type>(local_level);
}

std::string Logger::getLevelName(LogLevel level) {
    switch (level) {
    case LogLevel::Trace:
        return "TRACE";
    case LogLevel::Debug:
        return "DEBUG";
    case LogLevel::Prod:
        return "PROD";
    default:
        return "NONE";
    }
}

void Logger::writeLogToFile(const LogData& data) {

    std::string file_output = std::vformat(data.format, std::make_format_args(data));

    file << file_output << std::endl;
    file.flush();
}

void Logger::writeLogToConsole(const LogData& data) {
    const ConsoleLog content = ConsoleLog{ data };
    std::string console_output = std::vformat(data.format, std::make_format_args(content));

    std::cout << console_output << std::endl;
}


void Logger::flushMessage(const LogData& data)
{


    if (static_cast<int>(local_level) == 0 || static_cast<int>(data.level)==0) {
        return;
    }


    if (blockLog(data.level)) {
        return;
    }

    writeLogToFile(data);
    if (do_flush) {
        writeLogToConsole(data);
    }


}

std::vector<std::string> Logger::readAllLogs() const {
    std::shared_lock lock(mutex);

    std::ifstream file(output_path);
    std::vector<std::string> lines;
    std::string line;

    if (!file.is_open()) {
        LOG_ERROR(LogLevel::Prod) << "ERROR: cannot open log file: " << output_path;
        return lines;
    }

    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

std::vector<std::string> Logger::readLogsByKeyword(const std::string& keyword) const {
    std::shared_lock lock(mutex);

    std::ifstream file(output_path);
    std::vector<std::string> filtered;
    std::string line;

    if (!file.is_open()) {
        LOG_ERROR(LogLevel::Prod) << "ERROR: cannot open log file: " << output_path;
        return filtered;
    }

    while (std::getline(file, line)) {
        if (line.find(keyword) != std::string::npos) {
            filtered.push_back(line);
        }
    }
    return filtered;
}

void Logger::operator+=(const LogData& data) {
    log(data);
}

void Logger::log(LogData data) {
    LogData* msg = new LogData{ std::move(data) };
    while (!queue.push(msg)) {
        std::this_thread::yield();
    }
}

void Logger::log(std::string timestamp, std::string str, std::string type, std::string location,
    LogLevel level, std::thread::id id = std::this_thread::get_id())
{
    LogData* msg = new LogData{std::move(timestamp), std::move(str), std::move(type), std::move(location), level, id, format };
    while (!queue.push(msg)) {
        std::this_thread::yield();
    }
}
