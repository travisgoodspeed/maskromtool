all: test

marc4-topleft-bits.bin:
	gatorom --decode-marc4 marc4-topleft-bits.txt -o marc4-topleft-bits.bin

test: marc4-topleft-bits.bin
	md5sum -c md5.txt

clean:
	rm -f *.bin