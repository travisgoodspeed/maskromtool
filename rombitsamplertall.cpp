#include "rombitsamplertall.h"
#include <QDebug>
#include <maskromtool.h>

RomBitSamplerTall::RomBitSamplerTall(bool dark){
    name=(dark?"Tall":"TallBright");
    this->dark=dark;
}


QRgb RomBitSamplerTall::bitvalue_raw(MaskRomTool *mrt, QImage &bg, QPointF pos){
    //Start with the brightest, then fall to the darkest.
    uint16_t r,g,b; //Samples from each pixel.
    uint16_t R,G,B; //Darkest sample we've seen yet.

    if(dark)
        R=G=B=0xFF;
    else
        R=G=B=0;

    QPoint point=pos.toPoint();
    int ypos=point.y();

    for(int y=ypos-size/2; y<ypos+size/2; y++){
        point.setY(y);
        QRgb rgb=bg.pixel(point);

        //Extract the bits.
        r=(rgb>>16)&0xFF;
        g=(rgb>>8)&0xFF;
        b=(rgb)&0xFF;

        if(dark){
            //Take the darkest.
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
QRectF RomBitSamplerTall::getRect(MaskRomTool *mrt){
    qreal width=mrt->bitSize;
    qreal height=size;
    return QRectF(-width/2, -height/2, width, height);
}
