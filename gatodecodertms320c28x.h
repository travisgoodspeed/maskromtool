#ifndef GATODECODERTMS320C28X_H
#define GATODECODERTMS320C28X_H

#include "gatorom.h"

class GatoDecoderTMS320C28x : public GatoDecoder
{
public:
    GatoDecoderTMS320C28x();
    void decode(GatoROM *gr);
};

#endif // GATODECODERTMS320C28X_H
