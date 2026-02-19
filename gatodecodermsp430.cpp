#include "gatodecodermsp430.h"
#include "affinedecoder.h"
#include <array>

/* This decoder is based on the layout of MSP430 mask ROMs.
 * MSP430 ROMs use 16 bit words. Words are traversed in
 * the same order as cols-downr, but the ordering of bits
 * within each word is different. The MSB is in the leftmost
 * and the 2 bytes of the word are interleaved. Using 0xf as
 * the MSB index, and 0x0 as the LSB bit index, within each
 * word the bits are ordered (from leftmost column to rightmost column):
 * 7f6ed5c4b3a29180
 * Since currently all decoders are implicitly little-endian,
 * this means that we traverse the columns in two groups for the
 * affine decoder in the below order:
 * 0 1 2 3 4 5 6 7  (least significant byte)
 *  8 9 a b c d e f (most significant byte)
 * 08192a3b4c5d6e7f (combined order)
 */
GatoDecoderMSP430::GatoDecoderMSP430() { name = "msp430"; }

void GatoDecoderMSP430::decode(GatoROM *gr) {
  unsigned int nrows = gr->outputrows;
  unsigned int ncols = gr->outputcols;
  unsigned int wordsize = gr->wordsize;
  if (ncols % (wordsize) != 0 || wordsize % 2 != 0) {
    return;
  }
  int word_skip = ncols / wordsize;
  const size_t len = 4;
  std::array<unsigned int, len> counts = {wordsize / 2, 2, nrows,
                                          (unsigned int)word_skip};
  // Interleave bits by dividing into two sets of double word skip
  std::array<int, len> col_strides = {word_skip * 2, word_skip, 0, -1};
  std::array<int, len> row_strides = {0, 0, 1, 0};
  AffineDecoder<len> root_decode =
      AffineDecoder(counts, row_strides, col_strides, 0, word_skip - 1);
  return root_decode.decode(gr);
}
