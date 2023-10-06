#ifndef GATOGRADERASCII_H
#define GATOGRADERASCII_H

#include <QString>
#include <QSet>
#include "gatosolver.h"

class GatoGraderASCII : public GatoGrader
{
public:
    GatoGraderASCII();
    int grade(QByteArray ba);
};

#endif // GATOGRADERASCII_H
