#ifndef GATODECODERSQUEEZELR_H
#define GATODECODERSQUEEZELR_H

#include "gatorom.h"

class GatoDecoderSqueezeLR : public GatoDecoder
{
public:
    GatoDecoderSqueezeLR();
    QByteArray decode(GatoROM *gr);
};

#endif // GATODECODERSQUEEZELR_H
