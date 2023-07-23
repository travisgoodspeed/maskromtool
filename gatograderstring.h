#ifndef GATOGRADERSTRING_H
#define GATOGRADERSTRING_H

#include <QString>
#include <QByteArrayView>
#include "gatosolver.h"

/* This strategy is new to Gatorom, searching the resulting bytes from
 * an attempt to find a linear string of bytes.  It's useful when you
 * have one version of a ROM in software, but you need to solve for
 * the encoding of a different version.
 */

class GatoGraderString : public GatoGrader
{
public:
    GatoGraderString(QString setting);
    int grade(QByteArray ba);
private:
    QByteArray target; //String that we're hunting for.
};

#endif // GATOGRADERSTRING_H
