#include "gatodecodersqueezelr.h"

/* This decoder is based on Zorrom's squeeze-lr module.
 * Within each row, the even bits come in from the left
 * and the odd bits come in from the right.
 *
 * This only works in 8-bit mode.
 */

GatoDecoderSqueezeLR::GatoDecoderSqueezeLR(){
    name="squeeze-lr";
}

void GatoDecoderSqueezeLR::decode(GatoROM *gr){
    uint32_t leftadr=0, rightadr=0;
    QByteArray left, right;   //data
    QByteArray leftd, rightd; //damage
    QByteArray ba, bad; //data and damage
    int wordsize=gr->wordsize;

    gr->eval();

    //We might be dynamic, but we still don't want to crash.
    if(wordsize!=8 || gr->outputcols%wordsize!=0) return;
    int skip=gr->outputcols/wordsize;

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
        for(int word=(gr->outputcols/wordsize)-1; word>=0; word--){
            uint32_t w=0, wd=0; //Word and Word Damage

            for(int bit=wordsize-1; bit>=0; bit--){
                GatoBit *B=gr->outputbit(row, bit*skip+word);
                assert(B); //If this fails, we're about to crash.

                //Update target address and mask.
                if((1<<bit)&0xAA){
                    B->adr=leftadr;
                    B->mask=1<<bit;
                    if(B->getVal())
                        w|=B->mask;
                    if(B->ambiguous)
                        wd|=B->mask;
                }
            }
            left.append(w&0xFF);
            leftd.append(wd&0xff);
            leftadr++;
        }

        //cols-right
        for(unsigned int word=0; word<(gr->outputcols/wordsize); word++){
            uint32_t w=0, wd=0; //data and damage

            for(int bit=wordsize-1; bit>=0; bit--){
                GatoBit *B=gr->outputbit(row,bit*skip+word);
                assert(B); //If this fails, we're about to crash.

                //Update target address and mask.
                if((1<<bit)&0x55){
                    B->adr=rightadr;
                    B->mask=1<<bit;
                    if(B->getVal())
                        w|=B->mask;
                    if(B->ambiguous)
                        wd|=B->mask;
                }
            }
            right.append(w&0xFF);
            rightd.append(wd&0xff);
            rightadr++;
        }
    }

    for(int i=0; i<left.length(); i++){
        ba.append(left[i]|right[i]);
        bad.append(leftd[i]|rightd[i]);
    }


    //Apply result.
    gr->decoded=ba;
    gr->decodedDamage=bad;
}
