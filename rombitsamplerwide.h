#ifndef ROMBITSAMPLERWIDE_H
#define ROMBITSAMPLERWIDE_H

#include "rombitsampler.h"

/* The regular bit sampler just checks a single pixel.  This one takes a
 * small line of pixels at not quite the width between the nearest bits.
 */

class RomBitSamplerWide : public RomBitSampler
{
public:
    RomBitSamplerWide(bool dark);
    QRgb bitvalue_raw(MaskRomTool *mrt, QImage &bg, QPointF pos);
    QRectF getRect(MaskRomTool *mrt);
private:
    bool dark=true;
};

#endif // ROMBITSAMPLERWIDE_H
