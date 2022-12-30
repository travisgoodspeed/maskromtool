#ifndef ROMENCODERDIFF_H
#define ROMENCODERDIFF_H

#include "maskromtool.h"
#include "romdecoder.h"


class RomEncoderDiff : public RomEncoder
{
public:
    RomEncoderDiff();
    void readFile(MaskRomTool *m, QString filename);
};

#endif // ROMENCODERDIFF_H
