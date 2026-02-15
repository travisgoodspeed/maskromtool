#include "romrulecount.h"
#include "maskromtool.h"

RomRuleCount::RomRuleCount()
{

}

void RomRuleCount::evaluate(MaskRomTool *mrt){
    RomBitItem* rowbit = mrt->markBitTable();
    int firstlinecount=0;
    int nextlinecount=0;
    int rowi=0;

    //Prints allbits in the row.
    while(rowbit){
        nextlinecount=0;
        RomBitItem* bit=rowbit;
        while(bit){
            bit=bit->nexttoright;  //Skip down the row.
            nextlinecount++;
        }
        if(!firstlinecount)
            firstlinecount=nextlinecount;

        if(firstlinecount!=nextlinecount){
            QString title("Unexpected Count on Row %1");
            QString detail("Row %1 has %2 bits, not %3");

            RomRuleViolation* violation=new RomRuleViolation(rowbit->pos(),
                                       title.arg(rowi),
                                       detail.arg(rowi).arg(firstlinecount).arg(nextlinecount));
            violation->error=true;
            mrt->addViolation(violation);
        }

        rowi++;
        rowbit=rowbit->nextrow;  //Skip to the next row.
    }
}
