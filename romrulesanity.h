#ifndef ROMRULESANITY_H
#define ROMRULESANITY_H

/* This rule provides quick sanity checks of the lines.
 * It's expected to fire if:
 * 1. A row is vertical.
 * 2. A column is horizontal.
 * 3. A row or column is zero length.
 */

#include "romrule.h"
#include "romlineitem.h"

class RomRuleSanity : public RomRule
{
public:
    RomRuleSanity();
    void evaluate(MaskRomTool *mrt);
private:
    void testLine(MaskRomTool *mrt, RomLineItem *line);
};

#endif // ROMRULESANITY_H
