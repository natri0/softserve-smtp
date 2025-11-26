#pragma once

#include <SMTPIMailbox.h>
#include <DB/Connection.hpp>

/// @brief Class that manages database operations
class DatabaseManager {
public:
    DatabaseManager();

    /// @brief Returns the singleton instance of DatabaseManager
    static DatabaseManager& get();

    /// @brief Inserts a new mail into the database
    Result<> emplaceMail(std::string_view forwardPath, std::string_view reversePath, std::string_view mailData);

    /// @brief Checks if a mailbox with the given localpart is available
    Result<bool> checkMailboxAvailability(std::string_view localpart);

    /// @brief Suggests addresses based on the provided request string
    Result<std::string> suggestAddress(std::string_view request);

private:
    DB::SQLiteConnection m_connection;
};

/// @brief Implementation of SMTPIMailbox using SQLite database
class SQLiteMailbox : public ISXSMTP::SMTPIMailbox {
public:
    /// @brief Deposits mail into the database
    void DepositMail(ISXSMTP::SMTPBuffer forward_path, ISXSMTP::SMTPBuffer reverse_path, ISXSMTP::SMTPBuffer mail_data) override;

    /// @brief Checks if a mailbox with the given localpart is available
    bool IsMailboxAvailable(std::string const& localpart) override;

    /// @brief Suggests addresses based on the provided request string
    std::string SuggestAddress(std::string const& request) override;
};

// TODO: IMAP interface