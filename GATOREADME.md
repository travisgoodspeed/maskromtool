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

As a C++ library, it is not yet fully integrated into MaskRomTool.  To
decode your project, first export it as an ASCII bitstream and then
use GatoROM from the command line.

## CLI Usage

GatoROM operates on a textfile of bits as input.  There should be one
line per row, and because orientation is somewhat arbitrary, the
bits may be conveniently rotated, flipped or inverted.

Run with no parameters for usage information.

```
Usage: gatorom [options] bitstream
Gato ROM: A Decoder for Mask ROM Bits

Options:
  -h, --help                        Displays help on commandline options.
  --help-all                        Displays help including Qt specific
                                    options.
  -v, --version                     Displays version information.
  -V, --verbose                     Talk too much.
  -w, --wordsize <8>                Word size.bits
  -r, --rotate <degrees>            Rotates the image in multiples of 90
                                    degrees.
  --flipx                           Flips the bits along the X axis.
  --flipy                           Flips the bits along the Y axis.
  -i, --invert                      Inverts the bits.
  -o, --output <out.bin>            Output file.
  --random                          Randomize a ROM for testing.
  --Random                          Randomize a crazy ROM.
  --rawwidth, --seanriddle <width>  Width of a raw binary input, in Sean
                                    Riddle's style.
  -I, --info                        Info about input.
  --print                           Print with a GUI dialog.
  --printpdf <file.pdf>             Print to a PDF file.
  --decode-tlcs47font               Decodes as a TMP47C434N Font.
  --decode-z86x1                    Decodes as a Zilog Z86x1.
  --decode-cols-downl-swap          Decodes as a uCOM4 ROM.
  --decode-cols-downr               Decodes first down then right like a
                                    Gameboy.
  --decode-cols-downl               Decodes first down then left.
  --decode-cols-left                Decodes left-to-right.
  --decode-cols-right               Decodes right-to-left.
  --decode-squeeze-lr               Decodes even bits from the left, odd bits
                                    from right like in the TMS32C15.
  -z, --zorrom                      Zorrom compatibility mode, with flipx
                                    before rotation.
  --leftbank                        Only the left half of the bits.
  --rightbank                       Only the right half of the bits.
  -a, --print-bits                  Prints ASCII art of the transformed bits.
  -A, --print-pretty-bits           Prints ASCII art with spaces.
  --solve                           Solves for an unknown format.
  --solve-bytes <bytes>             Bytes as a hint to the solver.
                                    0:31,1:fe,2:ff
  --solve-ascii                     Look for ASCII strings.
  --solve-string <bytes>            Byte string as a hint to the solver.
                                    31,fe,ff

Arguments:
  bitstream                         ASCII art of ROM to decode.
```


## Order of Operations

The order of operations is defined in `GatoROM::eval()`.

1. Rotations first, before all others.
2. Bank selection comes after rotation.
3. Flipping next.
4. Inverting bits happens last.

[Zorrom](https://github.com/JohnDMcMaster/zorrom) performs flips
before rotation, while Gatorom rotates first before flips.  When
flipping X, you will need to adjust your rotation by 180 degrees or
pass the `-z` parameter to enable Zorrom compatibility mode.


## Specific Examples

These are examples from specific chips, and the rotation might vary by
your photography.  Plenty more examples are in the Makefiles inside
[gatotests/](gatotests/).

```
# Gameboy DMG_ROM requires inversion and cols-downr.
gatorom gameboy.txt -o gameboy.bin --decode-cols-downr --flipx --invert
# Same ROM from the solver, knowing only that the first two bytes are `0x31`,`0xfe`
gatorom --solve --solve-bytes 0:0x31,1:0xfe gameboy.txt
```

Sean Riddle has an [excellent
collection](https://seanriddle.com/decap.html) of ROM photographs and
bit dumps.  He provides his raw dumps as binary bytes rather than as
ASCII art bits, and you can use the `--seanriddle <width>` flag to
adjust the input format.  Here's a solver solution for thge MC6805P2
ROM in his dump of the [Milton
Game](https://seanriddle.com/mc6805p2.html).

```
gatorom --seanriddle 256 miltonraw.bin -o solved.bin \
        --solve --solve-bytes "0:23,1:05,2:06"
```

The MYK82 Clipper Chip has an ARM6 core and a 32-bit ROM.  If we
hypothesize that it has a `mov pc, lr` instruction somewhere in the
ROM, we can solve in little endian like this.  The first line is the
correct answer, and the others are false positives that match the
solver case.

```
carbon% gatorom -w 32 --solve --solve-string "0e,f0,a0,e1" rom20x6.txt
Grade 100   	9b 06 00 ea 63 00 00 ea 	-w 32 --decode-cols-left -r 0 
Grade 100   	81 6c 08 67 04 fb 13 72 	-w 32 --decode-cols-left -r 180 --flipx 
Grade 100   	05 d2 a0 e3 04 80 bd e8 	-w 32 --decode-cols-right -r 0 
Grade 100   	00 00 00 00 9c 3f 14 14 	-w 32 --decode-cols-right -r 180 --flipx 
Grade 100   	05 d2 a0 e3 04 b0 1b e5 	-w 32 --decode-cols-downl -r 0 
Grade 100   	00 00 00 00 64 e3 e7 fd 	-w 32 --decode-cols-downl -r 180 --flipx 
Grade 100   	9b 06 00 ea 02 db 8d e2 	-w 32 --decode-cols-downr -r 0 
Grade 100   	81 6c 08 67 f8 50 91 ad 	-w 32 --decode-cols-downr -r 180 --flipx 
carbon% 
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


