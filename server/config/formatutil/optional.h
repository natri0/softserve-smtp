#pragma once

#include <optional>
#include <format>

template<class Inner>
struct std::formatter<std::optional<Inner>> {
    using value_type = std::optional<Inner>;

    template<typename FormatCtx>
    constexpr static auto parse(FormatCtx &ctx) {
        return ctx.begin();
    }

    template<typename FormatCtx>
    constexpr static auto format(const value_type &v, FormatCtx &ctx) {
        if (v.has_value()) return std::format_to(ctx.out(), "optional({})", v.value());
        return std::format_to(ctx.out(), "optional(empty)");
    }
};
