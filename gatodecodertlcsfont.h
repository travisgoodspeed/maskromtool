#ifndef GATODECODERTLCSFONT_H
#define GATODECODERTLCSFONT_H

#include "gatorom.h"

class GatoDecoderTLCSFont : public GatoDecoder
{
public:
    GatoDecoderTLCSFont();
    void decode(GatoROM *gr);
};

#endif // GATODECODERTLCSFONT_H
