#ifndef GATODECODERDOWNR_H
#define GATODECODERDOWNR_H

#include "gatorom.h"


class GatoDecoderColsDownR : public GatoDecoder
{
public:
    GatoDecoderColsDownR();
    QByteArray decode(GatoROM *gr);
};

#endif // GATODECODERLR_H
