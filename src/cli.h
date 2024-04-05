#pragma once

#include <string>

// TODO move all these constants to config.h or something
static const unsigned DEFAULT_WPM = 25;

class Cli {
public:
	Cli(int ac, char *const av[]);

	bool getExitFlag() const { return exitFlag; }
	const std::string &getCallsign() const { return callsign; }
	const std::string &getCbrFile() const { return cbrFile; }
	const std::string &getAlsaDev() const { return alsaDev; }
	const std::string &getCatDev() const { return catDev; }
	const std::string &getTextToSend() const { return textToSend; }
	const std::string &getExchange() const { return exchange; }
	bool isExchangeFixed() const { return exchangeFixed; }
	bool getQSK() const { return qsk; }
	unsigned getInitialWPM() const { return initialWPM; }

private:
	bool exitFlag{false};
	std::string callsign{"<NOCALL>"};
	std::string cbrFile;
	std::string alsaDev;
	std::string catDev;
	std::string textToSend;
	std::string exchange{"1"};
	bool exchangeFixed{false};
	bool qsk{true};
	unsigned initialWPM{DEFAULT_WPM};
};
