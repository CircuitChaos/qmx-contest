#pragma once

#include <string>
#include "file.h"

class QMXCat {
public:
	QMXCat(const std::string &port);
	~QMXCat();

	int getFD() const;

	// AG
	unsigned getGain();
	void setGain(unsigned gain);

	// FA
	// TODO make it VFO-agnostic
	uint32_t getFreq();
	void setFreq(uint32_t freq);

	// MD
	bool isCW();
	void setCW(bool cw);

	// TQ
	bool isTX();
	void setTX(bool tx);

private:
	Fd fd;

	void sendReq(const std::string &s);
	std::string recvRsp(size_t numBytes);
	std::string exchange(const std::string &req, size_t numResponseBytes);
};
