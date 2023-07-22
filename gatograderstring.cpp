#include <QStringList>
#include <QString>
#include <QList>
#include <QDebug>

#include "gatograderstring.h"

/* The setting for the searcher rule is just a comma-delimited
 * set of 8-bit bytes.
 *
 * "3a,2b,97,6c", for example.
 */
GatoGraderString::GatoGraderString(QString setting){
    QStringList settings=setting.split(",");
    QByteArray ba;
    bool okay;

    qDebug()<<"Solving for a string of:"<<setting;

    for(int i=0; i<settings.length(); i++){
        ba.append(settings[i].toLong(&okay,16));
        if(!okay){
            qDebug()<<"Invalid hex byte:"<<settings[i];
        }else{

        }
    }
    target=QByteArrayView(ba);
}

/* 100 if we contain the string.
 *   0 if we don't.
 *
 * Middle grounds would be useful here.
 */
int GatoGraderString::grade(QByteArray ba){
    qDebug()<<"Hunting for"<<target.length()<<"bytes";
    if(ba.contains(target))
        return 100;
    return 0;
}
