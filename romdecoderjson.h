#ifndef ROMDECODERJSON_H
#define ROMDECODERJSON_H

#include "romdecoder.h"

#include <QJsonArray>
#include <QJsonDocument>

/* This quick little decoder just dumps the bit positions as
 * JSON, for easy import by other tools.
 *
 * I should probably disambiguate it from the regular load and save.
 */

class RomDecoderJson : public RomDecoder
{
public:
    RomDecoderJson();
    virtual QString preview(MaskRomTool *m) override;
    virtual void writeFile(MaskRomTool *m, QString filename) override;

private:
    QJsonDocument document(MaskRomTool *m);
};

#endif // ROMDECODERJSON_H
