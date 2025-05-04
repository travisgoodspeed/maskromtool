#ifndef ROMHEXDIALOG_H
#define ROMHEXDIALOG_H

/* This class is a poor man's hex editor, intended
 * to display the decoded data of a MaskRomTool
 * project.  It has no need for editing, but it's
 * very important that the current working address
 * be fed back to the main application.
 *
 * When showingDamage is set to true, this represents
 * a hex view of the damage mask, showing a 1 for each
 * ambiguous bit.  MaskRomTool maintains a hex view of
 * both the data bytes and the damage bytes.
 */

#include <QDialog>
#include <QByteArray>

class MaskRomTool;

namespace Ui {
class RomHexDialog;
}

class RomHexDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RomHexDialog(QWidget *parent = nullptr);
    ~RomHexDialog();
    void updatebinary(QByteArray bytes);
    void setMaskRomTool(MaskRomTool *mrt);
    uint32_t positionToAdr(int pos);
    int adrToPosition(uint32_t adr);
    int adrToEndPosition(uint32_t adr);

    //Selections are in terms of byte addresses.
    void select(uint32_t adr, uint32_t len);
    uint32_t start=0, end=0;

    //Set to true when showing ambiguous bits in hex.
    bool showingDamage=false;

private slots:
    void on_plaintextHex_selectionChanged();
    void on_buttonShow_clicked();

private:
    Ui::RomHexDialog *ui;
    MaskRomTool *mrt=0;
};

#endif // ROMHEXDIALOG_H
