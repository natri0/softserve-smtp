#pragma once

#include <optional>
#include <vector>

namespace json {
    class Value {
    public:
        Value(double val) : _type(Number), number(val) {}
        Value(int val) : _type(Number), number(val) {}

        Value(const std::vector<Value> &val) : _type(Array), array(val) {}
        Value(std::vector<Value> &&val) : _type(Array), array(std::move(val)) {}

        Value(const std::string &val) : _type(String), string(val) {}

        Value(const Value &val) {
            memcpy(this, &val, sizeof(val));
        }

        ~Value() {}

        enum Type {
            Number,
            Array,
            String,
            // todo: add more types to json::Value
        };

        Type type() const { return _type; }

        const double &as_number() const { return number; }
        const std::vector<Value> &as_array() const { return array; }
        const std::string &as_string() const { return string; }
    private:
        Type _type;
        union {
            double number;
            std::vector<Value> array;
            std::string string;
        };
    };

    using Array = std::vector<Value>;

    std::optional<double> visit_number(const char *&string);
    std::optional<std::string> visit_string(const char *&string);

    std::optional<Array> visit_array(const char *&string);

    std::optional<Value> visit_element(const char *&string);
}
