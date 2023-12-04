#include "gatodecodercolsdownlswap.h"

#include <QDebug>

/* NEC uCOM4 micros use a format much like cols-downl
 * except that the 128 byte pages periodically swap order.
 *
 * The first and secoond page come first.
 * Then the fourth before the third.
 * Then the fifth and sixth.
 * Then the eighth and seventh.
 */

GatoDecoderColsDownLSwap::GatoDecoderColsDownLSwap(){
    name="cols-downl-swap";
}


QByteArray GatoDecoderColsDownLSwap::decode(GatoROM *gr){
    uint32_t adr=0, vadr=0;  //Physical and virtual address.
    QByteArray ba, vba, empty;      //Physical and virtual bytes.

    gr->eval();

    //We might be dynamic, but we still don't want to crash.
    if(gr->outputcols%8!=0) return ba;
    int skip=gr->outputcols/8;

    //Each row contains sixteen 8-bit words.
    //We calculate that dynamically to be more generic.
    for(unsigned int word=0; word<(gr->outputcols/8); word++){
        for(unsigned int row=0; row<gr->outputrows; row++){
            uint32_t w=0;

            for(int bit=7; bit>=0; bit--){
                GatoBit *B=gr->outputbit(row,bit*skip+word);
                assert(B); //If this fails, we're about to crash.

                //Update target address and mask.
                B->adr=vadr;
                B->mask=1<<bit;
                if(B->getVal())
                    w|=B->mask;
            }
            ba.append(w&0xFF);

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

        if(ba.length()>vadr)
            vba.append(ba[vadr]);
        else
            return empty;
    }

    return vba;
}
