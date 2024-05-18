#include "romsolutionsdialog.h"
#include "ui_romsolutionsdialog.h"

#include "maskromtool.h"

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

/* Clears out the existing solutions.  Usually called before
 * another solver pass.
 */
void RomSolutionsDialog::clearSolutions(){
    ui->listWidget->clear();
}


/* This registers a solution in the listing, so that
 * the user can select it.
 */
void RomSolutionsDialog::registerSolution(int score, QString solution){
    //Fixme: This should include some hint at the solution.
    ui->listWidget->addItem(solution);
}


/* When a solution is double-clicked, we reconfigure the decoder to
 * match it.  All other results should then line up.
 */
void RomSolutionsDialog::on_listWidget_itemDoubleClicked(QListWidgetItem *item){
    Q_ASSERT(mrt);

    //Mark one undo point and prevent others from being made.
    mrt->markUndoPoint();
    mrt->importLock++;

    //Maybe move this to the tooltip, so that the text can be more descriptive?
    QString solution=item->text();
    qDebug()<<"Loading solution"<<solution;
    mrt->gr.configFromDescription(solution);
    mrt->decodeDialog.update();

    //We're done, so allow undo points again.
    mrt->importLock--;
}

