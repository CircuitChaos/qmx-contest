#pragma once

#include <string>
#include "file.h"
#include "cat.h"

class CatQMX : public Cat {
public:
	CatQMX(const std::string &port);
	virtual ~CatQMX();

	virtual int getFD() const final;

	// AG
	virtual unsigned getGain() final;
	virtual void setGain(unsigned gain) final;

	// FA
	virtual uint32_t getFreq() final;
	virtual void setFreq(uint32_t freq_) final;

	// MD
	virtual bool isCW() final;
	virtual void setCW(bool cw_) final;

	// TQ
	virtual bool isTX() final;
	virtual void setTX(bool tx_) final;

private:
	Fd fd;

	void sendReq(const std::string &s);
	std::string recvRsp(size_t numBytes);
	std::string exchange(const std::string &req, size_t numResponseBytes);
};
