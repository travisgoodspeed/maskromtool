all: test

tmp478c434n-font.bin:
	gatorom --decode-tlcs47font tmp478c434n-font.txt -i -r270 -o tmp478c434n-font.bin

test: tmp478c434n-font.bin
	md5sum -c md5.txt

clean:
	rm -f *.bin
