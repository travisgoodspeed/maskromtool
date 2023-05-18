#ifndef ROMBITFIX_H
#define ROMBITFIX_H

#include <QGraphicsRectItem>
#include <QJsonObject>

class RomBitItem;

/* However carefully we photograph the ROM, some bits might be
 * ambiguous or damaged.  Rather than try to figure this out after
 * export, we can use these bit fixes to indicate the correct
 * value of a bit when known.
 *
 * All bit fixes force the value of the overlapping bit.  If
 * the fix is ambiguous, it means that the operator does not
 * know what the bit ought to be, so the fixed value might
 * also be wrong.
 */

class RomBitFix : public QGraphicsRectItem
{
public:
    RomBitFix(RomBitItem* bit);

    //What's the last value of the bit?  Does *not* resample.
    bool bitValue();
    bool bitAmbiguous();

    //Configuration.
    void setValue(bool value);
    void setAmbiguious(bool ambiguous);

    //Dumps the state out to JSON.
    void write(QJsonObject &json);
    //Reads the state in from JSON.
    void read(const QJsonValue &json);

    //Sets the bit size.
    void setBitSize(qreal size);

    //Allows for fast lookups.
    virtual int type() const override;
private:
    bool value=false;
    bool ambiguous=false;
};

#endif // ROMBITFIX_H
