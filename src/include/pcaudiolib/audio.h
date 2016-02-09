/* Audio API.
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

#ifndef PCAUDIOLIB_AUDIO_H
#define PCAUDIOLIB_AUDIO_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

enum audio_object_format
{
	AUDIO_OBJECT_FORMAT_S8,
	AUDIO_OBJECT_FORMAT_U8,

	AUDIO_OBJECT_FORMAT_S16LE,
	AUDIO_OBJECT_FORMAT_S16BE,
	AUDIO_OBJECT_FORMAT_U16LE,
	AUDIO_OBJECT_FORMAT_U16BE,

	AUDIO_OBJECT_FORMAT_S18LE,
	AUDIO_OBJECT_FORMAT_S18BE,
	AUDIO_OBJECT_FORMAT_U18LE,
	AUDIO_OBJECT_FORMAT_U18BE,

	AUDIO_OBJECT_FORMAT_S20LE,
	AUDIO_OBJECT_FORMAT_S20BE,
	AUDIO_OBJECT_FORMAT_U20LE,
	AUDIO_OBJECT_FORMAT_U20BE,

	AUDIO_OBJECT_FORMAT_S24LE,
	AUDIO_OBJECT_FORMAT_S24BE,
	AUDIO_OBJECT_FORMAT_U24LE,
	AUDIO_OBJECT_FORMAT_U24BE,

	AUDIO_OBJECT_FORMAT_S24_32LE,
	AUDIO_OBJECT_FORMAT_S24_32BE,
	AUDIO_OBJECT_FORMAT_U24_32LE,
	AUDIO_OBJECT_FORMAT_U24_32BE,

	AUDIO_OBJECT_FORMAT_S32LE,
	AUDIO_OBJECT_FORMAT_S32BE,
	AUDIO_OBJECT_FORMAT_U32LE,
	AUDIO_OBJECT_FORMAT_U32BE,

	AUDIO_OBJECT_FORMAT_FLOAT32LE,
	AUDIO_OBJECT_FORMAT_FLOAT32BE,
	AUDIO_OBJECT_FORMAT_FLOAT64LE,
	AUDIO_OBJECT_FORMAT_FLOAT64BE,

	AUDIO_OBJECT_FORMAT_IEC958LE,
	AUDIO_OBJECT_FORMAT_IEC958BE,

	AUDIO_OBJECT_FORMAT_ALAW,
	AUDIO_OBJECT_FORMAT_ULAW,
	AUDIO_OBJECT_FORMAT_ADPCM,
	AUDIO_OBJECT_FORMAT_MPEG,
	AUDIO_OBJECT_FORMAT_GSM,
	AUDIO_OBJECT_FORMAT_AC3,
};

struct audio_object;

int
audio_object_open(struct audio_object *object,
                  enum audio_object_format format,
                  uint32_t rate,
                  uint8_t channels);

void
audio_object_close(struct audio_object *object);

void
audio_object_destroy(struct audio_object *object);

int
audio_object_write(struct audio_object *object,
                   const void *data,
                   size_t bytes);

int
audio_object_drain(struct audio_object *object);

int
audio_object_flush(struct audio_object *object);

const char *
audio_object_strerror(struct audio_object *object,
                      int error);

struct audio_object *
create_audio_device_object(const char *device,
                           const char *application_name,
                           const char *description);

#ifdef __cplusplus
}
#endif

#endif
