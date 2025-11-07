#include <DB/Connection.hpp>

#include <sqlite3.h>
#include <fmt/format.h>
#include <fmt/std.h>

namespace DB {
    struct SQLiteConnection::Impl {
        sqlite3* m_db = nullptr;

        void close() {
            if (m_db) {
                sqlite3_close(m_db);
                m_db = nullptr;
            }
        }

        ~Impl() { close(); }
    };

    SQLiteConnection::SQLiteConnection() : pImpl(std::make_shared<Impl>()) {}
    SQLiteConnection::~SQLiteConnection() = default;

    Result<> SQLiteConnection::open(std::filesystem::path const& db_path) {
        if (pImpl->m_db) {
            return Err("Database already opened");
        }

        int rc = sqlite3_open(fmt::to_string(db_path).c_str(), &pImpl->m_db);
        if (rc != SQLITE_OK) {
            pImpl->close();
            return Err(fmt::format("Cannot open database: {}", sqlite3_errmsg(pImpl->m_db)));
        }

        return Ok();
    }

    Result<> SQLiteConnection::close() {
        if (!pImpl->m_db) {
            return Err("Database not opened");
        }

        pImpl->close();
        return Ok();
    }

    Result<std::string> SQLiteConnection::Row::getString(size_t index) const {
        if (!m_stmt) {
            return Err("Invalid statement");
        }

        if (index >= static_cast<size_t>(sqlite3_column_count(m_stmt))) {
            return Err("Column index out of range");
        }

        if (sqlite3_column_type(m_stmt, static_cast<int>(index)) != SQLITE_TEXT) {
            return Err("Column is not of type TEXT");
        }

        unsigned char const* text = sqlite3_column_text(m_stmt, static_cast<int>(index));
        if (!text) {
            return Err("Failed to retrieve TEXT value");
        }

        return Ok(std::string(reinterpret_cast<char const*>(text)));
    }

    Result<int64_t> SQLiteConnection::Row::getInt(size_t index) const {
        if (!m_stmt) {
            return Err("Invalid statement");
        }

        if (index >= static_cast<size_t>(sqlite3_column_count(m_stmt))) {
            return Err("Column index out of range");
        }

        if (sqlite3_column_type(m_stmt, static_cast<int>(index)) != SQLITE_INTEGER) {
            return Err("Column is not of type INTEGER");
        }

        return Ok(static_cast<int64_t>(sqlite3_column_int64(m_stmt, static_cast<int>(index))));
    }

    Result<double> SQLiteConnection::Row::getDouble(size_t index) const {
        if (!m_stmt) {
            return Err("Invalid statement");
        }

        if (index >= static_cast<size_t>(sqlite3_column_count(m_stmt))) {
            return Err("Column index out of range");
        }

        if (sqlite3_column_type(m_stmt, static_cast<int>(index)) != SQLITE_FLOAT) {
            return Err("Column is not of type FLOAT");
        }

        return Ok(sqlite3_column_double(m_stmt, static_cast<int>(index)));
    }

    Result<std::vector<uint8_t>> SQLiteConnection::Row::getBlob(size_t index) const {
        if (!m_stmt) {
            return Err("Invalid statement");
        }

        if (index >= static_cast<size_t>(sqlite3_column_count(m_stmt))) {
            return Err("Column index out of range");
        }

        if (sqlite3_column_type(m_stmt, static_cast<int>(index)) != SQLITE_BLOB) {
            return Err("Column is not of type BLOB");
        }

        int blobSize = sqlite3_column_bytes(m_stmt, static_cast<int>(index));
        if (blobSize < 0) {
            return Err("Failed to retrieve BLOB value");
        }
        if (blobSize == 0) {
            return Ok(std::vector<uint8_t>{});
        }

        void const* blobData = sqlite3_column_blob(m_stmt, static_cast<int>(index));
        if (!blobData) {
            return Err("Failed to retrieve BLOB value");
        }

        return Ok(std::vector(
            static_cast<uint8_t const*>(blobData),
            static_cast<uint8_t const*>(blobData) + blobSize
        ));
    }

    Result<bool> SQLiteConnection::Row::isNull(size_t index) const {
        if (!m_stmt) {
            return Err("Invalid statement");
        }

        if (index >= static_cast<size_t>(sqlite3_column_count(m_stmt))) {
            return Err("Column index out of range");
        }

        return Ok(sqlite3_column_type(m_stmt, static_cast<int>(index)) == SQLITE_NULL);
    }

    Result<SQLiteConnection::Row> SQLiteConnection::fetchRow(std::string_view query) {
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(pImpl->m_db, query.data(), static_cast<int>(query.size()), &stmt, nullptr) != SQLITE_OK) {
            if (stmt) sqlite3_finalize(stmt);
            return Err(fmt::format("Failed to prepare statement: {}", sqlite3_errmsg(pImpl->m_db)));
        }

        int stepRc = sqlite3_step(stmt);
        if (stepRc != SQLITE_ROW) {
            sqlite3_finalize(stmt);
            return Err("No rows returned");
        }

        auto owner = std::shared_ptr<sqlite3_stmt>(stmt, [](auto s) { if (s) sqlite3_finalize(s); });

        return Ok(Row{std::move(owner)});
    }

    struct SQLiteConnection::FetchResult::Impl {
        std::shared_ptr<sqlite3_stmt> m_owner;
        sqlite3* m_db = nullptr;
    };

    SQLiteConnection::FetchResult::FetchResult() : pImpl(new Impl()) {}
    SQLiteConnection::FetchResult::~FetchResult() { delete pImpl; }

    Result<bool> SQLiteConnection::FetchResult::next() {
        if (!pImpl || !pImpl->m_owner) {
            return Err("Invalid statement");
        }

        int rc = sqlite3_step(pImpl->m_owner.get());
        if (rc == SQLITE_ROW) {
            return Ok(true);
        }

        if (rc == SQLITE_DONE) {
            return Ok(false);
        }

        return Err(fmt::format("sqlite3_step failed: {}", pImpl->m_db ? sqlite3_errmsg(pImpl->m_db) : "unknown"));
    }

    SQLiteConnection::Row SQLiteConnection::FetchResult::getRow() const {
        return Row{pImpl->m_owner};
    }

    Result<SQLiteConnection::FetchResult> SQLiteConnection::fetchAll(std::string_view query) {
        FetchResult result;

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(pImpl->m_db, query.data(), static_cast<int>(query.size()), &stmt, nullptr) != SQLITE_OK) {
            if (stmt) sqlite3_finalize(stmt);
            return Err(fmt::format("Failed to prepare statement: {}", sqlite3_errmsg(pImpl->m_db)));
        }

        result.pImpl->m_owner = std::shared_ptr<sqlite3_stmt>(stmt, [](auto s) { if (s) sqlite3_finalize(s); });
        result.pImpl->m_db = pImpl->m_db;
        return Ok(std::move(result));
    }

    struct SQLiteConnection::PreparedStatement::Impl {
        std::shared_ptr<sqlite3_stmt> m_owner;
        sqlite3* m_db = nullptr;
    };

    SQLiteConnection::PreparedStatement::PreparedStatement() : pImpl(new Impl()) {}
    SQLiteConnection::PreparedStatement::~PreparedStatement() { delete pImpl; }

    Result<SQLiteConnection::PreparedStatement> SQLiteConnection::prepareStatement(std::string_view query) {
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(pImpl->m_db, query.data(), static_cast<int>(query.size()), &stmt, nullptr) != SQLITE_OK) {
            if (stmt) sqlite3_finalize(stmt);
            return Err(fmt::format("Failed to prepare statement: {}", sqlite3_errmsg(pImpl->m_db)));
        }

        PreparedStatement ps;
        ps.pImpl->m_owner = std::shared_ptr<sqlite3_stmt>(stmt, [](auto s) { if (s) sqlite3_finalize(s); });
        ps.pImpl->m_db = pImpl->m_db;
        return Ok(std::move(ps));
    }

    Result<> SQLiteConnection::execute(std::string_view query) {
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(pImpl->m_db, query.data(), static_cast<int>(query.size()), &stmt, nullptr) != SQLITE_OK) {
            if (stmt) sqlite3_finalize(stmt);
            return Err(fmt::format("Failed to prepare statement: {}", sqlite3_errmsg(pImpl->m_db)));
        }

        std::shared_ptr<sqlite3_stmt> owner(stmt, [](auto s) { if (s) sqlite3_finalize(s); });

        int rc = sqlite3_step(owner.get());
        if (rc != SQLITE_DONE) {
            return Err(fmt::format("sqlite3_step failed: {}", sqlite3_errmsg(pImpl->m_db)));
        }

        return Ok();
    }

    Result<> SQLiteConnection::PreparedStatement::bindInt(size_t index, int64_t value) {
        if (!pImpl || !pImpl->m_owner) {
            return Err("Invalid statement");
        }

        int rc = sqlite3_bind_int64(pImpl->m_owner.get(), static_cast<int>(index), value);
        if (rc != SQLITE_OK) {
            return Err(fmt::format("bindInt failed: {}", sqlite3_errmsg(pImpl->m_db)));
        }

        return Ok();
    }

    Result<> SQLiteConnection::PreparedStatement::bindDouble(size_t index, double value) {
        if (!pImpl || !pImpl->m_owner) {
            return Err("Invalid statement");
        }

        int rc = sqlite3_bind_double(pImpl->m_owner.get(), static_cast<int>(index), value);
        if (rc != SQLITE_OK) {
            return Err(fmt::format("bindDouble failed: {}", sqlite3_errmsg(pImpl->m_db)));
        }

        return Ok();
    }

    Result<> SQLiteConnection::PreparedStatement::bindString(size_t index, std::string_view value) {
        if (!pImpl || !pImpl->m_owner) {
            return Err("Invalid statement");
        }

        int rc = sqlite3_bind_text(
            pImpl->m_owner.get(),
            static_cast<int>(index),
            value.data(),
            static_cast<int>(value.size()),
            SQLITE_TRANSIENT
        );

        if (rc != SQLITE_OK) {
            return Err(fmt::format("bindString failed: {}", sqlite3_errmsg(pImpl->m_db)));
        }

        return Ok();
    }

    Result<> SQLiteConnection::PreparedStatement::bindBlob(size_t index, std::vector<uint8_t> const& value) {
        if (!pImpl || !pImpl->m_owner) {
            return Err("Invalid statement");
        }

        int rc;
        if (value.empty()) {
            rc = sqlite3_bind_zeroblob(pImpl->m_owner.get(), static_cast<int>(index), 0);
        } else {
            rc = sqlite3_bind_blob(
                pImpl->m_owner.get(),
                static_cast<int>(index),
                value.data(),
                static_cast<int>(value.size()),
                SQLITE_TRANSIENT
            );
        }

        if (rc != SQLITE_OK) {
            return Err(fmt::format("bindBlob failed: {}", sqlite3_errmsg(pImpl->m_db)));
        }

        return Ok();
    }

    Result<> SQLiteConnection::PreparedStatement::bindNull(size_t index) {
        if (!pImpl || !pImpl->m_owner) {
            return Err("Invalid statement");
        }

        int rc = sqlite3_bind_null(pImpl->m_owner.get(), static_cast<int>(index));
        if (rc != SQLITE_OK) {
            return Err(fmt::format("bindNull failed: {}", sqlite3_errmsg(pImpl->m_db)));
        }

        return Ok();
    }

    Result<SQLiteConnection::Row> SQLiteConnection::PreparedStatement::fetchOne() {
        if (!pImpl || !pImpl->m_owner) {
            return Err("Invalid statement");
        }

        int rc = sqlite3_step(pImpl->m_owner.get());
        if (rc == SQLITE_ROW) {
            return Ok(Row{pImpl->m_owner});
        }

        if (rc == SQLITE_DONE) {
            return Err("No rows returned");
        }

        return Err(fmt::format("sqlite3_step failed: {}", pImpl->m_db ? sqlite3_errmsg(pImpl->m_db) : "unknown"));
    }

    Result<SQLiteConnection::FetchResult> SQLiteConnection::PreparedStatement::fetchAll() {
        if (!pImpl || !pImpl->m_owner) {
            return Err("Invalid statement");
        }

        FetchResult res;
        res.pImpl->m_owner = pImpl->m_owner;
        res.pImpl->m_db = pImpl->m_db;

        return Ok(std::move(res));
    }

    Result<> SQLiteConnection::PreparedStatement::reset() {
        if (!pImpl || !pImpl->m_owner) {
            return Err("Invalid statement");
        }

        int rc = sqlite3_reset(pImpl->m_owner.get());
        if (rc != SQLITE_OK) {
            return Err(fmt::format("reset failed: {}", sqlite3_errmsg(pImpl->m_db)));
        }

        return Ok();
    }

    Result<> SQLiteConnection::PreparedStatement::clearBindings() {
        if (!pImpl || !pImpl->m_owner) {
            return Err("Invalid statement");
        }

        int rc = sqlite3_clear_bindings(pImpl->m_owner.get());
        if (rc != SQLITE_OK) {
            return Err(fmt::format("clearBindings failed: {}", sqlite3_errmsg(pImpl->m_db)));
        }

        return Ok();
    }

    Result<> SQLiteConnection::PreparedStatement::execute() {
        if (!pImpl || !pImpl->m_owner) {
            return Err("Invalid statement");
        }

        int rc = sqlite3_step(pImpl->m_owner.get());
        if (rc == SQLITE_DONE) {
            return Ok();
        }

        return Err(fmt::format("sqlite3_step failed: {}", pImpl->m_db ? sqlite3_errmsg(pImpl->m_db) : "unknown"));
    }
}