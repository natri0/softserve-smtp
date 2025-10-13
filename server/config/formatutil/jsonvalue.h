#pragma once

#include "../json.h"
#include "vector.h"
#include <format>

namespace json {
    inline std::string to_string(const Value &value);
}

template<>
struct std::formatter<json::Value> {
    template<typename FormatCtx>
    constexpr static auto parse(FormatCtx &ctx) {
        return ctx.begin();
    }

    template<typename FormatCtx>
    constexpr static auto format(const json::Value &v, FormatCtx &ctx) {
        return format_to(ctx.out(), "{}", to_string(v));
    }
};

namespace json {
    inline std::string to_string(const Value &value) {
        switch (value.type()) {
            case Value::Number: return std::to_string(value.as_number());
            case Value::Array: {
                std::string buf = "[";
                for (int i = 0; i < value.as_array().size(); i++) {
                    if (i != 0) buf.append(", ");
                    buf.append(to_string(value.as_array()[i]));
                }
                return std::move(buf.append("]"));
            }
            default: throw std::runtime_error("invalid json::Value type");
        }
    }
}
