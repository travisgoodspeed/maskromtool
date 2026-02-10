#include "gatodecodermarc4.h"
#include "affinedecoder.h"
#include <array>

/* This decoder is based on the layout of MARC4 mask ROMS
 * MARC4 ROMS are split up into "meta columns" where each
 * meta column contains 2 interleaved columns of words.
 * Specifically, on MARC4, each meta column consists of
 * 16 columns and 2 words. A word consists of bits from
 * every-other column in a meta column, with the next word
 * being on the row below the current word. Once the last row
 * has been read, the next word is at the top of the next column,
 * or the top of the next meta-column, if this is the last
 * column in the current meta-column. For example, with a
 * word size of 2 bits and 2 meta columns, and 4 rows, the bit ordering
 * would look like:
 * 0 4 0 4 8 c 8 c
 * 1 5 1 5 9 d 9 d
 * 2 6 2 6 a e a e
 * 3 7 3 7 b f b f
 * In the above chart, number indicates which word that bit is a member of.
 * The dimensions of the ROM and word size for the chart above are smaller than
 * are realistic in order to keep the chart small. With a word size of 8 and 4
 * rows, the leftmost meta-column would have bit indices:
 * 0 4 0 4 0 4 0 4 0 4 0 4 0 4 0 4
 * 1 5 1 5 1 5 1 5 1 5 1 5 1 5 1 5
 * 2 6 2 6 2 6 2 6 2 6 2 6 2 6 2 6
 * 3 7 3 7 3 7 3 7 3 7 3 7 3 7 3 7
 */
GatoDecoderMarc4::GatoDecoderMarc4() { name = "marc4"; }

void GatoDecoderMarc4::decode(GatoROM *gr) {
  unsigned int nrows = gr->outputrows;
  unsigned int ncols = gr->outputcols;
  unsigned int wordsize = gr->wordsize;
  if (ncols % (2 * wordsize) != 0) {
    return;
  }
  std::array<unsigned int, 4> counts = {wordsize, nrows, 2, ncols / 16};
  std::array<int, 4> col_strides = {-2, 0, 1, (int)wordsize * 2};
  std::array<int, 4> row_strides = {0, 1, 0, 0};
  AffineDecoder<4> root_decode =
      AffineDecoder(counts, row_strides, col_strides, 0, wordsize*2-2);
  return root_decode.decode(gr);
}
