#include "romstringsdialog.h"
#include "ui_romstringsdialog.h"

#include "maskromtool.h"

RomStringsDialog::RomStringsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RomStringsDialog)
{
    ui->setupUi(this);
}

RomStringsDialog::~RomStringsDialog(){
    delete ui;
}


void RomStringsDialog::updatebinary(QByteArray bytes){
    QString longest="";
    QString last="";
    uint32_t lastcount=0;

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

void RomStringsDialog::on_listWidget_itemDoubleClicked(QListWidgetItem *item){
    /* Apologies for the shotgun parser.  The right way to do this would be
     * to subclass QListWidgetItem, but I don't think it's worth that complexity
     * for a feature that few users will appreciate. --Travis
     */
    QString str=item->text();
    QStringList words=str.split("\t");
    bool okay=false;
    uint32_t adr=0;
    uint32_t len=0;
    assert(mrt);

    if(words.length()>1){
        QString adrstr=words[0];
        adr=adrstr.toUInt(&okay, 16);
        len=str.length()-words[0].length()-1;
        assert(okay);
        qDebug()<<"Should be selecting adr "<<QString::number(adr,16).prepend("0x");
        mrt->hexDialog.select(adr,len);
    }
}

