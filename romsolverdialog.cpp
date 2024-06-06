#include "romsolverdialog.h"
#include "ui_romsolverdialog.h"

#include "gatograderascii.h"
#include "gatograderbytes.h"
#include "gatograderstring.h"
#include "gatograderyara.h"

#include "maskromtool.h"
#include "gatosolver.h"
#include "gatorom.h"

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

/* This runs through the potential solutions, using the GUI
 * settings.
 */
void RomSolverDialog::on_butSolve_clicked(){
    GatoGrader *grader=0;
    GatoROM *gr=&(mrt->gr);
    QString oldstate=gr->description();
    QString yararule;

    mrt->solutionsDialog.clearSolutions();
    mrt->solutionsDialog.show();

    int index=ui->tabWidget->currentIndex();
    switch(index){
    case 0: // Bytes
        grader=new GatoGraderBytes(ui->editBytes->toPlainText());
        break;
    case 1: // String
        grader=new GatoGraderString(ui->editString->toPlainText());
        break;
    case 2: // ASCII
        grader=new GatoGraderASCII();
        break;
    case 3: // Yara
        tmpfile.open();
        yararule=ui->editYara->toPlainText();
        tmpfile.write(yararule.toStdString().data());
        tmpfile.flush();
        tmpfile.close();

        grader=new GatoGraderYara(tmpfile.fileName());
        break;
    default:
        qDebug()<<"Unknown solver tab"<<index;
        return;
    }

    GatoSolver solver(gr, grader);
    for(solver.init(); !solver.finished(); solver.next()){
        //int state=solver.state;
        int grade=solver.grade();
        //QString statestring=QString::asprintf("%04x", state);
        QString statestring=gr->description();

        if(grade>=90){
            //qDebug()<<"Solution: "<<state;
            mrt->solutionsDialog.registerSolution(grade, statestring);
        }
    }

    mrt->gr.configFromDescription(oldstate);
    mrt->decodeDialog.update();


    if(grader)
        delete grader;
}

