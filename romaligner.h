#ifndef ROMALIGNER_H
#define ROMALIGNER_H

/* MaskRomTool first generates bit positions from rows and columns,
 * then aligns those into a linked list.  There are many good reasons
 * to align in different ways, so we abstract the aligner into a class.
 *
 * Eventually, we'd like to have multiple children of this class implementing
 * competing strategies, so that faster or more accurate strategies can be
 * tried out before replacing the default.
 */

#include "rombititem.h"

class RomAligner
{
public:
    RomAligner();

    //Marks the table of bits, returns top-left entry.
    virtual RomBitItem* markBitTable(QSet<RomBitItem*> &bits)=0;
};

class RomAlignerDefault : public RomAligner {
    RomBitItem* markBitTable(QSet<RomBitItem*> &bits);


    //Maybe make these private?
    QSet<RomBitItem*> bits;

    //Returns the next bit to the right, of all unmarked bits.
    RomBitItem* onetoright(RomBitItem* i);
    //Returns the top-left of all unmarked bits, but does not mark that bit.
    RomBitItem* startofnextrow();
};



#endif // ROMALIGNER_H
