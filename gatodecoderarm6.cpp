#include "gatodecoderarm6.h"

#include <QDebug>

GatoDecoderARM6::GatoDecoderARM6(){
    name="arm6";
}


QByteArray GatoDecoderARM6::decode(GatoROM *gr){
    uint32_t adr=0;
    QByteArray ba;

    //Fail when the column count is unexpected.
    if(gr->outputcols!=512) return ba;  //Looser check for other ROMs?

    for(unsigned int row=0; row<gr->outputrows; row++){
        //Each row contains sixteen 32-bit words.
        //They are ordered from right to left, with bits spread across 32 columns.
        for(int word=15; word>=0; word--){
            uint32_t w=0;
            for(int bit=31; bit>=0; bit--){
                GatoBit *gb=gr->outputbits[row][bit*16+word];
                assert(gb);

                //Mark the address and bit.
                gb->adr=adr+w/8;
                gb->mask=1<<(w%8);

                //Mark the value.
                int b=gb->getVal();
                w=(w<<1)|b;
            }
            ba.append(w&0xFF);
            ba.append((w>>8)&0xFF);
            ba.append((w>>16)&0xFF);
            ba.append((w>>24)&0xFF);
            adr+=4;
        }
    }

    return ba;
}
