#include "gatodecoderz86x1.h"

#include <QDebug>

/* This is a decoder for the ROM of the Zilog Z8 Z8601 and Z8611,
 * which is complex in several ways:
 * Each data bit is in a separate 'meta-column', 32 bits wide.
 * The bits within each column are in a complicated order, left to right.
 * The bits in the D0-D3 meta-columns are mirrored left to right vs D4-D7.
 * The rows are not in linear order, but top to bottom 1, 0, 2, 3, 5, 4, 6, 7, etc.
 * The top two rows are the 64-byte factory test ROM. This effectively means the
 *  actual row numbers top to bottom are T1 T0 0 1 3 2 4 5 7 6 ... up to 3f or 7f.
 */

GatoDecoderZ86x1::GatoDecoderZ86x1(){
    name="z86x1";
}

    /* Bitswap Macros from Aaron Giles, licensed 3BSD */
#define BIT(x,n) (((x)>>(n))&1)

#define BITSWAP8(val,B7,B6,B5,B4,B3,B2,B1,B0) \
        ((BIT(val, B7) <<  7) | \
         (BIT(val, B6) <<  6) | \
         (BIT(val, B5) <<  5) | \
         (BIT(val, B4) <<  4) | \
         (BIT(val, B3) <<  3) | \
         (BIT(val, B2) <<  2) | \
         (BIT(val, B1) <<  1) | \
         (BIT(val, B0) <<  0))

QByteArray GatoDecoderZ86x1::decode(GatoROM *gr){
    QByteArray ba;
    /* In this function, an *i* after the variable
     * indicates the locally transformed version.
     */

    //Bytes are interleaved in this order:
    //
    int wordorder[32];
    int colcount=(gr->outputcols/8);
    if(colcount>=sizeof(wordorder)) return ba;  //Fail when poor match.

    //Quickly produce an interleave table of words within the row.
    for(int i=0; i<colcount; i++){
        wordorder[BITSWAP8((i^0x1e), 7, 6, 5, 0, 1, 2, 3, 4)]=i;
    }

    //We might be dynamic, but we still don't want to crash.
    //if(gr->outputcols%8!=0) return ba;
    //if(gr->outputrows%8!=0) return ba;

    //Strictly check the size.  FIXME: Make this more generic.
    if((gr->outputrows!=2+64 && gr->outputrows!=2+128) || gr->outputcols!=32*8)
        return ba;

    //Top to bottom
    uint32_t adr=0;
    for(unsigned int row=0; row<gr->outputrows; row++){
        int rowi=row;

        /* Every 4 rows go in the order 1, 0, 2, 3 */
        if(!(row&2))
            rowi^=1;

        for(int word=0; word<=(gr->outputcols/8)-1; word++){
            uint8_t w=0;
            Q_ASSERT(word<sizeof(wordorder));
            for (int bit = 0; bit < 4; bit++) {
                int wordi=wordorder[word];  //Interleave the bytes.
                int coli=bit*32+wordi;
                GatoBit *gatobit=gr->outputbit(rowi,coli);

                if(!gatobit)   //Sizes don't line up.
                    return QByteArray();
                gatobit->adr=adr;     //Mark the address.
                gatobit->mask=1<<bit; //Mark the bit.

                if(gatobit->getVal())
                    w|=gatobit->mask;
            }
            for (int bit = 4; bit < 8; bit++) {
                int wordi=wordorder[((gr->outputcols/8)-1)-word];  //Interleave the bytes.
                int coli=bit*32+wordi;
                GatoBit *gatobit=gr->outputbit(rowi,coli);

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

