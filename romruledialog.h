#ifndef ROMRULEDIALOG_H
#define ROMRULEDIALOG_H

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
