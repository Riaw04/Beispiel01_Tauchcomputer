// ==========================
// File: dive_log.cpp
// Author: Niklas Riepl 
// Date: 20/04/2026
// Description: Implementation of the DiveLog class for managing and printing multiple dives.
// ==========================

#include "divecomputer/dive_log.h"

#include <format>
#include <ostream>
#include <string>

#include "divecomputer/constants.h"
#include "divecomputer/dive_data_exception.h"

namespace divecomputer {

	void DiveLog::AddDive(Dive dive) { //add dice to log.
		if (dive.Measurements().empty()) { //validate dive.
			throw DiveDataException(std::format("Dive '{}' contains no measurements.", dive.Name()));
		}
		dives_.push_back(std::move(dive));
	}

	void DiveLog::Print(std::ostream& out) const { //print details of all dives formatted.
		if (!out) { //validate output stream.
			throw DiveDataException("Output stream is not writable.");
		}

		for (std::size_t diveIndex = 0; diveIndex < dives_.size(); ++diveIndex) {
			PrintSingleDive(out, dives_[diveIndex], diveIndex + 1); // Print each dive with a serial number starting from 1.
			if (diveIndex + 1 < dives_.size()) { 
				out << '\n'; // Add a blank line between dives. //not after last one.
			}
		}
	}

	std::string DiveLog::FormatTime(const int totalSeconds) { //convert seconds to hh:mm:ss format. 
		const int hh = totalSeconds / 3600;
		const int mm = (totalSeconds % 3600) / 60;
		const int ss = totalSeconds % 60;
		return std::format("{:02}:{:02}:{:02}", hh, mm, ss);
	}

	std::string DiveLog::Column(const std::string& value) {
		return std::format("{:<{}}", value, kColumnWidth); // format to given width, left-aligned.
	}

	void DiveLog::PrintSingleDive(std::ostream& out, const Dive& dive, const std::size_t serial) {
		const std::string gap(kColumnGap, ' ');

		out << std::format("Dive {}: {}\n", serial, dive.Name()); // Print the dive header with serial number and name.
		out << Column("Dive Time") << gap << Column("Dive Depth") << gap << Column("Down/Up") << '\n'; // Print the column headers with appropriate labels.
		out << Column("(hh:mm:ss)") << gap << Column("(m)") << gap << Column("(m/sec)") << '\n'; // Print the units for each column.
		out << std::string(kColumnWidth * 3 + kColumnGap * 2, '-') << '\n'; // Print a separator line based on the total width of the columns and gaps.

		const auto& measurements = dive.Measurements();
		for (std::size_t i = 0; i < measurements.size(); ++i) {
			const Measurement& m = measurements[i];

			const std::string timeCell = Column(FormatTime(m.second));
			const std::string depthCell = Column(std::format("{:.{}f}", m.depthMeters, kDepthPrecision)); 

			std::string rateValue;
			if (const auto rate = dive.RateBetween(i); rate.has_value()) {
				rateValue = std::format("{:+.{}f}", rate.value(), kRatePrecision);
			}

			out << timeCell << gap << depthCell << gap << Column(rateValue) << '\n'; // Print the formatted time, depth, and rate for each measurement in the dive.
		}
	}

} // namespace divecomputer
