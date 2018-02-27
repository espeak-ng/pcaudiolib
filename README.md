# Portible C Audio Library 1.1

- [Build Dependencies](#build-dependencies)
  - [Debian](#debian)
  - [Mac OS](#mac-os)
- [Building](#building)
- [Bugs](#bugs)
- [License Information](#license-information)

----------

The Portable C Audio Library (pcaudiolib) provides a C API to different audio
devices. It supports:

| Audio Framework | Operating System |
|-----------------|------------------|
| ALSA            | Linux            |
| CoreAudio       | Mac OS           |
| OSS             | POSIX            |
| PulseAudio      | Linux            |
| QSA             | QNX              |
| XAudio2         | Windows          |

See the [CHANGELOG](CHANGELOG.md) for a description of the changes in the
various releases.

## Build Dependencies

In order to build the project, you need:

1.  a functional autotools system (`make`, `autoconf`, `automake`, `libtool`
    and `pkg-config`);
2.  a functional c compiler (e.g. gcc or clang).

__NOTE:__ Some of the Windows SDK APIs (e.g. the XAudio2 headers) don't compile
with a C compiler, so a C++ compiler is needed to compile them.

Optionally, you need:

1.  the alsa development libraries to enable alsa audio output;
2.  the pulseaudio development library to enable pulseaudio output.

### Debian

Core dependencies:

| Dependency | Install                                                          |
|------------|------------------------------------------------------------------|
| autotools  | `sudo apt-get install make autoconf automake libtool pkg-config` |
| c compiler | `sudo apt-get install gcc`                                       |

Optional Libraries:

| Dependency     | Install                                    |
|----------------|--------------------------------------------|
| alsa           | `sudo apt-get install libasound2-dev`      |
| pulseaudio     | `sudo apt-get install libpulse-dev`        |

### Mac OS

To enable CoreAudio output support you need to have the coreaudio framework on
your system. Installing XCode along with the Mac OS SDK is the recommended way
of getting it.

## Building

This project uses a standard autotools build system with an `autogen.sh` script.
You can thus build the project using:

	./autogen.sh
	./configure --prefix=/usr
	make

and install it using:

	sudo make install

## Bugs

Report bugs to the [pcaudiolib issues](https://github.com/espeak-ng/pcaudiolib/issues)
page on GitHub.

## License Information

The Portable C Audio Library is released under the [GPL version 3](COPYING) or
later license.
