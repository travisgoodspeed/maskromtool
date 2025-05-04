#include "gatodecodercolsdownlswap.h"

#include <QDebug>

/* NEC uCOM4 micros use a format much like cols-downl
 * except that the 128 byte pages periodically swap order.
 * This only works in 8-bit mode.
 *
 * The first and secoond page come first.
 * Then the fourth before the third.
 * Then the fifth and sixth.
 * Then the eighth and seventh.
 */

GatoDecoderColsDownLSwap::GatoDecoderColsDownLSwap(){
    name="cols-downl-swap";
}


void GatoDecoderColsDownLSwap::decode(GatoROM *gr){
    uint32_t adr=0, vadr=0;  //Physical and virtual address.
    QByteArray ba, vba, empty;      //Physical and virtual bytes.
    QByteArray bad, vbad; //Damage mask.
    int wordsize=gr->wordsize;

    gr->eval();

    //We might be dynamic, but we still don't want to crash.
    if(wordsize!=8 || gr->outputcols%wordsize!=0) return;
    int skip=gr->outputcols/wordsize;

    //Each row contains sixteen 8-bit words.
    //We calculate that dynamically to be more generic.
    for(unsigned int word=0; word<(gr->outputcols/wordsize); word++){
        for(unsigned int row=0; row<gr->outputrows; row++){
            uint32_t w=0, wd=0; //Data and damage.

            for(int bit=wordsize-1; bit>=0; bit--){
                GatoBit *B=gr->outputbit(row,bit*skip+word);
                assert(B); //If this fails, we're about to crash.

                //Update target address and mask.
                B->adr=vadr;
                B->mask=1<<bit;
                if(B->getVal())
                    w|=B->mask;
                if(B->ambiguous)
                    wd|=B->mask;
            }
            ba.append(w&0xFF);
            bad.append(w&0xFF);

            /* The virtual address is the same as the physical,
             * except when we are swapping.
             */
            vadr=++adr;
            if(adr&0x100) vadr=adr^0x80;
        }
    }

    //ba is in the physical order, so we need to swap it.
    for(unsigned int adr=0; adr<ba.length(); adr++){
        vadr=adr;
        if(adr&0x100) vadr=adr^0x80;

        if(ba.length()>vadr){
            vba.append(ba[vadr]);
            vbad.append(bad[vadr]);
        } else return; //Empty return on error.
    }

    //Return swapped bytes and damage.
    gr->decoded=vba;
    gr->decodedDamage=vbad;
}
