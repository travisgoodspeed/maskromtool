#include "romruledialog.h"
#include "ui_romruledialog.h"

#include "maskromtool.h"
#include "romrule.h"

#include "QDebug"

RomRuleDialog::RomRuleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RomRuleDialog)
{
    ui->setupUi(this);
}

RomRuleDialog::~RomRuleDialog(){
    delete ui;
}


void RomRuleDialog::clearViolations(){
    ui->listWidget->clear();
}
void RomRuleDialog::nextViolation(){
    //Avoid some edge cases by never running on empty.
    if(ui->listWidget->count()==0)
        return;

    //Grab the next row, looping to zero if it's too high.
    int row=ui->listWidget->currentRow();
    row++;
    if(row >= ui->listWidget->count())
        row=0;

    //Then apply it.
    ui->listWidget->setCurrentRow(row);
    on_listWidget_itemDoubleClicked(ui->listWidget->item(row));
}
void RomRuleDialog::addViolation(RomRuleViolation* violation){
    int count=ui->listWidget->count();
    ui->listWidget->addItem(new RomRuleDialogEntry(violation));
    setWindowTitle(QString::asprintf("%d Rule Violations",++count));
}
void RomRuleDialog::removeViolation(RomRuleViolation* violation){
    int count=ui->listWidget->count();
    for(int i=0; i<count; i++){
        RomRuleDialogEntry* entry= (RomRuleDialogEntry*) ui->listWidget->item(i);
        if(!entry) continue;  //Empty entries.
        if(entry->violation==violation){
            //ui->listWidget->removeItemWidget(entry); //Does not unlink it.
            delete entry;     //Also removed from GUI.
            mrt->removeItem(violation);
        }
    }

    setWindowTitle(QString::asprintf("%d Rule Violations",0));
}
void RomRuleDialog::setMaskRomTool(MaskRomTool* maskRomTool){
    mrt=maskRomTool;
}

RomRuleDialogEntry::RomRuleDialogEntry(RomRuleViolation* v){
    violation=v;
    setToolTip(violation->detail);
    setText(violation->title);
    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}


//Double clicking jumps to a violation.
void RomRuleDialog::on_listWidget_itemDoubleClicked(QListWidgetItem *item){
    RomRuleDialogEntry* entry=(RomRuleDialogEntry*) item;
    mrt->centerOn(entry->violation);
}

