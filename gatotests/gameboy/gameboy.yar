
// The GameBoy boot ROM contains a bitmap of Nintendo's logo, compared
// against a matching bitmap in every cartridge.  Here we check for the
// first row of the logo.

rule gameboy {
strings:
  $nintendo = {CE ED 66 66 CC 0D 00 0B 03 73 00 83 00 0C 00 0D}

condition:
  $nintendo
}