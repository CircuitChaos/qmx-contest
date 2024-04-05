#include <string.h>
#include <cstdio>
#include "alsadev.h"
#include "exception.h"

AlsaDev::AlsaDev(const std::string &devname)
{
	const int rs(snd_pcm_open(&hnd, devname.c_str(), SND_PCM_STREAM_PLAYBACK, 0));
	if(rs < 0) {
		xprint("Unable to open PCM device: %s", snd_strerror(rs));
		hnd = nullptr;
	}
}

AlsaDev::~AlsaDev()
{
	if(hnd) {
		snd_pcm_close(hnd);
	}
}
