#ifndef GATODECODERDOWNL_H
#define GATODECODERDOWNL_H

#include "gatorom.h"


class GatoDecoderColsDownL : public GatoDecoder
{
public:
    GatoDecoderColsDownL();
    QByteArray decode(GatoROM *gr);
};

#endif // GATODECODERLR_H
