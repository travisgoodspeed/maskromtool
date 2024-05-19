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

//Updates the disassembly view.  GatoROM is responsible for any error messages.
void RomDisDialog::update(){
    Q_ASSERT(mrt);

    ui->plainTextEdit->setPlainText(mrt->gr.dis());
    setWindowTitle(mrt->gr.arch+" Disassembly");
}
