#include <alsa/asoundlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <math.h>
#include "alsadev.h"
#include "alsahwparams.h"
#include "exception.h"
#include "toneqmx.h"

static const unsigned SAMPLE_RATE = 48000;
static const unsigned BUF_SIZE    = 4096;
static const unsigned TONE_FREQ   = 1000;

static bool setHWParams(AlsaDev &dev)
{
	AlsaHWParams hwp;
	if(!hwp()) {
		return false;
	}

	int err;
	if((err = snd_pcm_hw_params_any(dev(), hwp())) < 0) {
		xprint("Unable to initialize hardware parameters structure: %s", snd_strerror(err));
		return false;
	}

	if((err = snd_pcm_hw_params_set_access(dev(), hwp(), SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		xprint("Unable to set interleaved access: %s", snd_strerror(err));
		return false;
	}

	if((err = snd_pcm_hw_params_set_format(dev(), hwp(), SND_PCM_FORMAT_S24_3LE)) < 0) {
		xprint("Unable to set data format: %s", snd_strerror(err));
		return false;
	}

	if((err = snd_pcm_hw_params_set_rate(dev(), hwp(), SAMPLE_RATE, 0)) < 0) {
		xprint("Unable to set sample rate: %s", snd_strerror(err));
		return false;
	}

	if((err = snd_pcm_hw_params_set_channels(dev(), hwp(), 2)) < 0) {
		xprint("Unable to set channels count: %s", snd_strerror(err));
		return false;
	}

	if((err = snd_pcm_hw_params(dev(), hwp())) < 0) {
		xprint("Unable to set hardware parameters: %s", snd_strerror(err));
		return false;
	}

	return true;
}

static void prepareBuffer(uint8_t *buf, double &ang)
{
	for(unsigned i(0); i < BUF_SIZE; ++i) {
		const int32_t sample(sin(ang) * 8388607);

		ang += 2 * M_PI * TONE_FREQ / SAMPLE_RATE;
		if(ang > 2 * M_PI) {
			ang -= 2 * M_PI;
		}

		buf[i * 6 + 0] = buf[i * 6 + 3] = sample & 0xff;
		buf[i * 6 + 1] = buf[i * 6 + 4] = (sample >> 8) & 0xff;
		buf[i * 6 + 2] = buf[i * 6 + 5] = (sample >> 16) & 0xff;
	}
}

static void threadFunc(std::string alsaDev, Pipe *pipe, volatile bool *exitFlag)
{
	PipeGuard pg(*pipe);

	AlsaDev dev(alsaDev);
	if(!dev()) {
		return;
	}

	if(!setHWParams(dev)) {
		return;
	}

	pipe->writeChar();
	double ang(0.);
	while(!*exitFlag) {
		uint8_t buf[BUF_SIZE * 6];
		prepareBuffer(buf, ang);

		const snd_pcm_sframes_t rs(snd_pcm_writei(dev(), buf, BUF_SIZE));
		if(rs < 0) {
			const int err(snd_pcm_recover(dev(), rs, 0));
			if(err < 0) {
				xprint("snd_pcm_writei(): %s", snd_strerror(err));
				break;
			}
		}
		else if(rs != (snd_pcm_sframes_t) BUF_SIZE) {
			xprint("snd_pcm_writei(): requested %u frames, written only %ld", BUF_SIZE, rs);
			break;
		}
	}
}

ToneQMX::ToneQMX(const std::string &alsaDev)
{
	thread = std::thread(threadFunc, alsaDev, &notifyPipe, &exitFlag);

	if(!notifyPipe.readChar()) {
		thread.join();
		xthrow("Could not start audio thread");
	}
}

ToneQMX::~ToneQMX()
{
	exitFlag = true;
	notifyPipe.readChar();
	thread.join();
}

int ToneQMX::getNotifyFD() const
{
	return notifyPipe.getReadEnd();
}

unsigned ToneQMX::getFreq() const
{
	return TONE_FREQ;
}
