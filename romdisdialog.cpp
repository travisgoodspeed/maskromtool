#include "romdisdialog.h"
#include "ui_romdisdialog.h"
#include "maskromtool.h"

RomDisDialog::RomDisDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RomDisDialog)
{
    /* macOS is really picky, but this seems to work
     * on Mac, Linux and Windows.  Removing the style hint
     * breaks Windows.
     */
    QFont font("Andale Mono"); // macOS
    font.setStyleHint(QFont::Monospace);

    ui->setupUi(this);
    ui->plainTextEdit->setFont(font);
}

RomDisDialog::~RomDisDialog(){
    delete ui;
}


void RomDisDialog::setMaskRomTool(MaskRomTool *mrt){
    this->mrt=mrt;
}

//Updates GUI from class settings.
void RomDisDialog::updateGUISettings(){
    ui->checkComments->setCheckState(autocomment?Qt::Checked:Qt::Unchecked);
    ui->radioBits->setChecked(showbits);
    ui->radioDamage->setChecked(showdamage);
    if(!showbits && !showdamage)
        ui->radioHex->setChecked(true);
}

//Updates the disassembly view.  GatoROM is responsible for any error messages.
void RomDisDialog::update(){
    Q_ASSERT(mrt);

    //Update the settings.
    autocomment=ui->checkComments->isChecked();
    showbits=ui->radioBits->isChecked();
    showdamage=ui->radioDamage->isChecked();

    //Redraw the view.
    ui->plainTextEdit->setPlainText(
        mrt->gr.dis(
            autocomment,
            showbits,
            ui->radioHex->isChecked(),
            showdamage
            ));
    setWindowTitle(mrt->gr.arch+" Disassembly");
}

/* In any update of the settings, we just rerender the disassmbly.
 * Somewhat complicating matters, the event of the checkComments changing
 * state is inconsistent between Qt versions.
 */

void RomDisDialog::on_checkComments_toggled(bool checked){
    update();
}

void RomDisDialog::on_radioHex_toggled(bool checked){
    update();
}

void RomDisDialog::on_radioBits_toggled(bool checked){
    update();
}

void RomDisDialog::on_radioDamage_toggled(bool checked){
    update();
}

