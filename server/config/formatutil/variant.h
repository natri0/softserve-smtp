#pragma once

#include <variant>
#include <format>

template<class... Variants>
struct std::formatter<std::variant<Variants...>> {
    using variant_type = std::variant<Variants...>;

    template<typename FormatCtx>
    constexpr static auto parse(FormatCtx &ctx) {
        return ctx.begin();
    }

    template<typename FormatCtx>
    constexpr static auto format(const variant_type &v, FormatCtx &ctx) {
        return std::visit(
            [&ctx](auto const &value) { return std::format_to(ctx.out(), "variant({})", value); },
            v
        );
    }
};
