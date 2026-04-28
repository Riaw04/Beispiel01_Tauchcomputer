// ==========================
// File: dive_data_exception.h
// Author: Niklas Riepl 
// Date: 20/04/2026
// Description: Header file for the DiveDataException class, a custom exception type for handling errors. 
// ==========================

#pragma once

#include <stdexcept>
#include <string>

namespace divecomputer {

	class DiveDataException : public std::runtime_error { //custom exception type for handling errors related to dive data
	public:
		explicit DiveDataException(const std::string& message) 
			: std::runtime_error(message) {} 
	};

} // namespace divecomputer
