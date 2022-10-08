#include<QDebug>
#include<QFile>

#include "romdecoderascii.h"

RomDecoderAscii::RomDecoderAscii()
{
    //qDebug()<<"Launching ROM Decoder to ASCII.";
}


//This returns a text preview.
QString RomDecoderAscii::preview(MaskRomTool *m){
    QString ascii="";
    RomBitItem* rowbit = m->markBitTable();


    //Prints allbits in the row.
    while(rowbit){
        RomBitItem* bit=rowbit;
        while(bit){
            ascii.append(bit->bitValue()?"1":"0");
            bit=bit->nexttoright;  //Skip down the row.
        }
        ascii.append("\n");
        rowbit=rowbit->nextrow;  //Skip to the next row.
    }

    return ascii;
}


void RomDecoderAscii::writeFile(MaskRomTool *m, QString filename){
    QFile fout(filename);
    fout.open(QIODevice::WriteOnly);
    fout.write(preview(m).toUtf8());
}
