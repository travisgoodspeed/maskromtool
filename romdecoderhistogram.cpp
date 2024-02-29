#include "romdecoderhistogram.h"

RomDecoderHistogram::RomDecoderHistogram() {}


//This returns a text preview.
QString RomDecoderHistogram::preview(MaskRomTool *m){
    QString ascii="# Count of colors in bit samples.\n"
                  "# You probably want to render it in Matlab or GNUPlot.\n"
                  "# \n"
                  "# Value Reds  Greens Blues\n";

    //Force the updates, even when there's no histogram to see.
    m->updateThresholdHistogram(true);

    for(int i=0; i<256; i++){
        int red=(int) m->reds[i];
        int green=(int) m->greens[i];
        int blue=(int) m->blues[i];
        ascii.append(QString::asprintf("  %3d  %5d %5d %5d\n",
                                       i,
                                       red, green, blue)
                     );
    }

    return ascii;
}

//Exports the preview to a file.
void RomDecoderHistogram::writeFile(MaskRomTool *m, QString filename){
    QFile fout(filename);
    fout.open(QIODevice::WriteOnly);
    fout.write(preview(m).toUtf8());
}
