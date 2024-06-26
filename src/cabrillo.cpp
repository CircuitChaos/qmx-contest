#include "exception.h"
#include "cabrillo.h"
#include "util.h"
#include "file.h"

std::string logCabrillo(time_t t, const std::string &file, const CabrilloQSO &qso)
{
	if(t == 0) {
		t = time(NULL);
	}

	tm tm;
	gmtime_r(&t, &tm);

	char datetime[64];
	strftime(datetime, sizeof(datetime), "%Y-%m-%d %H%M", &tm);

	const std::string s(util::format("QSO: %u CW %s %s %s %s %s %s %s",
	    qso.freq / 1000,
	    datetime,
	    util::toUpper(qso.sentCall).c_str(),
	    util::toUpper(qso.sentRST).c_str(),
	    util::toUpper(qso.sentExchange).c_str(),
	    util::toUpper(qso.rcvdCall).c_str(),
	    util::toUpper(qso.rcvdRST).c_str(),
	    util::toUpper(qso.rcvdExchange).c_str()));

	if(!file.empty()) {
		File fp(fopen(file.c_str(), "ab"));
		xassert(fp, "Could not open Cabrillo file %s: %m", file.c_str());
		fprintf(fp, "%s\n", s.c_str());
	}

	return s;
}
