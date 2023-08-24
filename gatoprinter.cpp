#include <QPrinter>
#include <QPainter>


#include "gatoprinter.h"
#include "gatorom.h"

GatoPrinter::GatoPrinter(GatoROM *gr){
    this->gr=gr;
}

void GatoPrinter::print(QPrinter &printer){
    paperrect =
        printer.pageLayout().fullRectPixels(printer.resolution());

    //Add a little border.
    paperrect.setX(paperrect.x()+5);
    paperrect.setY(paperrect.y()+5);
    paperrect.setWidth(paperrect.width()-10);
    paperrect.setHeight(paperrect.height()-10);


    //Leave a little extra space for dividers.
    int colcount=gr->outputcols+(gr->outputcols/8);
    int rowcount=gr->outputrows+(gr->outputrows/8);
    xstep=paperrect.width()/colcount;
    ystep=paperrect.height()/rowcount;


    QPainter painter;
    painter.begin(&printer);

    //Draw the bits.
    for(unsigned int row=0; row<gr->outputrows; row++)
        for(unsigned int col=0; col<gr->outputcols; col++)
            drawbit(painter,
                    row, col,
                    gr->outputbits[row][col]->getVal());

    //Draw the guide lines.


    painter.end();
}


QPointF GatoPrinter::bitposition(int row, int col){
    QPointF pos;
    pos.setX(paperrect.x()+col*xstep+col/8*xstep);
    pos.setY(paperrect.y()+row*ystep+row/8*ystep);
    return pos;
}


void GatoPrinter::drawbit(QPainter &painter, int row, int col, bool value){
    QPointF pos=bitposition(row,col);
    QRectF bitbox;

    //Draw a bounding box for all bits.
    bitbox.setTopLeft(pos);
    pos.setX(pos.rx()+xstep/3);
    pos.setY(pos.ry()+xstep/3);
    bitbox.setBottomRight(pos);
    if(value)
        painter.setBrush(Qt::black);
    else
        painter.setBrush(Qt::NoBrush);

    painter.setPen(Qt::black);
    painter.drawRect(bitbox);

}
