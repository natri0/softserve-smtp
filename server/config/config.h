#pragma once

#include <any>
#include <filesystem>
#include <unordered_map>

/// This is the config parser. Values are available by dot-separated keys, for example in the following:
///
/// ```json
/// {
///     "server": {
///         "bind_address": [
///             "127.0.0.1"
///         ]
///     }
/// }
/// ```
///
/// the 127.0.0.1 would be available by `server.bind_address.0`.
///
/// Every object/map/dict/whatever you call it also has its keys available by `._keys.[i]`, and every object and list has its length available by `._length`.
class Config {
public:
    Config() = default;
    ~Config() = default;

    /// Add configuration from a given file into this Config object.
    ///
    /// You can invoke this multiple times to append configs from different sources!
    ///
    /// @return if the file was loaded successfully
    bool load_from_file(const std::filesystem::path &path);

    /// Add configuration from a given string into this Config object.
    ///
    /// You can invoke this multiple times to append configs from different sources!
    ///
    /// @return if the config was loaded successfully
    bool load_from_string(const std::string_view &str);

    bool has_key(const std::string_view &key) const;
    std::any get_any(const std::string_view &key) const;

    template<class T>
    T get(const std::string_view &key) const {
        if (!has_key(key)) throw std::invalid_argument("Key not found");
        return static_cast<T>(std::any_cast<
            std::conditional_t<std::is_arithmetic_v<T> && !std::is_same_v<bool, T>, double, T>
        >(data.at(std::string(key))));
    }

    template<class T>
    T get_with_default(const std::string_view &key, const T &default_value) const {
        if (!has_key(key)) return default_value;
        return static_cast<T>(std::any_cast<
            std::conditional_t<std::is_arithmetic_v<T> && !std::is_same_v<bool, T>, double, T>
        >(data.at(std::string(key))));
    }
private:
    std::unordered_map<std::string, std::any> data;
};
