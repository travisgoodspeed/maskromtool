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
    QPointF dpos=QPointF(0,0);

    switch(event->key()){
    // selected item translation via arrow keys
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Left:
    case Qt::Key_Right:
        maskRomTool->markUndoPoint();
        foreach(QGraphicsItem* item, selection){
            if(item && (item->type()==QGraphicsItem::UserType || item->type()==QGraphicsItem::UserType+1)){
                RomLineItem *rlitem=(RomLineItem*) item;
                switch(event->key()){
                case Qt::Key_Up: dpos.setY(-1);break;
                case Qt::Key_Down: dpos.setY(1);break;
                case Qt::Key_Left: dpos.setX(-1);break;
                case Qt::Key_Right: dpos.setX(1);break;
                }
                maskRomTool->moveLine(rlitem,rlitem->pos()+dpos);
            }
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

        foreach(QGraphicsItem* item, selection){
            if(item && (item->type()==QGraphicsItem::UserType || item->type()==QGraphicsItem::UserType+1)){
                RomLineItem *rlitem=(RomLineItem*) item;
                maskRomTool->moveLine(rlitem,rlitem->pos()+dpos);
            }
        }

        // update because we already moved it
        presspos = scenepos;
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

        qreal x=presspos.rx(),
              y=presspos.ry(),
              w=releasepos.rx()-presspos.rx(),
              h=releasepos.ry()-presspos.ry();

        //Negative sizes confuse Qt, so we make it positive.
        if(w<0) x-=(w=qFabs(w));
        if(h<0) y-=(h=qFabs(h));

        //qDebug()<<"Selection: "<<x<<y<<w<<h;

        selection=items(x, y, w, h,

                          //Qt::ItemSelectionMode::ContainsItemBoundingRect,  //Useful for bits.
                          //Qt::ItemSelectionMode::ContainsItemShape, // Only when fully covered.
                          //Qt::ItemSelectionMode::IntersectsItemBoundingRect,  //Useful for a near miss, sometimes too wide.
                          Qt::ItemSelectionMode::IntersectsItemShape, // Best for lines.

                          Qt::DescendingOrder
                        );

        //Restore visibility of the crosshairs.
        setCrosshairVisible(crosshairVisible);

        //The item is only focused if there is exactly one item.
        if(selection.count()==1)
            focusitem=selection[0];
        else
            focusitem=0;

        //Update the highlighting so the user knows what's going on.
        highlightSelection();
    }else if(mouseEvent->button()==Qt::RightButton){
        /* The right mouse is used for dragging, but dragging often breaks the bit
         * arrangement.  As a workaround, we remark the bits after releasing the
         * button from a moving drag.
         */
        if(maskRomTool->bitsVisible)
            maskRomTool->markBits();
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
    return focusitem;
}

void RomScene::highlightSelection(){
    //reset line focus first, would be nice if we knew the last one that was in focus
    for(QSet<RomLineItem*>::iterator i = maskRomTool->rows.begin(), end = maskRomTool->rows.end(); i != end; ++i){
        ((QGraphicsLineItem*)*i)->setPen(QPen(maskRomTool->lineColor, 2));
    }
    for(QSet<RomLineItem*>::iterator i = maskRomTool->cols.begin(), end = maskRomTool->cols.end(); i != end; ++i){
        ((QGraphicsLineItem*)*i)->setPen(QPen(maskRomTool->lineColor, 2));
    }

    //Highlight all selected lines.
    //qDebug()<<"Selected "<<selection.count()<<" items";
    foreach(QGraphicsItem* item, selection){
        if(!item){
            //qDebug()<<"Null item.";
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
}

void RomScene::setFocusItem(QGraphicsItem* item){
    //Update the focus item and the selection list.
    focusitem=item;
    selection=QList<QGraphicsItem*>();
    selection<<item;
    
    highlightSelection();
}
