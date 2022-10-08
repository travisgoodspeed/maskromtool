#include <QProgressDialog>
#include <QtMath>

#include "romaligner.h"

RomAligner::RomAligner()
{

}


RomBitItem* RomAlignerDefault::markBitTable(QSet<RomBitItem*> &bits){
    long row=0, col=0;
    long bitsintable=0;
    RomBitItem* topleft=0;

    this->bits=bits;


    // Aligning bits can take a while, so we show a progress dialog.
    QProgressDialog progress("Aligning Bits", "Abort Alignment", 0, bits.size());
    progress.setWindowModality(Qt::WindowModal);



    //First we remove all the old bit marks and pointers.
    foreach (RomBitItem* bit, bits){
        //Clear the marks.
        bit->marked=false;
        bit->nextrow=0;
        bit->nexttoright=0;
        bit->row=-1;
        bit->col=-1;
    }

    //Then we begin at the topleft.
    topleft=startofnextrow();

    // For every row, mark out the whole row and update the links.
    RomBitItem* rowleft=topleft;
    while(rowleft){
        // Step through every bit within the row.
        RomBitItem* bit=rowleft;
        while(bit){
            //bit->setToolTip(QString("%1,%2").arg(row).arg(col)); //Handy for debugging confused alignment.
            bit->row=row;
            bit->col=col++;
            /* Please note, we're removing references from the local copy
               of the table, not from the GUI's copy of the table.
               This roughly doubles the performance, from a 42 second alignment
               down to 18.
             */
            this->bits.remove(bit);
            bit=onetoright(bit);
            bitsintable++;
        }

        rowleft->nextrow=startofnextrow();
        rowleft=rowleft->nextrow;

        //Update the progress bar.
        progress.setValue(bitsintable);
        if(progress.wasCanceled())
            return 0;
        col=0;
        row++;
    }
    return topleft;
}

//Convenience function to calculate the distance between two points.
static qreal distance(QPointF a, QPointF b){
    return qSqrt(
                qPow(b.x()-a.x(),2)
                +qPow(b.y()-a.y(),2)
                );
}




//Returns the next bit to the right that is not marked, or 0 if there is none.
RomBitItem* RomAlignerDefault::onetoright(RomBitItem* i){
    /* The bit we're looking for will be the one that is
     * 1. Not marked.
     * 2. Has a larger difference in X than in Y from i.
     * 3. Has a smaller X than any other candidate.
     * 4. Gotta be to the right of the last sample.
     * 5. Gotta be to the left of every other candidate sample.
     */


    /* The slope variable is infuriating, and probably needs to be user-configurable.
     * If this number is too low, then we'll lose track of the row and see
     * something like Row 33 in the middle of Row 0.  If it's too high, we'll
     * see a break when we sample the top of one bit and the bottom of the next bit.
     */
    qreal slope=10.0;

    //Mark the first bit.
    i->marked=true;
    RomBitItem* next=i;

    foreach (RomBitItem* bit, bits){
        if(!bit->marked  // 1. Bit is not marked.
                &&( qAbs(bit->pos().x()-i->pos().x())/slope > qAbs(bit->pos().y()-i->pos().y()) )  // 2. Much larger difference in X than in Y from i.
                &&( next==i || qAbs(bit->pos().x()-i->pos().x()) < qAbs(next->pos().x()-i->pos().x()) )  // 3. Smaller X difference than last candidate.
                &&( bit->pos().x() > i->pos().x()  )  // 4. Position is to the right of the input sample.
                &&( next==i || bit->pos().x()<next->pos().x() )  // 5. New position is to the left of the last best sample.
                ){
            next=bit;
        }
    }

    if(next==i)
        return 0; //No next item.

    //Update the input's pointer to point to the output.
    i->nexttoright=next;
    //And mark that bit, so that we know it has already been traversed.
    next->marked=true;
    return next;
}

//Returns the top-left of all unmarked bits, but does not mark that bit.
RomBitItem* RomAlignerDefault::startofnextrow(){
    RomBitItem* topleft=0;
    QPointF origin(0,0);

    //First we remove all the old bit marks and pointers.
    foreach (RomBitItem* bit, bits){
        if(!topleft || topleft->marked) //Gotta start somewhere.
            topleft=bit;

        //The latest bit is closer to the origin, so we declare it to be topleft.
        if(!bit->marked && distance(origin,bit->pos())<distance(origin,topleft->pos())){
            topleft=bit;
        }
    }
    //Return early if there are no bits.
    if(!topleft || topleft->marked)
        return 0;
    return topleft;
}
