#ifndef ROMDECODEDIALOG_H
#define ROMDECODEDIALOG_H

/* This dialog is used to configure the decoder in the MRT GUI,
 * so that full decodings can be made including rotations without
 * the CLI.  The CLI will *not* be deprecated, and calling maskromtool
 * and gatorom separately is still preferred for project Makefiles.
 */

#include <QDialog>
#include <QListWidgetItem>

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
    void updateString();
    void update();

private slots:
    void on_listDecoder_itemSelectionChanged();
    void on_checkFlipX_stateChanged(int arg1);
    void on_checkFlipY_stateChanged(int arg1);
    void on_checkZorrom_stateChanged(int arg1);
    void on_checkInvertBits_stateChanged(int arg1);
    void on_spinRotation_valueChanged(int arg1);
    void on_listBank_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_wordsizeEdit_textEdited(const QString &arg1);

    void on_listArchitecture_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    Ui::RomDecodeDialog *ui;
    MaskRomTool *mrt=0;  //GUI project.
};

#endif // ROMDECODEDIALOG_H
