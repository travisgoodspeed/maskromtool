Howdy y'all,

This test of [Fabulous Fred's NEC uCOM4
ROM](https://seanriddle.com/necucom4.html) is currently failing, in
that even numbered blocks of 256 bytes are properly decoded but odd
blocks differ from Riddle's encoding.

The same decoding mistake is made by Zorrom, so I expect it's not a
problem in decoding Riddle's binary format.  Maybe groups of 16 rows
are treated differently from one other?

--Travis
