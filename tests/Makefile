
## This Makefile tests some of the DRC checks to ensure that we are
## properly flagging errors.  Running the tests offscreen keeps the
## display empty, and we intentionally have no lutleft.bmp.json file
## because it would interfere with the test cases that overlay it.

MRT?=maskromtool -platform offscreen

all:
	! $(MRT) lutleft.bmp -De duplicatebits.json
	! $(MRT) lutleft.bmp -de badalign.json
clean:
	rm -f *~
