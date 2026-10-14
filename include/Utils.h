#pragma once
// Small string helpers shared across the project.

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "Exceptions.h"

namespace util {

// Split on a delimiter, keeping empty fields ("a||b" -> {"a", "", "b"}).
inline std::vector<std::string> split(const std::string& text, char delim) {
    std::vector<std::string> parts;
    std::string current;
    for (char c : text) {
        if (c == delim) {
            parts.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    parts.push_back(current);
    return parts;
}

inline std::string join(const std::vector<std::string>& parts, char delim) {
    std::string out;
    for (std::size_t i = 0; i < parts.size(); ++i) {
        if (i) out += delim;
        out += parts[i];
    }
    return out;
}

inline std::string trim(const std::string& text) {
    const char* ws = " \t\r\n";
    const auto start = text.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    const auto end = text.find_last_not_of(ws);
    return text.substr(start, end - start + 1);
}

inline std::string toLower(std::string text) {
    for (char& c : text) {
        if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    }
    return text;
}

// '|' and '~' are reserved by the save-file format.
inline void requireClean(const std::string& value, const std::string& field) {
    if (trim(value).empty()) {
        throw ValidationException(field + " cannot be empty");
    }
    if (value.find('|') != std::string::npos || value.find('~') != std::string::npos) {
        throw ValidationException(field + " cannot contain '|' or '~'");
    }
}

// Replaces reserved characters instead of rejecting (used for free-text notes).
inline std::string sanitize(std::string text) {
    for (char& c : text) {
        if (c == '|' || c == '~') c = '/';
    }
    return trim(text);
}

inline std::string money(double amount) {
    std::ostringstream os;
    os << "Rs. " << std::fixed << std::setprecision(2) << amount;
    return os.str();
}

}  // namespace util
