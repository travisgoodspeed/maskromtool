#ifndef ROMDECODERPYTHON_H
#define ROMDECODERPYTHON_H

#include "romdecoder.h"

/* This decodes the bits into an array in the Python format,
 * for use when writing custom decoder scripts.
 */

class RomDecoderPython : public RomDecoder
{
public:
    RomDecoderPython();
    //This returns a text preview.
    virtual QString preview(MaskRomTool *m) override;
    //This dumps the real file.
    virtual void writeFile(MaskRomTool *m, QString filename) override;
};

#endif // ROMDECODERPYTHON_H
