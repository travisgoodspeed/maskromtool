#ifndef ROMSOLUTIONSDIALOG_H
#define ROMSOLUTIONSDIALOG_H

#include <QDialog>
#include <QListWidget>

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

    void clearSolutions();
    void registerSolution(int score, QString solution);

private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::RomSolutionsDialog *ui;
    MaskRomTool *mrt=0;
};

#endif // ROMSOLUTIONSDIALOG_H
