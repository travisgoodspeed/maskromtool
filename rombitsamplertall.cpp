#include "rombitsamplertall.h"
#include <QDebug>

RomBitSamplerTall::RomBitSamplerTall(){
    name="Tall";
}


QRgb RomBitSamplerTall::bitvalue_raw(MaskRomTool *mrt, QImage &bg, QPointF pos){
    //Start with the brightest, then fall to the darkest.
    uint16_t r=0xff,g=0xff,b=0xff; //Samples from each pixel.
    uint16_t R=0xff,G=0xff,B=0xff; //Darkest sample we've seen yet.

    QPoint point=pos.toPoint();
    int ypos=point.y();

    for(int y=ypos-size/2; y<ypos+size/2; y++){
        point.setY(y);
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
