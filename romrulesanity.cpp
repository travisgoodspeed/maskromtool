#include "romrulesanity.h"

#include "romlineitem.h"
#include "maskromtool.h"

RomRuleSanity::RomRuleSanity()
{

}

void RomRuleSanity::evaluate(MaskRomTool *mrt){
    foreach(RomLineItem* line, mrt->rows){
        testLine(mrt,line);
    }
    foreach(RomLineItem* line, mrt->cols){
        testLine(mrt,line);
    }
}

void RomRuleSanity::testLine(MaskRomTool *mrt, RomLineItem *line){
    //We just destroy zero-length lines, as they have no meaning.
    if(line->globalline().length()==0){
        mrt->removeItem(line);
        return; //We've killed it, so there's no more line to worry about.
    }

    //Very short lines.
    if(line->globalline().length()<2){
        RomRuleViolation* violation=new RomRuleViolation(line->pos(),
                                   "Tiny Line",
                                   "This line is very, very short.");
        violation->error=false;
        mrt->addViolation(violation);
        return; //No more violations on this line.
    }

    int angle=line->globalline().angle();
    angle%=180;
    if(line->linetype==RomLineItem::LINEROW){
        if( !(angle<45 || angle>180-45) ){
            qDebug()<<"Illegal angle."<<angle;
            RomRuleViolation* violation=new RomRuleViolation(line->pos(),
                                       "Suspicious Row Angle",
                                       "This row has an illegal angle.");
            violation->error=true;
            mrt->addViolation(violation);
        }
    }else if(line->linetype==RomLineItem::LINECOL){
        if( (angle<45 || angle>180-45) ){
            qDebug()<<"Illegal angle."<<angle;
            RomRuleViolation* violation=new RomRuleViolation(line->pos(),
                                       "Suspicious Column Angle",
                                       "This column has an illegal angle.");
            violation->error=true;
            mrt->addViolation(violation);
        }
    }
}
