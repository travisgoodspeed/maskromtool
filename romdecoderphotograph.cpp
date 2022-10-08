
#include "romscene.h"
#include "romdecoderphotograph.h"


RomDecoderPhotograph::RomDecoderPhotograph()
{

}

QString RomDecoderPhotograph::preview(MaskRomTool *m){
    return "";
}


void RomDecoderPhotograph::writeFile(MaskRomTool *m, QString filename){
    // Create the image with the exact size of the shrunk scene
    QImage image=m->background;
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    m->scene->render(&painter);
    image.save(filename);
}
