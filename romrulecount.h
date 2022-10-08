#ifndef ROMRULECOUNT_H
#define ROMRULECOUNT_H

#include "romrule.h"

/* This is the very first DRC rule, designed to count the rows and
 * verify that each has the same length.  Any row which does not
 * match the first received a violation near its first bit.
 */

class RomRuleCount : public RomRule
{
public:
    RomRuleCount();
    void evaluate(MaskRomTool *mrt);
};

#endif // ROMRULECOUNT_H
