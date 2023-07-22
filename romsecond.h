#ifndef ROMSECOND_H
#define ROMSECOND_H

#include <QDialog>

class RomView;
class MaskRomTool;

namespace Ui {
class RomSecond;
}

class RomSecond : public QDialog
{
    Q_OBJECT

public:
    explicit RomSecond(QWidget *parent = nullptr);
    ~RomSecond();
    //The view
    RomView* view=0;

    //Needed to forward keypress events to main window.
    MaskRomTool* mrt=0;
    void keyPressEvent(QKeyEvent *event);

    //Only call this from the matching function in MRT.
    void enableOpenGL(unsigned int antialiasing);

private:
    Ui::RomSecond *ui;
};

#endif // ROMSECOND_H
