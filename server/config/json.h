#pragma once

#include <optional>

namespace json {
    using Value = std::variant<double /* todo: add more types to json::Value */>;
    class Value {
    public:
        Value(double val) : _type(Number), number(val) {}
        Value(int val) : _type(Number), number(val) {}

        enum Type {
            Number,
            // todo: add more types to json::Value
        };

        Type type() const { return _type; }

        const double &as_number() const { return number; }
    private:
        Type _type;
        union {
            double number;
        };
    };

    std::optional<double> visit_number(const char *&string);
    // todo: more visit_xxx()

    std::optional<Value> visit_element(const char *&string);
}
