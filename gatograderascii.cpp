#include "gatograderascii.h"
#include <QDebug>

/* This works by counting the length of the longest string.
 */

GatoGraderASCII::GatoGraderASCII(){

}

int GatoGraderASCII::grade(QByteArray ba){
    QString longest="";
    QString last="";
    uint32_t longestcount=0, lastcount=0;

    for(int i=0; i<ba.length(); i++){
        char c=ba[i]; //Must be signed!
        if(((int) c)>=32
            && ((int) c)!=127){
            last.append(c);
            lastcount++;
        }else{
            if(lastcount>longestcount){
                longest=last;
                longestcount=lastcount;
            }
            last="";
            lastcount=0;
        }
    }

    if(longestcount>=15){
        qDebug()<<"String: "<<longest;
    }

    return longestcount>15?70+longestcount:0;
}
