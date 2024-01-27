#ifndef ROMALIGNERRELIABLE_H
#define ROMALIGNERRELIABLE_H

/* This is a replacement for RomAlignerNew.
 * It is not yet functional.
 *
 * This algorithm begins with bits sorted along the X axis,
 * supposing that each new bit must either fit into an existing
 * row or be the first bit of a new row.  New rows are created
 * whenever the Y distance is greater than the X distance from the most
 * recent bit of an existing row.
 */

#include "romaligner.h"

class RomAlignerReliable : public RomAligner
{
public:
    RomAlignerReliable();
    RomBitItem* markBitTable(MaskRomTool* mrt);
private:
    QList<RomBitItem *> rowstarts; //All left-most bits of a row.
    QList<RomBitItem *> leftsorted; //All bits sorted from left.
    QList<RomBitItem *> topsorted; //All bits sorted from left.

    //Nearest bit from existing rows.
    RomBitItem* nearestBit(RomBitItem *item);
    RomBitItem* linkresults();
};

#endif // ROMALIGNERRELIABLE_H
