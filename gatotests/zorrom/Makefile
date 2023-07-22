all: test
# Just info.
	gatorom --info ascii.txt

clean:
	rm -rf b/*

b:
	mkdir b

cols-downr: ascii.txt b
# Without flips, these exactly match.
	gatorom -r   0         --decode-cols-downr -i ascii.txt -o b/r-0_flipx-0_invert-1_cols-downr.bin
	gatorom -r  90         --decode-cols-downr -i ascii.txt -o b/r-90_flipx-0_invert-1_cols-downr.bin
	gatorom -r 180         --decode-cols-downr -i ascii.txt -o b/r-180_flipx-0_invert-1_cols-downr.bin
	gatorom -r 270         --decode-cols-downr -i ascii.txt -o b/r-270_flipx-0_invert-1_cols-downr.bin
	gatorom -r   0         --decode-cols-downr    ascii.txt -o b/r-0_flipx-0_invert-0_cols-downr.bin
	gatorom -r  90         --decode-cols-downr    ascii.txt -o b/r-90_flipx-0_invert-0_cols-downr.bin
	gatorom -r 180         --decode-cols-downr    ascii.txt -o b/r-180_flipx-0_invert-0_cols-downr.bin
	gatorom -r 270         --decode-cols-downr    ascii.txt -o b/r-270_flipx-0_invert-0_cols-downr.bin
# Flips occur before rotation on Zorrom, so we need to be off by 180 to compensate.
	gatorom -r 180 --flipx --decode-cols-downr -i ascii.txt -o b/r-0_flipx-1_invert-1_cols-downr.bin
	gatorom -r 270 --flipx --decode-cols-downr -i ascii.txt -o b/r-90_flipx-1_invert-1_cols-downr.bin
	gatorom -r   0 --flipx --decode-cols-downr -i ascii.txt -o b/r-180_flipx-1_invert-1_cols-downr.bin
	gatorom -r  90 --flipx --decode-cols-downr -i ascii.txt -o b/r-270_flipx-1_invert-1_cols-downr.bin
	gatorom -r 180 --flipx --decode-cols-downr    ascii.txt -o b/r-0_flipx-1_invert-0_cols-downr.bin
	gatorom -r 270 --flipx --decode-cols-downr    ascii.txt -o b/r-90_flipx-1_invert-0_cols-downr.bin
	gatorom -r   0 --flipx --decode-cols-downr    ascii.txt -o b/r-180_flipx-1_invert-0_cols-downr.bin
	gatorom -r  90 --flipx --decode-cols-downr    ascii.txt -o b/r-270_flipx-1_invert-0_cols-downr.bin

cols-downl: ascii.txt b
# Without flips, these exactly match.
	gatorom -r   0         --decode-cols-downl -i ascii.txt -o b/r-0_flipx-0_invert-1_cols-downl.bin
	gatorom -r  90         --decode-cols-downl -i ascii.txt -o b/r-90_flipx-0_invert-1_cols-downl.bin
	gatorom -r 180         --decode-cols-downl -i ascii.txt -o b/r-180_flipx-0_invert-1_cols-downl.bin
	gatorom -r 270         --decode-cols-downl -i ascii.txt -o b/r-270_flipx-0_invert-1_cols-downl.bin
	gatorom -r   0         --decode-cols-downl    ascii.txt -o b/r-0_flipx-0_invert-0_cols-downl.bin
	gatorom -r  90         --decode-cols-downl    ascii.txt -o b/r-90_flipx-0_invert-0_cols-downl.bin
	gatorom -r 180         --decode-cols-downl    ascii.txt -o b/r-180_flipx-0_invert-0_cols-downl.bin
	gatorom -r 270         --decode-cols-downl    ascii.txt -o b/r-270_flipx-0_invert-0_cols-downl.bin
# Flips occur before rotation on Zorrom, so we need to be off by 180 to compensate.
	gatorom -r 180 --flipx --decode-cols-downl -i ascii.txt -o b/r-0_flipx-1_invert-1_cols-downl.bin
	gatorom -r 270 --flipx --decode-cols-downl -i ascii.txt -o b/r-90_flipx-1_invert-1_cols-downl.bin
	gatorom -r   0 --flipx --decode-cols-downl -i ascii.txt -o b/r-180_flipx-1_invert-1_cols-downl.bin
	gatorom -r  90 --flipx --decode-cols-downl -i ascii.txt -o b/r-270_flipx-1_invert-1_cols-downl.bin
	gatorom -r 180 --flipx --decode-cols-downl    ascii.txt -o b/r-0_flipx-1_invert-0_cols-downl.bin
	gatorom -r 270 --flipx --decode-cols-downl    ascii.txt -o b/r-90_flipx-1_invert-0_cols-downl.bin
	gatorom -r   0 --flipx --decode-cols-downl    ascii.txt -o b/r-180_flipx-1_invert-0_cols-downl.bin
	gatorom -r  90 --flipx --decode-cols-downl    ascii.txt -o b/r-270_flipx-1_invert-0_cols-downl.bin

cols-left: ascii.txt b
# Without flips, these exactly match.
	gatorom -r   0         --decode-cols-left -i ascii.txt -o b/r-0_flipx-0_invert-1_cols-left.bin
	gatorom -r  90         --decode-cols-left -i ascii.txt -o b/r-90_flipx-0_invert-1_cols-left.bin
	gatorom -r 180         --decode-cols-left -i ascii.txt -o b/r-180_flipx-0_invert-1_cols-left.bin
	gatorom -r 270         --decode-cols-left -i ascii.txt -o b/r-270_flipx-0_invert-1_cols-left.bin
	gatorom -r   0         --decode-cols-left    ascii.txt -o b/r-0_flipx-0_invert-0_cols-left.bin
	gatorom -r  90         --decode-cols-left    ascii.txt -o b/r-90_flipx-0_invert-0_cols-left.bin
	gatorom -r 180         --decode-cols-left    ascii.txt -o b/r-180_flipx-0_invert-0_cols-left.bin
	gatorom -r 270         --decode-cols-left    ascii.txt -o b/r-270_flipx-0_invert-0_cols-left.bin
# Flips occur before rotation on Zorrom, so we need to be off by 180 to compensate.
	gatorom -r 180 --flipx --decode-cols-left -i ascii.txt -o b/r-0_flipx-1_invert-1_cols-left.bin
	gatorom -r 270 --flipx --decode-cols-left -i ascii.txt -o b/r-90_flipx-1_invert-1_cols-left.bin
	gatorom -r   0 --flipx --decode-cols-left -i ascii.txt -o b/r-180_flipx-1_invert-1_cols-left.bin
	gatorom -r  90 --flipx --decode-cols-left -i ascii.txt -o b/r-270_flipx-1_invert-1_cols-left.bin
	gatorom -r 180 --flipx --decode-cols-left    ascii.txt -o b/r-0_flipx-1_invert-0_cols-left.bin
	gatorom -r 270 --flipx --decode-cols-left    ascii.txt -o b/r-90_flipx-1_invert-0_cols-left.bin
	gatorom -r   0 --flipx --decode-cols-left    ascii.txt -o b/r-180_flipx-1_invert-0_cols-left.bin
	gatorom -r  90 --flipx --decode-cols-left    ascii.txt -o b/r-270_flipx-1_invert-0_cols-left.bin


cols-right: ascii.txt b
# Without flips, these exactly match.
	gatorom -r   0         --decode-cols-right -i ascii.txt -o b/r-0_flipx-0_invert-1_cols-right.bin
	gatorom -r  90         --decode-cols-right -i ascii.txt -o b/r-90_flipx-0_invert-1_cols-right.bin
	gatorom -r 180         --decode-cols-right -i ascii.txt -o b/r-180_flipx-0_invert-1_cols-right.bin
	gatorom -r 270         --decode-cols-right -i ascii.txt -o b/r-270_flipx-0_invert-1_cols-right.bin
	gatorom -r   0         --decode-cols-right    ascii.txt -o b/r-0_flipx-0_invert-0_cols-right.bin
	gatorom -r  90         --decode-cols-right    ascii.txt -o b/r-90_flipx-0_invert-0_cols-right.bin
	gatorom -r 180         --decode-cols-right    ascii.txt -o b/r-180_flipx-0_invert-0_cols-right.bin
	gatorom -r 270         --decode-cols-right    ascii.txt -o b/r-270_flipx-0_invert-0_cols-right.bin
# Flips occur before rotation on Zorrom, so we need to be off by 180 to compensate.
	gatorom -r 180 --flipx --decode-cols-right -i ascii.txt -o b/r-0_flipx-1_invert-1_cols-right.bin
	gatorom -r 270 --flipx --decode-cols-right -i ascii.txt -o b/r-90_flipx-1_invert-1_cols-right.bin
	gatorom -r   0 --flipx --decode-cols-right -i ascii.txt -o b/r-180_flipx-1_invert-1_cols-right.bin
	gatorom -r  90 --flipx --decode-cols-right -i ascii.txt -o b/r-270_flipx-1_invert-1_cols-right.bin
	gatorom -r 180 --flipx --decode-cols-right    ascii.txt -o b/r-0_flipx-1_invert-0_cols-right.bin
	gatorom -r 270 --flipx --decode-cols-right    ascii.txt -o b/r-90_flipx-1_invert-0_cols-right.bin
	gatorom -r   0 --flipx --decode-cols-right    ascii.txt -o b/r-180_flipx-1_invert-0_cols-right.bin
	gatorom -r  90 --flipx --decode-cols-right    ascii.txt -o b/r-270_flipx-1_invert-0_cols-right.bin

squeeze-lr: ascii.txt b
# Without flips, these exactly match.
	gatorom -r   0         --decode-squeeze-lr -i ascii.txt -o b/r-0_flipx-0_invert-1_squeeze-lr.bin
	gatorom -r  90         --decode-squeeze-lr -i ascii.txt -o b/r-90_flipx-0_invert-1_squeeze-lr.bin
	gatorom -r 180         --decode-squeeze-lr -i ascii.txt -o b/r-180_flipx-0_invert-1_squeeze-lr.bin
	gatorom -r 270         --decode-squeeze-lr -i ascii.txt -o b/r-270_flipx-0_invert-1_squeeze-lr.bin
	gatorom -r   0         --decode-squeeze-lr    ascii.txt -o b/r-0_flipx-0_invert-0_squeeze-lr.bin
	gatorom -r  90         --decode-squeeze-lr    ascii.txt -o b/r-90_flipx-0_invert-0_squeeze-lr.bin
	gatorom -r 180         --decode-squeeze-lr    ascii.txt -o b/r-180_flipx-0_invert-0_squeeze-lr.bin
	gatorom -r 270         --decode-squeeze-lr    ascii.txt -o b/r-270_flipx-0_invert-0_squeeze-lr.bin
# Flips occur before rotation on Zorrom, so we need to be off by 180 to compensate.
# Half of these have -z and do not manually adjust the rotation.
	gatorom -z -r 0 --flipx --decode-squeeze-lr -i ascii.txt -o b/r-0_flipx-1_invert-1_squeeze-lr.bin
	gatorom -z -r 90 --flipx --decode-squeeze-lr -i ascii.txt -o b/r-90_flipx-1_invert-1_squeeze-lr.bin
	gatorom -r   0 --flipx --decode-squeeze-lr -i ascii.txt -o b/r-180_flipx-1_invert-1_squeeze-lr.bin
	gatorom -r  90 --flipx --decode-squeeze-lr -i ascii.txt -o b/r-270_flipx-1_invert-1_squeeze-lr.bin
	gatorom -r 180 --flipx --decode-squeeze-lr    ascii.txt -o b/r-0_flipx-1_invert-0_squeeze-lr.bin
	gatorom -z -r 90 --flipx --decode-squeeze-lr    ascii.txt -o b/r-90_flipx-1_invert-0_squeeze-lr.bin
	gatorom -z -r  180 --flipx --decode-squeeze-lr    ascii.txt -o b/r-180_flipx-1_invert-0_squeeze-lr.bin
	gatorom -z -r 270 --flipx --decode-squeeze-lr    ascii.txt -o b/r-270_flipx-1_invert-0_squeeze-lr.bin



test: squeeze-lr cols-right cols-left cols-downl cols-downr
	cd b && md5sum * >../md5.txt
	cd a && md5sum -c ../md5.txt

