#ifndef ROMVIEW_H
#define ROMVIEW_H

#include <QGraphicsView>

/* Remember that any mouse events caught here
 * won't be found in the ROMScene unless forwarded
 * by calling the parent class.
 */

class RomView : public QGraphicsView
{
public:
    RomView(QWidget *parent = nullptr);

    void centerOn(QGraphicsItem* item);

    //Double click to select an item.
    void mouseDoubleClickEvent(QMouseEvent *event);
    //Middle-click panning.
    void mouseMoveEvent(QMouseEvent *mouseEvent);
    void mousePressEvent(QMouseEvent *mouseEvent);
    void mouseReleaseEvent(QMouseEvent *mouseEvent);
    //Wheel events.
    void wheelEvent(QWheelEvent *event);
    //Needed for pinch zooming.
    bool viewportEvent(QEvent *event);

    void scale(qreal currentScale);
    qreal totalScaleFactor = 1;

private:
    qreal dragx=0, dragy=0;
};

#endif // ROMVIEW_H
