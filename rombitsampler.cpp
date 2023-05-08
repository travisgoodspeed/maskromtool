#include "rombitsampler.h"

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
