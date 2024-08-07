#include "romalignerreliable.h"
#include "maskromtool.h"

/* These simple sorting functions were much more compliated
 * in lesser algorithms.  It's nice to have them be simple again.
 */
static bool leftOf(RomBitItem * left, RomBitItem * right){
    return (left->x() < right->x());
}
static bool above(RomBitItem * top, RomBitItem * bottom){
    return (top->y() < bottom->y());
}

RomAlignerReliable::RomAlignerReliable() {
    name="RomAlignerReliable";
}

RomBitItem* RomAlignerReliable::markBitTable(MaskRomTool* mrt){
    QList<RomBitItem *> bits=mrt->bits;

    //First we remove all the old bit marks and pointers.
    foreach (RomBitItem* bit, bits){
        //Clear the markup details.
        bit->marked=false;  //Not used by this aligner.
        bit->nextrow=0;
        bit->nexttoright=0;
        bit->lastinrow=0;   //Speeds up linked list usage.
        bit->row=-1;
        bit->col=-1;
    }
    rowstarts.clear();
    leftsorted.clear();

    // We'll need presorted collections by X and Y.
    for(RomBitItem *bit: bits){
        leftsorted<<bit;
    }
    //We read each row from the left.  Presorting doesn't help determinism.
    std::sort(leftsorted.begin(), leftsorted.end(), leftOf);

    //Each bit is either its own row or the next in an existing row.
    for(RomBitItem *bit: leftsorted){
        RomBitItem* nearest=nearestBit(bit);
        if(!nearest){  //First bit is its own row.
            rowstarts<<bit;
        }else{
            qreal dx=qFabs(nearest->x()-bit->x());
            qreal dy=qFabs(nearest->y()-bit->y());
            if(dx<dy){  //New row because Y distance is greater.
                rowstarts<<bit;
            }else{       //Existing row because X distance is greater.
                nearest->nexttoright=bit;
            }
        }
    }

    return linkresults();
}


//Nearest bit from existing rows.
RomBitItem* RomAlignerReliable::nearestBit(RomBitItem *item){
    /* This returns the best match, but it does not guarantee that it
     * is a good match.  If the best match is further in Y than in
     * X, you should start a new row.
     */

    RomBitItem* nearest=0;
    qreal nearestdy=1000.0;

    for(RomBitItem* bit: rowstarts){
        /* Rather than chase the entire linked list, we try to keep the frist
         * bit of each row pointing to either the end or some item near the end.
         * This drops one test case from 1m24s to 6s.
         */
        RomBitItem* startbit=bit;
        if(bit->lastinrow)
            bit=bit->lastinrow;
        while(bit->nexttoright) bit=bit->nexttoright;
        startbit->lastinrow=bit;

        qreal dy=qFabs(item->y()-bit->y());
        if(dy<nearestdy){
            nearestdy=dy;
            nearest=bit;
        }
    }

    return nearest;
}


//This updates the linked lists that MRT uses internally.
RomBitItem* RomAlignerReliable::linkresults(){
    //Having assembled rows, we now need to sort them and return the top left.
    std::sort(rowstarts.begin(), rowstarts.end(), above);

    //Apply the linked list.
    RomBitItem* lastbit=0;
    for(RomBitItem *bit: rowstarts){
        if(lastbit)
            lastbit->nextrow=bit;
        lastbit=bit;
    }

    //Number the bits by row and column.
    int expectedcols=0;
    int row=0;
    for(RomBitItem* bit: rowstarts){
        int col=0;
        while(bit){
            bit->col=col;
            bit->row=row;
            bit=bit->nexttoright;
            col++;
        }
        row++;
    }
    if(row)
        return rowstarts[0];

    return 0;
}
