/* Internal Audio API.
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

#ifndef PCAUDIOLIB_AUDIO_PRIV_H
#define PCAUDIOLIB_AUDIO_PRIV_H

#include <pcaudiolib/audio.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct audio_object
{
	int (*open)(struct audio_object *object,
	            enum audio_object_format format,
	            uint32_t rate,
	            uint8_t channels);

	void (*close)(struct audio_object *object);

	void (*destroy)(struct audio_object *object);

	int (*write)(struct audio_object *object,
	             const void *data,
	             size_t bytes);

	int (*drain)(struct audio_object *object);

	int (*flush)(struct audio_object *object);
};

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

struct audio_object *
create_pulseaudio_object(const char *device,
                         const char *application_name,
                         const char *description);

#ifdef __cplusplus
}
#endif

#endif
