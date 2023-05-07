#include "rombitsampler.h"

RomBitSampler::RomBitSampler(){

}


QRgb RomBitSampler::bitvalue_raw(MaskRomTool *mrt, QImage &bg, QPointF pos){
    QRgb pixel=bg.pixel(pos.toPoint());
    return pixel;
}
