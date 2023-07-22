all: test

test: gameboy.bin solved.bin
	md5sum -c md5.txt

clean:
	rm -f *.bin


solved.bin: gameboy.txt
	gatorom --solve --solve-bytes 0:0x31,1:0xfe gameboy.txt -o solved.bin
gameboy.bin: gameboy.txt
	gatorom gameboy.txt -o gameboy.bin --decode-cols-downr --flipx --invert
