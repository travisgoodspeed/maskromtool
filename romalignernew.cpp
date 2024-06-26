#include <QtMath>

#include "romalignernew.h"
#include "maskromtool.h"

/* This was the default aligner for MaskRomTool, but it has been deprecated in favor
 * of RomAlignerReliable.  It works from a leftmost sorting of the bits, first generating
 * the leftmost column and then constructing a linked list by sorting each new
 * bit into a row.
 *
 * This algorith has one serious complication:  When the columns are perfectly
 * vertical, the bits of the first column arrive in a random order, which can
 * confuse detection of when the column ends.  Similarly, too much tilt will
 * interleve the columns.  Most of this algorithm assumes a little tilt,
 * but it attempts to correct itself if things look very vertical.
 *
 * --Travis
 */

//Do we drift east or west as we go south?
static long int driftcount=0;

//FIXME: We also need rotation matrix sorting, to avoid tilt bugs.
bool leftOf(RomBitItem * left, RomBitItem * right){
    qreal a=left->x();
    qreal b=right->x();
    qreal distance=qFabs(left->x()-right->x());


    /* Here's the tricky part:  If the X coordinates are pretty much equal,
     * we don't know whether to tilt left or right, and guessing wrong
     * will break the alignment unless the column is *totally* vertical.
     */
    if(distance<0.01){
        /*
        if(verbose)
            qDebug()<<"WARNING: Bit distance "<<distance<<a<<b<<" leaning "
                     <<driftcount;
        */
        if(driftcount<=0)
            return left->y()>right->y();  //Use this one.
        else
            return left->y()<right->y(); //This one breaks my samples.
    }

    return (a<b);
}
bool above(RomBitItem * top, RomBitItem * bottom){
    return (top->y() < bottom->y());
}
/*
static bool below(RomBitItem * top, RomBitItem * bottom){
    return (top->y() > bottom->y());
}
*/

RomAlignerNew::RomAlignerNew(){
    name="RomAlignerNew";
}

//This one should be a lot faster.
//Remove this comment when we know it to be accurate.
RomBitItem* RomAlignerNew::markBitTable(MaskRomTool* mrt){
    mrt->getAlignSkipCountThreshold(this->threshold);
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

    //We read each row from the left.  Presorting doesn't help determinism.
    std::sort(leftsorted.begin(), leftsorted.end(), leftOf);
    //We use this to count the gap between rows.
    std::sort(topsorted.begin(), topsorted.end(), above);


    //Then we find the bits of the leftmost column.
    markRowStarts();
    if(verbose)
        qDebug()<<"Counted"<<rowstarts.count()<<"rows.";

    //If we have some starts,
    if(rowstarts.count()>0){
        //And we grow those bits outward to the right.
        markRemainingBits();

        //And finally we return a pointer to the topleft bit.
        return linkresults();
    }

    //Return null if we failed to make enough rows.
    return 0;
}

void RomAlignerNew::markRemainingBits(){
    /* This begins as a copy of the row starts, but it quickly
     * becomes a list of the right-most recognized bit in a row.
     * This begins sorted in Y.
     */
    int rowcount=rowstarts.count();
    RomBitItem** rowbits=new RomBitItem*[rowcount];
    for(int i=0; i<rowstarts.count(); i++)
        rowbits[i]=rowstarts[i];

    /* Then we walk all of the bits from the left, assigning each
     * bit to the bin with the nearest Y coordinate.  X's are known
     * to be in order because of the sort.
     */
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
    }

    delete[] rowbits;
}

void RomAlignerNew::markRowStarts(){
    //Smallest gap in the first column in X.
    qreal smallestxgap=1000;

    //Largest gap yet seen in Y.
    qreal largestgap=1;


    /* First we need to know the distance between bits.  This is rather
     * regular for bits that are evenly spaced, but many roms include
     * a larger jump between the left and right sides.  When oriented
     * off by 90 degrees, this gap confused older implementations of
     * this class.
     */
    qreal firstx=0, firsty=0;
    qreal lasty=0, lastx=0;
    qreal maxy=0, miny=0;
    for(RomBitItem *bit: topsorted){  //Finding gap distance.
        qreal y=bit->y();
        qreal x=bit->x();
        //Largest gap is only useful when there's a big jump between rows.
        if(lasty>1 && y-lasty>largestgap)
            largestgap=y-lasty;

        if(lasty>0 && qFabs(lastx-x)<smallestxgap)
            smallestxgap=qFabs(lastx-x);

        lasty=y;
        lastx=y;
        if(maxy<y) maxy=y;
        if(miny==0 || miny>y) miny=y;
    }
    qreal yspread=qFabs(maxy-miny);

    if(verbose)
        qDebug()<<"Smallest x gap was "<<smallestxgap;

    /* Getting this wrong will completely screw over the number
     * of columns in the result.  I tried some complicated algorithms
     * for it, but simply dividing the height by four seems to work for
     * many of my samples with tilt.
     */
    qreal shorthopthreshold=yspread/4;

    if(verbose)
        qDebug()<<"yspread is"<<yspread<<"and shorthopthreshold is"<<shorthopthreshold;

    /* Here we create a sorted list of the starts of row start
     * positions.  This is done by sweeping in from the left
     * while ignoring large gaps in the Y position.
     */
    //firsty=lasty=leftsorted[1]->y();
    firstx=lastx=leftsorted[1]->x();
    unsigned int rowcount=0, skipcount=0;
    for(RomBitItem *bit: leftsorted){
        if(
            (
            qFabs(bit->x()-lastx)<qFabs(bit->y()-lasty) //Bigger change in Y than X.
                &&
            qFabs(bit->y()-lasty)<shorthopthreshold  //Small Y change.
                )
            || (qFabs(bit->x()-firstx)<1)          //Exactly vertical in X.
            ){

            rowstarts<<bit;
            lasty=bit->y();
            lastx=bit->x();

            //if(verbose) qDebug()<<"Bit at"<<bit->x()<<bit->y()<<"row"<<rowcount++;
            bit->setToolTip(QString("Row header."));
            bit->marked=true; //Necessary so we don't double-count.
            skipcount=0;
        }else{  //Different column.
            if(verbose)
                qDebug()<<"Skipping hop"<<qFabs(bit->y()-lasty)<<"with threshold"<<shorthopthreshold
                         <<"and x distance"<<(qFabs(bit->x()-lastx));
            skipcount++;
        }
        //One or two long hops are normal, but many indicate end of first col.
        if(skipcount>threshold){
            if(verbose)
                qDebug()<<skipcount<<"is above skip count threshold of"<<threshold;
            break;
        }
    }

    /* Now we have the row headers in order, but we need to make sure
     * that we only have the first column.  Any extra entries will break
     * the sorting.
     */
    std::sort(rowstarts.begin(), rowstarts.end(), above);
    if(rowstarts.isEmpty()){
        qDebug()<<"Alignment list is empty!";
        return;
    }
    lastx=rowstarts[0]->x();
    driftcount=0; //Are we drifting west or east as we move south?
    for(RomBitItem *bit: rowstarts){
        if(bit->x()-lastx>0)
            driftcount++;
        if(bit->x()-lastx<0)
            driftcount--;

        lastx=bit->x();
    }

    if(verbose)
        qDebug()<<"Driftcount"<<driftcount;

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
        /* Alignment will fail.
        if(expectedcols && expectedcols!=col){
            qDebug()<<"WARNING: Column counts don't match.";
        }
        */
        row++;
        expectedcols=col;
    }
    return rowstarts[0];
}
