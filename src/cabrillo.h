#pragma once

#include <string>
#include <inttypes.h>

struct CabrilloQSO {
public:
	uint32_t freq;
	std::string sentCall;
	std::string sentRST;
	std::string sentExchange;
	std::string rcvdCall;
	std::string rcvdRST;
	std::string rcvdExchange;
};

std::string logCabrillo(const std::string &file, const CabrilloQSO &qso);
