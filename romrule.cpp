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
