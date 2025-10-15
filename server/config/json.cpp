#include "json.h"

#include <ostream>
#include <format>

#include "formatutil/optional.h"
#include "formatutil/jsonvalue.h"

static auto json_doc = R"({ "key1": "value1", "key2": ["array", null, 1.0], "key3": true, "key4": { "nested": "yes" } })";

/// convert unicode codepoint to utf8
static size_t code_to_utf8(unsigned char *const buffer, const unsigned int code)
{
    if (code <= 0x7F) {
        buffer[0] = code;
        return 1;
    }
    if (code <= 0x7FF) {
        buffer[0] = 0xC0 | (code >> 6);            /* 110xxxxx */
        buffer[1] = 0x80 | (code & 0x3F);          /* 10xxxxxx */
        return 2;
    }
    if (code <= 0xFFFF) {
        buffer[0] = 0xE0 | (code >> 12);           /* 1110xxxx */
        buffer[1] = 0x80 | ((code >> 6) & 0x3F);   /* 10xxxxxx */
        buffer[2] = 0x80 | (code & 0x3F);          /* 10xxxxxx */
        return 3;
    }
    if (code <= 0x10FFFF) {
        buffer[0] = 0xF0 | (code >> 18);           /* 11110xxx */
        buffer[1] = 0x80 | ((code >> 12) & 0x3F);  /* 10xxxxxx */
        buffer[2] = 0x80 | ((code >> 6) & 0x3F);   /* 10xxxxxx */
        buffer[3] = 0x80 | (code & 0x3F);          /* 10xxxxxx */
        return 4;
    }
    return 0;
}

std::optional<double> json::visit_number(const char *&string) {
    size_t idx_after;
    try {
        double val = std::stod(string, &idx_after);
        string += idx_after;
        return val;
    } catch (std::invalid_argument &e) {
        return {};
    }
}

std::optional<std::string> json::visit_string(const char *&string) {
    const char *begin = string;

    if (*string++ != '"') { string = begin; return {}; }

    std::string str;

    while (*string) {
        size_t next_escape_or_quote = strcspn(string, "\\\"");
        str.append(string, &string[next_escape_or_quote]); // append all characters before \escape or end of string

        switch (string[next_escape_or_quote]) {
            case '\\': {
                char esc = string[next_escape_or_quote+1];
                char bytes[5] = { 0 };
                size_t n_bytes_to_add = 1, to_skip = 1;
                switch (esc) {
                    case '"': bytes[0] = '"'; break;
                    case '\\': bytes[0] = '\\'; break;
                    case '/': bytes[0] = '/'; break;
                    case 'b': bytes[0] = '\b'; break;
                    case 'f': bytes[0] = '\f'; break;
                    case 'n': bytes[0] = '\n'; break;
                    case 'r': bytes[0] = '\r'; break;
                    case 't': bytes[0] = '\t'; break;
                    case 'u': {
                        strncpy(bytes, string + next_escape_or_quote, to_skip = 4);
                        if (!ishexnumber(bytes[0]) || !ishexnumber(bytes[1]) || !ishexnumber(bytes[2]) || !ishexnumber(bytes[3])) {
                            string = begin;
                            return {};
                        }

                        int codepoint = strtol(bytes, nullptr, 16);
                        n_bytes_to_add = code_to_utf8(reinterpret_cast<unsigned char *>(bytes), codepoint);
                        if (n_bytes_to_add == 0) {
                            string = begin;
                            return {};
                        }

                        break;
                    }
                    default: { string = begin; return {}; }
                }

                // skip to after the escape sequence
                string = &string[next_escape_or_quote+1+to_skip];
                str.append(bytes, n_bytes_to_add);
                break;
            }

            case '"': {
                string = &string[next_escape_or_quote+1];
                return { str };
            }
            default: {
                string = begin;
                return {};
            }
        }
    }

    return {};
}


std::optional<json::Array> json::visit_array(const char *&string) {
    const char *begin = string;

    if (*string++ != '[') {
        string = begin;
        return {};
    }

    Array arr;

    while (true) {
        if (auto value = visit_element(string); value.has_value()) {
            arr.push_back(*value);
        }

        while (isspace(*string)) string++;
        switch (*string++) {
            case ']': return { arr };
            case ',': continue;
            default: {
                string = begin;
                return {};
            }
        }
    }
}

std::optional<std::unordered_map<std::string, json::Value>> json::visit_object(const char *&string) {
    const char *begin = string;

    if (*string++ != '{') {
        string = begin;
        return {};
    }

    std::unordered_map<std::string, Value> map;

    while (true) {
        std::string key;
        while (isspace(*string)) string++;
        if (auto k = visit_string(string); k.has_value()) {
            key = k.value();
        } else {
            string = begin;
            return {};
        }

        while (isspace(*string)) string++;
        if (*string++ != ':') {
            string = begin;
            return {};
        }

        while (isspace(*string)) string++;
        if (auto value = visit_element(string); value.has_value()) {
            map.insert({ key, *value });
        }

        while (isspace(*string)) string++;
        switch (*string++) {
            case '}': return { map };
            case ',': continue;
            default: {
                string = begin;
                return {};
            }
        }
    }
}

std::optional<json::Value> json::visit_element(const char *&string) {
    while (isspace(*string)) string++;

    // using memcmp here because we only need to check if it _starts with_ "null" in cases this is part of an array/object
    if (!memcmp(string, "null", 4)) {
        string += 4;
        return { Value::get_null() };
    }

    if (!memcmp(string, "true", 4)) {
        string += 4;
        return { true };
    }

    if (!memcmp(string, "false", 5)) {
        string += 5;
        return { false };
    }

    if (auto number = visit_number(string); number.has_value()) return { *number };
    if (auto array = visit_array(string); array.has_value()) return { std::move(*array) };
    if (auto str = visit_string(string); str.has_value()) return { std::move(*str) };
    if (auto obj = visit_object(string); obj.has_value()) return { std::move(*obj) };

    throw BadJson(std::format("failed to parse json: '{}'", string));
}

int main() {
    std::println("{}", json::visit_element(json_doc));
}
