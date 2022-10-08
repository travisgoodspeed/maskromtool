#ifndef ROMRULEAMBIGUOUS_H
#define ROMRULEAMBIGUOUS_H

#include "romrule.h"

class RomRuleAmbiguous : public RomRule
{
public:
    RomRuleAmbiguous();
    void evaluate(MaskRomTool *mrt);
};

#endif // ROMRULEAMBIGUOUS_H
