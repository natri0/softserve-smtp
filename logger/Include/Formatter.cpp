#include "Formatter.h"

const std::unordered_map<std::string, std::string> colored{
    {"[ERROR]", ERROR_COLOR},
    {"[WARNING]", WARNING_COLOR},
    {"[INFO]", INFORMATION_COLOR},
    {"[DEFAULT]", DEFAULT_COLOR}
};


constexpr auto std::formatter<LogData>::parse(std::format_parse_context& context)
{
    auto it = context.begin();
    if (it == context.end()) return it;

    switch (*it)
    {
    case 'i': thr_id = true; break;
    case 'T': time = true; break;
    case 't': type = true; break;
    case 'l': level = true; break;
    case 'L': location = true; break;
    case 'm': text = true; break;
    default:
        throw std::format_error("Invalid placeholder");
    }

    ++it;
    if (it != context.end() && *it != '}') {
        throw std::format_error("Invalid format args.");
    }
    return it;
}

auto std::formatter<LogData>::format(const LogData& obj, auto& context) const
{
    std::ostringstream formatted;

    if (thr_id)
        formatted << "thread " << std::hash<std::thread::id>{}(obj.thr_id);

    if (time)
        formatted << std::format("{:%H.%M.%S-%d.%m.%y}", std::chrono::system_clock::now());

    if (type) formatted << obj.type;
    if (level) formatted << Logger::getLevelName(obj.level);
    if (location) formatted << obj.location;
    if (text) formatted << obj.msg;

    return std::ranges::copy(std::move(formatted).str(), context.out()).out;
}


auto std::formatter<ConsoleLog>::format(const ConsoleLog& obj, auto& context) const
{
    std::ostringstream formatted;

    if (thr_id)
        formatted << std::hash<std::thread::id>{}(obj.ref.thr_id);

    if (time)
        formatted << std::format("{:%H.%M.%S-%d.%m.%y}", std::chrono::system_clock::now());

    if (type) {
        if (auto it = colored.find(obj.ref.type); it != colored.end())
            formatted << it->second << obj.ref.type << DEFAULT_COLOR;
        else
            formatted << obj.ref.type;
    }

    if (level) formatted << Logger::getLevelName(obj.ref.level);
    if (location) formatted << obj.ref.location << " ";
    if (text) formatted << obj.ref.msg;

    return std::ranges::copy(std::move(formatted).str(), context.out()).out;
}
