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
        client_ip = "unknown (error)";
    }
}

void SmtpSession::enqueueCommand(const std::string& cmd) {
    std::lock_guard<std::mutex> lock(session_mutex);
    commandQueue.push(cmd);
}

bool SmtpSession::hasNextCommand() {
    std::lock_guard<std::mutex> lock(session_mutex);
    return !commandQueue.empty();
}

std::string SmtpSession::popCommand() {
    std::lock_guard<std::mutex> lock(session_mutex);
    if (commandQueue.empty()) return "";
    std::string cmd = commandQueue.front();
    commandQueue.pop();
    return cmd;
}

// --- Busy flag ---
void SmtpSession::setBusy(bool val) {
    busy.store(val, std::memory_order_release);
}

bool SmtpSession::isBusy() {
    return busy.load(std::memory_order_acquire); 
}

bool SmtpSession::compareBusy() {
    bool expected = false;
    return busy.compare_exchange_strong(expected, true);
}

bool SmtpSession::releaseIfEmpty()
{
    std::lock_guard<std::mutex> lock(session_mutex);
    if (commandQueue.empty()) {
        busy.store(false);
        return true;
    }
    return false;
}

bool SmtpSession::clearQueue(){
    std::lock_guard<std::mutex> lock(session_mutex);
    if(commandQueue.empty()) return false;
    while (!commandQueue.empty()){
        commandQueue.pop();
    }
    return true;
}

void SmtpSession::close() {
    if (closed.exchange(true)) return;
    if (socket && socket->is_open()) {
        std::error_code ec;
        socket->shutdown(asio::ip::tcp::socket::shutdown_both, ec);
        socket->close(ec);
    }
    busy.store(false);
    clearQueue();
}

bool SmtpSession::isClosed() const {
    return closed.load();
}

// --- Socket & info ---
std::shared_ptr<asio::ip::tcp::socket> SmtpSession::getSocket() { 
    if (closed.load(std::memory_order_acquire)) {
        return nullptr; 
    }
    return socket; 
}

std::string SmtpSession::getClientIp() const { 
    std::lock_guard<std::mutex> lock(session_mutex);
    return client_ip; 
}

// --- SMTP state ---
SmtpSession::SessionStatus SmtpSession::getStatus() const { return current_state; }
void SmtpSession::setStatus(SessionStatus st) { current_state = st; }

// --- SMTP data ---
void SmtpSession::setSender(const std::string& addr) { sender_address = addr; }
void SmtpSession::setRecipient(const std::string& addr) { recipient_address = addr; }
void SmtpSession::appendMessageLine(const std::string& line) { message_buffer += line + "\n"; }
void SmtpSession::clearMessage() { message_buffer.clear(); }

const std::string& SmtpSession::getSender() const { return sender_address; }
const std::string& SmtpSession::getRecipient() const { return recipient_address; }
const std::string& SmtpSession::getMessage() const { return message_buffer; }
