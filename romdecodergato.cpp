#include "romdecodergato.h"
#include "maskromtool.h"
#include "romdecoderascii.h"
#include "gatorom.h"

RomDecoderGato::RomDecoderGato(){}

//Returns a GatoROM structure of the bits.
GatoROM RomDecoderGato::gatorom(MaskRomTool *mrt){
    this->mrt=mrt;

    //FIXME: This works well enough to print, but doesn't return links to MRT objects.
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
            */
            mrt->gr.inputbits[row][col]->ptr=bit;
            bit=bit->nexttoright;  //Skip down the row.
            col++;
        }
        ascii.append("\n");
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
    QFile fout(filename);
    fout.open(QIODevice::WriteOnly);
    fout.write(gatorom(m).decode());
}
