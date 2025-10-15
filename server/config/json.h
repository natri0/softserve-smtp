#pragma once

#include <optional>
#include <vector>

namespace json {

    struct BadJson final : std::runtime_error {
        explicit BadJson(const char *str) : std::runtime_error(str) {}
        explicit BadJson(const std::string &str) : std::runtime_error(str) {}
    };

    class Value {
    public:
        Value(double val) : _type(Number), number(val) {}
        Value(int val) : _type(Number), number(val) {}
        Value(bool val) : _type(Boolean), boolean(val) {}

        Value(const std::vector<Value> &val) : _type(Array), array(val) {}
        Value(std::vector<Value> &&val) : _type(Array), array(std::move(val)) {}

        Value(const std::string &val) : _type(String), string(val) {}

        static Value get_null() {
            Value x{0};
            x._type = Null;
            return x;
        }

        Value(const Value &val) {
            memcpy(this, &val, sizeof(val));
        }

        Value &operator=(const Value &val) {
            switch (_type = val._type) {
                case Null: break;
                case Number: number = val.number; break;
                case Boolean: boolean = val.boolean; break;
                case String: new (&string) std::string(val.string); break;
                case Array: new (&array) std::vector(val.array); break;
            }
            return *this;
        }

        ~Value() {
            switch (_type) {
                case String: string.~basic_string(); break;
                case Array: array.~vector(); break;
                default: break;
            }
        }

        enum Type {
            Number,
            Array,
            String,
            Null,
            Boolean,
            // todo: add more types to json::Value
        };

        Type type() const { return _type; }

        const double &as_number() const { return number; }
        const std::vector<Value> &as_array() const { return array; }
        const std::string &as_string() const { return string; }
        const bool &as_boolean() const { return boolean; }
    private:
        Type _type;
        union {
            double number;
            std::vector<Value> array;
            std::string string;
            bool boolean;
        };
    };

    using Array = std::vector<Value>;

    std::optional<double> visit_number(const char *&string);
    std::optional<std::string> visit_string(const char *&string);

    std::optional<Array> visit_array(const char *&string);

    std::optional<Value> visit_element(const char *&string);
}
