#include "gatodecodercolsright.h"

/* This decoder is based on Zorrom's cols-right module.
 * Bits start at the upper right, move left, and then
 * wrap around to the next row.
 */

GatoDecoderColsRight::GatoDecoderColsRight(){
    name="cols-right";
}

void GatoDecoderColsRight::decode(GatoROM *gr){
    uint32_t adr=0;
    QByteArray ba, bad; //Data and Damage
    int wordsize=gr->wordsize;

    gr->eval();

    //We might be dynamic, but we still don't want to crash.
    if(gr->outputcols%wordsize!=0) return;
    int skip=gr->outputcols/wordsize;

    //Each row contains sixteen 8-bit words.
    //We calculate that dynamically to be more generic.
    for(unsigned int row=0; row<gr->outputrows; row++){
        for(unsigned int word=0; word<(gr->outputcols/wordsize); word++){
            uint32_t w=0, wd=0; //Data and Damage

            for(int bit=wordsize-1; bit>=0; bit--){
                GatoBit *B=gr->outputbit(row,bit*skip+word);
                assert(B); //If this fails, we're about to crash.

                //Update target address and mask.
                B->adr=adr;
                B->mask=1<<bit;
                if(B->getVal())
                    w|=B->mask;
                if(B->ambiguous)
                    wd|=B->mask;
            }

            //This is implicitly little endian.
            for(int bitcount=wordsize; bitcount>0; bitcount-=8){
                ba.append(w&0xFF);
                w=w>>8;
                bad.append(wd&0xff);
                wd=wd>>8;

                adr++;
            }
        }
    }

    //Apply result.
    gr->decoded=ba;
    gr->decodedDamage=bad;
}

