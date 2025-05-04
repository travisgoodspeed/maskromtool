#ifndef GATODECODERCOLSLEFT_H
#define GATODECODERCOLSLEFT_H

#include "gatorom.h"

class GatoDecoderColsLeft : public GatoDecoder
{
public:
    GatoDecoderColsLeft();
    void decode(GatoROM *gr);
};

#endif // GATODECODERCOLSLEFT_H
