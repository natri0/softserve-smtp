#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <Result.hpp>

struct sqlite3_stmt;

namespace DB {
    namespace impl {
        template <typename T>
        struct is_optional : std::false_type {};

        template <typename T>
        struct is_optional<std::optional<T>> : std::true_type {};

        template <typename T>
        inline constexpr bool is_optional_v = is_optional<T>::value;
    }

    /// @brief Class that represents a connection to SQLite database
    class SQLiteConnection {
    public:
        SQLiteConnection();
        ~SQLiteConnection();

        /// @brief Opens a connection to the SQLite database at the specified path
        /// @param db_path Path to the SQLite database file
        /// @return Result indicating success or failure
        Result<> open(std::filesystem::path const& db_path);

        /// @brief Closes the connection to the SQLite database
        /// @return Result indicating success or failure
        Result<> close();

        class PreparedStatement;

        /// @brief Class that represents a single row in the result set
        class Row {
        public:
            Row() = default;
            ~Row() = default;

            Row(Row const&) = default;
            Row& operator=(Row const&) = default;
            Row(Row&&) noexcept = default;
            Row& operator=(Row&&) noexcept = default;

            /// @brief Retrieves a string value from the specified column index
            /// @param index Column index
            /// @return Result containing the string value or an error
            Result<std::string> getString(size_t index) const;

            /// @brief Retrieves an integer value from the specified column index
            /// @param index Column index
            /// @return Result containing the integer value or an error
            Result<int64_t> getInt(size_t index) const;

            /// @brief Retrieves a double value from the specified column index
            /// @param index Column index
            /// @return Result containing the double value or an error
            Result<double> getDouble(size_t index) const;

            /// @brief Retrieves a BLOB value from the specified column index
            /// @param index Column index
            /// @return Result containing the BLOB value as a vector of bytes or an error
            Result<std::vector<uint8_t>> getBlob(size_t index) const;

            /// @brief Checks if the value at the specified column index is NULL
            /// @param index Column index
            /// @return Result containing true if the value is NULL, false otherwise, or an error
            Result<bool> isNull(size_t index) const;

            /// @brief Retrieves an optional value of type T from the specified column index
            /// @param index Column index
            /// @return Result containing an optional value of type T or an error
            template <typename T>
            Result<std::optional<T>> getOpt(size_t index) const {
                RESULT_UNWRAP_INTO(bool isNull, isNull(index));
                if (isNull) {
                    return Ok(std::nullopt);
                }

                auto valueResult = get<T>(index);
                if (!valueResult) {
                    return std::move(valueResult).asErr();
                }

                return Ok(std::move(valueResult).unwrapUnchecked());
            }

            /// @brief Retrieves a value of type T from the specified column index
            /// @param index Column index
            /// @return Result containing the value of type T or an error
            template <typename T>
            Result<T> get(size_t index) const {
                if constexpr (impl::is_optional_v<T>) {
                    return getOpt<typename T::value_type>(index);
                } else {
                    if constexpr (std::is_same_v<T, std::string>) {
                        RESULT_UNWRAP_RETURN(getString(index));
                    } else if constexpr (std::is_integral_v<T>) {
                        RESULT_UNWRAP_RETURN(getInt(index));
                    } else if constexpr (std::is_floating_point_v<T>) {
                        RESULT_UNWRAP_RETURN(getDouble(index));
                    } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
                        RESULT_UNWRAP_RETURN(getBlob(index));
                    } else {
                        static_assert(sizeof(T) != sizeof(T), "Unsupported type for Row::get()");
                    }
                }

                std::unreachable();
            }

        private:
            std::shared_ptr<sqlite3_stmt> m_owner;
            sqlite3_stmt* m_stmt = nullptr;

            explicit Row(std::shared_ptr<sqlite3_stmt> owner) : m_owner(std::move(owner)), m_stmt(m_owner.get()) {}

            friend class SQLiteConnection;
            friend class FetchResult;
            friend class PreparedStatement;
        };

        /// @brief Class that represents the result set of a query
        /// that can be iterated over to fetch rows one by one
        class FetchResult {
        public:
            FetchResult();
            ~FetchResult();

            FetchResult(FetchResult const&) = delete;
            FetchResult& operator=(FetchResult const&) = delete;

            FetchResult(FetchResult&& other) noexcept : pImpl(other.pImpl) {
                other.pImpl = nullptr;
            }

            FetchResult& operator=(FetchResult&& other) noexcept {
                if (this != &other) {
                    std::swap(pImpl, other.pImpl);
                }
                return *this;
            }

            /// @brief Advances to the next row in the result set
            /// @return Result containing true if there is a next row, false if no more rows, or an error
            Result<bool> next();

            /// @brief Retrieves the current row in the result set
            /// @return The current Row object
            [[nodiscard]] Row getRow() const;

            /// @brief Iterator for the FetchResult to enable range-based for loops
            struct Iterator {
                FetchResult* fetchResult{};

                Iterator& operator++() {
                    if (!fetchResult) return *this;
                    auto res = fetchResult->next();
                    if (!res.isOk() || !res.unwrapUnchecked()) {
                        fetchResult = nullptr;
                    }
                    return *this;
                }

                Row operator*() const {
                    return fetchResult->getRow();
                }

                bool operator!=(Iterator const& other) const {
                    return fetchResult != other.fetchResult;
                }
            };

            /// @brief Returns an iterator to the beginning of the result set
            Iterator begin() {
                auto res = next();
                if (!res.isOk() || !res.unwrapUnchecked()) {
                    return Iterator{nullptr};
                }
                return Iterator{this};
            }

            /// @brief Returns an iterator to the end of the result set
            static Iterator end() {
                return Iterator{nullptr};
            }

        private:
            struct Impl;
            Impl* pImpl;

            friend class SQLiteConnection;
            friend class PreparedStatement;
        };

        /// @brief Class that represents a prepared statement for executing parameterized queries
        /// @example "SELECT * FROM users WHERE id = ?", where '?' is a parameter to be bound
        class PreparedStatement {
        public:
            PreparedStatement();
            ~PreparedStatement();

            PreparedStatement(PreparedStatement const&) = delete;
            PreparedStatement& operator=(PreparedStatement const&) = delete;
            PreparedStatement(PreparedStatement&& other) noexcept : pImpl(other.pImpl) { other.pImpl = nullptr; }

            PreparedStatement& operator=(PreparedStatement&& other) noexcept {
                if (this != &other) {
                    std::swap(pImpl, other.pImpl);
                }
                return *this;
            }

            /// @brief Binds an integer value to the specified parameter index
            /// @param index Parameter index (1-based)
            /// @param value Integer value to bind
            /// @return Result indicating success or failure
            Result<> bindInt(size_t index, int64_t value);

            /// @brief Binds a double value to the specified parameter index
            /// @param index Parameter index (1-based)
            /// @param value Double value to bind
            /// @return Result indicating success or failure
            Result<> bindDouble(size_t index, double value);

            /// @brief Binds a string value to the specified parameter index
            /// @param index Parameter index (1-based)
            /// @param value String value to bind
            /// @return Result indicating success or failure
            Result<> bindString(size_t index, std::string_view value);

            /// @brief Binds a BLOB value to the specified parameter index
            /// @param index Parameter index (1-based)
            /// @param value BLOB value as a vector of bytes to bind
            /// @return Result indicating success or failure
            Result<> bindBlob(size_t index, std::vector<uint8_t> const& value);

            /// @brief Binds a NULL value to the specified parameter index
            /// @param index Parameter index (1-based)
            /// @return Result indicating success or failure
            Result<> bindNull(size_t index);

            /// @brief Resets the prepared statement to its initial state, allowing it to be re-executed
            /// @return Result indicating success or failure
            Result<> reset();

            /// @brief Clears all bound parameters from the prepared statement
            /// @return Result indicating success or failure
            Result<> clearBindings();

            /// @brief Executes the prepared statement without returning any result set
            /// @return Result indicating success or failure
            Result<> execute();

            /// @brief Fetches a single row from the result set of the prepared statement
            /// @return Result containing the Row object or an error
            Result<Row> fetchOne();

            /// @brief Fetches all rows from the result set of the prepared statement
            /// @return Result containing the FetchResult object or an error
            Result<FetchResult> fetchAll();

        private:
            struct Impl;
            Impl* pImpl;

            friend class SQLiteConnection;
        };

        /// @brief Fetches a single row from the database using the provided query
        /// @param query SQL query string
        /// @return Result containing the Row object or an error
        Result<Row> fetchRow(std::string_view query);

        /// @brief Fetches all rows from the database using the provided query
        /// @param query SQL query string
        /// @return Result containing the FetchResult object or an error
        Result<FetchResult> fetchAll(std::string_view query);

        /// @brief Prepares a SQL statement for execution
        /// @param query SQL query string
        /// @return Result containing the PreparedStatement object or an error
        Result<PreparedStatement> prepareStatement(std::string_view query);

        /// @brief Executes a SQL statement without returning any result set
        /// @param query SQL query string
        /// @return Result indicating success or failure
        Result<> execute(std::string_view query);

        /// @brief Fetches a single row from the database using the provided query and parameters
        /// @param query SQL query string
        /// @param args Parameters to bind to the query
        /// @return Result containing the Row object or an error
        template <class... Ts>
        Result<Row> fetchRow(std::string_view query, Ts&&... args) {
            if (sizeof...(args) == 0) {
                return fetchRow(query);
            }

            RESULT_UNWRAP_INTO(auto stmt, prepareStatement(query));
            std::optional<std::string> error;

            // Bind parameters
            size_t index = 0;
            ([&] {
                using ArgType = std::decay_t<decltype(args)>;
                if constexpr (std::is_convertible_v<ArgType, std::string_view>) {
                    RESULT_UNWRAP_ERR_INTO(error, stmt.bindString(++index, args));
                } else if constexpr (std::is_integral_v<ArgType>) {
                    RESULT_UNWRAP_ERR_INTO(error, stmt.bindInt(++index, args));
                } else if constexpr (std::is_floating_point_v<ArgType>) {
                    RESULT_UNWRAP_ERR_INTO(error, stmt.bindDouble(++index, args));
                } else if constexpr (std::is_same_v<ArgType, std::vector<uint8_t>>) {
                    RESULT_UNWRAP_ERR_INTO(error, stmt.bindBlob(++index, args));
                } else {
                    static_assert(sizeof(ArgType) != sizeof(ArgType), "Unsupported type for bind");
                }
            }(), ...);

            return stmt.fetchOne();
        }

        /// @brief Executes a SQL statement without returning any result set, with parameters
        /// @param query SQL query string
        /// @param args Parameters to bind to the query
        /// @return Result indicating success or failure
        template <class... Ts>
        Result<> execute(std::string_view query, Ts&&... args) {
            if (sizeof...(args) == 0) {
                return execute(query);
            }

            RESULT_UNWRAP_INTO(auto stmt, prepareStatement(query));
            std::optional<std::string> error;

            // Bind parameters
            size_t index = 0;
            ([&] {
                using ArgType = std::decay_t<decltype(args)>;
                if constexpr (std::is_convertible_v<ArgType, std::string_view>) {
                    RESULT_UNWRAP_ERR_INTO(error, stmt.bindString(++index, args));
                } else if constexpr (std::is_integral_v<ArgType>) {
                    RESULT_UNWRAP_ERR_INTO(error, stmt.bindInt(++index, args));
                } else if constexpr (std::is_floating_point_v<ArgType>) {
                    RESULT_UNWRAP_ERR_INTO(error, stmt.bindDouble(++index, args));
                } else if constexpr (std::is_same_v<ArgType, std::vector<uint8_t>>) {
                    RESULT_UNWRAP_ERR_INTO(error, stmt.bindBlob(++index, args));
                } else {
                    static_assert(sizeof(ArgType) != sizeof(ArgType), "Unsupported type for bind");
                }
            }(), ...);

            if (error.has_value()) {
                return Err(std::move(*error));
            }

            return stmt.execute();
        }

        /// @brief Fetches a single row from the database using the provided query and parameters,
        /// and unpacks the row into a tuple of specified types
        /// @tparam Ts Types to unpack the row into
        /// @param query SQL query string
        /// @param args Parameters to bind to the query
        /// @return Result containing a tuple of unpacked values or an error
        template <typename... Ts>
        Result<std::tuple<Ts...>> fetchOne(std::string_view query, auto&&... args) {
            RESULT_UNWRAP_INTO(auto row, fetchRow(query, std::forward<decltype(args)>(args)...));

            std::tuple<Ts...> result;
            std::optional<std::string> error;

            [&]<size_t... Is>(std::index_sequence<Is...>) {
                ([&] {
                    if (error.has_value()) return;
                    RESULT_UNWRAP_INTO_EITHER(
                        std::get<Is>(result), error,
                        row.template get<std::tuple_element_t<Is, std::tuple<Ts...>>>(Is)
                    );
                }(), ...);
            }(std::index_sequence_for<Ts...>{});

            if (error.has_value()) {
                return Err(std::move(*error));
            }

            return Ok(std::move(result));
        }

    private:
        struct Impl;
        std::shared_ptr<Impl> pImpl;
    };
}