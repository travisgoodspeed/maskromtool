#include "romalignertilting.h"
#include "maskromtool.h"


/* All comparisons and sorts are performed on tilted bit positions.
 * This tilt is based upon the angle of the top row in the previous
 * alignment, and that angle is not saved between runs of the program.
 *
 * Aside from the QTransform mapping, this is the same as RomAlignerReliable.
 */
static QTransform transform;
static bool leftOf(RomBitItem * left, RomBitItem * right){
    auto l=transform.map(left->pos());
    auto r=transform.map(right->pos());
    return l.x() < r.x();
}
static bool above(RomBitItem * top, RomBitItem * bottom){
    auto t=transform.map(top->pos());
    auto b=transform.map(bottom->pos());
    return t.y() < b.y();
}

RomAlignerTilting::RomAlignerTilting() {
    name="RomAlignerTilting";
    transform=QTransform();
}

qreal RomAlignerTilting::angle(){
    RomBitItem *first, *last;

    //Fail out with zero degrees.
    if(rowstarts.empty())
        return 0.0;

    first=rowstarts[0];
    last=first;
    while(last->nexttoright)
        last=last->nexttoright;

    QPointF left=first->pos(), right=last->pos();
    QLineF line(left, right);

    lastangle=line.angle();
    transform=QTransform().rotate(lastangle);
    angleset=true;

    /* How well did the transform work?
    auto l=transform.map(left), r=transform.map(right);
    auto newangle=QLineF(l,r).angle();
    qDebug()<<"Post transform, angle is "<<newangle; //Very small, we hope.
    */

    return lastangle;
}


RomBitItem* RomAlignerTilting::markBitTable(MaskRomTool* mrt){
    QList<RomBitItem *> bits=mrt->bits;

    //The very first time we align this, we continuously update the transform.
    bool firstrun=!angleset;

    //We recalculate the transformation whenever this grows by 16.
    int bitcount=0;

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
            auto nearestpos=transform.map(nearest->pos());
            auto bitpos=transform.map(bit->pos());
            qreal dx=qFabs(nearestpos.x()-bitpos.x());
            qreal dy=qFabs(nearestpos.y()-bitpos.y());
            if(dx<dy){  //New row because Y distance is greater.
                rowstarts<<bit;
            }else{       //Existing row because X distance is greater.
                nearest->nexttoright=bit;
                if(firstrun) angle();
            }
        }
    }

    return linkresults();
}


//Nearest bit from existing rows.
RomBitItem* RomAlignerTilting::nearestBit(RomBitItem *item){
    /* This returns the best match, but it does not guarantee that it
     * is a good match.  If the best match is further in Y than in
     * X, you should start a new row.
     */

    RomBitItem* nearest=0;
    qreal nearestdy=1000.0;
    auto itempos=transform.map(item->pos());

    for(RomBitItem* bit: rowstarts){
        /* Rather than chase the entire linked list, we try to keep the frist
         * bit of each row pointing to either the end or some item near the end.
         *
         * We compare the mapped positions, not the raw positions.
         */
        RomBitItem* startbit=bit;
        if(bit->lastinrow)
            bit=bit->lastinrow;
        while(bit->nexttoright) bit=bit->nexttoright;
        startbit->lastinrow=bit;

        auto bitpos=transform.map(bit->pos());
        qreal dy=qFabs(itempos.y()-bitpos.y());
        if(dy<nearestdy){
            nearestdy=dy;
            nearest=bit;
        }
    }

    return nearest;
}


//This updates the linked lists that MRT uses internally.
RomBitItem* RomAlignerTilting::linkresults(){
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

    //Do I want to correct by a tilt or by a shear?
    if(verbose)
        qDebug()<<"Tilt angle of the first row after alignment is "<<angle();

    if(row)
        return rowstarts[0];

    return 0;
}
