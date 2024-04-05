#pragma once

#include <string>
#include <alsa/asoundlib.h>

class AlsaDev {
public:
	AlsaDev(const std::string &devname);
	~AlsaDev();

	snd_pcm_t *operator()() { return hnd; }

private:
	snd_pcm_t *hnd{nullptr};
};
