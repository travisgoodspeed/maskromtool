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
    QString name="Default";
    virtual QRgb bitvalue_raw(MaskRomTool *mrt, QImage &bg, QPointF pos);
    virtual void setSize(int newsize);
    virtual int getSize();
    virtual QRectF getRect(MaskRomTool *mrt);
    int size=0;  //Zero must remain a legal size.
};

#endif // ROMBITSAMPLER_H
