#ifndef GATOGRADERBYTES_H
#define GATOGRADERBYTES_H

#include <QString>
#include <QSet>
#include "gatosolver.h"

/* This strategy is like the --bytes flag from Zorrom,
 * matching on either exact bytes or their masked equivalents.
 */


class GatoGraderBytesRule {
public:
    GatoGraderBytesRule(QString rule); //Creates a rule around a string.
    bool passes(QByteArray ba);        //Does a given decoding pass the rule?
private:
    //Each rule comes from a byte at an address, matched with a mask.
    uint32_t address=0;
    uint8_t value=0, mask=0xFF;
    QString rule="";
};

class GatoGraderBytes : public GatoGrader
{
public:
    GatoGraderBytes(QString setting);
    ~GatoGraderBytes();
    int grade(QByteArray ba);
private:
    QSet<GatoGraderBytesRule*> rules;
};

#endif // GATOGRADERBYTES_H
