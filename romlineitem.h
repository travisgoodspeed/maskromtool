#ifndef ROMLINEITEM_H
#define ROMLINEITEM_H

#include <QGraphicsLineItem>
#include <QJsonObject>

/* This class represents a line item in the display.
 */

class RomLineItem : public QGraphicsLineItem
{
public:
    RomLineItem(int linetype);

    //Type of the line.
    int linetype;
    static const int LINEROW=0;
    static const int LINECOL=1;

    //Dumps the state out to JSON.
    void write(QJsonObject &json);
    //Reads the state in from JSON.
    void read(const QJsonValue &json);

    //Returns the line in the image's coordinates.
    QLineF globalline();
};

#endif // ROMLINEITEM_H
