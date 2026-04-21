#include "divecomputer/dive.h"

#include <format>
#include <utility>

#include "divecomputer/dive_data_exception.h"

namespace divecomputer {

Dive::Dive(std::string name)
    : name_(std::move(name)) {
    if (name_.empty()) {
        throw DiveDataException("Dive name must not be empty.");
    }
}

void Dive::AddMeasurement(const int second, const double depthMeters) {
    ValidateMeasurement(second, depthMeters);

    if (!measurements_.empty() && second <= measurements_.back().second) {
        throw DiveDataException(std::format(
            "Timestamp {} must be greater than previous timestamp {}.",
            second,
            measurements_.back().second));
    }

    measurements_.push_back({second, depthMeters});
}

const std::string& Dive::Name() const noexcept {
    return name_;
}

const std::vector<Measurement>& Dive::Measurements() const noexcept {
    return measurements_;
}

std::optional<double> Dive::RateBetween(const std::size_t currentIndex) const {
    if (currentIndex == 0 || currentIndex >= measurements_.size()) {
        return std::nullopt;
    }

    const Measurement& previous = measurements_[currentIndex - 1];
    const Measurement& current = measurements_[currentIndex];
    const int deltaSeconds = current.second - previous.second;
    if (deltaSeconds <= 0) {
        throw DiveDataException("Internal error: non-positive timestamp delta.");
    }

    // Negativ => Abstieg (tiefer), Positiv => Aufstieg (flacher).
    return (previous.depthMeters - current.depthMeters) / static_cast<double>(deltaSeconds);
}

void Dive::ValidateMeasurement(const int second, const double depthMeters) {
    if (second < 0) {
        throw DiveDataException(std::format("Timestamp {} must be >= 0.", second));
    }
    if (depthMeters < 0.0) {
        throw DiveDataException(std::format("Depth {} must be >= 0.", depthMeters));
    }
}

} // namespace divecomputer
