// ==========================
// File: tests.cpp
// Author: Niklas Riepl 
// Date: 20/04/2026
// Description: Unit tests for the Dive and DiveLog classes to verify correct functionality and exception handling.
// ==========================

#include "divecomputer/tests.h"

#include <cassert>
#include <cmath>
#include <sstream>
#include <string>
#include <iostream>

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

	// Test 4: Leerer Tauchgang.
    sawException = false;
    try {
        Dive emptyDive("Empty");
        DiveLog log2;
        log2.AddDive(emptyDive);
    }
    catch (const DiveDataException&) {
        sawException = true;
    }
	assert(sawException);

	// Test 5: Ungültiger Ausgabe-Stream.

    sawException = false;
    try {
        DiveLog log3;
        log3.Print(std::cerr); // std::cerr is writable, so this should not throw.
    }
    catch (const DiveDataException&) {
        sawException = true;
    }
	assert(!sawException); // No exception should be thrown for std::cerr.

	// Test 6: Ungültige Tauchgangsname.

    sawException = false;
    try {
        Dive unnamed("");
    }
    catch (const DiveDataException&) {
        sawException = true;
	}

	// Test 7: Ungültige Zeitstempelreihenfolge.

    sawException = false;
    try {
        Dive outOfOrder("OutOfOrder");
        outOfOrder.AddMeasurement(0, 0.0);
        outOfOrder.AddMeasurement(10, 5.0);
        outOfOrder.AddMeasurement(5, 3.0); // Ungültiger Zeitstempel (kleiner als vorheriger).
    }
    catch (const DiveDataException&) {
        sawException = true;
	}

	// Test 8: RateBetween mit ungültigem Index.
    sawException = false;
    try {
        sample.RateBetween(0); // Erster Index, kein vorheriger Messpunkt.
    }
    catch (const DiveDataException&) {
        sawException = true;
	}

	// print test results

}

} // namespace divecomputer
