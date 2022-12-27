#ifndef ROMDECODERARM6_H
#define ROMDECODERARM6_H

#include "romdecoder.h"

/* This decodes 32-bit program ROMs as found in ARM6L SoC devices.
 * The initial decoder was built around the MYK82 chip from a Fortezza card.
 *
 * The rules of this ROM in general:
 * Rows are in linear order, with Row 0 at the very top.
 * Most significant bits appear to the right, with a very distinctive pattern for E (1110) at the far right.
 * 16 words (512 bits) appear on each row.
 * Unused rows are empty, all zeroes.
 *
 *
 */

class RomDecoderARM6 : public RomDecoder
{
public:
    RomDecoderARM6();

    virtual QString preview(MaskRomTool *m) override;
    virtual void writeFile(MaskRomTool *m, QString filename) override;
    QByteArray getbytes(MaskRomTool *m);
};

#endif // ROMDECODERARM6_H
