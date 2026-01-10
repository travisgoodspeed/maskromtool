#ifndef ROMDECODERBITIMAGES_H
#define ROMDECODERBITIMAGES_H

#include "romdecoder.h"

/* This exporter takes a directory and fills it with small PNG
 * images of the individual bits of the project, along with their
 * row, column and value.  This is useful if you want to validate
 * the sampling points of your project, and eventually it might
 * be used for a feature to correct sampling points when they
 * stray a little from the bit centers.
 *
 * maskromtool --export-bit-images tmp dmg01cpurom.bmp -e
 */

class RomDecoderBitImages : public RomDecoder
{
public:
    RomDecoderBitImages();
    virtual QString preview(MaskRomTool *m) override;
    virtual void writeFile(MaskRomTool *m, QString filename) override;
};

#endif // ROMDECODERBITIMAGES_H
