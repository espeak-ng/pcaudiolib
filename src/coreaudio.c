/* Coreaudio Output.
 *
 * Copyright (C) 2016 Rui Batista
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

#ifdef HAVE_COREAUDIO_H
#include <AudioToolbox/AudioToolbox.h>
#include <CoreServices/CoreServices.h>
#include "TPCircularBuffer/TPCircularBuffer+AudioBufferList.h"
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>


#define min(a,b) ((a) < (b) ? (a):(b))

#define COREAUDIO_BUFFER_SIZE (1024 * 64) // 4m


struct coreaudio_object
{
	struct audio_object vtable;
	AudioStreamBasicDescription format;
	AudioUnit outputUnit;
	TPCircularBuffer circularBuffer;
	bool initialized;
	bool running;

	char *device;
	char *application_name;
	char *description;
};

#define to_coreaudio_object(object) container_of(object, struct coreaudio_object, vtable)

static OSStatus graphRenderProc(void *inRefCon,
						 AudioUnitRenderActionFlags *ioActionFlags,
						 const AudioTimeStamp *inTimeStamp,
						 UInt32 inBusNumber,
						 UInt32 inNumberFrames,
						 AudioBufferList * ioData) {
	struct coreaudio_object *self = (struct coreaudio_object *)inRefCon;
	if(! self->running){
		// fill the buffer with sielence, stop was not acnoledged yet
		memset(ioData->mBuffers[0].mData,0,(self->format).mBytesPerFrame * inNumberFrames);
		ioData->mBuffers[0].mNumberChannels = 1;
		ioData->mBuffers[0].mDataByteSize = (self->format).mBytesPerFrame * inNumberFrames;
		return noErr;
 }
	
	UInt32 available = TPCircularBufferPeek(&(self->circularBuffer), NULL, &(self->format));
	if(available == 0) {
		self->running = FALSE;
		AudioOutputUnitStop(self->outputUnit);
	}
	// copy as much data as we can from the circular buffer
	TPCircularBufferDequeueBufferListFrames(&(self->circularBuffer), &inNumberFrames, ioData, NULL, &(self->format));
	return noErr;
}

int
coreaudio_object_open(struct audio_object *object,
                       enum audio_object_format format,
                       uint32_t rate,
                       uint8_t channels)
{
	struct coreaudio_object *self = to_coreaudio_object(object);
	OSStatus err = noErr;
	if (self->initialized)
		return noErr;

	memset(&(self->format), 0, sizeof(AudioStreamBasicDescription));
	(self->format).mSampleRate = rate;
	(self->format).mChannelsPerFrame = channels;
	(self->format).mFramesPerPacket = 1;
	switch (format)
	{
		case AUDIO_OBJECT_FORMAT_S16LE:
			(self->format).mFormatID = kAudioFormatLinearPCM;
			(self->format).mBitsPerChannel = 16;
			(self->format).mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked;
			break;
		default:
			return -1;
	}
	// guess the remaining of the AudioBasicStreamDescription structure
	UInt32 procSize = sizeof(AudioStreamBasicDescription);
	err = AudioFormatGetProperty(kAudioFormatProperty_FormatInfo, 0, NULL, &procSize, &(self->format));
	if(err != noErr) {
		goto cleanup;
	}


	// Find default output
	AudioComponentDescription outputcd = {0,};
	outputcd.componentType = kAudioUnitType_Output;
	outputcd.componentSubType = kAudioUnitSubType_DefaultOutput;
	outputcd.componentManufacturer = kAudioUnitManufacturer_Apple;
	
	// Create a component
	AudioComponent outputComponent = AudioComponentFindNext(NULL, &outputcd);
	err = AudioComponentInstanceNew(outputComponent, &(self->outputUnit));
	if(err != noErr) {
		goto cleanup;
	}

	// set render callback
	AURenderCallbackStruct callbackStruct;
	callbackStruct.inputProcRefCon = self;
	callbackStruct.inputProc = graphRenderProc;
	size_t propSize = sizeof(AURenderCallbackStruct);
	err = AudioUnitSetProperty(self->outputUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Output, 0, &callbackStruct, propSize);
	if(err != noErr) {
		goto cleanup;
	}
	
	// set output unit input format
	propSize = sizeof(AudioStreamBasicDescription);
	err = AudioUnitSetProperty(self->outputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &(self->format), propSize);
	if(err != noErr) {
		goto cleanup;
	}

	err = AudioUnitInitialize(self->outputUnit);
	if(err != noErr) {
		goto cleanup;
	}
	// create a circular buffer to produce and consume audio
	TPCircularBufferInit(&(self->circularBuffer), COREAUDIO_BUFFER_SIZE);

	self->initialized = TRUE;
	return noErr;
cleanup:
	if(self->outputUnit) {
		AudioUnitUninitialize(self->outputUnit);
		AudioComponentInstanceDispose(self->outputUnit);
	}
	return err;
}

void
coreaudio_object_close(struct audio_object *object)
{
	struct coreaudio_object *self = to_coreaudio_object(object);
	if(self->initialized) {
		AudioUnitUninitialize(self->outputUnit);
		AudioComponentInstanceDispose(self->outputUnit);
		TPCircularBufferCleanup(&(self->circularBuffer));
		self->initialized = FALSE;
	}

}

void
coreaudio_object_destroy(struct audio_object *object)
{
	struct coreaudio_object *self = to_coreaudio_object(object);

}

int
coreaudio_object_drain(struct audio_object *object)
{
	struct coreaudio_object *self = to_coreaudio_object(object);
	if (!self->initialized)
		return noErr;
	while(self->running)
		usleep(10000);
	return noErr;
}

int
coreaudio_object_flush(struct audio_object *object)
{
	struct coreaudio_object *self = to_coreaudio_object(object);
	if (!self->initialized)
		return noErr;

	OSStatus err = AudioOutputUnitStop(self->outputUnit);

	TPCircularBufferClear(&(self->circularBuffer));
	self->running = FALSE;

	return err;
}

int
coreaudio_object_write(struct audio_object *object,
                        const void *data,
                        size_t bytes)
{
	struct coreaudio_object *self = to_coreaudio_object(object);
	if (!self->initialized)
		return noErr;
	bool runningAtStart = self->running;
	if(!runningAtStart) {
		TPCircularBufferClear(&(self->circularBuffer));
	}
	char *dataPtr = (char*)data;

	// copy data to our circular buffer
	// Poll while we don't have space in the buffer.
	AudioBufferList *bufferList;
	UInt32 available;
	while(bytes > 0) {
		while((!runningAtStart || self->running) && (available = TPCircularBufferGetAvailableSpace(&(self->circularBuffer), &(self->format))) == 0)
			usleep(10000);

		// check we didn't stop
		if(runningAtStart &&  (!self->running))
			return noErr;

		bufferList = TPCircularBufferPrepareEmptyAudioBufferListWithAudioFormat(&(self->circularBuffer), &(self->format), available, NULL);

		// we are writing mono/interleaved data so we have one buffer
		UInt32 bytesToWrite = min(bytes, bufferList->mBuffers[0].mDataByteSize);
		bufferList->mBuffers[0].mNumberChannels = (self->format).mChannelsPerFrame;
		bufferList->mBuffers[0].mDataByteSize = bytesToWrite;
		memcpy(bufferList->mBuffers[0].mData, dataPtr, bytesToWrite);
		TPCircularBufferProduceAudioBufferList(&(self->circularBuffer), NULL);
		dataPtr += bytesToWrite;
		bytes -= bytesToWrite;
		if(! runningAtStart) {
			OSStatus err = AudioOutputUnitStart(self->outputUnit);
			if(err == noErr) {
			self->running = TRUE;
			runningAtStart = TRUE;
		} else {
			return err;
		}
	}
	}
	return noErr;
}

const char *
coreaudio_object_strerror(struct audio_object *object,
                           int error)
{
	return GetMacOSStatusCommentString(error);
}

static bool
coreaudio_is_available(const char *device,
                        const char *application_name,
                        const char *description)
{
	return true;
}

struct audio_object *
create_coreaudio_object(const char *device,
                         const char *application_name,
                         const char *description)
{
	if (!coreaudio_is_available(device, application_name, description))
		return NULL;

	struct coreaudio_object *self = malloc(sizeof(struct coreaudio_object));
	if (!self)
		return NULL;

	self->vtable.open = coreaudio_object_open;
	self->vtable.close = coreaudio_object_close;
	self->vtable.destroy = coreaudio_object_destroy;
	self->vtable.write = coreaudio_object_write;
	self->vtable.drain = coreaudio_object_drain;
	self->vtable.flush = coreaudio_object_flush;
	self->vtable.strerror = coreaudio_object_strerror;

	return &self->vtable;
}

#else

struct audio_object *
create_coreaudio_object(const char *device,
                         const char *application_name,
                         const char *description)
{
	return NULL;
}

#endif
