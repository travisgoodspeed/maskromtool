#include "romsolutionsdialog.h"
#include "ui_romsolutionsdialog.h"

RomSolutionsDialog::RomSolutionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RomSolutionsDialog)
{
    ui->setupUi(this);
}

RomSolutionsDialog::~RomSolutionsDialog(){
    delete ui;
}


void RomSolutionsDialog::setMaskRomTool(MaskRomTool *mrt){
    this->mrt=mrt;
}

/* This registers a solution in the listing, so that
 * the user can select it.
 */
void RomSolutionsDialog::registerSolution(int score, QString solution){

}
