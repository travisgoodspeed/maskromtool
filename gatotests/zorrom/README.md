Howdy y'all,

John McMaster pioneered open source decoding of mask ROM bits with
[ZorROM](https://github.com/JohnDMcMaster/zorrom), and it would be a
shame not to be compatible with it.

This collection was produced by first randomizing a bitstream in
GatoROM, then decoding with an intentionally bad wildcard in ZorROM so
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

Ideally we can decode all of these.  Until then, we'll treat the gaps
as warnings rather than errors so that we know what's missing.

--Travis


