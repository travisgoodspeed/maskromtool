#ifndef ROMSCENE_H
#define ROMSCENE_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>

class MaskRomTool;

/* If you don't find what you're looking for here, it's probably in romview.h.
 */

class RomScene : public QGraphicsScene
{
public:
    explicit RomScene(QObject *parent = nullptr);

    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void setCrosshairVisible(bool v);

    QPointF scenepos; //Last seen position.
    QPointF presspos; //Position where last pressed.
    QPointF releasepos; //Position where last released

    QList<QGraphicsItem*> selection; //Selected items.

    //These set the angles of the crosshairs, which tilt a little.
    void setRowAngle(qreal angle);
    void setColAngle(qreal angle);

    //Return just one focused item.
    QGraphicsItem* focusItem();
    void setFocusItem(QGraphicsItem* item);
    MaskRomTool* maskRomTool=0;

private:
    bool crosshairVisible=true;
    QGraphicsItem* focusitem=0;
    //Crosshair lines.
    QGraphicsLineItem xline, yline, xline2, yline2;
    //Crosshair angles.
    qreal rowAngle=180, colAngle=90;
    //Updates the crosshairs.
    void updateCrosshairs(bool dragging=false);
};

#endif // ROMSCENE_H
