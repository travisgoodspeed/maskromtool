#ifndef GATODECODERCOLSRIGHT_H
#define GATODECODERCOLSRIGHT_H

#include "gatorom.h"

class GatoDecoderColsRight : public GatoDecoder
{
public:
    GatoDecoderColsRight();
    QByteArray decode(GatoROM *gr);
};

#endif // GATODECODERCOLSLEFT_H
