#include <cassert>
#include <cmath>
#include <exception>
#include <fstream>
#include <format>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace divecomputer {

constexpr int kColumnWidth = 10;
constexpr int kColumnGap = 4;
constexpr int kDepthPrecision = 2;
constexpr int kRatePrecision = 3;

class DiveDataException : public std::runtime_error {
public:
    explicit DiveDataException(const std::string& message)
        : std::runtime_error(message) {}
};

struct Measurement {
    int second = 0;
    double depthMeters = 0.0;
};

class Dive {
public:
    explicit Dive(std::string name)
        : name_(std::move(name)) {
        if (name_.empty()) {
            throw DiveDataException("Dive name must not be empty.");
        }
    }

    void AddMeasurement(const int second, const double depthMeters) {
        ValidateMeasurement(second, depthMeters);

        if (!measurements_.empty() && second <= measurements_.back().second) {
            throw DiveDataException(std::format(
                "Timestamp {} must be greater than previous timestamp {}.",
                second,
                measurements_.back().second));
        }

        measurements_.push_back({second, depthMeters});
    }

    [[nodiscard]] const std::string& Name() const noexcept {
        return name_;
    }

    [[nodiscard]] const std::vector<Measurement>& Measurements() const noexcept {
        return measurements_;
    }

    [[nodiscard]] std::optional<double> RateBetween(const std::size_t currentIndex) const {
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

private:
    static void ValidateMeasurement(const int second, const double depthMeters) {
        if (second < 0) {
            throw DiveDataException(std::format("Timestamp {} must be >= 0.", second));
        }
        if (depthMeters < 0.0) {
            throw DiveDataException(std::format("Depth {} must be >= 0.", depthMeters));
        }
    }

    std::string name_;
    std::vector<Measurement> measurements_;
};

class DiveLog {
public:
    void AddDive(Dive dive) {
        if (dive.Measurements().empty()) {
            throw DiveDataException(std::format("Dive '{}' contains no measurements.", dive.Name()));
        }
        dives_.push_back(std::move(dive));
    }

    void Print(std::ostream& out) const {
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

private:
    static std::string FormatTime(const int totalSeconds) {
        const int hh = totalSeconds / 3600;
        const int mm = (totalSeconds % 3600) / 60;
        const int ss = totalSeconds % 60;
        return std::format("{:02}:{:02}:{:02}", hh, mm, ss);
    }

    static std::string Column(const std::string& value) {
        return std::format("{:<{}}", value, kColumnWidth);
    }

    static void PrintSingleDive(std::ostream& out, const Dive& dive, const std::size_t serial) {
        const std::string gap(kColumnGap, ' ');

        out << std::format("Dive {}: {}\n", serial, dive.Name());
        out << Column("Dive Time") << gap << Column("Dive Depth") << gap << Column("Down/Up") << '\n';
        out << Column("(hh:mm:ss)") << gap << Column("(m)") << gap << Column("(m/sec)") << '\n';
        out << std::string(kColumnWidth * 3 + kColumnGap * 2, '-') << '\n';

        const auto& measurements = dive.Measurements();
        for (std::size_t i = 0; i < measurements.size(); ++i) {
            const Measurement& m = measurements[i];

            const std::string timeCell = Column(FormatTime(m.second));
            const std::string depthCell = Column(std::format("{:.{}f}", m.depthMeters, kDepthPrecision));

            std::string rateValue;
            if (const auto rate = dive.RateBetween(i); rate.has_value()) {
                rateValue = std::format("{:+.{}f}", rate.value(), kRatePrecision);
            }

            out << timeCell << gap << depthCell << gap << Column(rateValue) << '\n';
        }
    }

    std::vector<Dive> dives_;
};

void RunTests() {
    // Test 1: Beispiel aus der Aufgabenstellung.
    Dive sample("Beispiel-Tauchgang");
    sample.AddMeasurement(0, 0.0);
    sample.AddMeasurement(10, 2.5);
    sample.AddMeasurement(50, 6.8);
    sample.AddMeasurement(150, 15.0);
    sample.AddMeasurement(270, 23.88);
    sample.AddMeasurement(800, 26.0);
    sample.AddMeasurement(1235, 20.5);
    sample.AddMeasurement(1780, 15.8);
    sample.AddMeasurement(2345, 8.3);
    sample.AddMeasurement(3876, 0.0);

    const double firstRate = sample.RateBetween(1).value_or(0.0);
    assert(std::abs(firstRate - (-0.25)) < 1e-9);

    // Test 2: Mehrere Tauchgänge.
    Dive shortDive("Kurztest");
    shortDive.AddMeasurement(0, 0.0);
    shortDive.AddMeasurement(120, 12.0);
    shortDive.AddMeasurement(240, 0.0);

    DiveLog log;
    log.AddDive(sample);
    log.AddDive(shortDive);

    std::ostringstream memoryOutput;
    log.Print(memoryOutput);
    const std::string text = memoryOutput.str();
    assert(text.find("Dive 1: Beispiel-Tauchgang") != std::string::npos);
    assert(text.find("Dive 2: Kurztest") != std::string::npos);
    assert(text.find("-0.250") != std::string::npos);
    assert(text.find("+0.100") != std::string::npos);

    // Test 3: Exceptions.
    bool sawException = false;
    try {
        Dive invalid("Invalid");
        invalid.AddMeasurement(-1, 2.0);
    }
    catch (const DiveDataException&) {
        sawException = true;
    }
    assert(sawException);
}

} // namespace divecomputer

int main() {
    using namespace divecomputer;

    try {
        RunTests();

        Dive firstDive("Ausgabe-Tauchgang A");
        firstDive.AddMeasurement(0, 0.0);
        firstDive.AddMeasurement(30, 4.0);
        firstDive.AddMeasurement(90, 12.5);
        firstDive.AddMeasurement(180, 18.5);
        firstDive.AddMeasurement(420, 10.0);
        firstDive.AddMeasurement(600, 0.0);

        Dive secondDive("Ausgabe-Tauchgang B");
        secondDive.AddMeasurement(0, 0.0);
        secondDive.AddMeasurement(60, 6.0);
        secondDive.AddMeasurement(180, 16.0);
        secondDive.AddMeasurement(240, 16.0);
        secondDive.AddMeasurement(480, 2.0);

        DiveLog log;
        log.AddDive(firstDive);
        log.AddDive(secondDive);

        std::cout << "Alle Tests erfolgreich.\n\n";
        log.Print(std::cout);

        std::ofstream file("tauchgaenge.txt");
        if (!file.is_open()) {
            throw DiveDataException("Could not open output file 'tauchgaenge.txt'.");
        }
        log.Print(file);

        return 0;
    }
    catch (const DiveDataException& ex) {
        std::cerr << "Dive data error: " << ex.what() << '\n';
    }
    catch (const std::exception& ex) {
        std::cerr << "Unexpected std::exception: " << ex.what() << '\n';
    }
    catch (...) {
        std::cerr << "Unknown fatal error occurred.\n";
    }

    return 1;
}
