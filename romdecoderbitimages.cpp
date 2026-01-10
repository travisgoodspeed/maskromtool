#include <QProgressDialog>

#include "romdecoderbitimages.h"
#include "maskromtool.h"

RomDecoderBitImages::RomDecoderBitImages()
{

}


QString RomDecoderBitImages::preview(MaskRomTool *m){
    return "No preview of image exports.";
}


void RomDecoderBitImages::writeFile(MaskRomTool *m, QString filename){
    // Create the image with the exact size of the shrunk scene
    QImage image=m->background;
    //QPainter painter(&image);
    //painter.setRenderHint(QPainter::Antialiasing);
    //m->scene->render(&painter);
    image.save(filename);

    QDir dir(filename);
    if(!dir.exists()){
        qDebug()<<"Needs a directory path, not a file.";
        return;
    }



    /* FIXME: This really ought to be done with a timer callback,
     * such that the progressdialog properly renders and the
     * operation can be cancelled partway.  Until then,
     * we'll comment out the progress dialog.
     */

    RomBitItem* rowbit = m->markBitTable();
    //QProgressDialog progress("Exporting", "Cancel", 0, m->rowcount);
    //Iterating by row and column.
    while(rowbit /* && !progress.wasCanceled() */ ){
        //int r=0;
        RomBitItem* bit=rowbit;
        while(bit){
            QImage image=bit->getImage();
            QString fn=dir.filePath(QString::asprintf("bit-%04d-%04d-%d.png", bit->row, bit->col, bit->bitValue()));
            if(bit->col==0)
                qDebug()<<fn;
            image.save(fn);

            bit=bit->nexttoright;  //Skip down the row.
        }
        //progress.setValue(++r);
        rowbit=rowbit->nextrow;  //Skip to the next row.
    }
    //progress.close();
}
