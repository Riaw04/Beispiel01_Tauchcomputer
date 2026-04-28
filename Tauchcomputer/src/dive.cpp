// ==========================
// File: dive.cpp
// Author: Niklas Riepl 
// Date: 20/04/2026
// Description: Implementation of the Dive class for managing dive data.
// ==========================

#include "divecomputer/dive.h"

#include <format>
#include <utility>

#include "divecomputer/dive_data_exception.h"

namespace divecomputer {

	Dive::Dive(std::string name) //constructor 
		: name_(std::move(name)) {
		if (name_.empty()) {
			throw DiveDataException("Dive name must not be empty.");
		}
	}

	// Adds a new measurement to the dive.
	void Dive::AddMeasurement(const int second, const double depthMeters) {
		ValidateMeasurement(second, depthMeters); // Validate the input values.

		if (!measurements_.empty() && second <= measurements_.back().second) { // Ensure that the new timestamp is greater than the last one.
			throw DiveDataException(std::format( "Timestamp {} must be greater than previous timestamp {}.", second, measurements_.back().second)); 
		}

		measurements_.push_back({ second, depthMeters }); //add 
	}

	const std::string& Dive::Name() const noexcept { //getter
		return name_;
	}

	const std::vector<Measurement>& Dive::Measurements() const noexcept { //getter
		return measurements_;
	}

	//rate of depth change between two measurements. 
	std::optional<double> Dive::RateBetween(const std::size_t currentIndex) const { //optional, first measurement or out-of-bounds index
		if (currentIndex == 0 || currentIndex >= measurements_.size()) {
			return std::nullopt; // No rate for the first measurement, or index out of bounds.
		}

		const Measurement& previous = measurements_[currentIndex - 1];
		const Measurement& current = measurements_[currentIndex];
		const int deltaSeconds = current.second - previous.second;

		if (deltaSeconds <= 0) { // check for internal consistency of measurements // should not happen due to validation in AddMeasurement.  
			throw DiveDataException("Internal error: non-positive timestamp delta.");
		}

		// Calculate and return the rate of depth change // positive = ascending, negative = descending.
		return (previous.depthMeters - current.depthMeters) / static_cast<double>(deltaSeconds); 
	}

	void Dive::ValidateMeasurement(const int second, const double depthMeters) {
		if (second < 0) { //validate timestamp 
			throw DiveDataException(std::format("Timestamp {} must be >= 0.", second));
		}
		if (depthMeters < 0.0) { //validate depth
			throw DiveDataException(std::format("Depth {} must be >= 0.", depthMeters));
		}
	}

} // namespace divecomputer
