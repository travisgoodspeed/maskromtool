#ifndef GATODECODERINFO_H
#define GATODECODERINFO_H

#include "gatorom.h"


class GatoDecoderInfo : public GatoDecoder
{
public:
    GatoDecoderInfo();
    QString name="INFO";
    void decode(GatoROM *gr);
};

#endif // GATODECODERINFO_H
