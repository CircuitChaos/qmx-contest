#pragma once

#include <alsa/asoundlib.h>

class AlsaHWParams {
public:
	AlsaHWParams();
	~AlsaHWParams();

	snd_pcm_hw_params_t *operator()() { return params; }

private:
	snd_pcm_hw_params_t *params{nullptr};
};
