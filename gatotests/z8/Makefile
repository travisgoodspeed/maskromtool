all: test

test: wersi.bin
	md5sum -c md5.txt

clean:
	rm -f *.bin

wersi.bin: wersi.txt
	gatorom wersi.txt --decode-z86x1 -o wersi.bin

