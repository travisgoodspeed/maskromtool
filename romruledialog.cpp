#include "romruledialog.h"
#include "ui_romruledialog.h"

#include "maskromtool.h"
#include "romrule.h"

RomRuleDialog::RomRuleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RomRuleDialog)
{
    ui->setupUi(this);
}

RomRuleDialog::~RomRuleDialog()
{
    delete ui;
}


void RomRuleDialog::clearViolations(){
    ui->listWidget->clear();
}
void RomRuleDialog::addViolation(RomRuleViolation* violation){
    ui->listWidget->addItem(new RomRuleDialogEntry(violation));
}
void RomRuleDialog::removeViolation(RomRuleViolation* violation){
    //TODO
    qDebug()<<"Removing just one violation doesn't work yet, so I'm clearing them all.";
    ui->listWidget->clear();
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
void RomRuleDialog::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    RomRuleDialogEntry* entry=(RomRuleDialogEntry*) item;
    mrt->centerOn(entry->violation);
}

