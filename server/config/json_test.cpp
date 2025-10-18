#include <gtest/gtest.h>

#include "json.h"

#include <format>
#include "formatutil/jsonvalue.h"
#include "formatutil/optional.h"

#define SUITE JSONParser
#define T(nm) TEST(SUITE, nm)

#define EXPECT_NO_VALUE(kind, str) do { \
    const char *s = str; \
    try { \
        auto res = json::visit_element(s); \
        EXPECT_FALSE(res.has_value()) << "'" str "' parsed successfully while it shouldn't've: " << std::format("{}", *res); \
    } catch (const json::BadJson &e) {} \
} while (0)

#define EXPECT_VALUE_OF_TYPE(kind, str) do { \
    const char *s = str; \
    auto res = json::visit_element(s); \
    EXPECT_TRUE(res.has_value()) << "'" str "' didn't parse successfully!"; \
    EXPECT_EQ(json::Value::kind, res->type()) << "expected type " #kind " for '" #str "'. parsed json: " << std::format("{}", *res); \
} while (0)

T(visit_number__ReturnsNoneOnInvalidNumber) {
    EXPECT_NO_VALUE(number, "");
    EXPECT_NO_VALUE(number, "just some random text");
    EXPECT_NO_VALUE(number, "some text and a number after: 123");
}

T(visit_number__ReturnsValidValueOnValidNumber) {
    EXPECT_VALUE_OF_TYPE(Number, "123");
    EXPECT_VALUE_OF_TYPE(Number, "0.1");
    EXPECT_VALUE_OF_TYPE(Number, "1e9");
}

T(visit_string__ReturnsNoneOnInvalidString) {
    EXPECT_NO_VALUE(string, "not a quoted string");
    EXPECT_NO_VALUE(string, "\"only one quote");
    EXPECT_NO_VALUE(string, "only one quote\"");
    EXPECT_NO_VALUE(string, "\"invalid \\Escape sequence\"");
    EXPECT_NO_VALUE(string, "\"invalid \\uXXXX\"");
}

T(visit_string__ReturnsValidValueOnValidString) {
    EXPECT_VALUE_OF_TYPE(String, "\"properly quoted, \\t \\n \\\" \\\\ \\/ \\b \\f \\r \\u0001 \\u0100 \\u1234 \"");
}

T(visit_element__ReturnsNullOnNullLiteral) {
    EXPECT_VALUE_OF_TYPE(Null, "null");
}

T(visit_element__ReturnsBoolOnTrueLiteral) {
    EXPECT_VALUE_OF_TYPE(Boolean, "true");
}

T(visit_element__ReturnsBoolOnFalseLiteral) {
    EXPECT_VALUE_OF_TYPE(Boolean, "false");
}

T(visit_array__ReturnsNoneOnInvalidArray) {
    EXPECT_NO_VALUE(array, "[");
    EXPECT_NO_VALUE(array, "]");
    EXPECT_NO_VALUE(array, "[invalid]");
    EXPECT_NO_VALUE(array, "[,");
    EXPECT_NO_VALUE(array, "[0");
    EXPECT_NO_VALUE(array, "[0,");
}

T(visit_array__ReturnsValidValueOnValidArray) {
    EXPECT_VALUE_OF_TYPE(Array, "[]");
    EXPECT_VALUE_OF_TYPE(Array, "[0]");
    EXPECT_VALUE_OF_TYPE(Array, "[\"\"]");
    EXPECT_VALUE_OF_TYPE(Array, "[0,true]");
    EXPECT_VALUE_OF_TYPE(Array, "[\n\t  0,  true\n]");
    EXPECT_VALUE_OF_TYPE(Array, "[0,]"); // we support trailing comma
}

T(visit_object__ReturnsNoneOnInvalidObject) {
    EXPECT_NO_VALUE(object, "{");
    EXPECT_NO_VALUE(object, "}");
    EXPECT_NO_VALUE(object, "{\"a");
    EXPECT_NO_VALUE(object, "{\"a\"");
    EXPECT_NO_VALUE(object, "{\"a\":");
    EXPECT_NO_VALUE(object, "{\"a\":123");
    EXPECT_NO_VALUE(object, "{true");
    EXPECT_NO_VALUE(object, "{0:");
    EXPECT_NO_VALUE(object, "{0:123}");
    EXPECT_NO_VALUE(object, "{\"a\":123,}");
    EXPECT_NO_VALUE(object, "{\"a\":123,\"b\"}");
}

T(visit_object__ReturnsValidValueOnValidObject) {
    EXPECT_VALUE_OF_TYPE(Object, "{\"a\":123}");
    EXPECT_VALUE_OF_TYPE(Object, "{    \"a\" :   \n123  ,\t\"b\" : true   }");
}
