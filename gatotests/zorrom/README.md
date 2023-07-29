Howdy y'all,

John McMaster pioneered open source decoding of mask ROM bits with
[ZorROM](https://github.com/JohnDMcMaster/zorrom), and it would be a
shame not to be compatible with it.

This collection was produced by first randomizing a bitstream in
Gatorom, then decoding with an intentionally bad wildcard in Zorrom so
that all potential decodings were created.

```
dell% gatorom --random -a >ascii.txt
dell% ./solver.py --bytes 0x00 --all ascii.txt out
  Writing out/r-0_flipx-0_invert-0_cols-right.bin
  Writing out/r-0_flipx-0_invert-0_cols-left.bin
  Writing out/r-0_flipx-0_invert-0_squeeze-lr.bin
  Writing out/r-0_flipx-0_invert-0_cols-downl.bin
  ...
dell%
```

The `Makefile` in this directory solves each and every one of these,
with one slight variation: Zorrom flips the image before rotating it,
while Gatorom usually performs rotations before flips.  To account for
this, either add 180 degrees to your rotation or pass the `-z` flag to
enable Zorrom compatibility.

--Travis


