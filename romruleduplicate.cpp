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

        /* FIXME: The duplicate bit rule is slow as dirt because every bit is checked
         * against every other object in the view.  It would be much faster to sort everything
         * and then run through the list linearly.
         */

        foreach(QGraphicsItem* item, others){
            if(item->type()==bit->type()){  //Are we colliding with another bit?
                RomRuleViolation* violation=new RomRuleViolation(bit->pos(),
                                           "Overlapping Bits",
                                           "Two bits overlap here.");
                violation->error=true;
                mrt->addViolation(violation);
            }
        }
    }
}
