#include "romdecodermarc4.h"
#include <stdint.h>

#include <QFile>

/* A MARC4 ROM has 16-bit wide columns.  Each row of a column contains two bytes
 * interlaced, and there's usually a large sequence of C1,C1 or 1111000000000011.
 *
 * The column order is not generically known, and some chips such as the T44C080C
 * also include a manufacturer test region that is not a part of the normal memory
 * map.  For now, columns are dumped left to right and it's the user's responsibility
 * to correct the page layout.
 */

RomDecoderMarc4::RomDecoderMarc4()
{

}


#define MAXCOLS 32


QByteArray RomDecoderMarc4::getbytes(MaskRomTool *m){
    RomBitItem* rowbit = m->markBitTable();

    QByteArray cols[MAXCOLS];
    QByteArray output;


    //Import all the rows into the byte arrays.
    while(rowbit){
        int bitinrow=0;
        int column=0;

        RomBitItem* bit=rowbit;
        uint8_t bytea=0, byteb=0;
        while(bit){
            //Bits are inteleved, so we grab one of each.
            if((bitinrow&1)==0)
                bytea=(bytea<<1)|(bit->bitValue()^1);
            else
                byteb=(byteb<<1)|(bit->bitValue()^1);

            bit=bit->nexttoright;  //Skip down the row.
            bitinrow++;

            if(bitinrow%16==0){
                //Insert the two bytes into the column.
                cols[column].append(bytea);
                cols[column].append(byteb);
                bytea=byteb=0;

                column++;
                if(column>=MAXCOLS){
                    qDebug()<<"No MARC4 ROM should have"<<column<<" columns.  What's going on?";
                    column=0;
                }
            }
        }
        rowbit=rowbit->nextrow;  //Skip to the next row.
    }

    for(int i=0; i<MAXCOLS; i++){
        for(int j=0; j<cols[i].size(); j++){
            output.append(cols[i].at(j));
        }
    }

    return output;
}

//This returns a text preview.
QString RomDecoderMarc4::preview(MaskRomTool *m){
    QString ascii="";


    return ascii;
}


void RomDecoderMarc4::writeFile(MaskRomTool *m, QString filename){
    QFile fout(filename);
    fout.open(QIODevice::WriteOnly);
    fout.write(getbytes(m));
}
