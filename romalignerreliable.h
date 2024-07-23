#ifndef ROMALIGNERRELIABLE_H
#define ROMALIGNERRELIABLE_H

/* This algorithm begins with bits sorted along the X axis,
 * supposing that each new bit must either fit into an existing
 * row or be the first bit of a new row.  New rows are created
 * whenever the Y distance is greater than the X distance from the most
 * recent bit of an existing row.
 *
 * There is no assumption that rows will grow at the same rate, so
 * this algorithm is very tolerant of both tilted images and perfectly
 * vertical images.  Tilted images trick other algorithms because bits
 * of a few columns arrive before the leftmost column is complete,
 * and pefectly vertical columns confuse other algorithms because
 * they sort to a random order.
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

    //Nearest bit from existing rows.
    RomBitItem* nearestBit(RomBitItem *item);
    RomBitItem* linkresults();
};

#endif // ROMALIGNERRELIABLE_H
