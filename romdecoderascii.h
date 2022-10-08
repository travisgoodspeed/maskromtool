#ifndef ROMDECODERASCII_H
#define ROMDECODERASCII_H

#include "romdecoder.h"
#include "maskromtool.h"

/* This is responsible for exporting ASCII bits, for use in Bitviewer
 * or Zorrom.
 */

class RomDecoderAscii : public RomDecoder
{
public:
    RomDecoderAscii();
    //This returns a text preview.
    virtual QString preview(MaskRomTool *m) override;
    //This dumps the real file.
    virtual void writeFile(MaskRomTool *m, QString filename) override;
};

#endif // ROMDECODERASCII_H
