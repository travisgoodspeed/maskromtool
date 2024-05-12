#ifndef GATODECODERARM6_H
#define GATODECODERARM6_H

#include "gatodecodercolsleft.h"

/* This is a decoder for the ARM firmware ROMs found in
 * the MYK82 chip from the Clipper/Capstone family.
 *
 * Each row of the ROM contains sixteen 32-bit words of
 * memory, with most significant bits on the right.  Bits
 * of significance are divided into 32 columns, and the
 * earliest words are packed to the right.  The ending
 * bytes are packed with zeroes.
 *
 * Other RISC ROMs from 32-bit architectures might look similar.
 */

class GatoDecoderARM6 : public GatoDecoderColsLeft{
public:
    GatoDecoderARM6();
//    QByteArray decode(GatoROM *gr);
};

#endif // GATODECODERARM6_H
