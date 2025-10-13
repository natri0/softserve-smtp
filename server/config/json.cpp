#include "json.h"

#include <ostream>
#include <format>

#include "formatutil/optional.h"
#include "formatutil/jsonvalue.h"

static auto json_doc = R"([1.0])";

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

std::optional<json::Value> json::visit_element(const char *&string) {
    while (isspace(*string)) string++;

    if (auto number = visit_number(string); number.has_value()) return { *number };
    if (auto array = visit_array(string); array.has_value()) return { std::move(*array) };
    if (auto str = visit_string(string); str.has_value()) return { std::move(*str) };
    return {};
}

int main() {
    std::println("{}", json::visit_element(json_doc));
}
