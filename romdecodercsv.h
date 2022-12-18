#ifndef ROMDECODERCSV_H
#define ROMDECODERCSV_H

#include "romdecoder.h"

class RomDecoderCSV : public RomDecoder
{
public:
    RomDecoderCSV();
    //This returns a text preview.
    virtual QString preview(MaskRomTool *m) override;
    //This dumps the real file.
    virtual void writeFile(MaskRomTool *m, QString filename) override;
};

#endif // ROMDECODERCSV_H
