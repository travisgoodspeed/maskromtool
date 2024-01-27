#include "romalignerreliable.h"
#include "maskromtool.h"
#include "romalignernew.h"

RomAlignerReliable::RomAlignerReliable() {
    name="RomAlignerReliable";
}

RomBitItem* RomAlignerReliable::markBitTable(MaskRomTool* mrt){
    QSet<RomBitItem *> bits=mrt->bits;

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

    // We'll need presorted collections by X and Y.
    for(RomBitItem *bit: bits){
        leftsorted<<bit;
        topsorted<<bit;
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

    for(RomBitItem *bit: rowstarts){
        while(bit->nexttoright) bit=bit->nexttoright;
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
    return rowstarts[0];
}
