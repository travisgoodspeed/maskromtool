#include "gatodecodercolsright.h"

/* This decoder is based on Zorrom's cols-right module.
 * Bits start at the upper right, move left, and then
 * wrap around to the next row.
 */

GatoDecoderColsRight::GatoDecoderColsRight(){
    name="cols-right";
}

QByteArray GatoDecoderColsRight::decode(GatoROM *gr){
    uint32_t adr=0;
    QByteArray ba;

    gr->eval();

    //We might be dynamic, but we still don't want to crash.
    if(gr->outputcols%8!=0) return ba;
    int skip=gr->outputcols/8;

    //Each row contains sixteen 8-bit words.
    //We calculate that dynamically to be more generic.
    for(unsigned int row=0; row<gr->outputrows; row++){
        for(unsigned int word=0; word<(gr->outputcols/8); word++){
            uint32_t w=0;

            for(int bit=7; bit>=0; bit--){
                GatoBit *B=gr->outputbits[row][bit*skip+word];
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

