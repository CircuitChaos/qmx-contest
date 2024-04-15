#include <cstdlib>
#include "exchange.h"
#include "util.h"

Exchange::Exchange(const std::string &prefix, const std::string &infix, const std::string &suffix)
    : prefix(prefix), infix(infix), suffix(suffix)
{
}

std::string Exchange::get() const
{
	return prefix + infix + suffix;
}

bool Exchange::next()
{
	if(infix.empty()) {
		return false;
	}

	infix = util::format(util::format("%%0%zulu", infix.size()).c_str(), strtol(infix.c_str(), NULL, 10) + 1);
	return true;
}
