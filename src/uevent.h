#pragma once

#include <string>
#include <inttypes.h>

class UEvent {
public:
	UEvent(const std::string &path);

	bool isValid() const { return valid; }
	const std::string &getDriver() const { return driver; }
	uint16_t getVID() const { return vid; }
	uint16_t getPID() const { return pid; }

private:
	bool valid{false};
	std::string driver;
	uint16_t vid{0};
	uint16_t pid{0};
};
