#include "gatodecodersqueezelr.h"

/* This decoder is based on Zorrom's squeeze-lr module.
 * Within each row, the even bits come in from the left
 * and the odd bits come in from the right.
 */

GatoDecoderSqueezeLR::GatoDecoderSqueezeLR(){
    name="squeeze-lr";
}

QByteArray GatoDecoderSqueezeLR::decode(GatoROM *gr){
    uint32_t leftadr=0, rightadr=0;
    QByteArray left, right;
    QByteArray ba;

    gr->eval();

    //We might be dynamic, but we still don't want to crash.
    if(gr->outputcols%8!=0) return ba;
    int skip=gr->outputcols/8;

    //Each row contains many 8-bit words.
    //We calculate that dynamically to be more generic.
    for(unsigned int row=0; row<gr->outputrows; row++){
        /* squeeze-lr is an interleved mix of cols-left
         * and cols-right, with even bits coming in from one
         * direction and odd bits coming in from the other.
         *
         * Here we decode both and them mix them at the end.
         */


        //cols-left
        for(int word=(gr->outputcols/8)-1; word>=0; word--){
            uint32_t w=0;

            for(int bit=7; bit>=0; bit--){
                GatoBit *B=gr->outputbit(row, bit*skip+word);
                assert(B); //If this fails, we're about to crash.

                //Update target address and mask.
                if((1<<bit)&0xAA){
                    B->adr=leftadr;
                    B->mask=1<<bit;
                    if(B->getVal())
                        w|=B->mask;
                }
            }
            left.append(w&0xFF);
            leftadr++;
        }

        //cols-right
        for(unsigned int word=0; word<(gr->outputcols/8); word++){
            uint32_t w=0;

            for(int bit=7; bit>=0; bit--){
                GatoBit *B=gr->outputbit(row,bit*skip+word);
                assert(B); //If this fails, we're about to crash.

                //Update target address and mask.
                if((1<<bit)&0x55){
                    B->adr=rightadr;
                    B->mask=1<<bit;
                    if(B->getVal())
                        w|=B->mask;
                }
            }
            right.append(w&0xFF);
            rightadr++;
        }
    }

    for(int i=0; i<left.length(); i++)
        ba.append(left[i]|right[i]);


    return ba;
}
