#ifndef ROMRULEDIALOG_H
#define ROMRULEDIALOG_H


/* This dialog displays design rule violations.  Hitting V will populate
 * it with the standard rules, such as RomRuleCount to mark rows of unexpected
 * length and RomRuleAmbiguous to mark bits whose colors look wrong.
 *
 * The same dialog is used for things like showing bits from a selection in the hex
 * dialog or differences from an imported bitstream.  These are not really DRC
 * violations, but the abstraction remains useful.
 */

#include <QDialog>
#include <QListWidgetItem>
class RomRuleViolation;
class MaskRomTool;

namespace Ui {
class RomRuleDialog;
}

class RomRuleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RomRuleDialog(QWidget *parent = nullptr);
    ~RomRuleDialog();
    void clearViolations();
    void nextViolation();
    void addViolation(RomRuleViolation* violation);
    void removeViolation(RomRuleViolation* violation);
    void setMaskRomTool(MaskRomTool* maskRomTool);

private slots:
    //Double clicking jumps to a violation.
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::RomRuleDialog *ui;
    MaskRomTool* mrt=0;
};

class RomRuleDialogEntry : public QListWidgetItem{
public:
    RomRuleDialogEntry(RomRuleViolation* v);
    RomRuleViolation* violation;
};


#endif // ROMRULEDIALOG_H
