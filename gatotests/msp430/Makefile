all: test

msp430f449.bin:
	gatorom --decode-msp430 -i -r 270 -w 16 msp430f449_bits.txt -o msp430f449.bin

test: msp430f449.bin
	md5sum -c md5.txt

clean:
	rm -f *.bin
