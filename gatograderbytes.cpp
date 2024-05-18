#include<QDebug>
#include <QRegularExpression>
#include "gatograderbytes.h"

GatoGraderBytes::GatoGraderBytes(QString setting){
    /* The setting string is a set of comma-delimited
     * values of an address, a byte value, and a bitmask.
     */
    QStringList settings=setting.split(QRegularExpression(",|\\s+"));
    for(int i=0; i<settings.length(); i++){
        GatoGraderBytesRule *rule=new GatoGraderBytesRule(settings[i]);
        rules.insert(rule);
    }
}

int GatoGraderBytes::grade(QByteArray ba){
    int grade=0;
    int attempts=0, passes=0;

    foreach(GatoGraderBytesRule* rule, rules){
        attempts++;
        if(rule->passes(ba))
            passes++;
    }

    grade=passes*100.0/attempts;
    return grade;
}


 //Creates a rule around a string.
GatoGraderBytesRule::GatoGraderBytesRule(QString rule){
    this->rule=rule;
    QStringList triple=rule.split(":");
    bool okmask=true, okval=true, okadr=true;
    switch(triple.length()){
    case 3:
        mask=triple[2].toLong(&okmask,16);
    case 2:
        value=triple[1].toLong(&okval,16);
    case 1:
        address=triple[0].toLong(&okadr,16);
        break;
    default:
        qDebug()<<"Illegal GatoGraderBytesRule rule:"<<rule;
    }
}
//Does a given decoding pass the rule?
bool GatoGraderBytesRule::passes(QByteArray ba){
    if(address>=ba.length()){
        //qDebug()<<"Rule"<<rule<<"is outside of byte array length"<<ba.length();
        return false;
    }

    return (((uint8_t)ba[address])&mask) == (value&mask);
}
