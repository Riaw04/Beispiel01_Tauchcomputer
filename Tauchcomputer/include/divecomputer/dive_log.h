#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

#include "divecomputer/dive.h"

namespace divecomputer {

class DiveLog {
public:
    void AddDive(Dive dive);
    void Print(std::ostream& out) const;

private:
    static std::string FormatTime(int totalSeconds);
    static std::string Column(const std::string& value);
    static void PrintSingleDive(std::ostream& out, const Dive& dive, std::size_t serial);

    std::vector<Dive> dives_;
};

} // namespace divecomputer
