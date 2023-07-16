#ifndef ROMBITSAMPLERTALL_H
#define ROMBITSAMPLERTALL_H

#include "rombitsampler.h"

/* The regular bit sampler just checks a single pixel.  This one takes a
 * small line of pixels at not quite the width between the nearest bits.
 */

class RomBitSamplerTall : public RomBitSampler
{
public:
    RomBitSamplerTall();
    QRgb bitvalue_raw(MaskRomTool *mrt, QImage &bg, QPointF pos);
};

#endif // ROMBITSAMPLERWIDE_H
