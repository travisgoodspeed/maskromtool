#ifndef ROMSOLUTIONSDIALOG_H
#define ROMSOLUTIONSDIALOG_H

#include <QDialog>

/* This dialog shows solutions from the solver as a list.  Selecting
 * a solution will decode the ROM in that style in the rest of the
 * GUI, updating strings and hexadecimal to match.
 */

class MaskRomTool;

namespace Ui {
class RomSolutionsDialog;
}

class RomSolutionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RomSolutionsDialog(QWidget *parent = nullptr);
    ~RomSolutionsDialog();
    void setMaskRomTool(MaskRomTool *mrt);

private:
    Ui::RomSolutionsDialog *ui;
    MaskRomTool *mrt=0;
    void registerSolution(int score, QString solution);
};

#endif // ROMSOLUTIONSDIALOG_H
