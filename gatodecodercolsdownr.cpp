#include "gatodecodercolsdownr.h"

#include <QDebug>

/* This decoder was intended for the GameBoy's LR35902 CPU and
 * its mask ROM.  Bytes are encoded in 16-bit logical columns made of
 * two 8-bit physical columns.  The leftmost column contains the most
 * significant bits, and the first byte of the row is in the leftmost
 * positions.  Go first down, then to the right.
 *
 * In Zorrom, the equivalent would be cols-downr.
 */

GatoDecoderColsDownR::GatoDecoderColsDownR(){
    name="cols-downr";
}


QByteArray GatoDecoderColsDownR::decode(GatoROM *gr){
    QByteArray ba;
    uint32_t adr=0;

    gr->eval();

    //We might be dynamic, but we still don't want to crash.
    if(gr->outputcols%8!=0) return ba;
    int skip=gr->outputcols/8;

    //Each row contains sixteen 8-bit words.
    //We calculate that dynamically to be more generic.
    for(int word=(gr->outputcols/8)-1; word>=0; word--){
        for(unsigned int row=0; row<gr->outputrows; row++){
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
