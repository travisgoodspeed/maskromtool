#ifndef ROMDECODERASCIIDAMAGE_H
#define ROMDECODERASCIIDAMAGE_H

#include "romdecoder.h"

class RomDecoderAsciiDamage : public RomDecoder
{
public:
    RomDecoderAsciiDamage();
    //This returns a text preview.
    virtual QString preview(MaskRomTool *m) override;
    //This dumps the real file.
    virtual void writeFile(MaskRomTool *m, QString filename) override;
};

#endif // ROMDECODERASCIIDAMAGE_H
