all: test


test:
	make clean
# Solve for the first three bytes into foo.
	gatorom --seanriddle 128 ffredraw.bin -o solved.bin \
		--solve --solve-bytes "0:8f,1:91,2:2c"
# Every other 256-bit block is wrong, so this will fail.
# Maybe groups of 16 rows are treated differently?
	diff ffred.bin solved.bin
clean:
	rm -f solved.bin

