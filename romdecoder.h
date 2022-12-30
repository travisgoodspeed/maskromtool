#ifndef ROMDECODER_H
#define ROMDECODER_H

#include<QString>
#include "maskromtool.h"

/* This class represents a ROM Decoder.  It has full access to the MRT GUI,
 * but usually runs from its own window or silently from the CLI.
 * See RomDecoderAscii for a bare bones example.
 */

class RomDecoder
{
public:
    //This returns a text preview.
    virtual QString preview(MaskRomTool *m)=0;
    virtual void writeFile(MaskRomTool *m, QString filename)=0;
};

/* A ROM Importer is the same thing, but in reverse.  It reads something
 * out of an external file, but might well be comparing it rather than importing
 * it.
 */

class RomEncoder
{
public:
    virtual void readFile(MaskRomTool *m, QString filename)=0;
};



#endif // ROMDECODER_H
