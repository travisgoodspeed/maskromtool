#ifndef ROMDECODEDIALOG_H
#define ROMDECODEDIALOG_H

#include <QDialog>

class MaskRomTool;

namespace Ui {
class RomDecodeDialog;
}

class RomDecodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RomDecodeDialog(QWidget *parent = nullptr);
    ~RomDecodeDialog();
    void setMaskRomTool(MaskRomTool* maskRomTool);

private slots:
    void on_listDecoder_itemSelectionChanged();

    void on_checkFlipX_stateChanged(int arg1);

    void on_checkZorrom_stateChanged(int arg1);

    void on_checkInvertBits_stateChanged(int arg1);

    void on_spinRotation_valueChanged(int arg1);

private:
    Ui::RomDecodeDialog *ui;
    MaskRomTool *mrt=0;
};

#endif // ROMDECODEDIALOG_H
