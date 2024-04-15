#pragma once

#include <string>

// TODO move all these constants to config.h or something
static const unsigned DEFAULT_WPM = 25;

class Cli {
public:
	Cli(int ac, char *const av[]);

	bool getExitFlag() const { return exitFlag; }
	bool getDummyFlag() const { return dummyFlag; }
	const std::string &getCallsign() const { return callsign; }
	const std::string &getCbrFile() const { return cbrFile; }
	const std::string &getAlsaDev() const { return alsaDev; }
	const std::string &getCatDev() const { return catDev; }
	const std::string &getTextToSend() const { return textToSend; }
	const std::string &getExchangePrefix() const { return exchangePrefix; }
	const std::string &getExchangeInfix() const { return exchangeInfix; }
	const std::string &getExchangeSuffix() const { return exchangeSuffix; }
	bool getQSK() const { return qsk; }
	unsigned getInitialWPM() const { return initialWPM; }

private:
	bool exitFlag{false};
	bool dummyFlag{false};
	std::string callsign{"<NOCALL>"};
	std::string cbrFile;
	std::string alsaDev;
	std::string catDev;
	std::string textToSend;
	std::string exchangePrefix;
	std::string exchangeInfix{"001"};
	std::string exchangeSuffix;
	bool qsk{true};
	unsigned initialWPM{DEFAULT_WPM};
};
