#include "json.h"

#include <ostream>
#include <format>

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
    return 0;
}

static bool is_hex(char x) {
    return (x >= '0' && x <= '9') || (x >= 'A' && x <= 'F') || (x >= 'a' && x <= 'f');
}

static void skip_whitespace(const char *&string) {
    while (*string && isspace(*string)) string++;
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
                        strncpy(bytes, string + next_escape_or_quote + 2, 4);
                        if (!is_hex(bytes[0]) || !is_hex(bytes[1]) || !is_hex(bytes[2]) || !is_hex(bytes[3])) {
                            string = begin;
                            return {};
                        }

                        int codepoint = strtol(bytes, nullptr, 16);
                        n_bytes_to_add = code_to_utf8(reinterpret_cast<unsigned char *>(bytes), codepoint);
                        // not checking for n_bytes_to_add==0 here because the only way for it to return 0 is if we supply a value of >0xffff
                        // which is impossible because we only have 4 hex digits

                        to_skip = 5; // u0123
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

    // CLion code analysis says that without a return here the function doesn't return on all control paths
    // that's not true because strcspn() will return the offset to \0 if there's no \ or " found
    // in that case, we go to the default: and return
    // so there's no way to get here
    std::unreachable();
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

        skip_whitespace(string);
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

        skip_whitespace(string);
        if (*string++ != ':') {
            string = begin;
            return {};
        }

        skip_whitespace(string);
        if (auto value = visit_element(string); value.has_value()) {
            map.insert({ key, *value });
        }

        skip_whitespace(string);
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
    skip_whitespace(string);

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

    // todo: log about invalid json
    return {};
}
