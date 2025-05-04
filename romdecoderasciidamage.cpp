#include "romdecoderasciidamage.h"

RomDecoderAsciiDamage::RomDecoderAsciiDamage() {}


//This returns a text preview.
QString RomDecoderAsciiDamage::preview(MaskRomTool *m){
    QString ascii="";
    RomBitItem* rowbit = m->markBitTable();


    //Prints all bits in the row.
    while(rowbit){
        RomBitItem* bit=rowbit;
        while(bit){
            ascii.append(bit->bitAmbiguous()?"1":"0");
            bit=bit->nexttoright;  //Skip down the row.
        }
        ascii.append("\n");
        rowbit=rowbit->nextrow;  //Skip to the next row.
    }

    return ascii;
}

//Exports the preview to a file.
void RomDecoderAsciiDamage::writeFile(MaskRomTool *m, QString filename){
    QFile fout(filename);
    fout.open(QIODevice::WriteOnly);
    fout.write(preview(m).toUtf8());
}
