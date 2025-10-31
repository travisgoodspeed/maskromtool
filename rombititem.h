#ifndef ROMBITITEM_H
#define ROMBITITEM_H

#include <QGraphicsRectItem>
#include <QJsonObject>

class RomBitFix;
class RomLineItem;
class MaskRomTool;

/* This is a graphical item that represents one bit of the ROM.
 * They are short lived, being killed off and regenerated as
 * the rows and columns are adjusted.  When the table is marked,
 * they might have linked lists as rows of bits.  The first
 * bit of each row also refers to the next row, but there are
 * no back references.
 *
 * The position of each bit is the same as the sample position,
 * but the rectangle has a negative starting position to mark the size
 * correctly.
 */

class RomBitItem : public QGraphicsRectItem
{
public:
    //Creates a bit at a crossing point.
    RomBitItem(QPointF pos, qreal size,
               RomLineItem *rlrow, RomLineItem *rlcol);

    //Sample the color of the bit.
    QRgb bitvalue_raw(MaskRomTool *mrt, QImage &bg);
    //What's the value of the bit?  Includes override and average color.
    bool bitvalue_sample(MaskRomTool *mrt, QImage &bg,
                         float red, float green, float blue,
                         float h, float s, float l);
    //What's the last value of the bit?  Does *not* resample.
    bool bitValue();
    //Is this bit too close to its threshold, or suspicious for some other reason?
    bool bitAmbiguous();

    //Applies a bit fix.
    void setFix(RomBitFix* fix);
    //Is the bit fixed?  Useful for DRC checks.
    bool isFixed();

    //Dumps the state out to JSON.
    void write(QJsonObject &json);
    //Reads the state in from JSON.
    void read(const QJsonValue &json);

    //Sets the bit size.
    void setBitSize(qreal size);
    //Gets the same size.  Used for DRC violations.
    qreal getBitSize();

    //Address and mask of the bit.
    uint32_t adr=0xFFFFFFFF, mask=0;

    bool marked=false;  //A marked bit is part of the bit table.
    RomBitItem *nexttoright=0; //Linked list of bits in a row.
    RomBitItem *lastinrow=0; //Shortcut to late in the row, used by RomAlignerReliable for performance.
    RomBitItem *nextrow=0; //Linked list of row starters.

    //Row and columnn are -1 until known by alignment.
    long row=-1, col=-1;
    RomLineItem *rlrow=0, *rlcol=0;

    //Allows for fast lookups.
    virtual int type() const override;

private:
    bool value=false;     //This is the value.
    bool ambiguous=false; //Do we suspect the value is wrong?
    bool fixed=false;     //Is this bit overridden?
    qreal bitSize=10;

    void setBrush();  //Adjust the color.
};

#endif // ROMBITITEM_H
