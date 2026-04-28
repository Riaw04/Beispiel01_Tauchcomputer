#include "divecomputer/dive.h"

#include <sstream>
#include <utility>

#include "divecomputer/dive_data_exception.h"

namespace divecomputer {

namespace {

template <typename T>
std::string ToString(const T& value) {
    std::ostringstream out;
    out << value;
    return out.str();
}

} // namespace

Dive::Dive(std::string name)
    : name_(std::move(name)) {
    if (name_.empty()) {
        throw DiveDataException("Dive name must not be empty.");
    }
}

void Dive::AddMeasurement(const int second, const double depthMeters) {
    ValidateMeasurement(second, depthMeters);

    if (!measurements_.empty() && second <= measurements_.back().second) {
        throw DiveDataException("Timestamp " + ToString(second) +
                                " must be greater than previous timestamp " +
                                ToString(measurements_.back().second) + ".");
    }

    measurements_.push_back({second, depthMeters});
}

const std::string& Dive::Name() const noexcept {
    return name_;
}

const std::vector<Measurement>& Dive::Measurements() const noexcept {
    return measurements_;
}

bool Dive::RateBetween(const std::size_t currentIndex, double& rateOut) const {
    if (currentIndex == 0 || currentIndex >= measurements_.size()) {
        return false;
    }

    const Measurement& previous = measurements_[currentIndex - 1];
    const Measurement& current = measurements_[currentIndex];
    const int deltaSeconds = current.second - previous.second;
    if (deltaSeconds <= 0) {
        throw DiveDataException("Internal error: non-positive timestamp delta.");
    }

    // Negativ => Abstieg (tiefer), Positiv => Aufstieg (flacher).
    rateOut = (previous.depthMeters - current.depthMeters) / static_cast<double>(deltaSeconds);
    return true;
}

void Dive::ValidateMeasurement(const int second, const double depthMeters) {
    if (second < 0) {
        throw DiveDataException("Timestamp " + ToString(second) + " must be >= 0.");
    }
    if (depthMeters < 0.0) {
        throw DiveDataException("Depth " + ToString(depthMeters) + " must be >= 0.");
    }
}

} // namespace divecomputer
