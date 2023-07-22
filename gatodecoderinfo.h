#ifndef GATODECODERINFO_H
#define GATODECODERINFO_H

#include "gatorom.h"


class GatoDecoderInfo : public GatoDecoder
{
public:
    GatoDecoderInfo();
    QString name="INFO";
    QByteArray decode(GatoROM *gr);
};

#endif // GATODECODERINFO_H
