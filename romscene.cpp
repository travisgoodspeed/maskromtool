#include "romscene.h"
#include "maskromtool.h"

#include <QGraphicsSceneMouseEvent>
#include <QtDebug>

RomScene::RomScene(QObject *parent)
    : QGraphicsScene{parent}
{
    addItem(&xline);
    addItem(&yline);
    addItem(&xline2);
    addItem(&yline2);
    xline.setPen(QPen(Qt::gray));
    yline.setPen(QPen(Qt::gray));
    xline2.setPen(QPen(Qt::gray));
    yline2.setPen(QPen(Qt::gray));


    //Linear size doesn't help.
    //setItemIndexMethod(ItemIndexMethod::NoIndex);

    //Tree method is default, but doesn't seem to change performance until it's too big.
    //setItemIndexMethod(ItemIndexMethod::BspTreeIndex);
    /* Setting the tree depth helps in large projects, faster than letting it grow.
     * Default 55s for MYK82 test cases.
     * 5 -- 55s
     * 10 -- 54s for MYK82 test cases.
     * 20 -- Too long.
     */
    //setBspTreeDepth(2);  //Maybe a decent depth helps initial performance?
}

void RomScene::keyPressEvent(QKeyEvent *event){
    RomLineItem *rlitem;
    QPointF dpos;

    switch(event->key()){
    // selected item translation via arrow keys
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Left:
    case Qt::Key_Right:
        if(focusItem()){
            maskRomTool->markUndoPoint();
            switch(focusItem()->type()){
            case QGraphicsItem::UserType: //row
            case QGraphicsItem::UserType+1: //column
                rlitem = (RomLineItem*)focusItem();
                dpos = QPointF(0,0);
                switch(event->key()){
                case Qt::Key_Up: dpos.setY(-1);break;
                case Qt::Key_Down: dpos.setY(1);break;
                case Qt::Key_Left: dpos.setX(-1);break;
                case Qt::Key_Right: dpos.setX(1);break;
                }
                maskRomTool->moveLine(rlitem,rlitem->pos()+dpos);
                break;
            }
            break;
        }else{// no selected item, default arrows handler will move the whole scene
            QGraphicsScene::keyPressEvent(event);
        }
        break;
    default:
        QGraphicsScene::keyPressEvent(event);
    }
}


//Update the crosshairs to the new position.
void RomScene::updateCrosshairs(bool dragging){
    /* If our images were perfect, the crosshairs would be perfectly vertical
     * and horizontal, but even the most careful camera operator will be off by
     * a fraction of a degree.  This fraction makes the sighting off, so that
     * perfect crosshairs would choose the wrong position when clicking far away!
     *
     * So what we do instead is quietly update the crosshairs to the latest
     * row and column angles.  This way, when you repeat a line, the angle
     * of the crosshair will accurately sight all of the bits in your new
     * row or column.  Nifty, huh?
     */

    //The horizontal line.
    xline.setPos(scenepos.x(), scenepos.y());
    xline.setLine(-linesizex,0,linesizex,0);
    QLineF l=xline.line();
    l.setAngle(rowAngle);
    xline.setLine(l);
    if(!dragging)
        xline2.setLine(l);
    //The line's angle is now right, but we still need to center it on the cursor.
    QPointF c=l.center();
    xline.setPos(scenepos.x()-c.rx(), scenepos.y()-c.ry());
    if(!dragging)
        xline2.setPos(scenepos.x()-c.rx(), scenepos.y()-c.ry());

    //The vertical line.
    yline.setPos(scenepos.x(), scenepos.y());
    yline.setLine(0,-linesizey,0,linesizey);
    l=xline.line();
    l.setAngle(colAngle);
    yline.setLine(l);
    if(!dragging)
        yline2.setLine(l);
    //The line's angle is now right, but we still need to center it on the cursor.
    c=l.center();
    yline.setPos(scenepos.x()-c.rx(), scenepos.y()-c.ry());
    if(!dragging)
        yline2.setPos(scenepos.x()-c.rx(), scenepos.y()-c.ry());

}

//Updates the status bar to describe the position.
void RomScene::updateStatus(){
    static QString str;
    maskRomTool->statusBar()->showMessage(
        str.asprintf(
            "%04x to %04x -- %05d x %05d -- %ld rows, %ld cols -- %2lld violations -- %ld bits, %ld words, %ldkB",
            maskRomTool->hexDialog.start,
            maskRomTool->hexDialog.end,
            (int) scenepos.x(), (int) scenepos.y(),
            maskRomTool->rowcount, maskRomTool->colcount,
            maskRomTool->violations.size(),
            maskRomTool->bitcount,
            maskRomTool->bitcount/maskRomTool->gr.wordsize,
            maskRomTool->bitcount/1024/8
            )
        );

}

//Store the last seen mouse position.
void RomScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent){
    //Now update the position.
    scenepos=mouseEvent->scenePos();

    updateCrosshairs(mouseEvent->buttons()!=Qt::NoButton);

    /* Whenever the mouse moves, we also update the status bar
     * to show our position and size.
     */
    updateStatus();

    //here instead of on release so we can have preview
    if(mouseEvent->buttons()==Qt::RightButton){
        QPointF dpos = mouseEvent->scenePos() - presspos;
        RomLineItem *rlitem = (RomLineItem *)focusItem();
        if(rlitem){
            switch(rlitem->type()){
                case QGraphicsItem::UserType: //row
                case QGraphicsItem::UserType+1: //column
                    maskRomTool->moveLine(rlitem,rlitem->pos()+dpos);
                    presspos = scenepos; // update because we already moved it
                    break;
            }
        }
    }
}

void RomScene::setRowAngle(qreal angle){
    rowAngle=angle;
    updateCrosshairs();
}
void RomScene::setColAngle(qreal angle){
    colAngle=angle;
    updateCrosshairs();
}

//Store the last pressed mouse position.
void RomScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent){
    //This could be handy in the selection.
    presspos=mouseEvent->scenePos();

    //Right mouse can drag a line, so we mark an undo point before moving.
    if(mouseEvent->buttons()==Qt::RightButton)
        maskRomTool->markUndoPoint();
}

//Store the last pressed mouse position.
void RomScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent){
    if(mouseEvent->button()==Qt::LeftButton){
        //Make the crosshairs temporarily invisible, so we don't select them.
        xline.setVisible(false);
        yline.setVisible(false);
        xline2.setVisible(false);
        yline2.setVisible(false);


        //Grab the intersecting items, which might be of many different types.
        releasepos=mouseEvent->scenePos();
        selection=items(presspos.rx(), presspos.ry(),
                        releasepos.rx()-presspos.rx(), releasepos.ry()-presspos.ry(),
                        //Qt::ItemSelectionMode::ContainsItemBoundingRect,  //Useful for bits.
                        //Qt::ItemSelectionMode::ContainsItemShape, // Only when fully covered.
                        Qt::ItemSelectionMode::IntersectsItemBoundingRect,  //Useful for a near miss?
                        //Qt::ItemSelectionMode::IntersectsItemShape, //Maybe best for lines?
                        Qt::DescendingOrder,
                        QTransform()
                        );

        //Restore visibility of the crosshairs.
        setCrosshairVisible(crosshairVisible);

        foreach(QGraphicsItem* item, selection){
            if(selection.count()==1)
                setFocusItem(item);
        }

        //More than one isn't supported right now.
        if(selection.count()>1){
            //qDebug()<<"Failing to set more than one focus item.";
            setFocusItem(0);
        }
    }
}


void RomScene::setCrosshairVisible(bool v){
    crosshairVisible=v;
    xline.setVisible(v);
    yline.setVisible(v);
    xline2.setVisible(v);
    yline2.setVisible(v);
}

QGraphicsItem* RomScene::focusItem(){
    //We have a single selection after placing a new item.
    if(focusitem)
        return focusitem;

    //Otherwise return zero.
    return 0;
}

void RomScene::setFocusItem(QGraphicsItem* item){
    focusitem=item;
    
    //reset line focus first, would be nice if we knew the last one that was in focus
    for(QSet<RomLineItem*>::iterator i = maskRomTool->rows.begin(), end = maskRomTool->rows.end(); i != end; ++i){
        ((QGraphicsLineItem*)*i)->setPen(QPen(maskRomTool->lineColor, 2));
    }
    for(QSet<RomLineItem*>::iterator i = maskRomTool->cols.begin(), end = maskRomTool->cols.end(); i != end; ++i){
        ((QGraphicsLineItem*)*i)->setPen(QPen(maskRomTool->lineColor, 2));
    }
    if(!item){
        //No item to mark, so don't worry about investigating it.
        return;
    }else if(item->type()==QGraphicsItem::UserType){
        //row
        maskRomTool->lastrow=((RomLineItem*)item)->line();
        ((QGraphicsLineItem*)item)->setPen(QPen(Qt::green, 2));
        maskRomTool->updateCrosshairAngle((RomLineItem*)item);
    }else if(item->type()==QGraphicsItem::UserType+1){
        //column
        maskRomTool->lastcol=((RomLineItem*)item)->line();
        ((QGraphicsLineItem*)item)->setPen(QPen(Qt::green, 2));
        maskRomTool->updateCrosshairAngle((RomLineItem*)item);
    }
}
