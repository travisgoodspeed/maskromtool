#ifndef ROMALIGNERNEW_H
#define ROMALIGNERNEW_H

#include "romaligner.h"

//Sorting functions, not part of a class.
bool leftOf(RomBitItem * left, RomBitItem * right);
bool above(RomBitItem * top, RomBitItem * bottom);

//This aligner is slow as dirt on large projects.  Must be replaced.
class RomAlignerNew : public RomAligner {
public:
    RomAlignerNew();
    RomBitItem* markBitTable(MaskRomTool* mrt);

private:
    QList<RomBitItem *> rowstarts; //All left-most bits of a row.
    QList<RomBitItem *> leftsorted; //All bits sorted from left.
    QList<RomBitItem *> topsorted; //All bits sorted from left.

    RomBitItem* linkresults();
    void markRowStarts();
    void markRemainingBits();

    /* This must be non-zero for tilted images where
     * one column overlaps another.  If you get errors
     * when this is zero, then the sorting algorithms are
     * likely ambiguous and returning the bits in an unsorted
     * order.
     */
    uint32_t threshold=5;  //Overwritten from MRT.
};

#endif // ROMALIGNERNEW_H
