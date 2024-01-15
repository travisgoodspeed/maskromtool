#include "romstringsdialog.h"
#include "ui_romstringsdialog.h"

#include "maskromtool.h"

RomStringsDialog::RomStringsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RomStringsDialog)
{
    ui->setupUi(this);
}

RomStringsDialog::~RomStringsDialog()
{
    delete ui;
}


void RomStringsDialog::updatebinary(QByteArray bytes){
    QString longest="";
    QString last="";
    uint32_t longestcount=0, lastcount=0;

    //Clear the old items.
    ui->listWidget->clear();

    int adr=0;
    for(int i=0; i<bytes.length(); i++){
        char c=bytes[i]; //Must be signed!
        if(((int) c)>=32 && ((int) c)!=127){
            //Just grow the string.
            last.append(c);
            lastcount++;
        }else{
            if(lastcount>minLength){
                longest=last;
                longestcount=lastcount;
                registerString(adr, last);
            }
            last="";
            lastcount=0;
            adr=i+1;
        }
    }
}


void RomStringsDialog::setMaskRomTool(MaskRomTool *mrt){
    this->mrt=mrt;
}


void RomStringsDialog::registerString(int adr, QString string){
    ui->listWidget->addItem("0x"+QString::number(adr,16)+"\t"+string);
}
