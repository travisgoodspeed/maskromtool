#include "gatodecodermsp430.h"
#include <QDebug>

/* This decoder is a work in progress, and it probably doesn't work yet.
 */

GatoDecoderMSP430::GatoDecoderMSP430(){
    name="msp430";
}


QByteArray GatoDecoderMSP430::decode(GatoROM *gr){
    QByteArray ba;
    uint32_t adr=0;

    //Q_ASSERT(gr->outputcols==128);
    //qDebug()<<gr->outputcols<<"output columns";
    if(gr->outputcols!=128)
        return ba;

    for(unsigned int row=0; row<gr->outputrows; row++){
        //Each row contains some 16-bit words.
        //This order is probably wrong.
        //for(unsigned int word=7; word>0; word--){
        for(unsigned int word=0; word<8; word++){
            uint32_t w=0;
            //for(unsigned int bit=15; bit>0; bit--){
            for(unsigned int bit=0; bit<16; bit++){
                //qDebug()<<"row"<<row<<"word"<<word<<"bit"<<bit;
                GatoBit *gb=gr->outputbit(row,bit*8+word);
                if(!gb){
                    qDebug()<<"Bit"<<bit<<"of row"<<row<<"is null.";
                    exit(1);
                }

                //Mark the address and bit.
                gb->adr=adr+w/8;
                gb->mask=1<<(bit%8);

                int b=gb->getVal();
                w=(w<<1)|b;
            }
            ba.append(w&0xFF);
            adr++;
            ba.append((w>>8)&0xFF);
            adr++;
        }
    }

    return ba;
}
