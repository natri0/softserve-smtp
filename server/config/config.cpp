#include "config.h"

#include <fstream>
#include "json.h"

static void populate(const json::Value &root, std::unordered_map<std::string, std::any> &map, const std::string &prefix = "") {
    switch (root.type()) {
        case json::Value::Object: {
            const auto &obj = root.as_object();
            for (const auto &[key, value] : obj) {
                std::string child_path = prefix.empty() ? key : prefix + "." + key;
                populate(value, map, child_path);
            }

            std::string length_path = (prefix.empty() ? "_length" : prefix + "._length");
            populate(double(obj.size()), map, length_path);

            int i = 0;
            for (const auto &[key, _] : obj) {
                std::string path = (prefix.empty() ? "_keys." : prefix + "._keys.") + std::to_string(i);
                map[std::move(path)] = key;
                i++;
            }
            break;
        }
        case json::Value::Array: {
            const auto &arr = root.as_array();
            for (size_t i = 0; i < arr.size(); i++) {
                std::string child_path = prefix + "." + std::to_string(i);
                populate(arr[i], map, child_path);
            }

            std::string length_path = (prefix.empty() ? "_length" : prefix + "._length");
            populate(double(arr.size()), map, length_path);
            break;
        }
        case json::Value::Number:
            map[prefix] = root.as_number();
            break;
        case json::Value::String:
            map[prefix] = root.as_string();
            break;
        case json::Value::Boolean:
            map[prefix] = root.as_boolean();
            break;
        case json::Value::Null:
            map[prefix] = nullptr;
            break;
    }
}

bool Config::load_from_file(const std::filesystem::path &path) {
    std::ifstream input(path);
    if (!input.is_open()) return false;

    input.seekg(0, std::ios::end);
    auto size = input.tellg();
    input.seekg(0, std::ios::beg);

    auto string = std::string(size, '\0');
    input.read(string.data(), size);
    input.close();

    return load_from_string(string);
}

bool Config::load_from_string(const std::string &str) {
    const char *c_str = str.c_str();
    const auto root = json::visit_element(c_str);
    if (!root.has_value()) return false;

    populate(*root, this->data);
    return true;
}

bool Config::has_key(const std::string &key) const {
    return data.contains(key);
}

std::any Config::get_any(const std::string &key) const {
    if (!has_key(key)) throw std::invalid_argument("Key not found");

    return data.at(key);
}
