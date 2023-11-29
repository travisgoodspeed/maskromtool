#include "romhexdialog.h"
#include "ui_romhexdialog.h"

#include "maskromtool.h"

RomHexDialog::RomHexDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RomHexDialog)
{
    ui->setupUi(this);
}

RomHexDialog::~RomHexDialog(){
    delete ui;
}

void RomHexDialog::updatebinary(QByteArray bytes){
    //Pre-allocate the buffer so we don't keep growing it.
    QString str="";
    str.reserve(bytes.length()*4);

    //Make a giant string of the hex view.
    for(uint32_t i=0; i<bytes.length(); i++){
        if((i%16==0) && i>0)
            str+="\n";
        if(i%16==0)
            str+=QString("%1  ").arg(i, 4, 16, QChar( '0' ));
        str+=QString("%1 ").arg((unsigned char) bytes[i], 2, 16, QChar( '0' ));
    }

    ui->plaintextHex->setPlainText(str);
}


void RomHexDialog::setMaskRomTool(MaskRomTool *mrt){
    this->mrt=mrt;
}
