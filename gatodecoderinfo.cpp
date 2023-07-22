#include "gatodecoderinfo.h"

#include <QDebug>

GatoDecoderInfo::GatoDecoderInfo()
{

}


QByteArray GatoDecoderInfo::decode(GatoROM *gr){
    QByteArray ba;  //No byte, just a dummy to keep the API happy.

    qDebug()<<"Input is"<<gr->inputrows<<"rows and"<<gr->inputcols<<"columns.";


    return ba;
}
