#ifndef ROMDECODERHISTOGRAM_H
#define ROMDECODERHISTOGRAM_H

#include "romdecoder.h"

class RomDecoderHistogram : public RomDecoder
{
public:
    RomDecoderHistogram();
    //This returns a text preview.
    virtual QString preview(MaskRomTool *m) override;
    //This dumps the real file.
    virtual void writeFile(MaskRomTool *m, QString filename) override;
};

#endif // ROMDECODERHISTOGRAM_H
