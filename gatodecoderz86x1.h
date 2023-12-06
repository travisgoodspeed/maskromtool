#ifndef GATODECODERZ86X1_H
#define GATODECODERZ86X1_H

#include "gatorom.h"

class GatoDecoderZ86x1 : public GatoDecoder
{
public:
    GatoDecoderZ86x1();
    QByteArray decode(GatoROM *gr);
};

#endif // GATODECODERZ86X1_H
