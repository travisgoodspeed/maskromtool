#ifndef ROMSTRINGSDIALOG_H
#define ROMSTRINGSDIALOG_H

#include <QDialog>
#include <QByteArray>

class MaskRomTool;

namespace Ui {
class RomStringsDialog;
}

class RomStringsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RomStringsDialog(QWidget *parent = nullptr);
    ~RomStringsDialog();
    void updatebinary(QByteArray bytes);
    void setMaskRomTool(MaskRomTool *mrt);
    int minLength=7;

private:
    Ui::RomStringsDialog *ui;
    MaskRomTool *mrt=0;
    void registerString(int adr, QString string);
};

#endif // ROMSTRINGSDIALOG_H
