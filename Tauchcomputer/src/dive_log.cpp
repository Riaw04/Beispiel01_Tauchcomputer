#include "divecomputer/dive_log.h"

#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>

#include "divecomputer/constants.h"
#include "divecomputer/dive_data_exception.h"

namespace divecomputer {

namespace {

std::string PadRight(const std::string& value, const int width) {
    if (static_cast<int>(value.size()) >= width) {
        return value;
    }

    return value + std::string(static_cast<std::size_t>(width - static_cast<int>(value.size())), ' ');
}

std::string FormatFixed(const double value, const int precision, const bool showPlus) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision);
    if (showPlus) {
        out << std::showpos;
    }
    out << value;
    return out.str();
}

template <typename T>
std::string ToString(const T& value) {
    std::ostringstream out;
    out << value;
    return out.str();
}

} // namespace

void DiveLog::AddDive(Dive dive) {
    if (dive.Measurements().empty()) {
        throw DiveDataException("Dive '" + dive.Name() + "' contains no measurements.");
    }
    dives_.push_back(std::move(dive));
}

void DiveLog::Print(std::ostream& out) const {
    if (!out) {
        throw DiveDataException("Output stream is not writable.");
    }

    for (std::size_t diveIndex = 0; diveIndex < dives_.size(); ++diveIndex) {
        PrintSingleDive(out, dives_[diveIndex], diveIndex + 1);
        if (diveIndex + 1 < dives_.size()) {
            out << '\n';
        }
    }
}

std::string DiveLog::FormatTime(const int totalSeconds) {
    const int hh = totalSeconds / 3600;
    const int mm = (totalSeconds % 3600) / 60;
    const int ss = totalSeconds % 60;

    std::ostringstream out;
    out << std::setfill('0') << std::setw(2) << hh << ':'
        << std::setw(2) << mm << ':'
        << std::setw(2) << ss;
    return out.str();
}

std::string DiveLog::Column(const std::string& value) {
    return PadRight(value, kColumnWidth);
}

void DiveLog::PrintSingleDive(std::ostream& out, const Dive& dive, const std::size_t serial) {
    const std::string gap(kColumnGap, ' ');

    out << "Dive " << ToString(serial) << ": " << dive.Name() << '\n';
    out << Column("Dive Time") << gap << Column("Dive Depth") << gap << Column("Down/Up") << '\n';
    out << Column("(hh:mm:ss)") << gap << Column("(m)") << gap << Column("(m/sec)") << '\n';
    out << std::string(kColumnWidth * 3 + kColumnGap * 2, '-') << '\n';

    const std::vector<Measurement>& measurements = dive.Measurements();
    for (std::size_t i = 0; i < measurements.size(); ++i) {
        const Measurement& m = measurements[i];

        const std::string timeCell = Column(FormatTime(m.second));
        const std::string depthCell = Column(FormatFixed(m.depthMeters, kDepthPrecision, false));

        std::string rateValue;
        double rate = 0.0;
        if (dive.RateBetween(i, rate)) {
            rateValue = FormatFixed(rate, kRatePrecision, true);
        }

        out << timeCell << gap << depthCell << gap << Column(rateValue) << '\n';
    }
}

} // namespace divecomputer
