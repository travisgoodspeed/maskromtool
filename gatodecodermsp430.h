#ifndef GATODECODERMSP430_H
#define GATODECODERMSP430_H

#include "gatorom.h"


class GatoDecoderMSP430 : public GatoDecoder
{
public:
    GatoDecoderMSP430();
    QByteArray decode(GatoROM *gr);
};

#endif // GATODECODERMSP430_H
