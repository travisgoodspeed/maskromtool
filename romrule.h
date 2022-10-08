#ifndef ROMRULE_H
#define ROMRULE_H

#include<QPointF>
#include<QString>
#include<QGraphicsRectItem>

class MaskRomTool;

/* Each rule violation is described to the operator in both a list and at a position.
 */
class RomRuleViolation : public QGraphicsRectItem
{
public:
    bool error;
    QString title, detail;
    static qreal bitSize;

    RomRuleViolation(QPointF position, QString title, QString detail, bool iserror=false);
};


/* This class represents a Design Rule, which takes as input the MaskRomTool state
 * and as output returns a number of Results in the form of Warnings and Errors.
 * These Results are rendered graphically in the GUI and as text in the CLI.
 */
class RomRule
{
public:
    virtual void evaluate(MaskRomTool *mrt)=0;
};

#endif // ROMRULE_H
