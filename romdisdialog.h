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

    //Updating from GUI or from file load.
    void updateGUISettings();
    bool autocomment=false;
    bool showbits=false;
    bool showdamage=false;

private slots:
    void on_checkComments_checkStateChanged(const Qt::CheckState &arg1);
    void on_radioHex_clicked();
    void on_radioBits_clicked();
    void on_radioDamage_clicked();

private:
    Ui::RomDisDialog *ui;
    MaskRomTool *mrt;
};

#endif // ROMDISDIALOG_H
