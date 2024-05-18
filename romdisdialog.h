#ifndef ROMDISDIALOG_H
#define ROMDISDIALOG_H

#include <QDialog>

class MaskRomTool;

namespace Ui {
class RomDisDialog;
}

class RomDisDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RomDisDialog(QWidget *parent = nullptr);
    ~RomDisDialog();
    void setMaskRomTool(MaskRomTool *mrt);
    void update();

private:
    Ui::RomDisDialog *ui;
    MaskRomTool *mrt;
};

#endif // ROMDISDIALOG_H
