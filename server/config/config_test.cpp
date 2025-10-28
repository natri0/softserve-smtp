#include <gtest/gtest.h>

#include <format>
#include "formatutil/jsonvalue.h"
#include "formatutil/optional.h"

#include "config.h"

struct ConfigReaderTest : public ::testing::Test {
    Config config;
};

#define SUITE ConfigReaderTest
#define T(nm) TEST_F(SUITE, nm)

T(just_created__IsEmpty) {
    EXPECT_EQ(0, config.size());
}

T(load_from_string__ReturnsTrueOnValidJson) {
    EXPECT_EQ(true, config.load_from_string(R"({ "key": "value" })"));
    EXPECT_EQ(true, config.has_key("key"));

    EXPECT_EQ(true, config.load_from_string(R"({ "array": [1,2,3], "bool": true, "null": null })"));
    EXPECT_EQ(true, config.has_key("array.0"));
    EXPECT_EQ(true, config.has_key("bool"));
    EXPECT_EQ(true, config.has_key("null"));
}

T(load_from_string__GeneratesFieldsForObject) {
    config.load_from_string(R"({ "key": "value" })");
    EXPECT_EQ(true, config.has_key("_length"));
    EXPECT_EQ(true, config.has_key("_keys.0"));
}

T(get__ThrowsIfNoSuchKey) {
    EXPECT_THROW({
        config.get<std::string>("DoesNotExist!!");
    }, std::invalid_argument);
}

T(get__ThrowsIfBadType) {
    config.load_from_string(R"({ "not-a-string": 123 })");

    EXPECT_THROW({
        config.get<std::string>("not-a-string");
    }, std::bad_any_cast);
}

T(get__ReturnsValueWhenExists) {
    config.load_from_string(R"({ "key": "value" })");
    EXPECT_EQ("value", config.get<std::string>("key"));
}

T(get_any__ReturnsValueWhenExists) {
    config.load_from_string(R"({ "key": "value" })");
    EXPECT_EQ("value", std::any_cast<std::string>(config.get_any("key")));
}
