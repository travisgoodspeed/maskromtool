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
    void on_checkComments_toggled(bool checked);
    void on_radioHex_toggled(bool checked);
    void on_radioBits_toggled(bool checked);
    void on_radioDamage_toggled(bool checked);

private:
    Ui::RomDisDialog *ui;
    MaskRomTool *mrt;
};

#endif // ROMDISDIALOG_H
