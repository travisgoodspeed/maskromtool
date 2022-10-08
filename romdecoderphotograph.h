#ifndef ROMDECODERPHOTOGRAPH_H
#define ROMDECODERPHOTOGRAPH_H

#include "romdecoder.h"

class RomDecoderPhotograph : public RomDecoder
{
public:
    RomDecoderPhotograph();
    virtual QString preview(MaskRomTool *m) override;
    virtual void writeFile(MaskRomTool *m, QString filename) override;
};

#endif // ROMDECODERPHOTOGRAPH_H
