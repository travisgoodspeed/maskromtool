#include "romdecoderarm6.h"

RomDecoderARM6::RomDecoderARM6()
{

}



void RomDecoderARM6::writeFile(MaskRomTool *m, QString filename){
    QFile fout(filename);
    fout.open(QIODevice::WriteOnly);
    fout.write(getbytes(m));
}
QByteArray RomDecoderARM6::getbytes(MaskRomTool *m){
    //Rows are in linear order, starting at the top.
    RomBitItem* rowbit = m->markBitTable();

    /* The ROM is designed to make 32-bit words rather than bytes,
     * so we'll have to do a little fiddling to get byte order.
     * For 32-bit word order, the most significant bytes are to the right
     * and the first bits are read from the right.
     *
     * Visually, we can see this because E is the most significant nybble
     * in most unconditional ARM instructions, and the rightmost three columns
     * strongly show a pattern of 1110 when read from right to left.
     */
    QByteArray output;

    //32-bit words of the current row.
    uint32_t rowwords[16];

    //Import all the rows into the byte arrays.
    RomBitItem* bit=rowbit;
    while(bit){
        //Zero the ephemerals.
        for(int i=0; i<16; i++)
            rowwords[i]=0;
        int wordi=0;

        //Process all bits on the row.
        while(bit){
            //Sample the bit into the word.
            rowwords[wordi]>>=1;
            rowwords[wordi]|=(bit->bitValue()?0x80000000:0);

            bit=bit->nexttoright;  //Skip down the row.
            wordi++;
            wordi%=16;
        }

        //Print the words in the right-to-left order.  (Opposite of the array.)
        for(int i=15; i>=0; i--){
            output.append((uint8_t) (rowwords[i]&0xFF));
            output.append((uint8_t) ((rowwords[i]>>8)&0xFF));
            output.append((uint8_t) ((rowwords[i]>>16)&0xFF));
            output.append((uint8_t) ((rowwords[i]>>24)&0xFF));
        }

        rowbit=rowbit->nextrow;
        bit=rowbit;
    }

    return output;
}


//This returns a text preview.
QString RomDecoderARM6::preview(MaskRomTool *m){
    QString ascii="";


    return ascii;
}

