#ifndef ROMALIGNDIALOG_H
#define ROMALIGNDIALOG_H

class MaskRomTool;

#include <QDialog>

namespace Ui {
class RomAlignDialog;
}

class RomAlignDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RomAlignDialog(QWidget *parent = nullptr);
    ~RomAlignDialog();
    void setMaskRomTool(MaskRomTool* parent);

private slots:
    void on_editMaxSkip_textChanged(const QString &arg1);

    void on_comboBox_activated(int index);

private:
    Ui::RomAlignDialog *ui;
    MaskRomTool *mrt;
};

#endif // ROMALIGNDIALOG_H
