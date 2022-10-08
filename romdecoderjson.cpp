#include "romdecoderjson.h"

#include<QFile>


RomDecoderJson::RomDecoderJson()
{

}


QJsonDocument RomDecoderJson::document(MaskRomTool *m){
    QJsonObject root;

    root["00about"]="Export from MaskROMTool by Travis Goodspeed";
    root["00github"]="http://github.com/travisgoodspeed/mcuexploits/";

    m->markBits();

    QJsonArray jbits;
    foreach (RomBitItem* bit, m->bits){
        QJsonObject o;
        bit->write(o);
        jbits.push_back(o);
    }
    root["bits"]=jbits;
    return QJsonDocument(root);
}


QString RomDecoderJson::preview(MaskRomTool *m){
    QByteArray ba = document(m).toJson();
    QTextStream ts(stdout);
    ts << ba;

    return ts.readAll();
}

void RomDecoderJson::writeFile(MaskRomTool *m, QString filename){
    QByteArray ba = document(m).toJson();

    QFile fout(filename);
    fout.open(QIODevice::WriteOnly);
    fout.write(ba);
}
