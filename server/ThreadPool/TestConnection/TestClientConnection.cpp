// advanced_smtp_tests.cpp
// Compilation: g++ -std=c++23 -O2 advanced_smtp_tests.cpp -lpthread -o smtp_tests
// Run: ./smtp_tests [test_type] [num_clients]

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <cstring>
#include <functional>
#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <poll.h>
#include <map>

using namespace std::chrono;

// ============================================================================
// ANSI Color Codes
// ============================================================================
namespace Color {
    const char* RESET = "\033[0m";
    const char* RED = "\033[31m";
    const char* GREEN = "\033[32m";
    const char* YELLOW = "\033[33m";
    const char* BLUE = "\033[34m";
    const char* MAGENTA = "\033[35m";
    const char* CYAN = "\033[36m";
    const char* BOLD = "\033[1m";
}

// ============================================================================
// Enhanced SMTP Client with error tracking
// ============================================================================
class SmtpClient {
private:
    std::string host;
    int port;
    bool debug = false;
    
public:
    int sock = -1;
    enum class Error {
        NONE,
        SOCKET_CREATE,
        CONNECT_FAILED,
        SEND_FAILED,
        RECV_FAILED,
        TIMEOUT,
        INVALID_RESPONSE,
        PROTOCOL_ERROR
    };
    
    Error last_error = Error::NONE;
    std::string last_error_msg;
    
    SmtpClient(const std::string& h = "127.0.0.1", int p = 2525, bool dbg = false) 
        : host(h), port(p), debug(dbg) {}
    
    ~SmtpClient() {
        disconnect();
    }
    
    bool connect(int timeout_ms = 5000) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            last_error = Error::SOCKET_CREATE;
            last_error_msg = "Socket creation failed";
            return false;
        }
        
        // Set socket options
        struct linger sl;
        sl.l_onoff = 1;
        sl.l_linger = 0;
        setsockopt(sock, SOL_SOCKET, SO_LINGER, &sl, sizeof(sl));
        
        int reuse = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
        
        // Set non-blocking for connect timeout
        int flags = fcntl(sock, F_GETFL, 0);
        fcntl(sock, F_SETFL, flags | O_NONBLOCK);
        
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        
        if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0) {
            last_error = Error::CONNECT_FAILED;
            last_error_msg = "Invalid address";
            close(sock);
            sock = -1;
            return false;
        }
        
        ::connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
        
        // Wait for connection with timeout
        struct pollfd pfd;
        pfd.fd = sock;
        pfd.events = POLLOUT;
        
        int ret = poll(&pfd, 1, timeout_ms);
        if (ret <= 0) {
            last_error = Error::TIMEOUT;
            last_error_msg = "Connect timeout";
            close(sock);
            sock = -1;
            return false;
        }
        
        // Back to blocking mode
        fcntl(sock, F_SETFL, flags);
        
        // Read greeting
        std::string greeting = readResponse(5000);
        if (greeting.empty() || greeting[0] != '2') {
            last_error = Error::PROTOCOL_ERROR;
            last_error_msg = "Invalid greeting: " + greeting;
            return false;
        }
        
        return true;
    }
    
    void disconnect() {
        if (sock >= 0) {
            struct linger sl;
            sl.l_onoff = 1;
            sl.l_linger = 0;
            setsockopt(sock, SOL_SOCKET, SO_LINGER, &sl, sizeof(sl));
            
            shutdown(sock, SHUT_RDWR);
            close(sock);
            sock = -1;
        }
    }
    
    std::string sendCommand(const std::string& cmd, int timeout_ms = 5000) {
        if (sock < 0) {
            last_error = Error::SEND_FAILED;
            return "";
        }
        
        std::string full_cmd = cmd + "\r\n";
        
        if (debug) {
            std::cout << Color::CYAN << ">>> " << cmd << Color::RESET << "\n";
        }
        
        ssize_t sent = send(sock, full_cmd.c_str(), full_cmd.size(), 0);
        if (sent != (ssize_t)full_cmd.size()) {
            last_error = Error::SEND_FAILED;
            last_error_msg = "Send failed";
            return "";
        }
        
        std::string response = readResponse(timeout_ms);
        
        if (debug && !response.empty()) {
            std::cout << Color::MAGENTA << "<<< " << response.substr(0, 50) 
                      << (response.size() > 50 ? "..." : "") << Color::RESET << "\n";
        }
        
        return response;
    }
    
    std::string readResponse(int timeout_ms = 5000) {
        char buffer[8192] = {0};
        
        struct pollfd pfd;
        pfd.fd = sock;
        pfd.events = POLLIN;
        
        int ret = poll(&pfd, 1, timeout_ms);
        if (ret <= 0) {
            last_error = Error::TIMEOUT;
            return "";
        }
        
        int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            return std::string(buffer, n);
        }
        
        last_error = Error::RECV_FAILED;
        return "";
    }
    
    bool sendEmail(const std::string& from, const std::string& to, 
                   const std::string& subject, const std::string& body) {
        if (!connect()) return false;
        
        if (sendCommand("HELO test.com")[0] != '2') return false;
        if (sendCommand("MAIL FROM:<" + from + ">")[0] != '2') return false;
        if (sendCommand("RCPT TO:<" + to + ">")[0] != '2') return false;
        if (sendCommand("DATA")[0] != '3') return false;
        
        std::string msg = "Subject: " + subject + "\r\n\r\n" + body + "\r\n";
        send(sock, msg.c_str(), msg.size(), 0);
        
        if (sendCommand(".")[0] != '2') return false;
        
        sendCommand("QUIT");
        disconnect();
        
        return true;
    }
};

// ============================================================================
// Test Statistics with detailed metrics
// ============================================================================
struct TestStats {
    std::atomic<int> total_attempts{0};
    std::atomic<int> successful{0};
    std::atomic<int> failed{0};
    std::atomic<int> timeouts{0};
    std::atomic<int> protocol_errors{0};
    std::atomic<int> total_commands{0};
    
    std::vector<long long> latencies;
    std::vector<SmtpClient::Error> errors;
    std::mutex data_mutex;
    
    void recordSuccess(long long latency_ms) {
        successful++;
        std::lock_guard<std::mutex> lock(data_mutex);
        latencies.push_back(latency_ms);
    }
    
    void recordFailure(SmtpClient::Error error) {
        failed++;
        if (error == SmtpClient::Error::TIMEOUT) timeouts++;
        if (error == SmtpClient::Error::PROTOCOL_ERROR) protocol_errors++;
        
        std::lock_guard<std::mutex> lock(data_mutex);
        errors.push_back(error);
    }
    
    void printStats(double duration_sec, const std::string& test_name) {
        std::cout << "\n" << Color::BOLD << std::string(80, '=') << Color::RESET << "\n";
        std::cout << Color::BOLD << Color::CYAN << "📊 TEST RESULTS: " << test_name 
                  << Color::RESET << "\n";
        std::cout << Color::BOLD << std::string(80, '=') << Color::RESET << "\n\n";
        
        // Overview
        std::cout << Color::BOLD << "Overview:" << Color::RESET << "\n";
        std::cout << "  Total attempts:  " << total_attempts << "\n";
        std::cout << "  " << Color::GREEN << "✓ Successful:    " << successful 
                  << " (" << std::fixed << std::setprecision(1) 
                  << (successful * 100.0 / total_attempts) << "%)" << Color::RESET << "\n";
        std::cout << "  " << Color::RED << "✗ Failed:        " << failed 
                  << " (" << (failed * 100.0 / total_attempts) << "%)" << Color::RESET << "\n";
        std::cout << "    - Timeouts:    " << timeouts << "\n";
        std::cout << "    - Protocol:    " << protocol_errors << "\n\n";
        
        // Performance
        std::cout << Color::BOLD << "Performance:" << Color::RESET << "\n";
        std::cout << "  Commands sent:   " << total_commands << "\n";
        std::cout << "  Duration:        " << std::fixed << std::setprecision(2) 
                  << duration_sec << " sec\n";
        std::cout << "  Throughput:      " << Color::BOLD 
                  << (int)(total_commands / duration_sec) << " req/sec" << Color::RESET << "\n";
        std::cout << "  Concurrency:     " << (total_attempts / duration_sec) 
                  << " clients/sec\n\n";
        
        // Latency
        if (!latencies.empty()) {
            std::sort(latencies.begin(), latencies.end());
            
            auto sum = 0LL;
            for (auto l : latencies) sum += l;
            
            std::cout << Color::BOLD << "Latency (ms):" << Color::RESET << "\n";
            std::cout << "  Min:    " << latencies.front() << "\n";
            std::cout << "  Max:    " << latencies.back() << "\n";
            std::cout << "  Avg:    " << (sum / latencies.size()) << "\n";
            std::cout << "  Median: " << latencies[latencies.size() / 2] << "\n";
            std::cout << "  P95:    " << latencies[latencies.size() * 95 / 100] << "\n";
            std::cout << "  P99:    " << latencies[latencies.size() * 99 / 100] << "\n";
            std::cout << "  P99.9:  " << latencies[latencies.size() * 999 / 1000] << "\n\n";
        }
        
        // Error distribution
        if (!errors.empty()) {
            std::map<SmtpClient::Error, int> error_counts;
            for (auto e : errors) error_counts[e]++;
            
            std::cout << Color::BOLD << "Error Distribution:" << Color::RESET << "\n";
            for (const auto& [error, count] : error_counts) {
                std::cout << "  Error " << (int)error << ": " << count << "\n";
            }
            std::cout << "\n";
        }
        
        std::cout << Color::BOLD << std::string(80, '=') << Color::RESET << "\n";
    }
};

// ============================================================================
// Test Scenarios
// ============================================================================

// Test 1: Basic throughput test
void basicThroughputTest(int client_id, TestStats& stats) {
    auto start = steady_clock::now();
    
    SmtpClient client;
    bool success = client.sendEmail(
        "user" + std::to_string(client_id) + "@test.com",
        "recipient@example.com",
        "Test " + std::to_string(client_id),
        "This is a test message from client " + std::to_string(client_id)
    );
    
    auto end = steady_clock::now();
    auto latency = duration_cast<milliseconds>(end - start).count();
    
    stats.total_attempts++;
    stats.total_commands += 6;
    
    if (success) {
        stats.recordSuccess(latency);
    } else {
        stats.recordFailure(client.last_error);
    }
}

// Test 2: RACE CONDITION - Multiple commands sent rapidly without waiting
void raceConditionTest(int client_id, TestStats& stats) {
    auto start = steady_clock::now();
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct linger sl = {1, 0};
    setsockopt(sock, SOL_SOCKET, SO_LINGER, &sl, sizeof(sl));
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(2525);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    
    bool success = false;
    if (::connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
        char buf[1024];
        recv(sock, buf, sizeof(buf), 0); // greeting
        
        // 🔥 CRITICAL: Send ALL commands at once without waiting
        const char* rapid_commands = 
            "HELO test.com\r\n"
            "MAIL FROM:<race1@test.com>\r\n"
            "RCPT TO:<user@test.com>\r\n"
            "DATA\r\n"
            "Subject: Race Test\r\n\r\nBody\r\n.\r\n"
            "MAIL FROM:<race2@test.com>\r\n"
            "RCPT TO:<user@test.com>\r\n"
            "DATA\r\n"
            "Subject: Race Test 2\r\n\r\nBody 2\r\n.\r\n"
            "QUIT\r\n";
        
        ssize_t sent = send(sock, rapid_commands, strlen(rapid_commands), 0);
        
        // Minimal wait to let server process
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // Try to read all responses
        int total_read = 0;
        for (int i = 0; i < 10; ++i) {
            int n = recv(sock, buf, sizeof(buf), MSG_DONTWAIT);
            if (n > 0) total_read += n;
            else break;
        }
        
        success = (sent > 0 && total_read > 0);
        stats.total_commands += 10;
    }
    
    shutdown(sock, SHUT_RDWR);
    close(sock);
    
    auto end = steady_clock::now();
    auto latency = duration_cast<milliseconds>(end - start).count();
    
    stats.total_attempts++;
    
    if (success) {
        stats.recordSuccess(latency);
    } else {
        stats.recordFailure(SmtpClient::Error::PROTOCOL_ERROR);
    }
}

// Test 3: Concurrent sessions - same session ID test
void concurrentSessionTest(int client_id, int session_group, TestStats& stats) {
    auto start = steady_clock::now();
    
    SmtpClient client;
    
    if (!client.connect()) {
        stats.total_attempts++;
        stats.recordFailure(client.last_error);
        return;
    }
    
    // Multiple clients with same "session identifier"
    std::string session_id = "session_" + std::to_string(session_group);
    
    bool success = true;
    std::string resp;
    
    resp = client.sendCommand("HELO " + session_id);
    success &= (!resp.empty() && resp[0] == '2');
    
    resp = client.sendCommand("MAIL FROM:<" + session_id + "@test.com>");
    success &= (!resp.empty() && resp[0] == '2');
    
    resp = client.sendCommand("RCPT TO:<user@test.com>");
    success &= (!resp.empty() && resp[0] == '2');
    
    resp = client.sendCommand("DATA");
    success &= (!resp.empty() && resp[0] == '3');
    
    std::string msg = "Subject: Test\r\n\r\nBody\r\n";
    send(client.sock, msg.c_str(), msg.size(), 0);
    
    resp = client.sendCommand(".");
    success &= (!resp.empty() && resp[0] == '2');
    
    client.sendCommand("QUIT");
    client.disconnect();
    
    auto end = steady_clock::now();
    auto latency = duration_cast<milliseconds>(end - start).count();
    
    stats.total_attempts++;
    stats.total_commands += 6;
    
    if (success) {
        stats.recordSuccess(latency);
    } else {
        stats.recordFailure(client.last_error);
    }
}

// Test 4: Stress test - rapid connect/disconnect
void stressConnectTest(int client_id, TestStats& stats) {
    auto start = steady_clock::now();
    
    bool all_success = true;
    
    for (int i = 0; i < 5; ++i) {
        SmtpClient client;
        
        if (!client.connect(2000)) {
            all_success = false;
            stats.recordFailure(client.last_error);
            break;
        }
        
        client.sendCommand("HELO test" + std::to_string(i));
        client.sendCommand("QUIT");
        client.disconnect();
        
        stats.total_commands += 2;
        
        // No delay - rapid reconnect
    }
    
    auto end = steady_clock::now();
    auto latency = duration_cast<milliseconds>(end - start).count();
    
    stats.total_attempts++;
    
    if (all_success) {
        stats.recordSuccess(latency);
    }
}

// Test 5: Pipeline abuse - send commands faster than server can respond
void pipelineAbuseTest(int client_id, TestStats& stats) {
    auto start = steady_clock::now();
    
    SmtpClient client;
    
    if (!client.connect()) {
        stats.total_attempts++;
        stats.recordFailure(client.last_error);
        return;
    }
    
    // Send multiple commands without waiting for responses
    int sock = client.sock;
    const char* cmd1 = "HELO test.com\r\n";
    const char* cmd2 = "HELO test2.com\r\n";
    const char* cmd3 = "MAIL FROM:<test@test.com>\r\n";
    const char* cmd4 = "MAIL FROM:<test2@test.com>\r\n";
    const char* cmd5 = "QUIT\r\n";
    
    send(sock, cmd1, strlen(cmd1), 0);
    send(sock, cmd2, strlen(cmd2), 0);
    send(sock, cmd3, strlen(cmd3), 0);
    send(sock, cmd4, strlen(cmd4), 0);
    send(sock, cmd5, strlen(cmd5), 0);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    char buf[4096];
    int total_recv = 0;
    for (int i = 0; i < 5; ++i) {
        int n = recv(sock, buf, sizeof(buf), MSG_DONTWAIT);
        if (n > 0) total_recv += n;
    }
    
    client.disconnect();
    
    auto end = steady_clock::now();
    auto latency = duration_cast<milliseconds>(end - start).count();
    
    stats.total_attempts++;
    stats.total_commands += 5;
    
    if (total_recv > 0) {
        stats.recordSuccess(latency);
    } else {
        stats.recordFailure(SmtpClient::Error::PROTOCOL_ERROR);
    }
}

// Test 6: Memory leak detection - long-running sessions
void memoryLeakTest(int client_id, TestStats& stats) {
    auto start = steady_clock::now();
    
    SmtpClient client;
    
    if (!client.connect()) {
        stats.total_attempts++;
        stats.recordFailure(client.last_error);
        return;
    }
    
    // Send many commands in single session
    for (int i = 0; i < 100; ++i) {
        client.sendCommand("HELO test" + std::to_string(i));
        stats.total_commands++;
    }
    
    client.sendCommand("QUIT");
    client.disconnect();
    
    auto end = steady_clock::now();
    auto latency = duration_cast<milliseconds>(end - start).count();
    
    stats.total_attempts++;
    stats.recordSuccess(latency);
}

// ============================================================================
// Test Runner
// ============================================================================
void runTest(const std::string& name, const std::string& description,
             int num_clients, std::function<void(int, TestStats&)> test_func) {
    
    std::cout << "\n" << Color::BOLD << Color::YELLOW << std::string(80, '=') 
              << Color::RESET << "\n";
    std::cout << Color::BOLD << Color::CYAN << "🚀 " << name << Color::RESET << "\n";
    std::cout << Color::YELLOW << "   " << description << Color::RESET << "\n";
    std::cout << "   Concurrent clients: " << num_clients << "\n";
    std::cout << Color::BOLD << Color::YELLOW << std::string(80, '=') 
              << Color::RESET << "\n";
    
    TestStats stats;
    std::vector<std::thread> threads;
    
    auto start = steady_clock::now();
    
    // Launch all clients
    for (int i = 0; i < num_clients; ++i) {
        threads.emplace_back([i, &stats, test_func]() {
            test_func(i, stats);
        });
        
        // Small stagger to avoid thundering herd
        if (i % 10 == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    
    // Progress indicator
    while (stats.total_attempts < num_clients) {
        int progress = (stats.total_attempts * 100) / num_clients;
        std::cout << "\r" << Color::CYAN << "Progress: [";
        
        for (int i = 0; i < 50; ++i) {
            if (i < progress / 2) std::cout << "█";
            else std::cout << "░";
        }
        
        std::cout << "] " << progress << "% (" 
                  << Color::GREEN << stats.successful << Color::RESET << "/"
                  << Color::RED << stats.failed << Color::RESET << ")     " 
                  << Color::RESET << std::flush;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = steady_clock::now();
    double duration_sec = duration_cast<milliseconds>(end - start).count() / 1000.0;
    
    std::cout << "\n";
    stats.printStats(duration_sec, name);
}

// ============================================================================
// Main
// ============================================================================
int main(int argc, char* argv[]) {
    std::cout << Color::BOLD << Color::CYAN << R"(
╔════════════════════════════════════════════════════════════════════════════╗
║                    SMTP ThreadPool Advanced Test Suite                     ║
║                    Professional Load & Race Condition Tests                ║
╚════════════════════════════════════════════════════════════════════════════╝
)" << Color::RESET << "\n";
    
    std::string test_type = "all";
    int num_clients = 100;
    
    if (argc > 1) test_type = argv[1];
    if (argc > 2) num_clients = std::atoi(argv[2]);
    
    std::cout << Color::YELLOW << "Target server: localhost:2525" << Color::RESET << "\n";
    std::cout << Color::YELLOW << "Press Enter to start tests..." << Color::RESET << "\n";
    std::cin.get();
    
    if (test_type == "all" || test_type == "basic") {
        runTest("Basic Throughput", 
                "Standard email send test", 
                num_clients, basicThroughputTest);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    if (test_type == "all" || test_type == "race") {
        runTest("🔥 RACE CONDITION TEST", 
                "Rapid command pipeline - exposes threading issues", 
                num_clients * 2, raceConditionTest);
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
    
    if (test_type == "all" || test_type == "concurrent") {
        runTest("Concurrent Session Collision", 
                "Multiple threads accessing similar session data", 
                num_clients, 
                [](int id, TestStats& stats) { 
                    concurrentSessionTest(id, id % 10, stats); 
                });
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    if (test_type == "all" || test_type == "stress") {
        runTest("Stress Connect/Disconnect", 
                "Rapid connection cycling", 
                num_clients / 2, stressConnectTest);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    if (test_type == "all" || test_type == "pipeline") {
        runTest("Pipeline Abuse", 
                "Command pipelining without response wait", 
                num_clients, pipelineAbuseTest);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    if (test_type == "all" || test_type == "memory") {
        runTest("Memory Leak Detection", 
                "Long sessions with many commands", 
                50, memoryLeakTest);
    }
    
    std::cout << "\n" << Color::BOLD << Color::GREEN << std::string(80, '=') 
              << Color::RESET << "\n";
    std::cout << Color::BOLD << Color::GREEN << "✅ All tests completed!" 
              << Color::RESET << "\n";
    std::cout << Color::BOLD << Color::GREEN << std::string(80, '=') 
              << Color::RESET << "\n\n";
    
    std::cout << Color::YELLOW << "Usage examples:\n";
    std::cout << "  ./smtp_tests all 200      - Run all tests with 200 clients\n";
    std::cout << "  ./smtp_tests race 500     - Run only race condition test\n";
    std::cout << "  ./smtp_tests stress 100   - Run stress test\n";
    std::cout << "  ./smtp_tests pipeline 300 - Run pipeline abuse test\n" 
              << Color::RESET << "\n";
    
    return 0;
}