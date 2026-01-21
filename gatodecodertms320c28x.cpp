#include "gatodecodertms320c28x.h"

/* This decoder is intended for ROM banks present on
 * TI TMS320C28x family of digital signal processors.
 *
 * ROM banks in this part family consist of rows of 1024 bits
 * comprising four meta-columns, each containing sixteen sub-columns.
 * Words are composed of bits from the same position in each sub-column,
 * advancing to the next meta-column for the next word. After four words,
 * the process continues from the next sub-column position and resets when
 * advancing to the next row once all sub-column positions in the row are
 * exhausted.
 */

GatoDecoderTMS320C28x::GatoDecoderTMS320C28x() { name = "tms320c28x"; }

void GatoDecoderTMS320C28x::decode(GatoROM *gr) {
  QByteArray ba;  // Data
  QByteArray bad; // Damage

  if (gr->wordsize != 16 || gr->outputcols != 1024)
    return; // Fail when poor match.

  int seccount = gr->outputcols / 4;

  // Top to bottom
  uint32_t adr = 0;
  for (int row = 0; row < gr->outputrows; row++) {
    for (int pos = 15; pos >= 0; pos--) {
      for (int sec = 3; sec >= 0; sec--) {
        uint16_t w = 0;  // Data
        uint16_t wd = 0; // Damage

        int base = sec * seccount;

        for (int bit = 0; bit < gr->wordsize; bit++) {
          int col =
              base + (seccount - (gr->wordsize * bit)) - (gr->wordsize - pos);

          GatoBit *B = gr->outputbit(row, col);
          assert(B);

          B->adr = adr;       // Mark the address.
          B->mask = 1 << bit; // Mark the bit.

          if (B->getVal())
            w |= B->mask;
          if (B->ambiguous)
            wd |= B->mask;
        }

        // This is implicitly little endian.
        for (int bitcount = gr->wordsize; bitcount > 0; bitcount -= 8) {
          ba.append(w & 0xFF);
          w = w >> 8;
          bad.append(wd & 0xff);
          wd = wd >> 8;

          adr++;
        }
      }
    }
  }

  gr->decoded = ba;
  gr->decodedDamage = bad;
}
