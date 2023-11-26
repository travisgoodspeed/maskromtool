#include "romdecodedialog.h"
#include "ui_romdecodedialog.h"

#include "maskromtool.h"

RomDecodeDialog::RomDecodeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RomDecodeDialog)
{
    ui->setupUi(this);
}

void RomDecodeDialog::setMaskRomTool(MaskRomTool* maskRomTool){
    Q_ASSERT(maskRomTool);
    mrt=maskRomTool;

    ui->checkFlipX->setCheckState(mrt->gr.flippedx ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->checkFlipY->setCheckState(mrt->gr.flippedy ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->checkInvertBits->setCheckState(mrt->gr.inverted ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->spinRotation->setValue(mrt->gr.angle);
    ui->listBank->item(mrt->gr.bank)->setSelected(true);

    if(mrt->gr.decoder){
        auto item =
            ui->listDecoder->findItems(mrt->gr.decoder->name, Qt::MatchFlag::MatchExactly);
        ui->listDecoder->setCurrentItem(item.value(0));
    }

    updateString();
}

void RomDecodeDialog::updateString(){
    ui->textFlags->setPlainText(mrt->gr.description());
}

RomDecodeDialog::~RomDecodeDialog(){
    delete ui;
}

void RomDecodeDialog::on_listDecoder_itemSelectionChanged(){
    Q_ASSERT(mrt);

    QString decoder=ui->listDecoder->currentItem()->text();
    mrt->gr.setDecoderByName(decoder);
    updateString();
}


void RomDecodeDialog::on_checkFlipX_stateChanged(int arg1){
    Q_ASSERT(mrt);

    //Are we flipping on the X axis?
    mrt->gr.flipx(arg1);
    updateString();
}


void RomDecodeDialog::on_checkFlipY_stateChanged(int arg1){
    Q_ASSERT(mrt);
    mrt->gr.flipy(arg1);
    updateString();
}


void RomDecodeDialog::on_checkZorrom_stateChanged(int arg1){
    Q_ASSERT(mrt);

    //Zorrom bug-compatibility mode.
    mrt->gr.zorrommode=arg1;
    updateString();
}


void RomDecodeDialog::on_checkInvertBits_stateChanged(int arg1){
    Q_ASSERT(mrt);

    //Set the inversion, don't repeat it.
    mrt->gr.inverted=arg1;
    updateString();
}


void RomDecodeDialog::on_spinRotation_valueChanged(int arg1){
    Q_ASSERT(mrt);

    //Sets the rotation, starting at zero.
    mrt->gr.rotate(arg1, true);
    updateString();
}


void RomDecodeDialog::on_listBank_currentItemChanged(QListWidgetItem *current,
                                                     QListWidgetItem *previous){
    Q_ASSERT(mrt);

    //List is in the order of the banking constants.
    int bank=current->listWidget()->currentRow();
    mrt->gr.setBank(bank);
    updateString();
}



