#ifndef GATODECODERMARC4_H
#define GATODECODERMARC4_H

#include "affinedecoder.h"
#include "gatorom.h"

class GatoDecoderMarc4 : public GatoDecoder {
public:
  GatoDecoderMarc4();
  void decode(GatoROM *gr);
};

#endif // GATODECODERMARC4_H
