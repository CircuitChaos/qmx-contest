#include "bands.h"
#include <cstddef>

bool bands::getLimits(uint32_t freq, uint32_t &bottom, uint32_t &top)
{
	struct Limits {
		uint16_t bottom, top; /* kHz */
	};

	static const Limits limits[] = {
	    {472, 479},
	    {1810, 2000},
	    {3500, 3800},
	    {7000, 7200},
	    {10100, 10150},
	    {14000, 14350},
	    {18068, 18168},
	    {21000, 21450},
	    {24890, 24990},
	    {28000, 29700},
	    {50000, 52000},
	};

	for(size_t i(0); i < sizeof(limits) / sizeof(*limits); ++i) {
		const uint32_t limBot(limits[i].bottom * 1000);
		const uint32_t limTop(limits[i].top * 1000);
		if(freq >= limBot && freq <= limTop) {
			bottom = limBot;
			top    = limTop;
			return true;
		}
	}

	return false;
}
