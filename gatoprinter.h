#ifndef GATOPRINTER_H
#define GATOPRINTER_H

class GatoROM;
class QPrinter;
class QPainter;

#include <QRectF>

class GatoPrinter
{
public:
    GatoPrinter(GatoROM *gr);
    void print(QPrinter &printer);
private:
    GatoROM *gr;

    QRectF paperrect;   //Portion of paper we can draw to.
    qreal xstep, ystep; //Step size of the bits.

    QPointF bitposition(int row, int col);
    void drawbit(QPainter &painter, int row, int col, bool value);
};

#endif // GATOPRINTER_H
