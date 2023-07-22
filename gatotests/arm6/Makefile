all: test

rom20x7.bin:
	gatorom --decode-arm6 rom20x7.txt -o rom20x7.bin
test: rom20x7.bin
	md5sum -c md5.txt
clean:
	rm -f *.bin

