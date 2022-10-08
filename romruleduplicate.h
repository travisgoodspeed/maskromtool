#ifndef ROMRULEDUPLICATE_H
#define ROMRULEDUPLICATE_H

#include "romrule.h"

/* This rule identifies bits whose boxes overlap one another,
 * which is usually caused by one row overlapping a neighboring
 * row when the length drifts a little.  The current implementation
 * is very inefficient, and it should be rewritten to be faster.
 */

class RomRuleDuplicate : public RomRule
{
public:
    RomRuleDuplicate();
    void evaluate(MaskRomTool *mrt);
};

#endif // ROMRULEDUPLICATE_H
