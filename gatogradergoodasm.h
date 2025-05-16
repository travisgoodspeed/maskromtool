#ifndef GATOGRADERGOODASM_H
#define GATOGRADERGOODASM_H

#include "gatosolver.h"
class GoodASM;

class GatoGraderGoodAsm : public GatoGrader
{
public:
    GatoGraderGoodAsm(GoodASM *gasm);
    ~GatoGraderGoodAsm();

    int grade(QByteArray ba);
    QString result="";

private:
    GoodASM *gasm=0;
};

#endif // GATOGRADERGOODASM_H
