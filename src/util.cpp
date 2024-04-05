#include <stdexcept>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cctype>
#include <sys/select.h>
#include "util.h"
#include "exception.h"

std::string util::format(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	char *p;
	if(vasprintf(&p, fmt, ap) == -1) {
		throw std::runtime_error("Out of memory");
	}

	va_end(ap);

	const std::string s(p);
	free(p);

	return s;
}

std::string util::toLower(const std::string &s)
{
	std::string rs(s);
	std::transform(rs.begin(), rs.end(), rs.begin(), [](char ch) { return std::tolower(ch); });
	return rs;
}

std::vector<std::string> util::tokenize(const std::string &s, const std::string &sep, size_t count)
{
	std::vector<std::string> rs;
	std::string tmp;
	for(auto ch : s) {
		if((sep.find(ch) != sep.npos) && (count == 0 || rs.size() < (count - 1))) {
			rs.push_back(tmp);
			tmp.clear();
		}
		else {
			tmp.push_back(ch);
		}
	}

	rs.push_back(tmp);
	return rs;
}

std::set<int> util::watch(const std::set<int> &in, int timeout)
{
	fd_set rfd;
	FD_ZERO(&rfd);

	int maxfd(-1);
	for(auto fd : in) {
		FD_SET(fd, &rfd);
		if(fd > maxfd) {
			maxfd = fd;
		}
	}

	timeval tv;
	tv.tv_sec  = (timeout >= 0) ? timeout : 0;
	tv.tv_usec = 0;

	for(;;) {
		const int selrs(select(maxfd + 1, &rfd, NULL, NULL, (timeout >= 0) ? &tv : NULL));
		if(selrs == 0) {
			return std::set<int>{};
		}

		if(selrs < 0) {
			if(errno == EAGAIN || errno == EINTR) {
				// TODO recalculate timeout
				continue;
			}

			xthrow("select(): %m");
		}

		std::set<int> out;
		for(auto fd : in) {
			if(FD_ISSET(fd, &rfd)) {
				out.insert(fd);
			}
		}

		return out;
	}

	/* NOTREACHED */
	return std::set<int>{};
}
