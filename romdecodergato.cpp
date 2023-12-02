#include "romdecodergato.h"
#include "maskromtool.h"
#include "romdecoderascii.h"
#include "gatorom.h"

RomDecoderGato::RomDecoderGato(){}

//Returns a GatoROM structure of the bits.
GatoROM RomDecoderGato::gatorom(MaskRomTool *mrt){
    this->mrt=mrt;

    RomDecoderAscii exporter;
    QString ascii=exporter.preview(mrt);
    RomBitItem* firstbit = mrt->markBitTable();

    //We'll need to reapply the angle after regenerating the matrix.
    int angle=mrt->gr.angle;

    //We rebuild from the preserved class
    mrt->gr.loadFromString(ascii);


    unsigned int row=0, col=0;
    RomBitItem* rowbit=firstbit;

    //Match them together.
    while(rowbit){
        RomBitItem* bit=rowbit;
        while(bit){
            /* Update the input bit to point to the RomBitItem*.
               This can be handy in drawing results back to the screen,
               but be careful that the pointers don't become stale!

               We can't set the address and mask yet because we don't know them.
             */
            mrt->gr.inputbits[row][col]->ptr=bit;

            bit=bit->nexttoright;  //Skip down the row.
            col++;
        }
        rowbit=rowbit->nextrow;  //Skip to the next row.
        col=0;
        row++;
    }

    row=0; col=0;
    rowbit=firstbit;

    //Set the bits.
    mrt->gr.decode();

    //Then loop again to set addresses and masks.
    while(rowbit){
        RomBitItem* bit=rowbit;
        while(bit){
            //Now we can set the address and mask.
            mrt->gr.inputbits[row][col]->ptr=bit;
            bit->adr=mrt->gr.inputbits[row][col]->adr;
            bit->mask=mrt->gr.inputbits[row][col]->mask;

            bit=bit->nexttoright;  //Skip down the row.
            col++;
        }
        rowbit=rowbit->nextrow;  //Skip to the next row.
        col=0;
        row++;
    }

    //Finally, reapply the preserved angle.
    mrt->gr.rotate(angle,true);

    return mrt->gr;
}


//This returns a text preview.
QString RomDecoderGato::preview(MaskRomTool *m){
    return "";
}

//Exports the preview to a file.
void RomDecoderGato::writeFile(MaskRomTool *m, QString filename){
    GatoROM gr=gatorom(m);
    QByteArray bytes=gr.decode();
    QFile fout(filename);
    fout.open(QIODevice::WriteOnly);
    fout.write(bytes);
}
