#include "asciidialog.h"
#include "ui_asciidialog.h"

ASCIIDialog::ASCIIDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ASCIIDialog)
{
    ui->setupUi(this);
}

ASCIIDialog::~ASCIIDialog()
{
    delete ui;
}


void ASCIIDialog::setText(QString ascii){
    ui->text->setPlainText(ascii);
}
