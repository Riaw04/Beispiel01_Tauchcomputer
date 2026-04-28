// ==========================
// File: dive_log.h
// Author: Niklas Riepl 
// Date: 20/04/2026
// Description: Header file for the DiveLog class.
// ==========================

#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

#include "divecomputer/dive.h"

namespace divecomputer {

	// The DiveLog class manages a collection of Dive objects.
	class DiveLog {
	public:
		void AddDive(Dive dive); // Adds a new dive to the log
		void Print(std::ostream& out) const; // Prints the details of all dives formatted

	private:
		// Helper functions for formatting and printing dive details
		static std::string FormatTime(int totalSeconds);
		static std::string Column(const std::string& value);
		static void PrintSingleDive(std::ostream& out, const Dive& dive, std::size_t serial);

		std::vector<Dive> dives_; // Collection of dives in the log
	};

} // namespace divecomputer
