/* OSS Output.
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

#if defined(HAVE_SYS_SOUNDCARD_H)
#include <sys/soundcard.h>
#define DEFAULT_OSS_DEVICE "/dev/dsp"
#elif defined(HAVE_LINUX_SOUNDCARD_H)
#include <linux/soundcard.h>
#define DEFAULT_OSS_DEVICE "/dev/dsp"
#elif defined(HAVE_SOUNDCARD_H)
#include <soundcard.h>
#define DEFAULT_OSS_DEVICE "/dev/dsp"
#endif

#ifdef DEFAULT_OSS_DEVICE

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

struct oss_object
{
	struct audio_object vtable;
	int fd;
	char *device;
};

#define to_oss_object(object) container_of(object, struct oss_object, vtable)

int
oss_object_open(struct audio_object *object,
                enum audio_object_format format,
                uint32_t rate,
                uint8_t channels)
{
	struct oss_object *self = to_oss_object(object);
	if (self->fd != -1)
		return EEXIST;

	int oss_format;
	switch (format)
	{
	case AUDIO_OBJECT_FORMAT_ALAW:  oss_format = AFMT_A_LAW;     break;
	case AUDIO_OBJECT_FORMAT_ULAW:  oss_format = AFMT_MU_LAW;    break;
	case AUDIO_OBJECT_FORMAT_S8:    oss_format = AFMT_S8;        break;
	case AUDIO_OBJECT_FORMAT_U8:    oss_format = AFMT_U8;        break;
	case AUDIO_OBJECT_FORMAT_S16LE: oss_format = AFMT_S16_LE;    break;
	case AUDIO_OBJECT_FORMAT_S16BE: oss_format = AFMT_S16_BE;    break;
	case AUDIO_OBJECT_FORMAT_U16LE: oss_format = AFMT_U16_LE;    break;
	case AUDIO_OBJECT_FORMAT_U16BE: oss_format = AFMT_U16_BE;    break;
	case AUDIO_OBJECT_FORMAT_ADPCM: oss_format = AFMT_IMA_ADPCM; break;
	case AUDIO_OBJECT_FORMAT_MPEG:  oss_format = AFMT_MPEG;      break;
	case AUDIO_OBJECT_FORMAT_AC3:   oss_format = AFMT_AC3;       break;
	default:                        return -EINVAL;
	}

	int data;
	if ((self->fd = open(self->device ? self->device : DEFAULT_OSS_DEVICE, O_RDWR, 0)) == -1)
		return errno;
	if (ioctl(self->fd, SNDCTL_DSP_SETFMT, &oss_format) == -1)
		goto error;
	data = rate;
	if (ioctl(self->fd, SNDCTL_DSP_SPEED, &data) == -1)
		goto error;
	data = channels;
	if (ioctl(self->fd, SNDCTL_DSP_CHANNELS, &data) == -1)
		goto error;

	return 0;
error:
	data = errno;
	close(self->fd);
	self->fd = -1;
	return data;
}

void
oss_object_close(struct audio_object *object)
{
	struct oss_object *self = to_oss_object(object);

	if (self->fd == -1) {
		close(self->fd);
		self->fd = -1;
	}
}

void
oss_object_destroy(struct audio_object *object)
{
	struct oss_object *self = to_oss_object(object);

	free(self->device);
	free(self);
}

int
oss_object_drain(struct audio_object *object)
{
	struct oss_object *self = to_oss_object(object);

	if (ioctl(self->fd, SNDCTL_DSP_SYNC, NULL) == -1)
		return errno;
	return 0;
}

int
oss_object_flush(struct audio_object *object)
{
	struct oss_object *self = to_oss_object(object);

	if (ioctl(self->fd, SNDCTL_DSP_RESET, NULL) == -1)
		return errno;
	return 0;
}

int
oss_object_write(struct audio_object *object,
                 const void *data,
                 size_t bytes)
{
	struct oss_object *self = to_oss_object(object);

	if (write(self->fd, data, bytes) == -1)
		return errno;
	return 0;
}

struct audio_object *
create_oss_object(const char *device,
                  const char *application_name,
                  const char *description)
{
	struct oss_object *self = malloc(sizeof(struct oss_object));
	if (!self)
		return NULL;

	self->fd = -1;
	self->device = device ? strdup(device) : NULL;

	self->vtable.open = oss_object_open;
	self->vtable.close = oss_object_close;
	self->vtable.destroy = oss_object_destroy;
	self->vtable.write = oss_object_write;
	self->vtable.drain = oss_object_drain;
	self->vtable.flush = oss_object_flush;

	return &self->vtable;
}

#else

struct audio_object *
create_oss_object(const char *device,
                  const char *application_name,
                  const char *description)
{
	return NULL;
}

#endif
