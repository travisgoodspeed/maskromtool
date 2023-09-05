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

    if(mrt->gr.decoder){
        auto item =
            ui->listDecoder->findItems(mrt->gr.decoder->name, Qt::MatchFlag::MatchExactly);
        ui->listDecoder->setCurrentItem(item.value(0));
    }

}

RomDecodeDialog::~RomDecodeDialog(){
    delete ui;
}

void RomDecodeDialog::on_listDecoder_itemSelectionChanged(){
    Q_ASSERT(mrt);

    QString decoder=ui->listDecoder->currentItem()->text();
    mrt->gr.setDecoderByName(decoder);
}


void RomDecodeDialog::on_checkFlipX_stateChanged(int arg1){
    Q_ASSERT(mrt);

    mrt->gr.flipx(arg1);
}


void RomDecodeDialog::on_checkZorrom_stateChanged(int arg1){
    Q_ASSERT(mrt);

    mrt->gr.zorrommode=arg1;
}


void RomDecodeDialog::on_checkInvertBits_stateChanged(int arg1){
    Q_ASSERT(mrt);

    mrt->gr.invert(arg1);
}


void RomDecodeDialog::on_spinRotation_valueChanged(int arg1){
    Q_ASSERT(mrt);

    mrt->gr.rotate(arg1, true);
}

