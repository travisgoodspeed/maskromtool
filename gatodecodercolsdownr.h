#ifndef GATODECODERDOWNR_H
#define GATODECODERDOWNR_H

#include "gatorom.h"


class GatoDecoderColsDownR : public GatoDecoder
{
public:
    GatoDecoderColsDownR();
    void decode(GatoROM *gr);
};

#endif // GATODECODERLR_H
