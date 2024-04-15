#pragma once

#include <string>

class Exchange {
public:
	Exchange(const std::string &prefix, const std::string &infix, const std::string &suffix);

	std::string get() const;
	bool next();

private:
	const std::string prefix;
	std::string infix;
	const std::string suffix;
};
