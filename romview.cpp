#include "romview.h"

#include<QMouseEvent>
#include<QScrollBar>
#include<QGraphicsItem>
#include <QPixmapCache>



RomView::RomView(QWidget *parent){
    setMouseTracking(true);

    //I hoped that this would improve performance, but it doesn't seem to.
    //setCacheMode(QGraphicsView::CacheBackground);
    //setCacheMode(QGraphicsView::CacheNone);
    //QPixmapCache::setCacheLimit(1024000);



    //Update the whole damned box.  Must be slow?
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    //Update the whole bounding box.  Doesn't seem to hurt CLI performance.
    //setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    //Never updates the view.  Maybe good for CLI?
    //setViewportUpdateMode(QGraphicsView::NoViewportUpdate);


}

void RomView::centerOn(QGraphicsItem* item){
    //No point focusing on an item that doesn't exist.
    if(!item)
        return;

    QGraphicsView::centerOn(item->pos());
}

//This selects an item when double-clicked.
void RomView::mouseDoubleClickEvent(QMouseEvent *event){
    //Does nothing for now.
    event->ignore();
    QGraphicsView::mouseDoubleClickEvent(event);
}

//Here we pan the view on a middle-click drag.
void RomView::mouseMoveEvent(QMouseEvent *mouseEvent){
    qreal x=mouseEvent->pos().x();
    qreal y=mouseEvent->pos().y();

    if(mouseEvent->buttons() & Qt::MiddleButton){
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (x - dragx));
        verticalScrollBar()->setValue(verticalScrollBar()->value() - (y - dragy));
        dragx=x;
        dragy=y;
    }

    //Other events are handed in RomScene, so we forward them.
    mouseEvent->accept();
    QGraphicsView::mouseMoveEvent(mouseEvent);
}
void RomView::mousePressEvent(QMouseEvent *mouseEvent){
    qreal x=mouseEvent->pos().x();
    qreal y=mouseEvent->pos().y();

    //Set the origin point for panning.
    if(mouseEvent->button()==Qt::MiddleButton){
        dragx=x;
        dragy=y;
    }

    mouseEvent->ignore();
    QGraphicsView::mousePressEvent(mouseEvent);
}
void RomView::mouseReleaseEvent(QMouseEvent *mouseEvent){
    mouseEvent->ignore();
    QGraphicsView::mouseReleaseEvent(mouseEvent);
}


//Wheel events.
void RomView::wheelEvent(QWheelEvent *event){
    if (event->modifiers() & Qt::ControlModifier) {
        QPoint degrees=event->angleDelta()/8.0;
        qreal factor=1.0+degrees.ry()/100.0;
        scale(factor);
        event->accept();
    }else{
        QGraphicsView::wheelEvent(event);
    }
}

void RomView::scale(qreal currentScale){
    totalScaleFactor*=currentScale;
    QGraphicsView::scale(currentScale, currentScale);
}

//Pinch event.
bool RomView::viewportEvent(QEvent *event) {
    /* We want a little resistance before the first scaling, or
     * the size will keep jumping all over the place.  This variable
     * becomes true when we exceed that threshold, then drops to false
     * when we're done.
     */
    static bool scaling=false;

    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
        const auto touchPoints = touchEvent->points();
        if (touchPoints.count()==2 ) {
            // determine scale factor
            const QEventPoint &touchPoint0 = touchPoints.first();
            const QEventPoint &touchPoint1 = touchPoints.last();
            qreal currentScaleFactor =
                    QLineF(touchPoint0.position(), touchPoint1.position()).length()
                    / QLineF(touchPoint0.pressPosition(), touchPoint1.pressPosition()).length();

            //We only scale if the fingers have every been far enough apart.
            qreal distance=QLineF(touchPoint0.position(), touchPoint1.position()).length();
            if(distance>80)
                scaling=true;


            if (touchEvent->touchPointStates() & QEventPoint::Released) {
                // if one of the fingers is released, remember the current scale
                // factor so that adding another finger later will continue zooming
                // by adding new scale factor to the existing remembered value.
                totalScaleFactor *= currentScaleFactor;
                currentScaleFactor = 1;

                scale(currentScaleFactor);
                scaling=false;
            }

            if(scaling)
                setTransform(QTransform::fromScale(totalScaleFactor * currentScaleFactor,
                                                   totalScaleFactor * currentScaleFactor));
        }
        //return true;
#endif
    }
    default:
        break;
    }
    return QGraphicsView::viewportEvent(event);
}
