all: test


test:
	make clean
# Solve for the first three bytes into foo.
	gatorom --seanriddle 256 miltonraw.bin -o solved.bin \
		--solve --solve-bytes "0:23,1:05,2:06"
# milton.bin is reorganized a little, so we just check the MD5 of our
# solution rather than an exact comparison.
	md5sum -c md5.txt
clean:
	rm -f solved.bin

