#ifndef GATOGRADERYARA_H
#define GATOGRADERYARA_H

#include<QString>

#include "gatosolver.h"

/* Most solvers work by rules given as simple parameters,
 * such as a string or a byte/address/mask.  This one shells
 * out to Yara, taking a rule as its parameter.
 *
 * It could be considerably faster if we linked against libyara
 * and compiled the rule internally.  I'm not clear on whether
 * this will be slow enough for that to be worthwhile.
 */

class GatoGraderYara : public GatoGrader
{
public:
    GatoGraderYara(QString rulefile);
    ~GatoGraderYara();
    int grade(QByteArray ba);

    QString result="";

private:
    QString rulefile="";
};

#endif // GATOGRADERYARA_H
