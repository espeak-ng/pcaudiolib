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

	const char * (*strerror)(struct audio_object *object,
	                         int error);
};

/* We try to aim for 10ms cancelation latency, which will be perceived as
 * "snappy" by users */
#define LATENCY 10

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>

#define container_of(ptr, type, member) ((type *)( (char *)ptr - offsetof(type,member) ))

LPWSTR
str2wcs(const char *str);

HRESULT
CreateWaveFormat(enum audio_object_format format,
                 uint32_t rate,
                 uint8_t channels,
                 WAVEFORMATEX **wfmt);

const char *
windows_hresult_strerror(struct audio_object *object,
                         int error);

struct audio_object *
create_xaudio2_object(const char *device,
                      const char *application_name,
                      const char *description);

#else

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

#ifdef __APPLE__

struct audio_object *
create_coreaudio_object(const char *device,
                        const char *application_name,
                        const char *description);

#else

struct audio_object *
create_pulseaudio_object(const char *device,
                         const char *application_name,
                         const char *description);

struct audio_object *
create_alsa_object(const char *device,
                   const char *application_name,
                   const char *description);

struct audio_object *
create_qsa_object(const char *device,
                   const char *application_name,
                   const char *description);

struct audio_object *
create_oss_object(const char *device,
                  const char *application_name,
                  const char *description);

#endif
#endif

#ifdef __cplusplus
}
#endif

#endif
