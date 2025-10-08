#pragma once

#include "../json.h"
#include <format>

template<>
struct std::formatter<json::Value> {
    template<typename FormatCtx>
    constexpr static auto parse(FormatCtx &ctx) {
        return ctx.begin();
    }

    template<typename FormatCtx>
    constexpr static auto format(const json::Value &v, FormatCtx &ctx) {
        switch (v.type()) {
            case json::Value::Number: return std::format_to(ctx.out(), "{}", v.as_number());
            default: throw std::runtime_error("invalid json::Value type");
        }
    }
};