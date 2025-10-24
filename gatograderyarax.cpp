#include <QDebug>
#include "gatograderyarax.h"

/* VERY IMPORTANT:
 *
 * YaraX is lovely, but it's a Rust project that many C++
 * developers will not know how to build.  We work around
 * this by the YARAX_FOUND definition, so that the feature
 * gracefully degrades when YaraX is missing.
 *
 * In any changes to this executable, your code must compile
 * with or without YaraX.
 */

#ifdef YARAX_FOUND

GatoGraderYaraX::GatoGraderYaraX(QString rule) {
    //Someday we might hold more than one rule.
    this->rule=rule;

    //First compile the rules.
    YRX_RESULT res=yrx_compile(rule.toStdString().c_str(), &rules);
    if(res!=YRX_SUCCESS){
        qDebug()<<QString(yrx_last_error());
    }

    //Then build a scanner.
    res=yrx_scanner_create(rules, &scanner);
    if(res!=YRX_SUCCESS){
        qDebug()<<QString(yrx_last_error());
    }
}

GatoGraderYaraX::~GatoGraderYaraX(){
    if(scanner) yrx_scanner_destroy(scanner);
    if(rules) yrx_rules_destroy(rules);
}

static void callback(const struct YRX_RULE *rule, void *user_data){
    GatoGraderYaraX* grader=(GatoGraderYaraX*) user_data;
    grader->match++;
}

int GatoGraderYaraX::grade(QByteArray ba){
    assert(scanner);

    //Scan the bytes.
    this->match=0; //Reset match count.
    yrx_scanner_on_matching_rule(scanner, callback, (void*) this);
    YRX_RESULT res=yrx_scanner_scan(scanner,(uint8_t*) ba.data(), ba.length());
    return this->match*100; //Did we get a match?
}


#endif //YARAX_FOUND
