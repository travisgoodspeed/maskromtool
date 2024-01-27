#include "maskromtool.h"
#include "rombitfix.h"

#include <QBrush>
#include <QDebug>

RomBitItem::RomBitItem(QPointF pos, qreal size){
    setPos(pos);
    setBitSize(size);
}

int RomBitItem::type() const{
    /* UserType+0 -- Row
     * UserType+1 -- Col
     * UserType+2 -- Bit
     * UserType+3 -- BitFix
     */
    return QGraphicsItem::UserType+2;
}


//Sets the bit size.
void RomBitItem::setBitSize(qreal s){
    bitSize=s;
    setRect(-s/2, -s/2, s, s);
}

qreal RomBitItem::getBitSize(){
    return bitSize;
}

QRgb RomBitItem::bitvalue_raw(MaskRomTool *mrt, QImage &bg){
    //Colors now come from an adjustable sampler.
    return mrt->sampler->bitvalue_raw(mrt, bg, pos());
}

void RomBitItem::setBrush(){
    static QBrush truebrush(Qt::GlobalColor::red, Qt::SolidPattern);
    static QBrush falsebrush(Qt::GlobalColor::blue, Qt::SolidPattern);
    QGraphicsRectItem::setBrush(value?truebrush:falsebrush);
}

bool RomBitItem::bitvalue_sample(MaskRomTool *mrt, QImage &bg, float red, float green, float blue){
    if(!fixed){
        //First we grab a fresh sample of the pixel.
        QRgb pixel=bitvalue_raw(mrt, bg);

        //Just the values.
        bool r, g, b;
        r=(red>((pixel>>16)&0xFF));
        g=(green>((pixel>>8)&0xFF));
        b=(blue>((pixel)&0xFF));
        //Value is true if any sample fits.
        value=r|g|b;

        //Is the value just on the threshold?
        int ambiguitythreshold=3;
        int R, G, B;
        ambiguous=false;
        R=(red-((pixel>>16)&0xFF));
        G=(green-((pixel>>8)&0xFF));
        B=(blue-((pixel)&0xFF));
        //Value is ambiguous is any color is off by more than threshold.
        if(value){  //Ambigously one.
            ambiguous |= (r && R<ambiguitythreshold);
            ambiguous |= (g && G<ambiguitythreshold);
            ambiguous |= (b && B<ambiguitythreshold);
        }else{      //Ambiguously zero.
            ambiguous |= (!r && R>-ambiguitythreshold);
            ambiguous |= (!g && G>-ambiguitythreshold);
            ambiguous |= (!b && B>-ambiguitythreshold);
        }

        //Invert the bit if it's wrong.
        if(mrt->inverted)
            value=!value;
    }
    setBrush();

    return value;
}

bool RomBitItem::bitValue(){
    return value;
}

bool RomBitItem::bitAmbiguous(){
    return ambiguous;
}

//Applies a bit fix.
void RomBitItem::setFix(RomBitFix* fix){
    /* For now, we don't keep a pointer to the fix,
     * because it might have been deleted.
     */
    fixed=true;
    value=fix->bitValue();
    ambiguous=fix->bitAmbiguous();
    setBrush();
}

bool RomBitItem::isFixed(){
    return fixed;
}

//Dumps the state out to JSON.
void RomBitItem::write(QJsonObject &json){
    json["x"]=pos().x();
    json["y"]=pos().y();
    json["value"]=value;
    json["ambiguous"]=ambiguous;
}
//Reads the state in from JSON.
void RomBitItem::read(const QJsonValue &json){
    qDebug()<<"Don't read a bit from JSON.  They should be regenerated from rows and columns.";
    exit(1);
}
