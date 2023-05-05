#include "romaligndialog.h"
#include "ui_romaligndialog.h"

#include "maskromtool.h"

RomAlignDialog::RomAlignDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RomAlignDialog){
    ui->setupUi(this);
}

RomAlignDialog::~RomAlignDialog(){
    delete ui;
}

void RomAlignDialog::setMaskRomTool(MaskRomTool* parent){
    this->mrt=parent;
    static uint32_t threshold;
    mrt->getAlignSkipCountThreshold(threshold);

    ui->editMaxSkip->setText(QString::number(threshold));
}


//Called whenever the text changes.
void RomAlignDialog::on_editMaxSkip_textChanged(const QString &arg1){
    qDebug()<<"Value set to"<<arg1;

    bool ok;
    uint32_t threshold=arg1.toInt(&ok, 10);
    if(ok){
        mrt->setAlignSkipCountThreshold(threshold);
    }else{
        qDebug()<<"Illegal threshold number: "<<arg1;
    }
}

