//#include "Logger.h"
//#include <iostream>
//#include <thread>
//
//void exampleFunction() {
//    /*LOG_INFO(LogLevel::DEBUG) << "Function is started";
//    LOG_INFO(LogLevel::TRACE) << "Example function started";
//    std::this_thread::sleep_for(std::chrono::milliseconds(50));
//    LOG_WARNING(LogLevel::DEBUG) << "This is a warning from exampleFunction";
//    LOG_ERROR(LogLevel::PROD) << "Error occurred in exampleFunction";
//    LOG_INFO(LogLevel::TRACE) << "Example function finished";
//    LOG_INFO(LogLevel::DEBUG) << "Function is finished";*/
//    LOG_INFO(LogLevel::Trace) << "Program started!";
//    LOG_WARNING(LogLevel::Debug) << "Potential issue detected";
//    LOG_ERROR(LogLevel::Prod) << "Critical failure!";
//}
//
//
//void multiThreadLogging() {
//
//    Logger& logger = Logger::getInstance();
//    logger.setLevel(LogLevel::Prod);
//    logger.setFlush(false);
//
//    std::thread t1([] { LOG_INFO(LogLevel::Debug) << "Thread 1 running"; });
//    std::thread t2([] { LOG_ERROR(LogLevel::Prod) << "Thread 2 error"; });
//    t1.join();
//    t2.join();
//}
//
//void readingLogs() {
//    Logger& logger = Logger::getInstance();
//    logger.setFlush(false);
//
//    auto all = logger.readAllLogs();
//    auto errors = logger.readLogsByKeyword("[INFO]");
//
//    std::cout << "--- All logs ---\n";
//    for (auto& l : all) std::cout << l << '\n';
//
//    std::cout << "--- Only [INFO] ---\n";
//    for (auto& e : errors) std::cout << e << '\n';
//}
//
//
//int main() {
//    exampleFunction();
//    //multiThreadLogging();
//
//    //readingLogs();
//
//    return 0;
//}
//

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

#include "Logger.h"
#include "LogData.h"
#include "Macros.h"

class UserData
{
public:
    int id;
    std::string name;

    UserData(int i, std::string n) : id(i), name(n) {}

    friend std::ostream& operator<<(std::ostream& os, const UserData& u)
    {
        return os << "{User id=" << u.id << ", name=" << u.name << "}";
    }
};



void workerFunction(int tid)
{
    for (int i = 0; i < 5; i++)
    {
        LOG_INFO(DEBUG) << "Thread " << tid << " iteration " << i;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}



int main()
{

    Logger& log = Logger::getInstance(
        TRACE,   
        "",    
        20,            
        true, 
        FORMAT_TRACE 
    );

    

    LOG_INFO(TRACE) << "===== Logger demo started =====";

 
    LOG_INFO(TRACE) << "Integer example: " << 42;
    LOG_INFO(TRACE) << "Double example: " << 3.1415;
    LOG_INFO(DEBUG) << "String example: Hello logging system!";


    UserData user{ 15, "Alice" };
    LOG_INFO(DEBUG) << "Custom object: " << user;

   
 
    LOG_WARNING(PROD) << "This is a warning example";
    LOG_ERROR(PROD) << "This is an error example";

    //log.setLevel(DEBUG);

    //log.setFormat(log.chooseFormat(DEBUG));
 
    LOG_INFO(NO) << "This will NOT be printed (because level None)";
    LOG_INFO(TRACE) << "This WILL be printed";

 
    log.setFormat("{0:T} {0:m}");
    LOG_INFO(DEBUG) << "Custom format applied!";

    
    LOG_INFO(TRACE) << "Starting 3 worker threads...";

    std::vector<std::thread> thr;
    for (int i = 0; i < 3; i++)
        thr.emplace_back(workerFunction, i);

    for (auto& t : thr)
        t.join();

    LOG_INFO(TRACE) << "All threads finished";

    log.setFlush(false);

    LOG_INFO(DEBUG) << "Disappearance of output in the console!";

    std::cout << "\n";

    auto all_logs = log.readAllLogs();
    std::cout << "\n--- LOG FILE CONTENT (" << all_logs.size() << " lines) ---\n";
    for (auto& line : all_logs)
        std::cout << line << "\n";

    auto filtered = log.readLogsByKeyword("Custom object");
    std::cout << "\n--- FILTERED LOGS (keyword: 'Custom object') ---\n";
    for (auto& line : filtered)
        std::cout << line << "\n";

    std::cout << "\n";

    LOG_INFO(TRACE) << "===== Logger demo finished =====";

    return 0;
}
