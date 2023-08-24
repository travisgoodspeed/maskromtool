#ifndef ROMDECODERGATO_H
#define ROMDECODERGATO_H

#include "romdecoder.h"

class MaskRomTool;
class GatoROM;

/* This doesn't support previews or file writes like the
 * other decoder classes, but it will support things like
 * rotations in the future.
 *
 * Hopefully it's not too confusing that this class doesn't
 * quite fit in.
 * --Travis
 */

class RomDecoderGato : public RomDecoder
{
public:
    RomDecoderGato();
    GatoROM gatorom(MaskRomTool *mrt);

    //Virtual functions we don't yet implement.
    virtual QString preview(MaskRomTool *m) override;
    virtual void writeFile(MaskRomTool *m, QString filename) override;
};

#endif // ROMDECODERGATO_H
