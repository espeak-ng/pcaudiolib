/* ALSA Output.
 *
 * Copyright (C) 2016 Reece H. Dunn
 *
 * This file is part of pcaudiolib.
 *
 * pcaudiolib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * pcaudiolib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with pcaudiolib.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "audio_priv.h"

#ifdef HAVE_ALSA_ASOUNDLIB_H

#include <alsa/asoundlib.h>
#include <string.h>

struct alsa_object
{
	struct audio_object vtable;
	snd_pcm_t *handle;
	uint8_t sample_size;
	char *device;
};

#define to_alsa_object(object) container_of(object, struct alsa_object, vtable)

int
alsa_object_open(struct audio_object *object,
                 enum audio_object_format format,
                 uint32_t rate,
                 uint8_t channels)
{
	struct alsa_object *self = to_alsa_object(object);
	if (self->handle)
		return -EEXIST;

	snd_pcm_format_t pcm_format;
#define FORMAT(srcfmt, dstfmt, size) case srcfmt: pcm_format = dstfmt; self->sample_size = size; break;
	switch (format)
	{
	FORMAT(AUDIO_OBJECT_FORMAT_ALAW,      SND_PCM_FORMAT_A_LAW, 1)
	FORMAT(AUDIO_OBJECT_FORMAT_ULAW,      SND_PCM_FORMAT_MU_LAW, 1)
	FORMAT(AUDIO_OBJECT_FORMAT_S8,        SND_PCM_FORMAT_S8, 1)
	FORMAT(AUDIO_OBJECT_FORMAT_U8,        SND_PCM_FORMAT_U8, 1)
	FORMAT(AUDIO_OBJECT_FORMAT_S16LE,     SND_PCM_FORMAT_S16_LE, 2)
	FORMAT(AUDIO_OBJECT_FORMAT_S16BE,     SND_PCM_FORMAT_S16_BE, 2)
	FORMAT(AUDIO_OBJECT_FORMAT_U16LE,     SND_PCM_FORMAT_U16_LE, 2)
	FORMAT(AUDIO_OBJECT_FORMAT_U16BE,     SND_PCM_FORMAT_U16_BE, 2)
	FORMAT(AUDIO_OBJECT_FORMAT_S18LE,     SND_PCM_FORMAT_S18_3LE, 3)
	FORMAT(AUDIO_OBJECT_FORMAT_S18BE,     SND_PCM_FORMAT_S18_3BE, 3)
	FORMAT(AUDIO_OBJECT_FORMAT_U18LE,     SND_PCM_FORMAT_U18_3LE, 3)
	FORMAT(AUDIO_OBJECT_FORMAT_U18BE,     SND_PCM_FORMAT_U18_3BE, 3)
	FORMAT(AUDIO_OBJECT_FORMAT_S20LE,     SND_PCM_FORMAT_S20_3LE, 3)
	FORMAT(AUDIO_OBJECT_FORMAT_S20BE,     SND_PCM_FORMAT_S20_3BE, 3)
	FORMAT(AUDIO_OBJECT_FORMAT_U20LE,     SND_PCM_FORMAT_U20_3LE, 3)
	FORMAT(AUDIO_OBJECT_FORMAT_U20BE,     SND_PCM_FORMAT_U20_3BE, 3)
	FORMAT(AUDIO_OBJECT_FORMAT_S24LE,     SND_PCM_FORMAT_S24_3LE, 3)
	FORMAT(AUDIO_OBJECT_FORMAT_S24BE,     SND_PCM_FORMAT_S24_3BE, 3)
	FORMAT(AUDIO_OBJECT_FORMAT_U24LE,     SND_PCM_FORMAT_U24_3LE, 3)
	FORMAT(AUDIO_OBJECT_FORMAT_U24BE,     SND_PCM_FORMAT_U24_3BE, 3)
	FORMAT(AUDIO_OBJECT_FORMAT_S24_32LE,  SND_PCM_FORMAT_S24_LE, 4)
	FORMAT(AUDIO_OBJECT_FORMAT_S24_32BE,  SND_PCM_FORMAT_S24_BE, 4)
	FORMAT(AUDIO_OBJECT_FORMAT_U24_32LE,  SND_PCM_FORMAT_U24_LE, 4)
	FORMAT(AUDIO_OBJECT_FORMAT_U24_32BE,  SND_PCM_FORMAT_U24_BE, 4)
	FORMAT(AUDIO_OBJECT_FORMAT_S32LE,     SND_PCM_FORMAT_S32_LE, 4)
	FORMAT(AUDIO_OBJECT_FORMAT_S32BE,     SND_PCM_FORMAT_S32_BE, 4)
	FORMAT(AUDIO_OBJECT_FORMAT_U32LE,     SND_PCM_FORMAT_U32_LE, 4)
	FORMAT(AUDIO_OBJECT_FORMAT_U32BE,     SND_PCM_FORMAT_U32_BE, 4)
	FORMAT(AUDIO_OBJECT_FORMAT_FLOAT32LE, SND_PCM_FORMAT_FLOAT_LE, 4)
	FORMAT(AUDIO_OBJECT_FORMAT_FLOAT32BE, SND_PCM_FORMAT_FLOAT_BE, 4)
	FORMAT(AUDIO_OBJECT_FORMAT_FLOAT64LE, SND_PCM_FORMAT_FLOAT64_LE, 8)
	FORMAT(AUDIO_OBJECT_FORMAT_FLOAT64BE, SND_PCM_FORMAT_FLOAT64_BE, 8)
	FORMAT(AUDIO_OBJECT_FORMAT_IEC958LE,  SND_PCM_FORMAT_IEC958_SUBFRAME_LE, 1)
	FORMAT(AUDIO_OBJECT_FORMAT_IEC958BE,  SND_PCM_FORMAT_IEC958_SUBFRAME_BE, 1)
	FORMAT(AUDIO_OBJECT_FORMAT_ADPCM,     SND_PCM_FORMAT_IMA_ADPCM, 1)
	FORMAT(AUDIO_OBJECT_FORMAT_MPEG,      SND_PCM_FORMAT_MPEG, 1)
	FORMAT(AUDIO_OBJECT_FORMAT_GSM,       SND_PCM_FORMAT_GSM, 1)
	default:                              return -EINVAL;
	}
#undef  FORMAT

	snd_pcm_hw_params_t *params = NULL;
	snd_pcm_hw_params_malloc(&params);

	int err = 0;
	if ((err = snd_pcm_open(&self->handle, self->device ? self->device : "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0)
		goto error;
	if ((err = snd_pcm_hw_params_any(self->handle, params)) < 0)
		goto error;
	if ((err = snd_pcm_hw_params_set_access(self->handle, params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
		goto error;
	if ((err = snd_pcm_hw_params_set_format(self->handle, params, pcm_format)) < 0)
		goto error;
	if ((err = snd_pcm_hw_params_set_rate_near(self->handle, params, &rate, 0)) < 0)
		goto error;
	if ((err = snd_pcm_hw_params_set_channels(self->handle, params, channels)) < 0)
		goto error;
	if ((err = snd_pcm_hw_params(self->handle, params)) < 0)
		goto error;
	if ((err = snd_pcm_prepare(self->handle)) < 0)
		goto error;

	return 0;
error:
	if (params)
		snd_pcm_hw_params_free(params);
	if (self->handle) {
		snd_pcm_close(self->handle);
		self->handle = NULL;
	}
	return err;
}

void
alsa_object_close(struct audio_object *object)
{
	struct alsa_object *self = to_alsa_object(object);

	if (self->handle) {
		snd_pcm_close(self->handle);
		self->handle = NULL;
	}
}

void
alsa_object_destroy(struct audio_object *object)
{
	struct alsa_object *self = to_alsa_object(object);

	free(self->device);
	free(self);
}

int
alsa_object_drain(struct audio_object *object)
{
	struct alsa_object *self = to_alsa_object(object);

	return self->handle ? snd_pcm_drain(self->handle) : 0;
}

int
alsa_object_flush(struct audio_object *object)
{
	struct alsa_object *self = to_alsa_object(object);

	return self->handle ? snd_pcm_drop(self->handle) : 0;
}

int
alsa_object_write(struct audio_object *object,
                  const void *data,
                  size_t bytes)
{
	struct alsa_object *self = to_alsa_object(object);
	if (!self->handle)
		return 0;

	int err = 0;
	snd_pcm_state_t state = snd_pcm_state(self->handle);

// State needs to be PREPARED or RUNNING if we want to write.
	if (state != SND_PCM_STATE_PREPARED && state != SND_PCM_STATE_RUNNING) {
		err = snd_pcm_prepare(self->handle);
		if (err)
			return err;
	}

	err = snd_pcm_writei(self->handle, data, bytes / self->sample_size);
	if (err == -EPIPE) // underrun
		err = snd_pcm_prepare(self->handle);
	return err >= 0 ? 0 : err;
}

const char *
alsa_object_strerror(struct audio_object *object,
                     int error)
{
	return snd_strerror(error);
}

struct audio_object *
create_alsa_object(const char *device,
                   const char *application_name,
                   const char *description)
{
	struct alsa_object *self = malloc(sizeof(struct alsa_object));
	if (!self)
		return NULL;

	self->handle = NULL;
	self->sample_size = 0;
	self->device = device ? strdup(device) : NULL;

	self->vtable.open = alsa_object_open;
	self->vtable.close = alsa_object_close;
	self->vtable.destroy = alsa_object_destroy;
	self->vtable.write = alsa_object_write;
	self->vtable.drain = alsa_object_drain;
	self->vtable.flush = alsa_object_flush;
	self->vtable.strerror = alsa_object_strerror;

	return &self->vtable;
}

#else

struct audio_object *
create_alsa_object(const char *device,
                   const char *application_name,
                   const char *description)
{
	return NULL;
}

#endif
