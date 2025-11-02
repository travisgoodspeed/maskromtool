#ifndef ROMBITSAMPLERWIDE_H
#define ROMBITSAMPLERWIDE_H

#include "rombitsampler.h"

/* The regular bit sampler just checks a single pixel.  This one takes a
 * small line of pixels at not quite the width between the nearest bits.
 */

class RomBitSamplerWide : public RomBitSampler
{
public:
    RomBitSamplerWide();
    QRgb bitvalue_raw(MaskRomTool *mrt, QImage &bg, QPointF pos);
    QRectF getRect(MaskRomTool *mrt);
};

#endif // ROMBITSAMPLERWIDE_H
