#pragma once

#include <variant>
#include <optional>

namespace json {
    using Value = std::variant<double /* todo: add more types to json::Value */>;

    std::optional<double> visit_number(const char *&string);
    // todo: more visit_xxx()

    std::optional<Value> visit_element(const char *&string);
}
