#include "divecomputer/tests.h"

#include <cassert>
#include <cmath>
#include <sstream>
#include <string>

#include "divecomputer/dive.h"
#include "divecomputer/dive_data_exception.h"
#include "divecomputer/dive_log.h"

namespace divecomputer {

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

    double firstRate = 0.0;
    const bool hasRate = sample.RateBetween(1, firstRate);
    assert(hasRate);
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
