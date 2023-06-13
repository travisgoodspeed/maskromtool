#include <QBrush>

#include "rombititem.h"
#include "rombitfix.h"


RomBitFix::RomBitFix(RomBitItem* bit){

    //We take a null bit only for JSON imports.
    if(bit){ //Real fix.
        setPos(bit->pos());
        value=bit->bitValue();
        setBitSize(bit->getBitSize());
    }else{
        //We have to start somewhere.
        setBitSize(10);
    }
    updateColor();
    setZValue(-5);
}

//Sets the color to indicate ambiguity.
void RomBitFix::updateColor(){
    //Green if fixed, Cyan if ambiguous.
    static QBrush fixbrush(Qt::GlobalColor::green, Qt::SolidPattern);
    static QBrush ambigbrush(Qt::GlobalColor::cyan, Qt::SolidPattern);
    setBrush(ambiguous?ambigbrush:fixbrush);
}


int RomBitFix::type() const{
    /* UserType+0 -- Row
     * UserType+1 -- Col
     * UserType+2 -- Bit
     * UserType+3 -- BitFix
     * UserType+4 -- RomRuleViolation
     */
    return QGraphicsItem::UserType+3;
}


//Takes the bit's size as input.
void RomBitFix::setBitSize(qreal size){
    //Fixes are a little bigger than bits.
    qreal s=size+6;
    setRect(-s/2, -s/2, s, s);
}

//What value do we force the bit to?
bool RomBitFix::bitValue(){
    return value;
}

//Is the bit damaged?
bool RomBitFix::bitAmbiguous(){
    return ambiguous;
}

//Configuration.
void RomBitFix::setValue(bool value){
    this->value=value;
}
void RomBitFix::setAmbiguious(bool ambiguous){
    this->ambiguous=ambiguous;
    updateColor();
}

//Dumps the state out to JSON.
void RomBitFix::write(QJsonObject &json){
    json["x"]=pos().x();
    json["y"]=pos().y();
    json["value"]=value;
    json["ambiguous"]=ambiguous;
}
//Reads the state in from JSON.
void RomBitFix::read(const QJsonValue &json){
    qreal x, y;
    x=json["x"].toDouble();
    y=json["y"].toDouble();
    value=json["value"].toBool();
    ambiguous=json["ambiguous"].toBool();
    setPos(x, y);
    updateColor();
}
