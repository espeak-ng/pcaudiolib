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
	return object->open(object, format, rate, channels);
}

void
audio_object_close(struct audio_object *object)
{
	object->close(object);
}

void
audio_object_destroy(struct audio_object *object)
{
	object->destroy(object);
}

int
audio_object_write(struct audio_object *object,
                   const void *data,
                   size_t bytes)
{
	return object->write(object, data, bytes);
}

int
audio_object_drain(struct audio_object *object)
{
	return object->drain(object);
}

int
audio_object_flush(struct audio_object *object)
{
	return object->flush(object);
}

struct audio_object *
create_audio_device_object(const char *device,
                           const char *application_name,
                           const char *description)
{
	return create_pulseaudio_object(device, application_name, description);
}
