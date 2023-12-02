#include "gatodecodercolsleft.h"

/* This decoder is based on Zorrom's cols-left module.
 * Bits start at the upper left, move right, and then
 * wrap around to the next row.
 */

GatoDecoderColsLeft::GatoDecoderColsLeft(){
    name="cols-left";
}

QByteArray GatoDecoderColsLeft::decode(GatoROM *gr){
    uint32_t adr=0;
    QByteArray ba;

    gr->eval();

    //We might be dynamic, but we still don't want to crash.
    if(gr->outputcols%8!=0) return ba;
    int skip=gr->outputcols/8;

    //Each row contains sixteen 8-bit words.
    //We calculate that dynamically to be more generic.
    for(unsigned int row=0; row<gr->outputrows; row++){
        for(int word=(gr->outputcols/8)-1; word>=0; word--){
            uint32_t w=0;

            for(int bit=7; bit>=0; bit--){
                GatoBit *B=gr->outputbit(row,bit*skip+word);
                assert(B); //If this fails, we're about to crash.

                //Update target address and mask.
                B->adr=adr;
                B->mask=1<<bit;
                if(B->getVal())
                    w|=B->mask;
            }
            ba.append(w&0xFF);
            adr++;
        }
    }

    return ba;
}

