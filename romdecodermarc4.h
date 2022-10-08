#ifndef ROMDECODERMARC4_H
#define ROMDECODERMARC4_H

#include "romdecoder.h"

/* This decodes a MARC4 ROM into a .bin file of all
 * logical bytes.  It doesn't yet allow for the ordering
 * of ROM banks, so you should expect to have to adjust
 * the order a little.
 */

class RomDecoderMarc4 : public RomDecoder
{
public:
    RomDecoderMarc4();

    virtual QString preview(MaskRomTool *m) override;
    virtual void writeFile(MaskRomTool *m, QString filename) override;
    QByteArray getbytes(MaskRomTool *m);
};

#endif // ROMDECODERMARC4_H
