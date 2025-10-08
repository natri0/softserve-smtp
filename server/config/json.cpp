#include "json.h"

#include <ostream>
#include <format>

#include "formatutil/optional.h"
#include "formatutil/variant.h"

static auto json_doc = R"(1.0)";

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

std::optional<json::Value> json::visit_element(const char *&string) {
    if (auto number = visit_number(string); number.has_value()) return { *number };
    return {};
}

int main() {
    std::println("{}", json::visit_element(json_doc));
}
