#include "romsolverdialog.h"
#include "ui_romsolverdialog.h"

#include "gatograderascii.h"
#include "gatograderbytes.h"
#include "gatograderstring.h"
#include "gatograderyara.h"
#include "gatogradergoodasm.h"

#include "maskromtool.h"
#include "gatosolver.h"
#include "gatorom.h"
#include "extern/goodasm/goodasm.h"


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

void RomSolverDialog::setYaraRule(QString rule){
    yararule=rule;
    ui->editYara->setPlainText(yararule);
}

void RomSolverDialog::on_editYara_textChanged(){
    yararule=ui->editYara->toPlainText();
}

/* Returns a new GatoGrader based upon the GUI settings.
 */
GatoGrader* RomSolverDialog::grader(){
    GatoGrader *grader=0;

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
        if(tmpfile)
            delete tmpfile;
        tmpfile=new QTemporaryFile();
        tmpfile->open();
        tmpfile->write(yararule.toStdString().data());
        tmpfile->flush();
        tmpfile->close();
        grader=new GatoGraderYara(tmpfile->fileName());
        break;
    case 4: // GoodASM
        grader=new GatoGraderGoodAsm(mrt->gatorom().goodasm());
        break;
    default:
        qDebug()<<"Unknown solver tab"<<index;
        break;
    }

    return grader;
}

/* This generates the GUI solutions and populates the solution set.
 * If "solveset" is not empty, each solution will be exported as
 * well.
 */
void RomSolverDialog::solve(QString solveset){
    GatoGrader *grader=0;
    GatoROM *gr=&(mrt->gr);
    QString oldstate=gr->description();

    mrt->solutionsDialog.clearSolutions();
    mrt->solutionsDialog.show();

    grader=this->grader();
    if(!grader) return;

    GatoSolver solver(gr, grader);
    for(solver.init(); !solver.finished(); solver.next()){
        int grade=solver.grade();
        QString statestring=gr->description();

        if(grade>=90){
            mrt->solutionsDialog.registerSolution(grade, statestring);

            //When called from File/Export/SolveSetBINs
            if(solveset.length()>0){
                QFile outfile(solveset+gr->descriptiveFilename()+".bin");
                outfile.open(QIODevice::WriteOnly);
                outfile.write(gr->decoded);
                outfile.close();
            }
        }
    }

    //Only show last error to avoid flooding.
    if(grader && grader->error.length()>0){
        QMessageBox msgBox;
        msgBox.setText(grader->error);
        msgBox.exec();
    }


    mrt->gr.configFromDescription(oldstate);
    mrt->decodeDialog.update();

    if(grader)
        delete grader;
}

/* This runs through the potential solutions, using the GUI
 * settings.
 */
void RomSolverDialog::on_butSolve_clicked(){
    solve();
}



