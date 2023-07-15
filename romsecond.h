#ifndef ROMSECOND_H
#define ROMSECOND_H

#include <QDialog>

class RomView;

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
    RomView* view;

private:
    Ui::RomSecond *ui;
};

#endif // ROMSECOND_H
