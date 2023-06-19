#include "romruleambiguous.h"

#include "maskromtool.h"

RomRuleAmbiguous::RomRuleAmbiguous()
{

}


/* We trust the bit to tell us whether it's ambiguous.
 * Later on, this will need to account for damage and
 * forcing.
 */
void RomRuleAmbiguous::evaluate(MaskRomTool *mrt){
    //We walk them in order so errors are per row.
    RomBitItem* bit = mrt->markBitTable();  //Current bit in row.
    RomBitItem* bitfirst=bit;               //First bit in the current row.
    while(bit){
        if(bit->bitAmbiguous() && !bit->isFixed()){
            RomRuleViolation* violation=
                new RomRuleViolation(bit->pos(),
                                     QString("Ambiguous bit %1,%2").arg(bit->row).arg(bit->col),
                                     "This bit is close to the threshold.  Try fixing it with Shift+F?");
            violation->error=false;
            mrt->addViolation(violation);
        }
        if(bit->nexttoright)
            bit=bit->nexttoright;
        else
            bit=bitfirst=bitfirst->nextrow;

    }
}
