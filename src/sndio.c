/* sndio Output.
 *
 * Copyright (C) 2021 Haelwenn (lanodan) Monnier
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

#ifdef HAVE_SNDIO_H

#include <sndio.h>
#include <string.h> /* strdup */

struct sndio_object
{
	struct audio_object vtable;
	struct sio_hdl *hdl;
	char *application_name;
	char *description;
};

#define to_sndio_object(object) container_of(object, struct sndio_object, vtable)

int
audio_format_get_bits(enum audio_object_format format) {
	switch(format)
	{
	case AUDIO_OBJECT_FORMAT_S8:
	case AUDIO_OBJECT_FORMAT_U8:
		return 8;
	break;

	case AUDIO_OBJECT_FORMAT_S16LE:
	case AUDIO_OBJECT_FORMAT_S16BE:
	case AUDIO_OBJECT_FORMAT_U16LE:
	case AUDIO_OBJECT_FORMAT_U16BE:
		return 16;
	break;

	case AUDIO_OBJECT_FORMAT_S18LE:
	case AUDIO_OBJECT_FORMAT_S18BE:
	case AUDIO_OBJECT_FORMAT_U18LE:
	case AUDIO_OBJECT_FORMAT_U18BE:
		return 18;
	break;

	case AUDIO_OBJECT_FORMAT_S20LE:
	case AUDIO_OBJECT_FORMAT_S20BE:
	case AUDIO_OBJECT_FORMAT_U20LE:
	case AUDIO_OBJECT_FORMAT_U20BE:
		return 20;
	break;

	case AUDIO_OBJECT_FORMAT_S24LE:
	case AUDIO_OBJECT_FORMAT_S24BE:
	case AUDIO_OBJECT_FORMAT_U24LE:
	case AUDIO_OBJECT_FORMAT_U24BE:
		return 24;
	break;

	case AUDIO_OBJECT_FORMAT_S24_32LE:
	case AUDIO_OBJECT_FORMAT_S24_32BE:
	case AUDIO_OBJECT_FORMAT_U24_32LE:
	case AUDIO_OBJECT_FORMAT_U24_32BE:
		return -1; // ???
	break;

	case AUDIO_OBJECT_FORMAT_S32LE:
	case AUDIO_OBJECT_FORMAT_S32BE:
	case AUDIO_OBJECT_FORMAT_U32LE:
	case AUDIO_OBJECT_FORMAT_U32BE:
		return 32;
	break;
	default:
		return -1;
	}
}

int
audio_format_is_sig(enum audio_object_format format) {
	switch(format)
	{
	case AUDIO_OBJECT_FORMAT_S8:
	case AUDIO_OBJECT_FORMAT_S16LE:
	case AUDIO_OBJECT_FORMAT_S16BE:
	case AUDIO_OBJECT_FORMAT_S18LE:
	case AUDIO_OBJECT_FORMAT_S18BE:
	case AUDIO_OBJECT_FORMAT_S20LE:
	case AUDIO_OBJECT_FORMAT_S20BE:
	case AUDIO_OBJECT_FORMAT_S24LE:
	case AUDIO_OBJECT_FORMAT_S24BE:
	case AUDIO_OBJECT_FORMAT_S24_32LE:
	case AUDIO_OBJECT_FORMAT_S24_32BE:
	case AUDIO_OBJECT_FORMAT_S32LE:
	case AUDIO_OBJECT_FORMAT_S32BE:
		return 1;
	case AUDIO_OBJECT_FORMAT_U8:
	case AUDIO_OBJECT_FORMAT_U16LE:
	case AUDIO_OBJECT_FORMAT_U16BE:
	case AUDIO_OBJECT_FORMAT_U18LE:
	case AUDIO_OBJECT_FORMAT_U18BE:
	case AUDIO_OBJECT_FORMAT_U20LE:
	case AUDIO_OBJECT_FORMAT_U20BE:
	case AUDIO_OBJECT_FORMAT_U24LE:
	case AUDIO_OBJECT_FORMAT_U24BE:
	case AUDIO_OBJECT_FORMAT_U24_32LE:
	case AUDIO_OBJECT_FORMAT_U24_32BE:
	case AUDIO_OBJECT_FORMAT_U32LE:
	case AUDIO_OBJECT_FORMAT_U32BE:
		return 0;
	break;
	default:
		return -1;
	}
}

int
audio_format_is_le(enum audio_object_format format) {
	switch(format)
	{
	case AUDIO_OBJECT_FORMAT_S8:
	case AUDIO_OBJECT_FORMAT_U8:
	case AUDIO_OBJECT_FORMAT_S16BE:
	case AUDIO_OBJECT_FORMAT_U16BE:
	case AUDIO_OBJECT_FORMAT_S18BE:
	case AUDIO_OBJECT_FORMAT_U18BE:
	case AUDIO_OBJECT_FORMAT_S20BE:
	case AUDIO_OBJECT_FORMAT_U20BE:
	case AUDIO_OBJECT_FORMAT_S24BE:
	case AUDIO_OBJECT_FORMAT_U24BE:
	case AUDIO_OBJECT_FORMAT_S24_32BE:
	case AUDIO_OBJECT_FORMAT_U24_32BE:
		return 0;

	case AUDIO_OBJECT_FORMAT_S16LE:
	case AUDIO_OBJECT_FORMAT_U16LE:
	case AUDIO_OBJECT_FORMAT_S18LE:
	case AUDIO_OBJECT_FORMAT_U18LE:
	case AUDIO_OBJECT_FORMAT_S20LE:
	case AUDIO_OBJECT_FORMAT_U20LE:
	case AUDIO_OBJECT_FORMAT_S24LE:
	case AUDIO_OBJECT_FORMAT_U24LE:
	case AUDIO_OBJECT_FORMAT_S24_32LE:
	case AUDIO_OBJECT_FORMAT_U24_32LE:
		return 1;

	default:
		return -1;
	}
}

int
sndio_object_open(struct audio_object *object,
                  enum audio_object_format format,
                  uint32_t rate,
                  uint8_t channels)
{
	struct sndio_object *self = to_sndio_object(object);
	struct sio_par par;
	if (self->hdl)
		return -1;

	self->hdl = sio_open(self->application_name, SIO_PLAY, 0);
	if(!self->hdl) {
		return -1;
	}

	sio_initpar(&par);

	int bits = audio_format_get_bits(format);
	if(bits != -1) {
		par.bits = bits;
	}

	int sig = audio_format_is_sig(format);
	if(sig != -1) {
		par.sig = sig;
	}

	int le = audio_format_is_le(format);
	if(le != -1) {
		par.le = le;
	}

	par.rate = rate;
	par.rchan = 0;
	par.pchan = channels;

	if(!sio_setpar(self->hdl, &par))
		return -1;

	if(!sio_getpar(self->hdl, &par))
		return -1;

	if(par.bits != bits)
		return -1;
	if(par.sig != sig)
		return -1;
	if(par.le != le)
		return -1;

	return 0;
}

void
sndio_object_close(struct audio_object *object)
{
	struct sndio_object *self = to_sndio_object(object);

	if (self->hdl) {
		sio_close(self->hdl);
		self->hdl = NULL;
	}
}

void
sndio_object_destroy(struct audio_object *object)
{
	struct sndio_object *self = to_sndio_object(object);

	free(self->application_name);
	free(self->description);
	free(self);
}

int
sndio_object_write(struct audio_object *object,
                        const void *data,
                        size_t bytes)
{
	struct sndio_object *self = to_sndio_object(object);
	if (!self->hdl)
		return 0;

	return sio_write(self->hdl, data, bytes) == bytes;
}

struct audio_object *
create_sndio_object(const char *device,
                    const char *application_name,
                    const char *description)
{
	struct sndio_object *self = malloc(sizeof(struct sndio_object));
	if (!self)
		return NULL;

	self->hdl = NULL;
	self->application_name = application_name ? strdup(application_name) : NULL;
	self->description = description ? strdup(description) : NULL;

	self->vtable.open = sndio_object_open;
	self->vtable.close = sndio_object_close;
	self->vtable.destroy = sndio_object_destroy;
	self->vtable.write = sndio_object_write;

	return &self->vtable;
}

#else

struct audio_object *
create_sndio_object(const char *device,
                    const char *application_name,
                    const char *description)
{
	return NULL;
}

#endif
