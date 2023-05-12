#include "romthresholddialog.h"
#include "ui_romthresholddialog.h"

#include "maskromtool.h"

#include "rombititem.h"


/* This tool was designed hastily, and the mrt
 * public symbol is unfortunately one of the
 * blockers to maintaining multiple projects
 * in memory simultaneously.
 *
 * FIXME Remove this symbol from the global scope.
 */
MaskRomTool *mrt;

RomThresholdDialog::RomThresholdDialog() :
    QDialog(),
    ui(new Ui::RomThresholdDialog)
{
    ui->setupUi(this);

    chartview = new QChartView();
    chartview->setRenderHint(QPainter::Antialiasing);

    ui->verticalLayout->addWidget(chartview);
}

void RomThresholdDialog::setMaskRomTool(MaskRomTool* parent){
    mrt=parent;

    //This gets corrupted later, so we cache it early.
    int samplersize=mrt->getSamplerSize();

    qreal r=3, g=3, b=3;
    mrt->getBitThreshold(r,g,b);

    //This must come first, or we'll redraw the bits and destroy the thresholds.
    ui->sizeScrollBar->setValue(mrt->getBitSize());

    //We've buffered these values, so they won't jump around.
    ui->redScrollBar->setValue((int) r);
    ui->greenScrollBar->setValue((int) g);
    ui->blueScrollBar->setValue((int) b);
    ui->redEdit->setText(QString::number((int) r));
    ui->greenEdit->setText(QString::number((int) g));
    ui->blueEdit->setText(QString::number((int) b));

    //Populate the chooser box with each sampler algorithm.
    ui->samplesizeScrollBar->setValue(samplersize);
    ui->samplesizeEdit->setText(QString::number(samplersize));
    foreach(RomBitSampler* sampler, mrt->samplers){
        ui->samplerBox->addItem(sampler->name);
    }
    ui->samplerBox->setCurrentText(mrt->sampler->name);

    setModal(false);
}

RomThresholdDialog::~RomThresholdDialog(){
    //Bad things happen if we delete the view but not the chart,
    //so we make a fake chart and then delete the UI.
    QChart tmpchart;
    chartview->setChart(&tmpchart);

    delete ui;
}

void RomThresholdDialog::setChart(QChart *chart){
    chartview->setChart(chart);
}

void RomThresholdDialog::refreshStats(){
    float r=0, g=0, b=0;
    float red=0, redmax=0,
          green=0, greenmax=0,
          blue=0, bluemax=0;
    uint64_t count=0;

    //Make sure we know where the bits are.
    mrt->remarkBits();

    //Average the values.
    foreach (RomBitItem *bit, mrt->bits){
        QRgb rgb=bit->bitvalue_raw(mrt, mrt->background);
        r=(rgb>>16)&0xFF;
        red+=r;
        g=(rgb>>8)&0xFF;
        green+=g;
        b=(rgb)&0xFF;
        blue+=b;

        redmax=(redmax>r)?redmax:r;
        greenmax=(greenmax>g)?greenmax:g;
        bluemax=(bluemax>b)?bluemax:b;

        count++;
    }
    red/=count;
    green/=count;
    blue/=count;

    average_red=red;
    average_green=green;
    average_blue=blue;

    //Maybe move this to a button?
    //mrt->setBitThreshold(red, green, blue);
    mrt->remarkBits();

    //Print some debugging items.
    if(verbose){
        qDebug()<<"Pixel averages are "<<red<<","<<green<<","<<blue;
        qDebug()<<"Pixel maxes are "<<redmax<<","<<greenmax<<","<<bluemax;
    }
}

void RomThresholdDialog::on_averageButton_clicked(){
    refreshStats();

    //Apply the values.
    ui->redScrollBar->setValue((int) average_red);
    ui->greenScrollBar->setValue((int) average_green);
    ui->blueScrollBar->setValue((int) average_blue);
}

void RomThresholdDialog::postThresholds(){
    //Sends the thresholds back to MRT.
    mrt->setBitThreshold(ui->redScrollBar->value(),
                         ui->greenScrollBar->value(),
                         ui->blueScrollBar->value());

    //Updates the textboxes.
    ui->redEdit->setText(QString::number((int) ui->redScrollBar->value()));
    ui->greenEdit->setText(QString::number((int) ui->greenScrollBar->value()));
    ui->blueEdit->setText(QString::number((int) ui->blueScrollBar->value()));

    //Updates the text size.
    mrt->setBitSize(ui->sizeScrollBar->value());

    //Updates the sampler box.
    if(ui->samplerBox->currentText()!=""){
        mrt->chooseSampler(ui->samplerBox->currentText());
        mrt->setSamplerSize(ui->samplesizeScrollBar->value());
        ui->samplesizeEdit->setText(QString::number((int)ui->samplesizeScrollBar->value()));
    }

    //Redraws the bits.
    mrt->remarkBits();
}


void RomThresholdDialog::on_redScrollBar_valueChanged(int value){
    postThresholds();
}

void RomThresholdDialog::on_greenScrollBar_valueChanged(int value){
    postThresholds();
}

void RomThresholdDialog::on_blueScrollBar_valueChanged(int value){
    postThresholds();
}

void RomThresholdDialog::on_sizeScrollBar_valueChanged(int value){
    postThresholds();
}



void RomThresholdDialog::on_redEdit_textEdited(const QString &arg1){
    bool ok=false;
    int val=arg1.toInt(&ok,10);
    if(ok)
        ui->redScrollBar->setValue(val);
}
void RomThresholdDialog::on_greenEdit_textEdited(const QString &arg1){
    bool ok=false;
    int val=arg1.toInt(&ok,10);
    if(ok)
        ui->greenScrollBar->setValue(val);
}
void RomThresholdDialog::on_blueEdit_textEdited(const QString &arg1){
    bool ok=false;
    int val=arg1.toInt(&ok,10);
    if(ok)
        ui->blueScrollBar->setValue(val);
}

void RomThresholdDialog::on_samplerBox_activated(int index){
    mrt->chooseSampler(ui->samplerBox->currentText());
}

void RomThresholdDialog::on_samplesizeEdit_textEdited(const QString &arg1){
    bool ok=false;
    int val=arg1.toInt(&ok,10);
    if(ok)
        ui->samplesizeScrollBar->setValue(val);
}


void RomThresholdDialog::on_samplesizeScrollBar_valueChanged(int value){
    postThresholds();
}

