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

class MaskRomTool;

class RomAligner
{
public:
    RomAligner();
    QString name="RomAligner";

    //Marks the table of bits, returns top-left entry.
    virtual RomBitItem* markBitTable(MaskRomTool* mrt)=0;
};



#endif // ROMALIGNER_H
