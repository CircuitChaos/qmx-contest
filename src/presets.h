#pragma once

#include <vector>
#include <string>

class Presets {
public:
	Presets(const std::string &callsign);

	size_t numPresets() const;
	std::string getPreset(size_t preset, const std::string &exchange) const;

private:
	std::vector<std::string> presets;
};
