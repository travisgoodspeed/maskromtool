all: test

test: gameboy.bin solved.bin gameboy-damaged.bin
	md5sum -c md5.txt

clean:
	rm -f *.bin


solved.bin: gameboy.txt
	# This solves the ROM from its first two bytes.
	gatorom --solve --solve-bytes 0:0x31,1:0xfe gameboy.txt -o solved.bin
gameboy.bin: gameboy.txt
	# This solves the ROM from its settings.
	gatorom gameboy.txt -o gameboy.bin --decode-cols-downr --flipx --invert
gameboy-damaged.bin: gameboy-damaged.txt
	# Dummy target that makes no sense.  This is to ensure we don't crash.
	gatorom --solve --solve-bytes 0:0x31,1:0xfe gameboy-damaged.txt -o gameboy-damaged.bin || true
