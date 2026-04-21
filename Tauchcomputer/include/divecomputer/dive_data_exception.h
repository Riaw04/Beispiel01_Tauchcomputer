#pragma once

#include <stdexcept>
#include <string>

namespace divecomputer {

class DiveDataException : public std::runtime_error {
public:
    explicit DiveDataException(const std::string& message)
        : std::runtime_error(message) {}
};

} // namespace divecomputer
