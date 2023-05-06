#include "romlineitem.h"

RomLineItem::RomLineItem(int linetype)
{
    //Remember my type.
    this->linetype=linetype;
}

QLineF RomLineItem::globalline(){
    QLineF linea=this->line();

    qreal x1, y1, x2, y2;
    x1=linea.x1()+this->x();
    y1=linea.y1()+this->y();
    x2=linea.x2()+this->x();
    y2=linea.y2()+this->y();

    return QLineF(x1, y1, x2, y2);
}


void RomLineItem::write(QJsonObject &json){
    QLineF linef=globalline();

    json["linetype"]=linetype;
    json["x1"]=linef.x1();
    json["y1"]=linef.y1();
    json["x2"]=linef.x2();
    json["y2"]=linef.y2();
}


void RomLineItem::read(const QJsonValue &json){
    qreal x1, y1, x2, y2;
    linetype=json["linetype"].toInt();
    x1=json["x1"].toDouble();
    y1=json["y1"].toDouble();
    x2=json["x2"].toDouble();
    y2=json["y2"].toDouble();

    setLine(0, 0, x2-x1, y2-y1);
    setPos(x1, y1);
}
