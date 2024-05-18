#include "romsolverdialog.h"
#include "ui_romsolverdialog.h"

RomSolverDialog::RomSolverDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RomSolverDialog)
{
    ui->setupUi(this);
}

RomSolverDialog::~RomSolverDialog()
{
    delete ui;
}

void RomSolverDialog::setMaskRomTool(MaskRomTool *mrt){
    this->mrt=mrt;
}
