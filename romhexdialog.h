#ifndef ROMHEXDIALOG_H
#define ROMHEXDIALOG_H

/* This class is a poor man's hex editor, intended
 * to display the decoded data of a MaskRomTool
 * project.  It has no need for editing, but it's
 * very important that the current working address
 * be fed back to the main application.
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

private:
    Ui::RomHexDialog *ui;
    MaskRomTool *mrt=0;
};

#endif // ROMHEXDIALOG_H
