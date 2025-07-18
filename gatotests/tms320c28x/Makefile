all: test
# Just info.
	gatorom --info ascii.txt

clean:
	rm -rf b/*

b:
	mkdir b

tms320c28x: ascii.txt b
	gatorom -r   0 -w 16 --decode-tms320c28x            ascii.txt -o b/r-0_tms320c28x.bin
	gatorom -r 180 -w 16 --decode-tms320c28x            ascii.txt -o b/r-180_tms320c28x.bin
	gatorom -r   0 -w 16 --decode-tms320c28x         -i ascii.txt -o b/r-0_invert_tms320c28x.bin
	gatorom -r 180 -w 16 --decode-tms320c28x         -i ascii.txt -o b/r-180_invert_tms320c28x.bin
	gatorom -r   0 -w 16 --decode-tms320c28x --flipx    ascii.txt -o b/r-0_flipx_tms320c28x.bin
	gatorom -r 180 -w 16 --decode-tms320c28x --flipx    ascii.txt -o b/r-180_flipx_tms320c28x.bin
	gatorom -r   0 -w 16 --decode-tms320c28x --flipy    ascii.txt -o b/r-0_flipy_tms320c28x.bin
	gatorom -r 180 -w 16 --decode-tms320c28x --flipy    ascii.txt -o b/r-180_flipy_tms320c28x.bin
	gatorom -r   0 -w 16 --decode-tms320c28x --flipy -i ascii.txt -o b/r-0_flipy_invert_tms320c28x.bin
	gatorom -r 180 -w 16 --decode-tms320c28x --flipy -i ascii.txt -o b/r-180_flipy_invert_tms320c28x.bin

test: tms320c28x
	cd b && md5sum * >../md5.txt

