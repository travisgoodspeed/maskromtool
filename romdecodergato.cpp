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

    //Reapply the preserved angle before processing.
    mrt->gr.rotate(angle,true);

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
            GatoBit* gb=mrt->gr.inputbit(row,col);
            if(!gb){
                qDebug()<<"Bailing early from RomDecoderGato::gatorom() as alignment is bad.";
                return mrt->gr;
            }
            gb->ptr=bit;
            gb->ambiguous=bit->bitAmbiguous();

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
            mrt->gr.inputbit(row,col)->ptr=bit;
            bit->adr=mrt->gr.inputbit(row,col)->adr;
            bit->mask=mrt->gr.inputbit(row,col)->mask;

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

//Exports the binary to a file.
void RomDecoderGato::writeFile(MaskRomTool *m, QString filename){
    GatoROM gr=gatorom(m);
    QByteArray bytes=gr.decode();
    QFile fout(filename);
    fout.open(QIODevice::WriteOnly);
    fout.write(bytes);
}


//Export the error bitmask to a file.
void RomDecoderGato::writeDamageFile(MaskRomTool *m, QString filename){
    GatoROM gr=gatorom(m);
    QFile fout(filename);

    gr.decode();
    fout.open(QIODevice::WriteOnly);
    fout.write(gr.decodedDamage);
}
