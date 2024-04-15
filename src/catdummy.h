#pragma once

#include "cat.h"

class CatDummy : public Cat {
public:
	virtual ~CatDummy() {}

	virtual int getFD() const final { return -1; }

	virtual unsigned getGain() final { return gain; }
	virtual void setGain(unsigned gain_) final { gain = gain_; }

	virtual uint32_t getFreq() final { return freq; }
	virtual void setFreq(uint32_t freq_) final { freq = freq_; }

	virtual bool isCW() final { return cw; }
	virtual void setCW(bool cw_) final { cw = cw_; }

	virtual bool isTX() final { return tx; }
	virtual void setTX(bool tx_) final { tx = tx_; }

private:
	unsigned gain{80};
	uint32_t freq{7000 * 1000};
	bool cw{true};
	bool tx{false};
};
