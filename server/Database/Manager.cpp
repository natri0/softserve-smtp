#include "Manager.hpp"

#include <fmt/format.h>

constexpr std::string_view CREATE_TABLES = R"(
CREATE TABLE IF NOT EXISTS domains (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE
);
CREATE TABLE IF NOT EXISTS users (
    user_id INTEGER PRIMARY KEY AUTOINCREMENT,
    localpart TEXT NOT NULL UNIQUE
    domain_id INTEGER NOT NULL,
    FOREIGN KEY (domain_id) REFERENCES domains(id)
);
CREATE TABLE IF NOT EXISTS mail (
    mail_id INTEGER PRIMARY KEY AUTOINCREMENT,
    reverse_path TEXT NOT NULL,
    data BLOB NOT NULL
);
CREATE TABLE IF NOT EXISTS mail_recipients (
    mail_id INTEGER NOT NULL,
    user_id INTEGER NOT NULL,
    FOREIGN KEY (mail_id) REFERENCES mail(mail_id),
    FOREIGN KEY (user_id) REFERENCES users(user_id)
);)";

DatabaseManager::DatabaseManager() {
    m_connection.open("database.db")
                .expect("Failed to open database");

    m_connection.execute(CREATE_TABLES)
                .expect("Failed to create tables");
}

DatabaseManager& DatabaseManager::get() {
    static DatabaseManager instance;
    return instance;
}

Result<> DatabaseManager::emplaceMail(
    std::string_view forwardPath, std::string_view reversePath, std::string_view mailData
) {
    auto res = m_connection.execute(
        "INSERT INTO mail (reverse_path, data) VALUES (?, ?);",
        reversePath,
        mailData
    );

    if (!res) {
        return Err(fmt::format("Failed to insert mail: {}", res.unwrapErrUnchecked()));
    }

    return Ok();
}

Result<bool> DatabaseManager::checkMailboxAvailability(std::string_view localpart) {
    auto res = m_connection.fetchOne<int>(
        "SELECT COUNT(*) FROM users WHERE localpart = ?;",
        localpart
    );
    if (!res) {
        return Err(fmt::format("Failed to check mailbox availability: {}", res.unwrapErrUnchecked()));
    }

    auto [count] = std::move(res).unwrapUnchecked();
    return Ok(count == 0);
}

Result<std::string> DatabaseManager::suggestAddress(std::string_view request) {
    auto res = m_connection.fetchOne<std::string>(
        "SELECT localpart FROM users WHERE localpart LIKE ? LIMIT 1;",
        fmt::format("%{}%", request)
    );
    if (!res) {
        return Err(fmt::format("Failed to suggest address: {}", res.unwrapErrUnchecked()));
    }

    auto [localpart] = std::move(res).unwrapUnchecked();
    return Ok(localpart);
}

void SQLiteMailbox::DepositMail(
    ISXSMTP::SMTPBuffer forward_path, ISXSMTP::SMTPBuffer reverse_path, ISXSMTP::SMTPBuffer mail_data
) {
    auto res = DatabaseManager::get().emplaceMail(
        forward_path.GetString(),
        reverse_path.GetString(),
        mail_data.GetString()
    );

    if (!res) {
        // TODO: log error
    }
}

bool SQLiteMailbox::IsMailboxAvailable(std::string const& localpart) {
    auto res = DatabaseManager::get().checkMailboxAvailability(localpart);
    if (!res) {
        // TODO: log error
        return false;
    }

    return res.unwrapUnchecked();
}

std::string SQLiteMailbox::SuggestAddress(std::string const& request) {
    auto res = DatabaseManager::get().suggestAddress(request);
    if (!res) {
        // TODO: log error
        return {};
    }

    return std::move(res).unwrapUnchecked();
}
