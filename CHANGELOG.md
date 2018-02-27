# Change Log

## 1.1 - \[27 Feb 2018\]

*  Fix handling EBADFD, EPIPE and short write when writing to the ALSA device.
*  Fix audio echos when using `snd_pcm_drop` in ALSA devices.
*  Experimental support for Mac OS CoreAudio.

## 1.0 \[10 Sep 2016\]

*  Provide a consistent API for accessing the different audio interfaces.
*  Support for PulseAudio using the simple API.
*  Support for ALSA.
*  Support for OSS version 3 and later.
*  Support for QSA.
*  Support for XAudio2.
