#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace divecomputer {

struct Measurement {
    int second = 0;
    double depthMeters = 0.0;
};

class Dive {
public:
    explicit Dive(std::string name);

    void AddMeasurement(int second, double depthMeters);

    [[nodiscard]] const std::string& Name() const noexcept;
    [[nodiscard]] const std::vector<Measurement>& Measurements() const noexcept;
    [[nodiscard]] std::optional<double> RateBetween(std::size_t currentIndex) const;

private:
    static void ValidateMeasurement(int second, double depthMeters);

    std::string name_;
    std::vector<Measurement> measurements_;
};

} // namespace divecomputer
