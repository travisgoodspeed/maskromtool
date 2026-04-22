#include "romdecodercsv.h"

RomDecoderCSV::RomDecoderCSV()
{

}


//This returns a text preview.
QString RomDecoderCSV::preview(MaskRomTool *m){
    QString ascii="";
    RomBitItem* rowbit = m->markBitTable();

    //Prints all bits in the row.
    while(rowbit){
        RomBitItem* bit=rowbit;
        while(bit){
            ascii.append(bit->bitValue()?"1":"0");
            bit=bit->nexttoright;  //Skip down the row.
            if(bit)
                ascii.append(",");
        }
        ascii.append("\n");
        rowbit=rowbit->nextrow;  //Skip to the next row.
    }

    return ascii;
}


void RomDecoderCSV::writeFile(MaskRomTool *m, QString filename){
    QFile fout(filename);
    if(!fout.open(QIODevice::WriteOnly)){
        qDebug()<<"Unable to open"<<filename;
        return;
    }
    fout.write(preview(m).toUtf8());
    fout.close();
}
