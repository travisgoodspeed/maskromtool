#include "rombitsamplerwide.h"
#include "maskromtool.h"
#include <QDebug>

RomBitSamplerWide::RomBitSamplerWide(){
    name="Wide";
}

QRgb RomBitSamplerWide::bitvalue_raw(MaskRomTool *mrt, QImage &bg, QPointF pos){
    //Start with the brightest, then fall to the darkest.
    uint16_t r=0xff,g=0xff,b=0xff; //Samples from each pixel.
    uint16_t R=0xff,G=0xff,B=0xff; //Darkest sample we've seen yet.

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
        if(r<R) R=r;
        if(g<G) G=g;
        if(b<B) B=b;
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

