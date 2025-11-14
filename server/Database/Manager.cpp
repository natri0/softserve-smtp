#include "Manager.hpp"

#include <Logger.h>
#include <Macros.h>

#include <iostream>
#include <format>

// constexpr std::string_view CREATE_TABLES = R"(
// CREATE TABLE IF NOT EXISTS domains (
//     id INTEGER PRIMARY KEY AUTOINCREMENT,
//     name TEXT NOT NULL UNIQUE
// );
// CREATE TABLE IF NOT EXISTS users (
//     user_id INTEGER PRIMARY KEY AUTOINCREMENT,
//     localpart TEXT NOT NULL UNIQUE,
//     domain_id INTEGER NOT NULL,
//     FOREIGN KEY (domain_id) REFERENCES domains(id)
// );
// CREATE TABLE IF NOT EXISTS mail (
//     mail_id INTEGER PRIMARY KEY AUTOINCREMENT,
//     reverse_path TEXT NOT NULL,
//     data BLOB NOT NULL
// );
// CREATE TABLE IF NOT EXISTS mail_recipients (
//     mail_id INTEGER NOT NULL,
//     user_id INTEGER NOT NULL,
//     FOREIGN KEY (mail_id) REFERENCES mail(mail_id),
//     FOREIGN KEY (user_id) REFERENCES users(user_id)
// );
// )";

constexpr std::string_view CREATE_TABLE_0 = R"(
CREATE TABLE IF NOT EXISTS domains (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE
);
)";

constexpr std::string_view CREATE_TABLE_1 = R"(
CREATE TABLE IF NOT EXISTS users (
    user_id INTEGER PRIMARY KEY AUTOINCREMENT,
    localpart TEXT NOT NULL UNIQUE,
    domain_id INTEGER NOT NULL,
    FOREIGN KEY (domain_id) REFERENCES domains(id)
);
)";

constexpr std::string_view CREATE_TABLE_2 = R"(
CREATE TABLE IF NOT EXISTS mail (
    mail_id INTEGER PRIMARY KEY AUTOINCREMENT,
    reverse_path TEXT NOT NULL,
    data BLOB NOT NULL
);
)";

constexpr std::string_view CREATE_TABLE_3 = R"(

CREATE TABLE IF NOT EXISTS mail_recipients (
    mail_id INTEGER NOT NULL,
    user_id INTEGER NOT NULL,
    FOREIGN KEY (mail_id) REFERENCES mail(mail_id),
    FOREIGN KEY (user_id) REFERENCES users(user_id)
);
)";

DatabaseManager::DatabaseManager()
{
    m_connection.open("database.db")
                .expect("Failed to open database");

    // m_connection.execute(CREATE_TABLES)
    //             .expect("Failed to create tables");

    m_connection.execute(CREATE_TABLE_0)
                .expect("Failed to create tables");

    m_connection.execute(CREATE_TABLE_1)
                .expect("Failed to create tables");

    m_connection.execute(CREATE_TABLE_2)
                .expect("Failed to create tables");

    m_connection.execute(CREATE_TABLE_3)
                .expect("Failed to create tables");
}

DatabaseManager& DatabaseManager::get()
{
    static DatabaseManager instance;
    return instance;
}

Result<> DatabaseManager::emplaceMail(
    std::string_view forwardPath, std::string_view reversePath, std::string_view mailData
)
{
    int localpartEnd = forwardPath.find('@');
    std::string_view localpart = forwardPath.substr(0, localpartEnd);

    auto userRes = m_connection.fetchOne<int>("select user_id from users where localpart = ?;", localpart);
    if (userRes.isErr()) {
        return Err(std::format("Failed to fetch user ID: {}", userRes.unwrapErrUnchecked()));
    }

    auto [userId] = std::move(userRes).unwrapUnchecked();

    auto res = m_connection.execute(
        "INSERT INTO mail (reverse_path, data) VALUES (?, ?);",
        reversePath,
        mailData
    );

    auto res2 = m_connection.execute(
        "INSERT INTO mail_recipients (mail_id, user_id) VALUES (last_insert_rowid(), ?);",
        userId
    );

    if (!res || !res2)
    {
        return Err(std::format("Failed to insert mail: {}", res.unwrapErrUnchecked()));
    }

    return Ok();
}

Result<bool> DatabaseManager::checkMailboxAvailability(std::string_view localpart)
{
    auto res = m_connection.fetchOne<int>(
        "SELECT COUNT(*) FROM users WHERE localpart = ?;",
        localpart
    );
    if (!res)
    {
        return Err(std::format("Failed to check mailbox availability: {}", res.unwrapErrUnchecked()));
    }

    auto [count] = std::move(res).unwrapUnchecked();
    return Ok(count == 0);
}

Result<std::string> DatabaseManager::suggestAddress(std::string_view request)
{
    auto res = m_connection.fetchOne<std::string>(
        "SELECT localpart FROM users WHERE localpart LIKE ? LIMIT 1;",
        std::format("%{}%", request)
    );
    if (!res)
    {
        return Err(std::format("Failed to suggest address: {}", res.unwrapErrUnchecked()));
    }

    auto [localpart] = std::move(res).unwrapUnchecked();
    return Ok(localpart);
}

Result<std::vector<DbMail>> DatabaseManager::fetchMailsForUser(std::string_view localpart, int since_uid) {
    int userid;

    if (auto idres = m_connection.fetchOne<int>("select user_id from users where localpart = ?;", localpart); idres.isErr()) {
        return Err(std::format("Failed to find user '{}': {}", localpart, idres.unwrapErrUnchecked()));
    } else {
        std::tie(userid) = idres.unwrapUnchecked();
    }

    DB::SQLiteConnection::PreparedStatement st;
    if (auto stres = m_connection.prepareStatement("select m.data, m.reverse_path, m.mail_id from mail as m left join mail_recipients as r "
                          "on m.mail_id = r.mail_id where r.user_id = ? "
                          "and m.mail_id > ?"); stres.isErr()) {
        return Err(std::format("Failed to find mail: {}", stres.unwrapErrUnchecked()));
    } else {
        st = std::move(stres).asOk().unwrap();
    }

    std::vector<DbMail> mails;

    if (auto res = st.bindInt(1, userid); res.isErr())
        return Err(std::format("Failed to bind user id: {}", res.unwrapErrUnchecked()));
    if (auto res = st.bindInt(2, since_uid); res.isErr())
        return Err(std::format("Failed to bind since_uid: {}", res.unwrapErrUnchecked()));

    if (auto res = st.fetchAll(); res.isErr()) {
        return Err(std::format("Failed to fetch mails: {}", res.unwrapErrUnchecked()));
    } else {
        auto mailsIt = std::move(res.unwrapUnchecked());
        for (auto row : mailsIt) {
            mails.emplace_back(row.getString(0).unwrap(), row.getString(1).unwrap(), row.getInt(2).unwrap());
        }
    }

    return Ok(std::move(mails));
}

void SQLiteMailbox::DepositMail(
    ISXSMTP::SMTPBuffer forward_path, ISXSMTP::SMTPBuffer reverse_path, ISXSMTP::SMTPBuffer mail_data
)
{
    auto res = DatabaseManager::get().emplaceMail(
        forward_path.GetString(),
        reverse_path.GetString(),
        mail_data.GetString()
    );

    if (!res)
    {
        LOG_ERROR(LogLevel::DEBUG) << "Error depositing mail: " << res.unwrapErr();
    }
}

bool SQLiteMailbox::IsMailboxAvailable(std::string const& localpart)
{
    auto res = DatabaseManager::get().checkMailboxAvailability(localpart);
    if (!res)
    {
        // TODO: log error
        return false;
    }

    return res.unwrapUnchecked();
}

std::string SQLiteMailbox::SuggestAddress(std::string const& request)
{
    auto res = DatabaseManager::get().suggestAddress(request);
    if (!res)
    {
        // TODO: log error
        return {};
    }

    return std::move(res).unwrapUnchecked();
}
