all: test

test: gameboy.bin solved.bin gameboy-damaged.bin gameboy.asm
	md5sum -c md5.txt

clean:
	rm -f *.bin *.asm


solved.bin: gameboy.txt
# This solves the ROM from its first two bytes.
	gatorom --solve --solve-bytes 0:0x31,1:0xfe gameboy.txt -o solved.bin
gameboy.bin: gameboy.txt
# This decodes the ROM from its settings.
	gatorom gameboy.txt -o gameboy.bin --decode-cols-downr --flipx --invert
gameboy.asm: gameboy.txt
# Disassemble with MAME's Unidasm.
	gatorom gameboy.txt --decode-cols-downr --flipx --invert --dis unidasm/lr35902 > gameboy.asm
# Check that the last instruction is right.
	cat gameboy.asm | grep ld | grep FF50
gameboy-damaged.bin: gameboy-damaged.txt
# Dummy target that makes no sense.  This is to ensure we don't crash.
	gatorom --solve --solve-bytes 0:0x31,1:0xfe gameboy-damaged.txt -o gameboy-damaged.bin || true
