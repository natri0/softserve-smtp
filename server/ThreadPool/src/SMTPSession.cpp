#include "SMTPSession.hpp"

SmtpSession::SmtpSession(std::shared_ptr<asio::ip::tcp::socket> sock)
    : socket(sock)
{
    initialize();
}

void SmtpSession::initialize(){
     try {
        if (socket && socket->is_open()) {
            client_ip = socket->remote_endpoint().address().to_string();
        } else {
            client_ip = "unknown (socket closed)";
        }
    } catch (const std::exception& e) {
        //maybe loger 
        client_ip = "unknown (error)";
    }
}

bool SmtpSession::enqueueCommand(const std::string& cmd) {
    std::lock_guard<std::mutex> lock(session_mutex);
    if (closed.load(std::memory_order_acquire)) {
        return false; 
    }
    commandQueue.push(cmd);
    return true;
}

bool SmtpSession::hasNextCommand() {
    std::lock_guard<std::mutex> lock(session_mutex);
    return !commandQueue.empty();
}

std::string SmtpSession::popCommand() {
    std::lock_guard<std::mutex> lock(session_mutex);
    if (commandQueue.empty()) {
        busy.store(false, std::memory_order_release);
        return "";
    }
    std::string cmd = std::move(commandQueue.front());
    commandQueue.pop();
    return cmd;
}

// --- Busy flag ---
void SmtpSession::setBusy(bool val) {
    busy.store(val, std::memory_order_release);
}

bool SmtpSession::isBusy() {
    return busy.load(std::memory_order_relaxed); 
}

bool SmtpSession::compareBusy() {
    bool expected = false;
    return busy.compare_exchange_strong(expected, true, 
                                        std::memory_order_acq_rel,
                                        std::memory_order_acquire);
}

bool SmtpSession::releaseIfEmpty()
{
    std::lock_guard<std::mutex> lock(session_mutex);
    if (commandQueue.empty()) {
        busy.store(false, std::memory_order_release);
        return true;
    }
    return false;
}

bool SmtpSession::clearQueue(){
    std::lock_guard<std::mutex> lock(session_mutex);
    if(commandQueue.empty()) return false;
    std::queue<std::string> empty;
    std::swap(commandQueue, empty);
    return true;
}

void SmtpSession::close() {

    if (closed.exchange(true, std::memory_order_acq_rel)) return;
    
    busy.store(false, std::memory_order_release);

    clearQueue();
    {
        std::lock_guard<std::mutex> lock(socket_mutex);
        if (socket && socket->is_open()) {
            std::error_code ec;
            socket->shutdown(asio::ip::tcp::socket::shutdown_both, ec);
            socket->close(ec);
        }
    }
}

bool SmtpSession::isClosed() const {
    return closed.load(std::memory_order_acquire);
}

// --- Socket & info ---
std::shared_ptr<asio::ip::tcp::socket> SmtpSession::getSocket() { 
    std::lock_guard<std::mutex> lock(socket_mutex);
    if (closed.load(std::memory_order_acquire)) {
        return nullptr; 
    }
    return socket; 
}

const std::string SmtpSession::getClientIp() const noexcept{ 
    return client_ip; 
}

// --- SMTP state ---
SmtpSession::SessionStatus SmtpSession::getStatus() const { 
    std::lock_guard<std::mutex> lock(session_mutex);
    return current_state; 
}
void SmtpSession::setStatus(SessionStatus st) { 
    std::lock_guard<std::mutex> lock(session_mutex);
    current_state = st; 
}

// --- SMTP data ---
void SmtpSession::setSender(const std::string& addr) { 
    std::lock_guard<std::mutex> lock(session_mutex);
    sender_address = addr; 
}
void SmtpSession::setRecipient(const std::string& addr) { 
    std::lock_guard<std::mutex> lock(session_mutex);
    recipient_address = addr; 
}
void SmtpSession::appendMessageLine(const std::string& line) { 
    std::lock_guard<std::mutex> lock(session_mutex);
    message_buffer += line + "\n"; 
}

void SmtpSession::clearMessage() { 
    std::lock_guard<std::mutex> lock(session_mutex);
    message_buffer.clear(); 
}

const std::string SmtpSession::getSender() const { 
    std::lock_guard<std::mutex> lock(session_mutex);
    return sender_address; 
}

const std::string SmtpSession::getRecipient() const { 
    std::lock_guard<std::mutex> lock(session_mutex);
    return recipient_address; 
}

const std::string SmtpSession::getMessage() const { 
    std::lock_guard<std::mutex> lock(session_mutex);
    return message_buffer; 
}
