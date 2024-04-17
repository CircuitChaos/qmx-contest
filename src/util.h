#pragma once

#include <vector>
#include <string>
#include <set>

namespace util {

template <typename T>
bool inSet(const std::set<T> &set, T value)
{
	return set.find(value) != set.end();
}

std::string format(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
std::string toUpper(const std::string &s);
std::string toLower(const std::string &s);
std::vector<std::string> tokenize(const std::string &s, const std::string &sep, size_t count);
std::set<int> watch(const std::set<int> &in, int timeout);

} // namespace util
