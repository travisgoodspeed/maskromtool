Howdy y'all,

This decoding of [Fabulous Fred's NEC uCOM4
ROM](https://seanriddle.com/necucom4.html) is almost like cols-downl,
except that every other pair of 128 byte pages is swapped.

This test runs by solving for a byte in one of those swapped pages, so
it ought to gracefully adapt when the --decode-cols-downl-swap option
is changed or gets a new name.

--Travis
