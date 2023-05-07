#ifndef ROMBITSAMPLER_H
#define ROMBITSAMPLER_H

#include <QImage>

class MaskRomTool;


/* The purpose of this sampler class is to take a project, an image, and a position,
 * then return a QRgb color for that position.  The value here is that we can make
 * new sampling strategies to average bits, or to take the extreme color within a range.
 */

class RomBitSampler{
public:
    RomBitSampler();
    QRgb bitvalue_raw(MaskRomTool *mrt, QImage &bg, QPointF pos);
};

#endif // ROMBITSAMPLER_H
