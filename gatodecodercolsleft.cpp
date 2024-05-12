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
    if(gr->outputcols%wordsize!=0) return ba;
    int skip=gr->outputcols/wordsize;

    //Each row contains sixteen 8-bit words.
    //We calculate that dynamically to be more generic.
    for(unsigned int row=0; row<gr->outputrows; row++){
        for(int word=(gr->outputcols/wordsize)-1; word>=0; word--){
            uint32_t w=0;

            for(int bit=wordsize-1; bit>=0; bit--){
                GatoBit *B=gr->outputbit(row,bit*skip+word);
                assert(B); //If this fails, we're about to crash.

                //Update target address and mask.
                B->adr=adr;
                B->mask=1<<bit;
                if(B->getVal())
                    w|=B->mask;
            }

            //This is implicitly big endian.
            for(int bitcount=wordsize; bitcount>0; bitcount-=8){
                ba.append(w&0xFF);
                w=w>>8;

                adr++;
            }
        }
    }

    return ba;
}

