#ifndef ROMALIGNERTILTING_H
#define ROMALIGNERTILTING_H

/* This is a fork of RomAlignerReliable that also measures
 * the tilt of the lines and corrects for that tilt to better
 * support images with a gap in the middle.  It works by calculating
 * a "tilt" of the image on each pass, then using the Reliable algorithm
 * on the tilted bit positions.
 *
 * It isn't well tested yet, so if you do have need for it, consider
 * pre-rotating your image.  Gaps inside rows (between columns)
 * will trigger the problem with RomAlignerReliable, but
 * gaps inside columns (between rows) will not.
 *
 * https://github.com/travisgoodspeed/maskromtool/issues/120
 */

#include "romaligner.h"

#include<QTransform>

class RomAlignerTilting : public RomAligner
{
public:
    RomAlignerTilting();
    RomBitItem* markBitTable(MaskRomTool* mrt);
private:
    QList<RomBitItem *> rowstarts; //All left-most bits of a row.
    QList<RomBitItem *> leftsorted; //All bits sorted from left.

    //Nearest bit from existing rows.
    RomBitItem* nearestBit(RomBitItem *item);
    RomBitItem* linkresults();
    //QTransform transform;
    qreal angle();
    qreal lastangle=0;
    bool angleset=false;
};

#endif // ROMALIGNERTILTING_H
