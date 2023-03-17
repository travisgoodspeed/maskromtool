#include <QtMath>

#include "romalignernew.h"
#include "maskromtool.h"


//FIXME: These constants need to be adjustable.
#define SKIPCOUNTTHRESHOLD 10 //Number of far hops before row count ends.
#define SHORTHOPTHRESHOLD  50 //Max distance of a short hop.


//FIXME: We also need rotation matrix sorting, to avoid tilt bugs.
static bool leftOf(RomBitItem * left, RomBitItem * right){
    return (left->x() < right->x());
}
static bool above(RomBitItem * top, RomBitItem * bottom){
    return (top->y() < bottom->y());
}

//This one should be a lot faster faster.
//Remove this comment when we know it to be accurate.
RomBitItem* RomAlignerNew::markBitTable(MaskRomTool* mrt){
    QSet<RomBitItem *> bits=mrt->bits;

    //Return null if there are no bits to align.
    if(bits.count()==0)
        return 0;

    /* Some hopefully brief theory:
     * 1. We should mostly get rows in order if we slide down from the top.
     * 2. We should mostly get columns in order if we slide in from the left.
     * 3. "Mostly in order" means that we'll get the row, but its bits might be
     *    scrambled or overlap a little with the next rows.
     * 4. Diagonal tilt is what makes this hard.  Maybe a simple rotation matrix
     *    would fix things?
     */


    //First we remove all the old bit marks and pointers.
    foreach (RomBitItem* bit, bits){
        //Clear the marks.
        bit->marked=false;
        bit->nextrow=0;
        bit->nexttoright=0;
        bit->row=-1;
        bit->col=-1;
    }
    rowstarts.clear();
    leftsorted.clear();
    topsorted.clear();

    // We'll need presorted collections by X and Y.
    for(RomBitItem *bit: bits){
        leftsorted<<bit;
        topsorted<<bit;
    }

    //We read each row from the left.
    std::sort(leftsorted.begin(), leftsorted.end(), leftOf);
    //We use this to count the gap between rows.
    std::sort(topsorted.begin(), topsorted.end(), above);


    //Then we find the bits of the leftmost column.
    markRowStarts();
    //qDebug()<<"Counted"<<rowstarts.count()<<"rows.";

    //And we grow those bits outward to the right.
    markRemainingBits();

    //And finally we return a pointer to the topleft bit.
    return linkresults();
}

void RomAlignerNew::markRemainingBits(){
    /* This begins as a copy of the row starts, but it quickly
     * becomes a list of the right-most recognized bit in a row.
     * This begins sorted in Y.
     */
    int rowcount=rowstarts.count();
    RomBitItem* rowbits[rowcount];
    for(int i=0; i<rowstarts.count(); i++)
        rowbits[i]=rowstarts[i];

    /* Then we walk all of the bits from the left, assigning each
     * bit to the bin with the nearest Y coordinate.  X's are known
     * to be in order because of the sort.
     */
    long bitcount=0;
    for(RomBitItem *bit: leftsorted){
        if(bit->marked) //Skip bits we've already passed.
            continue;   //such as start bits.

        //Fixme: This really ought to be a binary search.
        qreal leastydist=10000;
        int leastyi=0;
        for(int i=0; i<rowcount; i++){
            qreal ydist=qFabs(rowbits[i]->y()-bit->y());
            if(ydist<leastydist){
                leastyi=i;
                leastydist=ydist;
            }
        }
        rowbits[leastyi]->nexttoright=bit;
        rowbits[leastyi]=bit;
        bit->marked=true;
        bitcount++;
    }
}

void RomAlignerNew::markRowStarts(){
    qreal largestgap=1;
    /* First we need to know the distance between bits.  This is rather
     * regular for bits that are evenly spaced, but many roms include
     * a larger jump between the left and right sides.  When oriented
     * off by 90 degrees, this gap confused older implementations of
     * this class.
     */
    qreal lasty=0;
    for(RomBitItem *bit: topsorted){
        qreal y=bit->y();
        if(lasty>1 && y-lasty>largestgap)
            largestgap=y-lasty;
        lasty=y;
    }
    //qDebug()<<"Largest gap is measured to be"<<largestgap;


    /* Here we create a sorted list of the starts of row start
     * positions.  This is done by sweeping in from the left
     * while ignoring large gaps in the Y position.
     */
    qreal shorthop;  //should be pretty small until it's not.
    qreal shorthopthreshold=largestgap*2; //Adjusted later.
    lasty=leftsorted[0]->y();
    int rowcount=0, skipcount=0;
    for(RomBitItem *bit: leftsorted){
        if(qFabs(bit->y()-lasty)<shorthopthreshold){ //Same column
            rowstarts<<bit;
            shorthop=qFabs(bit->y()-lasty);
            lasty=bit->y();
            //qDebug()<<"Bit at"<<bit->x()<<bit->y()<<"row"<<rowcount++;
            //bit->setToolTip(QString("Row header."));
            bit->marked=true; //Necessary so we don't double-count.
            skipcount=0;

            //We want the long hops to be anything more than ~5 bits.
            //qDebug()<<"Short hop of"<<shorthop;
            //if(shorthop>0 && shorthopthreshold>shorthop*5)
            //    shorthopthreshold=shorthop*5;
        }else{  //Different column.
            //qDebug()<<"Skipping hop"<<qFabs(bit->y()-lasty);
            skipcount++;
        }
        //One or two long hops are normal, but many indicate end of first col.
        if(skipcount>SKIPCOUNTTHRESHOLD)
            break;
    }
    std::sort(rowstarts.begin(), rowstarts.end(), above);

    //Apply the linked list.
    RomBitItem* lastbit=0;
    for(RomBitItem *bit: rowstarts){
        if(lastbit)
            lastbit->nextrow=bit;
        lastbit=bit;
    }
}

//This updates the linked lists that MRT uses internally.
RomBitItem* RomAlignerNew::linkresults(){
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
    return rowstarts[0];
}
