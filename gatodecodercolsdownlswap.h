#ifndef GATODECODERCOLSDOWNLSWAP_H
#define GATODECODERCOLSDOWNLSWAP_H

#include "gatorom.h"

class GatoDecoderColsDownLSwap : public GatoDecoder
{
public:
    GatoDecoderColsDownLSwap();
    QByteArray decode(GatoROM *gr);
};

#endif // GATODECODERCOLSDOWNLSWAP_H
