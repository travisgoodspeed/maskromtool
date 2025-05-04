#include "gatodecoderinfo.h"

#include <QDebug>

GatoDecoderInfo::GatoDecoderInfo()
{

}


void GatoDecoderInfo::decode(GatoROM *gr){
    /* All other decoders apply a result.  This one doesn't,
     * and I'm not sure whether or not that means this class
     * should be scrapped.
     */
    qDebug()<<"Input is"<<gr->inputrows<<"rows and"<<gr->inputcols<<"columns.";
}
