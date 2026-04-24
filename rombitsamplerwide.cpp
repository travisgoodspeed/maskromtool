#include "rombitsamplerwide.h"
#include "maskromtool.h"
#include <QDebug>

RomBitSamplerWide::RomBitSamplerWide(bool dark){
    name=(dark?"Wide":"WideBright");
    this->dark=dark;
}

QRgb RomBitSamplerWide::bitvalue_raw(MaskRomTool *mrt, QImage &bg, QPointF pos){
    //Start with the brightest, then fall to the darkest.
    uint16_t r,g,b; //Samples from each pixel.
    uint16_t R,G,B; //Darkest sample we've seen yet.

    if(dark)
        R=G=B=0xFF;
    else
        R=G=B=0;

    QPoint point=pos.toPoint();
    int xpos=point.x();

    for(int x=xpos-size/2; x<xpos+size/2; x++){
        point.setX(x);
        QRgb rgb=bg.pixel(point);

        //Extract the bits.
        r=(rgb>>16)&0xFF;
        g=(rgb>>8)&0xFF;
        b=(rgb)&0xFF;

        //Take the darkest.
        if(dark){
            if(r<R) R=r;
            if(g<G) G=g;
            if(b<B) B=b;
        }else{
            if(r>R) R=r;
            if(g>G) G=g;
            if(b>B) B=b;
        }
    }

    //Recompile the colors to the darkest pixel in the line.
    QRgb newpixel=(R<<16)+(G<<8)+(B);

    return newpixel;
}


/* This returns a rectangle for the bit to be
 * rendered.  Sometimes that's just a box, but
 * it can also show more complex shapes when the
 * sampling needs it.
 */
QRectF RomBitSamplerWide::getRect(MaskRomTool *mrt){
    qreal width=size;
    qreal height=mrt->bitSize;
    return QRectF(-width/2, -height/2, width, height);
}

