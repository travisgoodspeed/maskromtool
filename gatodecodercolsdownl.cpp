#include "gatodecodercolsdownl.h"

#include <QDebug>

/* In Zorrom, the equivalent would be cols-downl.
 */

GatoDecoderColsDownL::GatoDecoderColsDownL(){
    name="cols-downl";
}


QByteArray GatoDecoderColsDownL::decode(GatoROM *gr){
    uint32_t adr=0;
    QByteArray ba;

    gr->eval();

    //We might be dynamic, but we still don't want to crash.
    if(gr->outputcols%wordsize!=0) return ba;
    int skip=gr->outputcols/wordsize;

    //Each row contains sixteen 8-bit words.
    //We calculate that dynamically to be more generic.
    for(unsigned int word=0; word<(gr->outputcols/wordsize); word++){
        for(unsigned int row=0; row<gr->outputrows; row++){
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
