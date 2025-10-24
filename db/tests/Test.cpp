#include <DB/Connection.hpp>
#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace {
    struct SQLiteConnectionFixture : public ::testing::Test {
        DB::SQLiteConnection conn;

        void SetUp() override {
            auto res = conn.open(":memory:");
            ASSERT_TRUE(res.isOk()) << res.unwrapErr();

            ASSERT_TRUE(conn.execute(R"(CREATE TABLE users (
                id INTEGER PRIMARY KEY,
                name TEXT NOT NULL,
                age INTEGER NULL,
                rating FLOAT NULL,
                data BLOB NULL
            ))").isOk());
        }

        void TearDown() override {
            auto res = conn.close();
            ASSERT_TRUE(res.isOk()) << res.unwrapErr();
        }
    };
}

TEST(DB_SQLiteConnection, OpenCloseGuards) {
    DB::SQLiteConnection c;

    auto closeRes = c.close();
    ASSERT_TRUE(closeRes.isErr());
    EXPECT_EQ(closeRes.unwrapErr(), std::string("Database not opened"));

    ASSERT_TRUE(c.open(":memory:").isOk());

    auto reopen = c.open(":memory:");
    ASSERT_TRUE(reopen.isErr());
    EXPECT_EQ(reopen.unwrapErr(), std::string("Database already opened"));

    ASSERT_TRUE(c.close().isOk());
}

TEST_F(SQLiteConnectionFixture, ExecuteDDLAndInvalidSQL) {
    EXPECT_TRUE(conn.execute("CREATE TABLE items (id INTEGER PRIMARY KEY, title TEXT)").isOk());

    auto bad = conn.execute("CREAT TABLE broken(");
    ASSERT_TRUE(bad.isErr());
    auto msg = bad.unwrapErr();
    EXPECT_NE(msg.find("Failed to prepare statement:"), std::string::npos) << msg;
}

TEST_F(SQLiteConnectionFixture, PreparedStatementInsertAndSelect) {
    auto prepInsRes = conn.prepareStatement("INSERT INTO users (id, name, age, rating, data) VALUES (?, ?, ?, ?, ?)");
    ASSERT_TRUE(prepInsRes.isOk()) << prepInsRes.unwrapErr();
    auto ps = std::move(prepInsRes).unwrap();

    ASSERT_TRUE(ps.bindInt(1, 1).isOk());
    ASSERT_TRUE(ps.bindString(2, "Alice").isOk());
    ASSERT_TRUE(ps.bindInt(3, 30).isOk());
    ASSERT_TRUE(ps.bindDouble(4, 4.5).isOk());
    std::vector<std::uint8_t> blob1{1,2,3,4};
    ASSERT_TRUE(ps.bindBlob(5, blob1).isOk());
    ASSERT_TRUE(ps.execute().isOk());

    ASSERT_TRUE(ps.reset().isOk());
    ASSERT_TRUE(ps.clearBindings().isOk());

    ASSERT_TRUE(ps.bindInt(1, 2).isOk());
    ASSERT_TRUE(ps.bindString(2, "Bob").isOk());
    ASSERT_TRUE(ps.bindNull(3).isOk());
    ASSERT_TRUE(ps.bindNull(4).isOk());
    ASSERT_TRUE(ps.bindNull(5).isOk());
    ASSERT_TRUE(ps.execute().isOk());

    auto rowRes = conn.fetchRow("SELECT id, name, age, rating, data FROM users WHERE id = 1");
    ASSERT_TRUE(rowRes.isOk()) << rowRes.unwrapErr();
    auto row = std::move(rowRes).unwrap();

    EXPECT_EQ(row.get<int>(0).unwrap(), 1);
    EXPECT_EQ(row.get<std::string>(1).unwrap(), "Alice");
    EXPECT_EQ(row.get<int>(2).unwrap(), 30);
    EXPECT_DOUBLE_EQ(row.get<double>(3).unwrap(), 4.5);
    auto blob = row.get<std::vector<std::uint8_t>>(4).unwrap();
    EXPECT_EQ(blob, (std::vector<std::uint8_t>{1,2,3,4}));

    auto prepSelRes = conn.prepareStatement("SELECT id, name, age, rating, data FROM users WHERE id = ?");
    ASSERT_TRUE(prepSelRes.isOk()) << prepSelRes.unwrapErr();
    auto sel = std::move(prepSelRes).unwrap();

    ASSERT_TRUE(sel.bindInt(1, 2).isOk());
    auto row2Res = sel.fetchOne();
    ASSERT_TRUE(row2Res.isOk()) << row2Res.unwrapErr();
    auto row2 = std::move(row2Res).unwrap();

    EXPECT_EQ(row2.get<int>(0).unwrap(), 2);
    EXPECT_EQ(row2.get<std::string>(1).unwrap(), "Bob");

    auto ageOpt = row2.get<std::optional<int>>(2).unwrap();
    EXPECT_FALSE(ageOpt.has_value());
    auto ratingOpt = row2.get<std::optional<double>>(3).unwrap();
    EXPECT_FALSE(ratingOpt.has_value());
    auto blobOpt = row2.get<std::optional<std::vector<std::uint8_t>>>(4).unwrap();
    EXPECT_FALSE(blobOpt.has_value());
}

TEST_F(SQLiteConnectionFixture, RowGetterErrorsAndBounds) {
    ASSERT_TRUE(conn.execute("INSERT INTO users (id, name) VALUES (10, 'X')").isOk());

    auto rowRes = conn.fetchRow("SELECT id, name FROM users WHERE id = 10");
    ASSERT_TRUE(rowRes.isOk());
    auto row = std::move(rowRes).unwrap();

    auto s0 = row.getString(0);
    ASSERT_TRUE(s0.isErr());
    EXPECT_EQ(s0.unwrapErr(), std::string("Column is not of type TEXT"));

    auto i1 = row.getInt(1);
    ASSERT_TRUE(i1.isErr());
    EXPECT_EQ(i1.unwrapErr(), std::string("Column is not of type INTEGER"));

    auto bad = row.getString(2);
    ASSERT_TRUE(bad.isErr());
    EXPECT_EQ(bad.unwrapErr(), std::string("Column index out of range"));
}

TEST_F(SQLiteConnectionFixture, FetchAllIterationAndNext) {
    ASSERT_TRUE(conn.execute("INSERT INTO users (id, name) VALUES (1, 'A')").isOk());
    ASSERT_TRUE(conn.execute("INSERT INTO users (id, name) VALUES (2, 'B')").isOk());
    ASSERT_TRUE(conn.execute("INSERT INTO users (id, name) VALUES (3, 'C')").isOk());

    auto frRes = conn.fetchAll("SELECT id, name FROM users ORDER BY id ASC");
    ASSERT_TRUE(frRes.isOk()) << frRes.unwrapErr();
    auto fr = std::move(frRes).unwrap();

    std::vector<std::pair<int, std::string>> items;
    for (auto r : fr) {
        items.emplace_back(r.get<int>(0).unwrap(), r.get<std::string>(1).unwrap());
    }
    ASSERT_EQ(items.size(), 3u);
    EXPECT_EQ(items[0], std::make_pair(1, std::string("A")));
    EXPECT_EQ(items[1], std::make_pair(2, std::string("B")));
    EXPECT_EQ(items[2], std::make_pair(3, std::string("C")));

    auto frRes2 = conn.fetchAll("SELECT id FROM users ORDER BY id DESC");
    ASSERT_TRUE(frRes2.isOk());
    auto fr2 = std::move(frRes2).unwrap();
    std::vector<int> ids;
    while (true) {
        auto has = fr2.next();
        ASSERT_TRUE(has.isOk()) << has.unwrapErr();
        if (!has.unwrap()) break;
        ids.push_back(fr2.getRow().get<int>(0).unwrap());
    }
    EXPECT_EQ(ids, (std::vector<int>{3,2,1}));
}

TEST_F(SQLiteConnectionFixture, TupleFetchAndVariadicExecute) {
    auto ins = conn.execute("INSERT INTO users (id, name) VALUES (?, ?)", 7, std::string("Seven"));
    ASSERT_TRUE(ins.isOk()) << ins.unwrapErr();

    auto tupRes = conn.fetchOne<int, std::string>("SELECT id, name FROM users WHERE id = ?", 7);
    ASSERT_TRUE(tupRes.isOk()) << tupRes.unwrapErr();
    auto [id, name] = std::move(tupRes).unwrap();
    EXPECT_EQ(id, 7);
    EXPECT_EQ(name, "Seven");

    auto none = conn.fetchRow("SELECT id FROM users WHERE id = -999");
    ASSERT_TRUE(none.isErr());
    EXPECT_EQ(none.unwrapErr(), std::string("No rows returned"));
}

TEST_F(SQLiteConnectionFixture, FetchAllEmptyResult) {
    auto frRes = conn.fetchAll("SELECT id FROM users WHERE 1 = 0");
    ASSERT_TRUE(frRes.isOk()) << frRes.unwrapErr();
    auto fr = std::move(frRes).unwrap();

    size_t count = 0;
    for (auto r : fr) {
        (void)r;
        ++count;
    }
    EXPECT_EQ(count, 0u);

    auto frRes2 = conn.fetchAll("SELECT id FROM users WHERE 1 = 0");
    ASSERT_TRUE(frRes2.isOk());
    auto fr2 = std::move(frRes2).unwrap();
    auto has = fr2.next();
    ASSERT_TRUE(has.isOk());
    EXPECT_FALSE(has.unwrap());
}

TEST_F(SQLiteConnectionFixture, BindIndexOutOfRangeErrorsHavePrefix) {
    auto psRes = conn.prepareStatement("SELECT 1");
    ASSERT_TRUE(psRes.isOk()) << psRes.unwrapErr();
    auto ps = std::move(psRes).unwrap();

    auto b1 = ps.bindInt(0, 42);
    ASSERT_TRUE(b1.isErr());
    EXPECT_NE(b1.unwrapErr().find("bindInt failed:"), std::string::npos);

    auto b2 = ps.bindString(0, "x");
    ASSERT_TRUE(b2.isErr());
    EXPECT_NE(b2.unwrapErr().find("bindString failed:"), std::string::npos);

    auto b3 = ps.bindBlob(0, {});
    ASSERT_TRUE(b3.isErr());
    EXPECT_NE(b3.unwrapErr().find("bindBlob failed:"), std::string::npos);
}

TEST_F(SQLiteConnectionFixture, LargeIntegerAndEmptyBlobRoundtrip) {
    auto prepInsRes = conn.prepareStatement("INSERT INTO users (id, name, age, data) VALUES (?, ?, ?, ?)");
    ASSERT_TRUE(prepInsRes.isOk()) << prepInsRes.unwrapErr();
    auto ps = std::move(prepInsRes).unwrap();

    int64_t big = std::numeric_limits<int64_t>::max();
    ASSERT_TRUE(ps.bindInt(1, 100).isOk());
    ASSERT_TRUE(ps.bindString(2, "Big").isOk());
    ASSERT_TRUE(ps.bindInt(3, big).isOk());
    std::vector<std::uint8_t> emptyBlob;
    ASSERT_TRUE(ps.bindBlob(4, emptyBlob).isOk());
    ASSERT_TRUE(ps.execute().isOk());

    auto rowRes = conn.fetchRow("SELECT age, data FROM users WHERE id = 100");
    ASSERT_TRUE(rowRes.isOk()) << rowRes.unwrapErr();
    auto row = std::move(rowRes).unwrap();

    EXPECT_EQ(row.get<int64_t>(0).unwrap(), big);
    auto blob = row.get<std::vector<std::uint8_t>>(1).unwrap();
    EXPECT_TRUE(blob.empty());
}

TEST_F(SQLiteConnectionFixture, InvalidSQLInFetchAllAndFetchRow) {
    auto all = conn.fetchAll("SELEC * FROM nope");
    ASSERT_TRUE(all.isErr());
    EXPECT_NE(all.unwrapErr().find("Failed to prepare statement:"), std::string::npos);

    auto one = conn.fetchRow("SELEC * FROM nope");
    ASSERT_TRUE(one.isErr());
    EXPECT_NE(one.unwrapErr().find("Failed to prepare statement:"), std::string::npos);
}

TEST_F(SQLiteConnectionFixture, TupleFetchWithOptionalsAndMismatch) {
    ASSERT_TRUE(conn.execute("INSERT INTO users (id, name, age, rating) VALUES (55, 'N', 42, NULL)").isOk());

    auto tupRes = conn.fetchOne<std::optional<int64_t>, std::string>(
        "SELECT age, name FROM users WHERE id = ?", 55
    );
    ASSERT_TRUE(tupRes.isOk()) << tupRes.unwrapErr();
    auto [age, name] = std::move(tupRes).unwrap();
    ASSERT_TRUE(age.has_value());
    EXPECT_EQ(*age, 42);
    EXPECT_EQ(name, "N");

    auto bad = conn.fetchOne<int>("SELECT name FROM users WHERE id = ?", 55);
    ASSERT_TRUE(bad.isErr());
    EXPECT_NE(bad.unwrapErr().find("Column is not of type INTEGER"), std::string::npos);
}

TEST_F(SQLiteConnectionFixture, IsNullChecks) {
    ASSERT_TRUE(conn.execute("INSERT INTO users (id, name, rating) VALUES (60, 'Z', NULL)").isOk());
    auto rowRes = conn.fetchRow("SELECT rating FROM users WHERE id = 60");
    ASSERT_TRUE(rowRes.isOk());
    auto row = std::move(rowRes).unwrap();

    auto isNull = row.isNull(0);
    ASSERT_TRUE(isNull.isOk());
    EXPECT_TRUE(isNull.unwrap());

    auto dbl = row.getDouble(0);
    ASSERT_TRUE(dbl.isErr());
    EXPECT_EQ(dbl.unwrapErr(), std::string("Column is not of type FLOAT"));

    auto opt = row.get<std::optional<double>>(0);
    ASSERT_TRUE(opt.isOk());
    EXPECT_FALSE(opt.unwrap().has_value());
}

TEST(DB_SQLiteConnection, DoubleCloseGuard) {
    DB::SQLiteConnection c;
    ASSERT_TRUE(c.open(":memory:").isOk());
    ASSERT_TRUE(c.close().isOk());
    auto closeAgain = c.close();
    ASSERT_TRUE(closeAgain.isErr());
    EXPECT_EQ(closeAgain.unwrapErr(), std::string("Database not opened"));
}
