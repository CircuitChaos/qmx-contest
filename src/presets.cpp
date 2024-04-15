#include "presets.h"
#include "exception.h"
#include "util.h"

Presets::Presets(const std::string &callsign)
{
	// TODO do it properly (configurable, etc.)
	presets.push_back(util::format("CQ %s TEST", callsign.c_str()));
	presets.push_back(callsign);
	presets.push_back("PLACEHOLDER"); // TODO hack
	presets.push_back("PLACEHOLDER"); // TODO hack
	presets.push_back("AGN");
	presets.push_back("QRS");
	presets.push_back("QRL?");
	presets.push_back("TU");
}

size_t Presets::numPresets() const
{
	return presets.size();
}

std::string Presets::getPreset(size_t preset, const std::string &exchange) const
{
	xassert(preset < presets.size(), "Preset %zu not found", preset);

	if(preset == 2) {
		/* Hack */
		return util::format("5NN %s", exchange.c_str());
	}

	if(preset == 3) {
		/* Hack */
		return util::format("TU 5NN %s", exchange.c_str());
	}

	return presets[preset];
}
