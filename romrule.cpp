#include<QBrush>

#include "romrule.h"

//All future violations will be this size, but past ones are not resized.
qreal RomRuleViolation::bitSize=10;

RomRuleViolation::RomRuleViolation(QPointF position, QString title, QString detail, bool iserror){
    QBrush yellowbrush(Qt::GlobalColor::yellow, Qt::SolidPattern);
    qreal size=bitSize+10;
    this->detail=detail;
    this->title=title;
    this->error=iserror;

    setVisible(true);
    setToolTip(this->title);
    setRect(-size/2, -size/2,
            size, size);
    this->setPos(position);
    setBrush(yellowbrush);
    setZValue(-10);
}



int RomRuleViolation::type() const{
    /* UserType+0 -- Row
     * UserType+1 -- Col
     * UserType+2 -- Bit
     * UserType+3 -- BitFix
     * UserType+4 -- RomRuleViolation
     */
    return QGraphicsItem::UserType+4;
}

