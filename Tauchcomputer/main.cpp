// ==========================
// File: main.cpp
// Author: Niklas Riepl 
// Date: 20/04/2026
// Description: Main program to demonstrate the functionality of the Dive and DiveLog classes. 
// ==========================

#include <exception>
#include <fstream>
#include <iostream>

#include "divecomputer/dive.h"
#include "divecomputer/dive_data_exception.h"
#include "divecomputer/dive_log.h"
#include "divecomputer/tests.h"

int main() {
    using namespace divecomputer;

    try {
		
        RunTests(); // Run unit tests. 

		// sample dives

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

		log.Print(std::cout); //print to console.

        std::ofstream file("tauchgaenge.txt");
        if (!file.is_open()) {
            throw DiveDataException("Could not open output file 'tauchgaenge.txt'.");
        }
		log.Print(file); //print to file.

        std::cout << "Alle Tests erfolgreich.\n\n";

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
