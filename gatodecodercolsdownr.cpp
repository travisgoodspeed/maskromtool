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


void GatoDecoderColsDownR::decode(GatoROM *gr){
    QByteArray ba, bad; //Data and damage.
    uint32_t adr=0;
    int wordsize=gr->wordsize;

    gr->eval();

    //We might be dynamic, but we still don't want to crash.
    if(gr->outputcols%wordsize!=0) return;
    int skip=gr->outputcols/wordsize;

    //Each row contains sixteen 8-bit words.
    //We calculate that dynamically to be more generic.
    for(int word=(gr->outputcols/wordsize)-1; word>=0; word--){
        for(unsigned int row=0; row<gr->outputrows; row++){
            uint32_t w=0, wd=0; //Data and damage.

            for(int bit=wordsize-1; bit>=0; bit--){
                GatoBit *B=gr->outputbit(row, bit*skip+word);
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
                bad.append(wd&0xff);
                w=w>>8;
                wd=wd>>8;

                adr++;
            }
        }
    }

    //Pass back the data and the damage.
    gr->decoded=ba;
    gr->decodedDamage=bad;
}
