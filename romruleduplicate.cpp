#include "maskromtool.h"

#include "romrule.h"
#include "romruleduplicate.h"


RomRuleDuplicate::RomRuleDuplicate()
{

}


void RomRuleDuplicate::evaluate(MaskRomTool *mrt){
    foreach(RomBitItem* bit, mrt->bits){
        //List of everything that collids with this bit.  Lines are expected.
        QList<QGraphicsItem *> others=bit->collidingItems();
        foreach(QGraphicsItem* item, others){
            if(mrt->bits.contains((RomBitItem*) item)){
                RomRuleViolation* violation=new RomRuleViolation(bit->pos(),
                                           "Overlapping Bits",
                                           "Two bits overlap here.");
                violation->error=true;
                mrt->addViolation(violation);
            }
        }
    }
}
