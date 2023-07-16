#include "romsecond.h"
#include "ui_romsecond.h"

#include "maskromtool.h"

RomSecond::RomSecond(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RomSecond)
{
    ui->setupUi(this);
    view=ui->graphicsView;
}

RomSecond::~RomSecond(){
    delete ui;
}


void RomSecond::keyPressEvent(QKeyEvent *event){
    if(mrt){
        mrt->keyPressEvent(event);
    }
}
