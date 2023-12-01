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

    //We rebuild from the preserved class
    mrt->gr.loadFromString(ascii);

    RomBitItem* rowbit = mrt->markBitTable();
    unsigned int row=0, col=0;
    //Prints all bits in the row.
    while(rowbit){
        RomBitItem* bit=rowbit;
        while(bit){
            /* Update the input bit to point to the RomBitItem*.
               This can be handy in drawing results back to the screen,
               but be careful that the pointers don't become stale!

               We also set the address and the mask for visualizing
               selections.
            */
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
