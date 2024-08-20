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

    //Linear size doesn't help.
    //setItemIndexMethod(ItemIndexMethod::NoIndex);

    //Tree method is default, but doesn't seem to change performance until it's too big.
    //setItemIndexMethod(ItemIndexMethod::BspTreeIndex);
}

void RomScene::keyPressEvent(QKeyEvent *event){
    /* Key combos are handled in three places:
     * 1. RomView
     * 2. RomScene
     * 3. MaskRomTool.
     */

    QPointF dpos=QPointF(0,0);

    switch(event->key()){
    // selected item translation via arrow keys
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Left:
    case Qt::Key_Right:
        maskRomTool->markUndoPoint();
        switch(event->key()){
        case Qt::Key_Up: dpos.setY(-1);   break;
        case Qt::Key_Down: dpos.setY(1);  break;
        case Qt::Key_Left: dpos.setX(-1); break;
        case Qt::Key_Right: dpos.setX(1); break;
        }
        maskRomTool->moveList(selection, dpos);
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

    //Sets the colors.
    xline.setPen(QPen(maskRomTool->crosshairColor));
    yline.setPen(QPen(maskRomTool->crosshairColor));
    xline2.setPen(QPen(maskRomTool->crosshairColor));
    yline2.setPen(QPen(maskRomTool->crosshairColor));

    if(dragging){
        /* When dragging, we use the lines to draw a green box around
         * the selection.  The box fits within the view, as crosshair
         * lines are not useful in this operation.
         */
        xline.setPen(QPen(maskRomTool->selectionColor,2,Qt::DashDotLine));
        xline2.setPen(QPen(maskRomTool->selectionColor,2,Qt::DashDotLine));
        yline.setPen(QPen(maskRomTool->selectionColor,2,Qt::DashDotLine));
        yline2.setPen(QPen(maskRomTool->selectionColor,2,Qt::DashDotLine));

        xline.setPos(scenepos.x(), scenepos.y());
        xline.setLine(0,0,presspos.x()-scenepos.x(),0);
        yline.setPos(scenepos.x(), scenepos.y());
        yline.setLine(0,0,0,presspos.y()-scenepos.y());

        xline2.setPos(presspos.x(), presspos.y());
        xline2.setLine(0,0,scenepos.x()-presspos.x(),0);
        yline2.setPos(presspos.x(), presspos.y());
        yline2.setLine(0,0,0,scenepos.y()-presspos.y());
    }else{
        /* When not dragging, the crosshairs are tilted to match the most
         * recent row and column lines.
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
    updateCrosshairs(mouseEvent->buttons()==Qt::LeftButton);

    //here instead of on release so we can have preview
    if(mouseEvent->buttons()==Qt::RightButton){
        QPointF dpos = mouseEvent->scenePos() - presspos;
        maskRomTool->moveList(selection, dpos);

        // update because we already moved it
        presspos = scenepos;
    }else{
        //Update some of the bits if they aren't finished.

        switch(maskRomTool->state){
        case MaskRomTool::STATE_MARKING:
            maskRomTool->markBits(false);
            break;
        case MaskRomTool::STATE_CLEARING:
            maskRomTool->clearBits(false);
            break;
        case MaskRomTool::STATE_IDLE:
            break;
        }
    }

    /* Whenever the mouse moves, we also update the status bar
     * to show our position and size.
     */
    updateStatus();
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
    //Don't update the scene position when dragging with middle mouse button.
    if(mouseEvent->buttons()!=Qt::MiddleButton)
        presspos=mouseEvent->scenePos();

    //Right mouse can drag a line, so we mark an undo point before moving.
    if(mouseEvent->buttons()==Qt::RightButton){
        maskRomTool->dragging=true;
        maskRomTool->markUndoPoint();
        maskRomTool->clearBits(false);
    }
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

        auto newselection=items(x, y, w, h,
                          //Qt::ItemSelectionMode::ContainsItemBoundingRect,  //Useful for bits.
                          //Qt::ItemSelectionMode::ContainsItemShape, // Only when fully covered.
                          //Qt::ItemSelectionMode::IntersectsItemBoundingRect,  //Useful for a near miss, sometimes too wide.
                          Qt::ItemSelectionMode::IntersectsItemShape, // Best for lines.

                          Qt::DescendingOrder
                        );


        /* We can select static items like lines but never
         * ephemeral items like bits.
         */
        foreach(QGraphicsItem* item, newselection){
            switch(item->type()){
            case QGraphicsItem::UserType:   //row
            case QGraphicsItem::UserType+1: //col
            case QGraphicsItem::UserType+3: //bitfix
                break;
            default: // All others.
            case QGraphicsItem::UserType+2: //bit
            case QGraphicsItem::UserType+4: //rule violation
                newselection.removeOne(item);
                break;
            }
        }

        switch(mouseEvent->modifiers()){
        case Qt::CTRL: //Subtract from group.
            foreach(QGraphicsItem* item, newselection)
                selection.removeOne(item);
            break;
        case Qt::SHIFT: //Add to group.
            //Remove anything that's already there to prevent double-selection.
            foreach(QGraphicsItem* item, newselection)
                selection.removeOne(item);
            //Then add the rest.
            selection.append(newselection);
            break;
        default:
            selection=newselection;
            break;
        }

        //Restore visibility of the crosshairs.
        setCrosshairVisible(crosshairVisible);
        //Redraw the crosshairs without dragging.
        updateCrosshairs(false);

        //Update the highlighting so the user knows what's going on.
        highlightSelection();
    }else if(mouseEvent->button()==Qt::RightButton){
        /* The right mouse is used for dragging, but dragging often breaks the bit
         * arrangement.  As a workaround, we remark the bits after releasing the
         * button from a moving drag.
         */
        maskRomTool->dragging=false;
        if(maskRomTool->bitsVisible){
            //Clear the old bits in the background.
            //This will continue until redrawing them.
            maskRomTool->clearBits(false);
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

void RomScene::highlightSelection(){
    /* Qt will redraw the main view when changing a line's pen, but for some
     * reason it doesn't update the line's color in the secondary view.
     * We work around this by hiding and then showing the line as we change
     * its pen.  Perhaps overloading setPen() or a global refresh would
     * be more efficient.
     */


    if(!maskRomTool->linesVisible)
        return;

    //reset line focus first, would be nice if we knew the last one that was in focus
    for(auto i = maskRomTool->rows.constBegin(); i != maskRomTool->rows.constEnd(); i++){
        ((QGraphicsLineItem*)*i)->setPen(QPen(maskRomTool->lineColor, 2));
        ((QGraphicsLineItem*)*i)->hide();
        ((QGraphicsLineItem*)*i)->show();
    }
    for(auto i = maskRomTool->cols.constBegin(); i != maskRomTool->cols.constEnd(); i++){
        ((QGraphicsLineItem*)*i)->setPen(QPen(maskRomTool->lineColor, 2));
        ((QGraphicsLineItem*)*i)->hide();
        ((QGraphicsLineItem*)*i)->show();
    }

    //Highlight all selected lines.
    foreach(QGraphicsItem* item, selection){
        if(!item){
            //qDebug()<<"Null item.";
        }else if(item->type()==QGraphicsItem::UserType){
            //row
            maskRomTool->lastrow=((RomLineItem*)item)->line();
            ((QGraphicsLineItem*)item)->setPen(QPen(maskRomTool->selectionColor, 2));
            ((QGraphicsLineItem*)item)->hide();
            ((QGraphicsLineItem*)item)->show();
            maskRomTool->updateCrosshairAngle((RomLineItem*)item);
        }else if(item->type()==QGraphicsItem::UserType+1){
            //column
            maskRomTool->lastcol=((RomLineItem*)item)->line();
            ((QGraphicsLineItem*)item)->setPen(QPen(maskRomTool->selectionColor, 2));
            ((QGraphicsLineItem*)item)->hide();
            ((QGraphicsLineItem*)item)->show();
            maskRomTool->updateCrosshairAngle((RomLineItem*)item);
        }
    }
}

void RomScene::setFocusItem(QGraphicsItem* item){
    //Update the focus item and the selection list.
    selection=QList<QGraphicsItem*>();
    selection<<item;

    highlightSelection();
}

QGraphicsItem* RomScene::focusItem(){
    if(selection.count()==1)
        return selection.at(0);
    return 0;
}
