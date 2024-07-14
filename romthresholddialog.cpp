#include "romthresholddialog.h"
#include "ui_romthresholddialog.h"

#include "maskromtool.h"

#include "rombititem.h"

/* This dialog is used to choose the bit threshold colors,
 * and also to view the color channel histograms that
 * are needed to choose the right threshold.
 */


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
    ui->checkInverted->setCheckState(mrt->inverted?Qt::CheckState::Checked:Qt::CheckState::Unchecked);

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
    if(ui->samplerBox->count()==0){ //Only update the first time.
        foreach(RomBitSampler* sampler, mrt->samplers){
            ui->samplerBox->addItem(sampler->name);
        }
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

void RomThresholdDialog::refreshStats(bool remarkbits){
    float r=0, g=0, b=0;
    float red=0, redmax=0,
          green=0, greenmax=0,
          blue=0, bluemax=0;
    uint64_t count=0;

    if(!mrt) //Can't update stats without bits.
        return;

    //Make sure we know where the bits are.
    if(remarkbits)
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
    if(remarkbits)
        mrt->remarkBits();

    //Print some debugging items.
    if(verbose){
        qDebug()<<"Pixel averages are "<<red<<","<<green<<","<<blue;
        qDebug()<<"Pixel maxes are "<<redmax<<","<<greenmax<<","<<bluemax;
    }
}

void RomThresholdDialog::on_averageButton_clicked(){
    mrt->markUndoPoint();
    refreshStats();

    //Apply the values.
    ui->redScrollBar->setValue((int) average_red);
    ui->greenScrollBar->setValue((int) average_green);
    ui->blueScrollBar->setValue((int) average_blue);
}

void RomThresholdDialog::postThresholds(){
    //Updates the sampler box.
    if(ui->samplerBox->currentText()!=""){
        mrt->chooseSampler(ui->samplerBox->currentText());
        mrt->setSamplerSize(ui->samplesizeScrollBar->value());
        ui->samplesizeEdit->setText(QString::number((int)ui->samplesizeScrollBar->value()));
    }

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

    //Redraws the bits.
    mrt->remarkBits();
}

/* We do *not* mark an undo point when the slider values are changed
 * for performance.  Instead we only save the new state when the
 * slider is released.
 */
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
    if(ok){
        mrt->markUndoPoint();
        ui->redScrollBar->setValue(val);
        mrt->gatorom();
    }
}
void RomThresholdDialog::on_greenEdit_textEdited(const QString &arg1){
    bool ok=false;
    int val=arg1.toInt(&ok,10);
    if(ok){
        mrt->markUndoPoint();
        ui->greenScrollBar->setValue(val);
        mrt->gatorom();
    }
}
void RomThresholdDialog::on_blueEdit_textEdited(const QString &arg1){
    bool ok=false;
    int val=arg1.toInt(&ok,10);
    if(ok){
        mrt->markUndoPoint();
        ui->blueScrollBar->setValue(val);
        mrt->gatorom();
    }
}

void RomThresholdDialog::on_samplerBox_activated(int index){
    mrt->markUndoPoint();
    mrt->chooseSampler(ui->samplerBox->currentText());
}

void RomThresholdDialog::on_samplesizeEdit_textEdited(const QString &arg1){
    bool ok=false;
    int val=arg1.toInt(&ok,10);

    if(ok){
        mrt->markUndoPoint();
        ui->samplesizeScrollBar->setValue(val);
        postThresholds();
        mrt->gatorom();
    }
}

void RomThresholdDialog::on_samplesizeScrollBar_valueChanged(int value){
    postThresholds();
}


void RomThresholdDialog::on_checkInverted_stateChanged(int arg1){
    mrt->markUndoPoint();
    //2 means checked, 0 means unchecked.
    mrt->inverted=(arg1==2);
    postThresholds();
    mrt->gatorom();

}

/* We update the colors a lot, but we only mark an undo point
 * when we press the slider, as that's when it has the *old*
 * value.
 */
void RomThresholdDialog::on_redScrollBar_sliderPressed(){
    mrt->markUndoPoint();
}
void RomThresholdDialog::on_greenScrollBar_sliderPressed(){
    mrt->markUndoPoint();
}
void RomThresholdDialog::on_blueScrollBar_sliderPressed(){
    mrt->markUndoPoint();
}
void RomThresholdDialog::on_samplesizeScrollBar_sliderPressed(){
    mrt->markUndoPoint();
}

/* Updating bits and decodings is expensive, so we only do it
 * when the slider is released.
 */
void RomThresholdDialog::on_redScrollBar_sliderReleased(){
    mrt->gatorom();
}
void RomThresholdDialog::on_greenScrollBar_sliderReleased(){
    mrt->gatorom();
}
void RomThresholdDialog::on_blueScrollBar_sliderReleased(){
    mrt->gatorom();
}
void RomThresholdDialog::on_samplesizeScrollBar_sliderReleased(){
    mrt->gatorom();
}



