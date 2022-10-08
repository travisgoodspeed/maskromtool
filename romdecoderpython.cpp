#include<QFile>
#include "romdecoderpython.h"

RomDecoderPython::RomDecoderPython()
{

}

//This returns a text preview.
QString RomDecoderPython::preview(MaskRomTool *m){
    QString python="bits=[\n";
    RomBitItem* rowbit = m->markBitTable();


    //Prints allbits in the row.
    while(rowbit){
        RomBitItem* bit=rowbit;
        python.append("  [");
        while(bit){
            python.append(bit->bitValue()?"1,":"0,");
            bit=bit->nexttoright;  //Skip down the row.
        }
        python.append("],\n");
        rowbit=rowbit->nextrow;  //Skip to the next row.
    }

    python.append("]\n");
    return python;
}


void RomDecoderPython::writeFile(MaskRomTool *m, QString filename){
    QFile fout(filename);
    fout.open(QIODevice::WriteOnly);
    fout.write(preview(m).toUtf8());
}
