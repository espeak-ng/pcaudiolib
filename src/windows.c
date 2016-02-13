/* Windows Helpers for Writing Windows Audio Objects.
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

#include <mmreg.h>

LPWSTR
str2wcs(const char *str)
{
	size_t len = strlen(str);
	LPWSTR ret = calloc(len + 1, sizeof(wchar_t));
	if (!ret)
		return NULL;
	mbstowcs(ret, str, len + 1);
	return ret;
}

static HRESULT
CreateWaveFormatEx(WORD format,
                   uint32_t bits_per_sample,
                   uint32_t rate,
                   uint8_t channels,
                   WAVEFORMATEX **wfmt)
{
	(*wfmt) = CoTaskMemAlloc(sizeof(WAVEFORMATEX));
	if (!(*wfmt))
		return E_OUTOFMEMORY;

	(*wfmt)->wFormatTag = format;
	(*wfmt)->nChannels = channels;
	(*wfmt)->nBlockAlign = (channels * bits_per_sample) / 8;
	(*wfmt)->nSamplesPerSec = rate;
	(*wfmt)->wBitsPerSample = bits_per_sample;
	(*wfmt)->nAvgBytesPerSec = rate * (*wfmt)->nBlockAlign;
	(*wfmt)->cbSize = 0;

	return S_OK;
}

HRESULT
CreateWaveFormat(enum audio_object_format format,
                 uint32_t rate,
                 uint8_t channels,
                 WAVEFORMATEX **wfmt)
{
	if (!wfmt)
		return E_INVALIDARG;

	switch (format)
	{
	case AUDIO_OBJECT_FORMAT_ALAW:      return CreateWaveFormatEx(WAVE_FORMAT_ALAW, 8, rate, channels, wfmt);
	case AUDIO_OBJECT_FORMAT_ULAW:      return CreateWaveFormatEx(WAVE_FORMAT_MULAW, 8, rate, channels, wfmt);
	case AUDIO_OBJECT_FORMAT_S8:        return CreateWaveFormatEx(WAVE_FORMAT_PCM, 8, rate, channels, wfmt);
	case AUDIO_OBJECT_FORMAT_S16LE:     return CreateWaveFormatEx(WAVE_FORMAT_PCM, 16, rate, channels, wfmt);
	case AUDIO_OBJECT_FORMAT_S32LE:     return CreateWaveFormatEx(WAVE_FORMAT_PCM, 32, rate, channels, wfmt);
	case AUDIO_OBJECT_FORMAT_FLOAT32LE: return CreateWaveFormatEx(WAVE_FORMAT_IEEE_FLOAT, 32, rate, channels, wfmt);
	case AUDIO_OBJECT_FORMAT_FLOAT64LE: return CreateWaveFormatEx(WAVE_FORMAT_IEEE_FLOAT, 64, rate, channels, wfmt);
	default:                            return E_INVALIDARG;
	}
}

const char *
windows_hresult_strerror(struct audio_object *object,
                         int error)
{
	char *msg = NULL;
	DWORD result = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), &msg, 256, NULL);
	if (!result)
		return NULL;

	size_t len = strlen(msg);
	while (len > 1 && msg[len - 1] == '\n' || msg[len - 1] == '\r')
		msg[--len] = 0;

	char *ret = strdup(msg);
	LocalFree(msg);
	return ret;
}
