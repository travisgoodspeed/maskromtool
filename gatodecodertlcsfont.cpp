#include "gatodecodertlcsfont.h"

#include <QDebug>

/* This is my decoder for the ROM of the TMP47C434N's font ROM,
 * which is much like down-left except that:
 * 1. Bytes are interleaved in each row.  (word, wordi)
 * 2. Every 64 bytes (8 rows), the rows reverse direction. (row, rowi).
 */

GatoDecoderTLCSFont::GatoDecoderTLCSFont(){
    name="tlcs47font";
}

QByteArray GatoDecoderTLCSFont::decode(GatoROM *gr){
    QByteArray ba;

    /* In this function, an *i* after the variable
     * indicates the locally transformed version.
     */

    //Bytes are interleaved in this order.
    //const int wordorder[]={0,2,4,6,1,3,5,7};
    int wordorder[1024];
    int colcount=(gr->outputcols/8);
    if(colcount>=sizeof(wordorder)) return ba;  //Fail when poor match.

    //Quickly produce an interleave table of words within the row.
    for(int i=0; i<colcount; i++){
        if(i&1)  //1, 3, 5, 7, etc
            wordorder[(i>>1)+colcount/2]=i;
        else     //0, 2, 4, 6, etc
            wordorder[i>>1]=i;
    }

    //We might be dynamic, but we still don't want to crash.
    if(gr->outputcols%8!=0) return ba;

    //Top to bottom
    uint32_t adr=0;
    for(unsigned int row=0; row<gr->outputrows; row++){
        int rowi=row&~0x7;

        /* Every 64 bytes or 8 rows, the rows reverse direction.
         * That's exact in the font ROM, but might be different
         * in program ROMs, which are larger.
         */
        if(row&0x8)
            rowi|=7-(row&7);
        else
            rowi=row;

        for(int word=(gr->outputcols/8)-1; word>=0; word--){
            uint8_t w=0;
            Q_ASSERT(word<sizeof(wordorder));
            int wordi=wordorder[word];  //Interleave the bytes.
            for (int bit = 0; bit < 8; bit++) {
                GatoBit *gatobit=gr->outputbits[rowi][bit*8+wordi];
                if(!gatobit)   //Sizes don't line up.
                    return QByteArray();
                gatobit->adr=adr;     //Mark the address.
                gatobit->mask=1<<bit; //Mark the bit.

                if(gatobit->getVal())
                    w|=gatobit->mask;
            }
            ba.append(w&0xFF);
            adr++;
        }
    }

    return ba;
}

