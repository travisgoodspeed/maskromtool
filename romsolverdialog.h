#ifndef ROMSOLVERDIALOG_H
#define ROMSOLVERDIALOG_H

#include <QDialog>

class MaskRomTool;

#include "gatosolver.h"

namespace Ui {
class RomSolverDialog;
}

class RomSolverDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RomSolverDialog(QWidget *parent = nullptr);
    ~RomSolverDialog();
    void setMaskRomTool(MaskRomTool *mrt);

private slots:
    void on_butSolve_clicked();

private:
    Ui::RomSolverDialog *ui;
    MaskRomTool *mrt=0;
};

#endif // ROMSOLVERDIALOG_H