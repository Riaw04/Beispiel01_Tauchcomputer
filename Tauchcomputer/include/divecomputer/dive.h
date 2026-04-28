// ==========================
// File: dive.h
// Author: Niklas Riepl 
// Date: 20/04/2026
// Description: Header file for the Dive class.
// ==========================

#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace divecomputer {

	struct Measurement { //depth measurement at a specific time during the dive
		int second = 0;
		double depthMeters = 0.0;
	};

	class Dive {
	public:
		explicit Dive(std::string name); //constructor //explicit to prevent implicit conversions from string to Dive

		void AddMeasurement(int second, double depthMeters); //adds a new measurement to the dive, validating the input

		// Accessors
		[[nodiscard]] const std::string& Name() const noexcept;
		[[nodiscard]] const std::vector<Measurement>& Measurements() const noexcept;
		[[nodiscard]] std::optional<double> RateBetween(std::size_t currentIndex) const;

	private:
		static void ValidateMeasurement(int second, double depthMeters); //validates the input for a measurement

		std::string name_; 
		std::vector<Measurement> measurements_;
	};

} // namespace divecomputer
