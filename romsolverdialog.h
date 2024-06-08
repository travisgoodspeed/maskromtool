#ifndef ROMSOLVERDIALOG_H
#define ROMSOLVERDIALOG_H

#include <QDialog>
#include <QTemporaryFile>

class MaskRomTool;


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
    void setYaraRule(QString rule);
    QString yararule;

private slots:
    void on_butSolve_clicked();

    void on_editYara_textChanged();

private:
    Ui::RomSolverDialog *ui;
    MaskRomTool *mrt=0;
    QTemporaryFile *tmpfile=0;
};

#endif // ROMSOLVERDIALOG_H
