/* Windows xAudio2 API Output.
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

// NOTE: XAudio2.h fails to build with a C compiler
#include <XAudio2.h>
#pragma comment(lib, "xaudio2.lib")

struct xaudio2_object
{
	struct audio_object vtable;
	IXAudio2 *audio;
	IXAudio2MasteringVoice *mastering;
	IXAudio2SourceVoice *source;
	WAVEFORMATEX *format;
	LPWSTR devicename;
};

void
xaudio2_object_close(struct audio_object *object);

#define to_xaudio2_object(object) container_of(object, struct xaudio2_object, vtable)

int
xaudio2_object_open(struct audio_object *object,
                    enum audio_object_format format,
                    uint32_t rate,
                    uint8_t channels)
{
	struct xaudio2_object *self = to_xaudio2_object(object);
	if (self->mastering != NULL)
		return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);

	HRESULT hr;
	hr = self->audio->CreateMasteringVoice(&self->mastering);
	if (FAILED(hr))
		goto error;

	hr = CreateWaveFormat(format, rate, channels, &self->format);
	if (FAILED(hr))
		goto error;

 	hr = self->audio->CreateSourceVoice(&self->source, self->format);
	if (FAILED(hr))
		goto error;

	return S_OK;
error:
	xaudio2_object_close(object);
	return hr;
}

void
xaudio2_object_close(struct audio_object *object)
{
	struct xaudio2_object *self = to_xaudio2_object(object);

	if (self->source != NULL)
	{
		self->source->DestroyVoice();
		self->source = NULL;
	}

	if (self->format != NULL)
	{
		CoTaskMemFree(self->format);
		self->format = NULL;
	}

	if (self->mastering != NULL)
	{
		self->mastering->DestroyVoice();
		self->mastering = NULL;
	}
}

void
xaudio2_object_destroy(struct audio_object *object)
{
	struct xaudio2_object *self = to_xaudio2_object(object);

	self->audio->Release();
	free(self->devicename);
	free(self);

	CoUninitialize();
}

int
xaudio2_object_drain(struct audio_object *object)
{
	struct xaudio2_object *self = to_xaudio2_object(object);

	return S_OK;
}

int
xaudio2_object_flush(struct audio_object *object)
{
	struct xaudio2_object *self = to_xaudio2_object(object);

	return S_OK;
}

int
xaudio2_object_write(struct audio_object *object,
                     const void *data,
                     size_t bytes)
{
	struct xaudio2_object *self = to_xaudio2_object(object);

	XAUDIO2_BUFFER buffer = {0};
	buffer.AudioBytes = bytes;
	buffer.pAudioData = (const BYTE *)data;

	HRESULT hr = S_OK;
	if (SUCCEEDED(hr))
		hr = self->source->SubmitSourceBuffer(&buffer);

	if (SUCCEEDED(hr))
		hr = self->source->Start(0);

	if (SUCCEEDED(hr)) while (true)
	{
		Sleep(10);

		XAUDIO2_VOICE_STATE state = { 0 };
		self->source->GetState(&state);
		if (state.pCurrentBufferContext == NULL && state.BuffersQueued == 0)
			return hr;
	}

	return hr;
}

struct audio_object *
create_xaudio2_object(const char *device,
                      const char *application_name,
                      const char *description)
{
	CoInitialize(NULL);

	IXAudio2 *audio = NULL;
	HRESULT hr = XAudio2Create(&audio, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(hr) || audio == NULL) {
		if (audio != NULL)
			audio->Release();

		CoUninitialize();
		return NULL;
	}

	struct xaudio2_object *self = (struct xaudio2_object *)malloc(sizeof(struct xaudio2_object));
	if (!self)
		return NULL;

	self->audio = audio;
	self->mastering = NULL;
	self->source = NULL;
	self->format = NULL;
	self->devicename = device ? str2wcs(device) : NULL;

	self->vtable.open = xaudio2_object_open;
	self->vtable.close = xaudio2_object_close;
	self->vtable.destroy = xaudio2_object_destroy;
	self->vtable.write = xaudio2_object_write;
	self->vtable.drain = xaudio2_object_drain;
	self->vtable.flush = xaudio2_object_flush;
	self->vtable.strerror = windows_hresult_strerror;

	return &self->vtable;
}
