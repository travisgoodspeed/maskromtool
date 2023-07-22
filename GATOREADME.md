Howdy y'all,

This is the README file for GatoROM, my friendly competitor to
[ZorROM](https://github.com/JohnDMcMaster/zorrom).  As either a unix
CLI or as a C++ library, its job is to convert a physically-arranged
matrix of bits into logically arranged bytes, suitable for emulation
or disassembly.

For convenience of packaging and distribution, it is included with
[Mask ROM Tool](https://github.com/travisgoodspeed/maskromtool/).

--Travis

## Status

GatoROM is fully functional as a command line tool, performing all
decoding methods of Zorrom and adding some extra solvers as well.

As a C++ library, it is not yet being used by MaskRomTool.  To decode
your project, first export it as an ASCII bitstream and then use
GatoROM from the command line.

## CLI Usage

GatoROM operates on a textfile of bits as input.  There should be one
line per row, and because orientation is somewhat arbitrary, the
bits may be conveniently rotated, flipped or inverted.

Run with no parameters for usage information.

```
% gatorom
Usage: gatorom [options] bitstream
Gato ROM: A Decoder for Mask ROM Bits

Options:
  -h, --help               Displays help on commandline options.
  --help-all               Displays help including Qt specific options.
  -v, --version            Displays version information.
  -r, --rotate <degrees>   Rotates the image in multiples of 90 degrees.
  --flipx                  Flips the bits along the X axis.
  --flipy                  Flips the bits along the Y axis.
  -i, --invert             Inverts the bits.
  -o, --output <out.bin>   Output file.
  --random                 Randomize a ROM.
  -I, --info               Info about input.
  --decode-arm6            Decodes the ROM as ARM6 (MYK82).
  --decode-msp430          Decodes the ROM as MSP430. (Broken.)
  --decode-tlcs47font      Decodes as a TMP47C434N Font.
  --decode-cols-downr      Decodes the ROM first down then right like a
                           Gameboy.
  --decode-cols-downl      Decodes the ROM first down then left.
  --decode-cols-left       Decodes the ROM left-to-right.
  --decode-cols-right      Decodes the ROM right-to-left.
  --decode-squeeze-lr      Decodes even bits from the left, odd bits from right
                           like in the TMS32C15.
  -z, --zorrom             Zorrom compatibility mode, with flipx before
                           rotation.
  -a, --print-bits         Prints ASCII art of the transformed bits.
  -A, --print-pretty-bits  Prints ASCII art with spaces.

Arguments:
  bitstream                ASCII art of ROM to decode.
```


## Specific Examples

These are examples from specific chips.  I should probably add
thumbnails here, to make clear what the rotation is.

```
# Gameboy DMG_ROM requires inversion and cols-downr.
gatorom gameboy.txt -o gameboy.bin --decode-cols-downr --flipx --invert
# Same ROM from the solver, knowing only that the first two bytes are `0x31`,`0xfe`
gatorom --solve --solve-bytes 0:0x31,1:0xfe gameboy.txt
```

## Library Usage

I first wrote GatoROM because I wanted to do ROM parsing inside of
[Mask ROM Tool](https://github.com/travisgoodspeed/maskromtool/) with
some sort of solver for unknown arrangements.  This integration hasn't
happened yet, but working toward it, the library carefully mutates
matrices of both bit values *and* pointers, so that the C++ library
can work backward from a solution to display the relevant bits.


## Test Cases

The `gatotests/` directory contains subdirectories with test cases for
a number of different targets, including a complete collection of
Zorrom solver results.  Each is verified by MD5 checksums, and any
assertion failure or segfault is something about which you should file
an issue.

## Zorrom Compatibility

This project aims to be compatible with
[ZorROM](https://github.com/JohnDMcMaster/zorrom), but some
incompatibilities have snuck in.

Zorrom performs flips before rotation, while Gatorom rotates first
before flips.  When flipping X, you will need to adjust your rotation
by 180 degrees or pass the `-z` parameter to enable Zorrom
compatibility mode.
