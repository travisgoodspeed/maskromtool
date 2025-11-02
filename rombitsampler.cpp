#include "rombitsampler.h"
#include "maskromtool.h"

RomBitSampler::RomBitSampler(){
    name="Default";
}

QRgb RomBitSampler::bitvalue_raw(MaskRomTool *mrt, QImage &bg, QPointF pos){
    QRgb pixel=bg.pixel(pos.toPoint());
    return pixel;
}

void RomBitSampler::setSize(int newsize){
    size=newsize;
}
int RomBitSampler::getSize(){
    return size;
}

/* This returns a rectangle for the bit to be
 * rendered.  Sometimes that's just a box, but
 * it can also show more complex shapes when the
 * sampling needs it.
 */
QRectF RomBitSampler::getRect(MaskRomTool *mrt){
    qreal width=mrt->bitSize;
    qreal height=mrt->bitSize;
    return QRectF(-width/2, -height/2, width, height);
}
