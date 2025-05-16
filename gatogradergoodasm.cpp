#include "gatogradergoodasm.h"
#include "extern/goodasm/goodasm.h"
#include "extern/goodasm/gagrader.h"

#include <qdebug.h>

GatoGraderGoodAsm::GatoGraderGoodAsm(GoodASM *gasm) {
    assert(gasm);
    this->gasm=gasm;
}

GatoGraderGoodAsm::~GatoGraderGoodAsm() {
    delete gasm;
}


/* This grader works by asking GoodAsm if the
 * binary is valid in the currently selected language.
 * We generally don't allow for an unknown language
 * because that would result in too many false positives,
 * and unnecessary scanning time.
 */
int GatoGraderGoodAsm::grade(QByteArray ba){
    //No sense grading an empty result.
    if(ba.length()==0) return 0;

    int valids=0, invalids=0;

    gasm->load(ba);

    auto grades=gasm->grade();

    foreach(GAGraderGrade g, grades){
        bool valid=g.valid;
        printf("%s\t%s\n",
               g.grader->name.toStdString().c_str(),
               (valid?"valid":"invalid")
               );
        if(valid) valids++;
        else invalids++;
    }
    if(valids>invalids) return 100;

    return 0;
}
