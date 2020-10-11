/* PulseAudio Output.
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

#ifdef HAVE_PULSE_SIMPLE_H

#include <pulse/error.h>
#include <pulse/simple.h>
#include <string.h>
#include <stdbool.h>

struct pulseaudio_object
{
	struct audio_object vtable;
	pa_sample_spec ss;
	pa_simple *s;
	char *device;
	char *application_name;
	char *description;
};

#define to_pulseaudio_object(object) container_of(object, struct pulseaudio_object, vtable)

int
pulseaudio_object_open(struct audio_object *object,
                       enum audio_object_format format,
                       uint32_t rate,
                       uint8_t channels)
{
	struct pulseaudio_object *self = to_pulseaudio_object(object);
	if (self->s)
		return PA_ERR_EXIST;

	self->ss.rate = rate;
	self->ss.channels = channels;

	switch (format)
	{
	case AUDIO_OBJECT_FORMAT_ALAW:      self->ss.format = PA_SAMPLE_ALAW;      break;
	case AUDIO_OBJECT_FORMAT_ULAW:      self->ss.format = PA_SAMPLE_ULAW;      break;
	case AUDIO_OBJECT_FORMAT_U8:        self->ss.format = PA_SAMPLE_U8;        break;
	case AUDIO_OBJECT_FORMAT_S16LE:     self->ss.format = PA_SAMPLE_S16LE;     break;
	case AUDIO_OBJECT_FORMAT_S16BE:     self->ss.format = PA_SAMPLE_S16BE;     break;
#ifdef PA_SAMPLE_S24LE
	case AUDIO_OBJECT_FORMAT_S24LE:     self->ss.format = PA_SAMPLE_S24LE;     break;
	case AUDIO_OBJECT_FORMAT_S24BE:     self->ss.format = PA_SAMPLE_S24BE;     break;
	case AUDIO_OBJECT_FORMAT_S24_32LE:  self->ss.format = PA_SAMPLE_S24_32LE;  break;
	case AUDIO_OBJECT_FORMAT_S24_32BE:  self->ss.format = PA_SAMPLE_S24_32BE;  break;
#endif
	case AUDIO_OBJECT_FORMAT_S32LE:     self->ss.format = PA_SAMPLE_S32LE;     break;
	case AUDIO_OBJECT_FORMAT_S32BE:     self->ss.format = PA_SAMPLE_S32BE;     break;
	case AUDIO_OBJECT_FORMAT_FLOAT32LE: self->ss.format = PA_SAMPLE_FLOAT32LE; break;
	case AUDIO_OBJECT_FORMAT_FLOAT32BE: self->ss.format = PA_SAMPLE_FLOAT32BE; break;
	default:                            return PA_ERR_INVALID; // Invalid argument.
	}

	int error = 0;
	pa_buffer_attr battr;

	battr.fragsize = (uint32_t) -1;
	battr.maxlength = (uint32_t) -1;
	battr.minreq = (uint32_t) -1;
	battr.prebuf = (uint32_t) -1;
	battr.tlength = pa_bytes_per_second(&self->ss) * LATENCY / 1000;
	self->s = pa_simple_new(NULL,
	                        self->application_name,
	                        PA_STREAM_PLAYBACK,
	                        self->device,
	                        self->description,
	                        &self->ss,
	                        NULL,
	                        &battr,
	                        &error);
	return error;
}

void
pulseaudio_object_close(struct audio_object *object)
{
	struct pulseaudio_object *self = to_pulseaudio_object(object);

	if (self->s) {
		pa_simple_free(self->s);
		self->s = NULL;
	}
}

void
pulseaudio_object_destroy(struct audio_object *object)
{
	struct pulseaudio_object *self = to_pulseaudio_object(object);

	free(self->device);
	free(self->application_name);
	free(self->description);
	free(self);
}

int
pulseaudio_object_drain(struct audio_object *object)
{
	struct pulseaudio_object *self = to_pulseaudio_object(object);
	if (!self->s)
		return 0;

	int error = 0;
	pa_simple_drain(self->s, &error);
	return error;
}

int
pulseaudio_object_flush(struct audio_object *object)
{
	struct pulseaudio_object *self = to_pulseaudio_object(object);
	if (!self->s)
		return 0;

	int error = 0;
	pa_simple_flush(self->s, &error);
	return error;
}

int
pulseaudio_object_write(struct audio_object *object,
                        const void *data,
                        size_t bytes)
{
	struct pulseaudio_object *self = to_pulseaudio_object(object);
	if (!self->s)
		return 0;

	int error = 0;
	pa_simple_write(self->s, data, bytes, &error);
	return error;
}

const char *
pulseaudio_object_strerror(struct audio_object *object,
                           int error)
{
	return pa_strerror(error);
}

static bool
pulseaudio_is_available(const char *device,
                        const char *application_name,
                        const char *description)
{
	pa_sample_spec ss;
	ss.format = PA_SAMPLE_S16LE;
	ss.rate = 44100;
	ss.channels = 1;

	pa_simple *s = pa_simple_new(NULL,
	                             application_name,
	                             PA_STREAM_PLAYBACK,
	                             device,
	                             description,
	                             &ss,
	                             NULL,
	                             NULL,
	                             NULL);

	if (!s)
		return false;

	pa_simple_free(s);
	return true;
}

struct audio_object *
create_pulseaudio_object(const char *device,
                         const char *application_name,
                         const char *description)
{
	if (!pulseaudio_is_available(device, application_name, description))
		return NULL;

	struct pulseaudio_object *self = malloc(sizeof(struct pulseaudio_object));
	if (!self)
		return NULL;

	self->s = NULL;
	self->device = device ? strdup(device) : NULL;
	self->application_name = application_name ? strdup(application_name) : NULL;
	self->description = description ? strdup(description) : NULL;

	self->vtable.open = pulseaudio_object_open;
	self->vtable.close = pulseaudio_object_close;
	self->vtable.destroy = pulseaudio_object_destroy;
	self->vtable.write = pulseaudio_object_write;
	self->vtable.drain = pulseaudio_object_drain;
	self->vtable.flush = pulseaudio_object_flush;
	self->vtable.strerror = pulseaudio_object_strerror;

	return &self->vtable;
}

#else

struct audio_object *
create_pulseaudio_object(const char *device,
                         const char *application_name,
                         const char *description)
{
	return NULL;
}

#endif
