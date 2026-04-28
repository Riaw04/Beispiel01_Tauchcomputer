#pragma once

#include <cstddef>
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

    const std::string& Name() const noexcept;
    const std::vector<Measurement>& Measurements() const noexcept;
    bool RateBetween(std::size_t currentIndex, double& rateOut) const;

private:
    static void ValidateMeasurement(int second, double depthMeters);

    std::string name_;
    std::vector<Measurement> measurements_;
};

} // namespace divecomputer
