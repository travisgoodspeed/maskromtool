#ifndef GATOGRADERYARAX_H
#define GATOGRADERYARAX_H

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

#include "gatosolver.h"
#include "config.h"     //Needed for "YARAX_FOUND" from Cmake.

#ifdef YARAX_FOUND

//Library is C, not C++, so name mangling must be disabled.
extern "C" {
#include <yara_x.h>
}


class GatoGraderYaraX : public GatoGrader
{
public:
    GatoGraderYaraX(QString rule);
    ~GatoGraderYaraX();
    int grade(QByteArray ba);

    //Incremented on a callback.
    int match=0;

private:
    YRX_RULES* rules=0;
    YRX_SCANNER* scanner=0;
    QString rule="";
};

#endif // YARAX_FOUND
#endif // GATOGRADERYARAX_H
