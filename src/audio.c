/* Public API Implementation.
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

int
audio_object_open(struct audio_object *object,
                  enum audio_object_format format,
                  uint32_t rate,
                  uint8_t channels)
{
	if (object)
		return object->open(object, format, rate, channels);
	return 0;
}

void
audio_object_close(struct audio_object *object)
{
	if (object)
		object->close(object);
}

void
audio_object_destroy(struct audio_object *object)
{
	if (object)
		object->destroy(object);
}

int
audio_object_write(struct audio_object *object,
                   const void *data,
                   size_t bytes)
{
	if (object)
		return object->write(object, data, bytes);
	return 0;
}

int
audio_object_drain(struct audio_object *object)
{
	if (object)
		return object->drain(object);
	return 0;
}

int
audio_object_flush(struct audio_object *object)
{
	if (object)
		return object->flush(object);
	return 0;
}

const char *
audio_object_strerror(struct audio_object *object,
                      int error)
{
	if (object)
		return object->strerror(object, error);
	return NULL;
}

struct audio_object *
create_audio_device_object(const char *device,
                           const char *application_name,
                           const char *description)
{
	struct audio_object *object;
	if ((object = create_pulseaudio_object(device, application_name, description)) != NULL)
		return object;
	if ((object = create_alsa_object(device, application_name, description)) != NULL)
		return object;
	if ((object = create_qsa_object(device, application_name, description)) != NULL)
		return object;
	if ((object = create_oss_object(device, application_name, description)) != NULL)
		return object;
	return NULL;
}
