# Portible C Audio Library

- [Build Dependencies](#build-dependencies)
  - [Debian](#debian)
- [Building](#building)
- [Bugs](#bugs)
- [License Information](#license-information)

----------

The Portable C Audio Library (pcaudiolib) supports many different devices with
an object-orientated C API.

See the [CHANGELOG](CHANGELOG.md) for a description of the changes in the
various releases.

## Build Dependencies

In order to build the project, you need:

1.  a functional autotools system (`make`, `autoconf`, `automake`, `libtool`
    and `pkg-config`);
2.  a functional c compiler (e.g. gcc or clang).

### Debian

Core dependencies:

| Dependency | Install                                                          |
|------------|------------------------------------------------------------------|
| autotools  | `sudo apt-get install make autoconf automake libtool pkg-config` |
| c compiler | `sudo apt-get install gcc`                                       |

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
