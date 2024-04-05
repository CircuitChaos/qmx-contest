#include "alsahwparams.h"
#include "exception.h"

AlsaHWParams::AlsaHWParams()
{
	const int rs(snd_pcm_hw_params_malloc(&params));
	if(rs < 0) {
		xprint("Unable to allocate HW params: %s", snd_strerror(rs));
		params = nullptr;
	}
}

AlsaHWParams::~AlsaHWParams()
{
	if(params) {
		snd_pcm_hw_params_free(params);
	}
}
