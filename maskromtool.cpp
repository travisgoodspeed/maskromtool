/* This only constructs the GUI.  See main.cpp
 * if you are looking for the CLI argument parsing.
 */

#include "maskromtool.h"
#include "gatorom.h"
#include "maskromtool_autogen/include/ui_maskromtool.h"
#include "romsecond.h"
#include "romlineitem.h"
#include "rombititem.h"
#include "rombitfix.h"
#include "romscene.h"
#include "romthresholddialog.h"
#include "aboutdialog.h"

#include "romalignernew.h"
#include "rombitsamplerwide.h"
#include "rombitsamplertall.h"

//Decoders should be abstracted more, and menus auto-generated.
#include "romdecoderascii.h"
#include "romdecodercsv.h"
#include "romdecoderpython.h"
#include "romdecoderjson.h"
#include "romdecoderphotograph.h"
#include "romdecodergato.h"
//Importers too, weird as they are.
#include "romencoderdiff.h"

//DRC rules.
#include "romrulecount.h"
#include "romruleduplicate.h"
#include "romrulesanity.h"
#include "romruleambiguous.h"

//Qt libraries.  Faster to include in .cpp than .h.
#include <QFileDialog>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QImageReader>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QtGlobal>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <QtMath>
#include <QProgressDialog>

//Printing
#include <QPrinter>
#include <QPrintDialog>


using namespace std;


MaskRomTool::MaskRomTool(QWidget *parent, bool opengl)
    : QMainWindow(parent)
    , ui(new Ui::MaskRomTool) {


    //Set up the main window.
    ui->setupUi(this);
    scene=new RomScene();
    scene->maskRomTool=this;
    ui->graphicsView->setScene(scene);
    view=ui->graphicsView;
    violationDialog.setMaskRomTool(this);
    decodeDialog.setMaskRomTool(this);
    hexDialog.setMaskRomTool(this);
    RomRuleViolation::bitSize=bitSize;
    lineColor = QColor(Qt::black);

    //Strategies should be initialized.
    aligner=new RomAlignerNew();
    addSampler(new RomBitSampler());
    addSampler(new RomBitSamplerWide());
    addSampler(new RomBitSamplerTall());

    //Set up the second view.
    second.view->setScene(scene);

    //Enable OpenGL without antialiasing, now that it's stable.
    if(opengl)
        enableOpenGL(0);
}


/* Undo works by saving the state during important actions
 * to one of two stacks, and moving the state between those
 * stacks.
 */
void MaskRomTool::undo(){
    if(undostack.isEmpty())
        return;

    if(verbose)
        qDebug()<<"Proceeding with undo. "<<undostack.size()<<"undos available.";

    //Move an item from the undo stack to redo stack.
    QJsonObject oldstate=exportJSON();
    QJsonObject newstate=undostack.pop();
    redostack.push(oldstate);
    clear();
    importJSON(newstate);
}
void MaskRomTool::redo(){
    if(redostack.isEmpty())
        return;

    //Move a item from the redo stack to undo stack.
    QJsonObject oldstate=exportJSON();
    QJsonObject newstate=redostack.pop();
    undostack.push(oldstate);
    clear();
    importJSON(newstate);
}
void MaskRomTool::markUndoPoint(){
    /* We don't mark save states in the middle of file loads,
     * as that would trigger recursion that would exhaust the
     * call stack.  Same goes for restoring undoes.
     */
    if(importLock>0)
        return;

    if(verbose)
        qDebug()<<"Marking undo point"<<undostack.size();

    //Marking a point is just saving the state and emptying the redo buffer.
    undostack.push(exportJSON());
    redostack.empty();
}
void MaskRomTool::clear(){
    /* For performance, we are just obliterating the entire state
     * and not taking care to update things individually.  This is
     * why we call .remove() and delete directly, rather than
     * calling MaskRomTool::removeItem().
     */

    foreach (RomBitItem* item, bits){
        //removeItem(item);
        bits.remove(item);
        delete item;
    }
    assert(bits.isEmpty());
    bits.empty();
    foreach (RomBitFix* item, bitfixes){
        //removeItem(item);
        bitfixes.remove(item);
        delete item;
    }
    assert(bitfixes.isEmpty());
    bitfixes.empty();
    foreach (RomLineItem* item, rows){
        //removeItem(item);  //Slow!
        rows.remove(item);
        delete item;
    }
    assert(rows.isEmpty());
    rows.empty();
    foreach (RomLineItem* item, cols){
        //removeItem(item);  //Slow!
        cols.remove(item);
        delete item;
    }
    assert(cols.isEmpty());
    cols.empty();

    markBits();
}


//Returns a GatoROM structure of the bits.
GatoROM MaskRomTool::gatorom(){
    RomDecoderGato exporter;
    exporter.gatorom(this);
    hexDialog.updatebinary(gr.decode());
    return this->gr;
}

//Prints the bits.
void MaskRomTool::on_actionPrint_triggered(){
    GatoROM gr=gatorom();
    QPrinter printer;

    QPrintDialog dialog(&printer);
    dialog.setWindowTitle("Print Bits");
    if (dialog.exec() == QDialog::Accepted)
        gr.print(printer);
}

//Adds support for a sampler.  Does not select it.
void MaskRomTool::addSampler(RomBitSampler *sampler){
    samplers.insert(sampler);

    //The first one to be inserted is the default.
    if(!this->sampler)
        this->sampler=sampler;
}

//Chooses a sampler by name.
void MaskRomTool::chooseSampler(QString name){
    //Fast path if we've already selected it.
    if(sampler->name==name)
        return;

    //Slow path if we need to find it.
    foreach(RomBitSampler* sampler, samplers){
        if(sampler->name==name){
            this->sampler=sampler;
            if(verbose)
                qDebug()<<"Selected sampler"<<sampler->name;
            remarkBits();
            return;
        }
    }

    //Error path.
    qDebug()<<"Missing sampler algorithm"<<name;
    qDebug()<<"Defaulting to sampler algoirthm"<<sampler->name;
}

void MaskRomTool::setSamplerSize(int size){
    sampler->setSize(size);
    getSamplerSize();  //Grab it back in case the size was illegal.
}
int MaskRomTool::getSamplerSize(){
    int size=sampler->getSize();
    return size;
}

void MaskRomTool::on_actionOpenGL_triggered(){
    //Do the switch!
    enableOpenGL();
}

void MaskRomTool::enableOpenGL(unsigned int antialiasing){
    //Enabled OpenGL.
    QOpenGLWidget *gl = new QOpenGLWidget();
    if(antialiasing>0){
        QSurfaceFormat format;
        format.setSamples(antialiasing);
        gl->setFormat(format);
    }
    gl->setMouseTracking(true); //Required to track all mouse clicks.
    view->setViewport(gl);

    second.enableOpenGL(antialiasing);

    //One way street.
    ui->actionOpenGL->setEnabled(false);
    ui->actionOpenGL->setChecked(true);
}

//Defined as extern in maskromtool.h.
unsigned int verbose=0;
//Set the verbose value.
void MaskRomTool::enableVerbose(unsigned int level){
    qDebug()<<"Enabling verbose mode level"<<level;
    verbose=level;
}

MaskRomTool::~MaskRomTool() {
    //ui->graphicsView->setScene(0);
    //delete scene;
    delete ui;
}


//Marks all of the bits that are on a line.
void MaskRomTool::removeLine(RomLineItem* line, bool fromsets){
    /* Regenerating all bits takes forever.  The purpose of this function
     * is to quickly remove the bits from a single line along with the
     * line itself.
     *
     * If 'fromsets' is true, we're completely removing it.
     * If 'fromsets' is false, we're just removing the bits and probably moving the line.
     */

    //qDebug()<<"Removing all bits from a line.";
    foreach(QGraphicsItem* item, scene->collidingItems(line)){
        if(item->type()==QGraphicsItem::UserType+2){  //Is it a bit?
            //qDebug()<<"Removing a bit.";
            removeItem(item);
        }
    }

    //After removing the bits, we've gotta remove the line itself.
    if(fromsets)
        switch(line->type()){
        case QGraphicsItem::UserType: //row
            rows.remove((RomLineItem*) line);
            break;
        case QGraphicsItem::UserType+1: //column
            cols.remove((RomLineItem*) line);
            break;
        }
}


//Always call this to remove an item, to keep things clean.
void MaskRomTool::removeItem(QGraphicsItem* item){
    switch(item->type()){
    case QGraphicsItem::UserType: //row
    case QGraphicsItem::UserType+1: //column
        removeLine((RomLineItem*) item);
        break;
    case QGraphicsItem::UserType+2: //bit
        bits.remove((RomBitItem*) item);
        //alignmentdirty=true;
        break;
    case QGraphicsItem::UserType+3: //bit fix
        bitfixes.remove((RomBitFix*) item);
        alignmentdirty=true;
        break;
    case QGraphicsItem::UserType+4: //rule violation
        violations.remove((RomRuleViolation*) item);
        //Gotta remove the violation so that it isn't used after a free.
        violationDialog.removeViolation((RomRuleViolation*) item);
        //Violations do not dirty the alignment.
        break;
    default:
        qDebug()<<"Removing unknown type"<<item->type()<<"in object"<<item;
    }

    //Remove the item if it's a part of the scene.
    if(item->scene()==scene){
        /* Documentation of QGraphicsItem says that it's
         * faster to remove it first and then destroy it,
         * but my benchmarking says the opposite.
         * --Travis
         */

        //scene->removeItem(item);
    }
    //Leaking memory would be bad.
    delete item;
}

//Switch between viewing modes.
void MaskRomTool::nextMode(){
    setBitsVisible(!bitsVisible);
    setViolationsVisible(bitsVisible);
    ui->actionBits->setChecked(bitsVisible);
    ui->actionViolations->setChecked(bitsVisible);
}

void MaskRomTool::on_actionPhotograph_triggered(){
    static QBrush whitebrush(Qt::GlobalColor::white, Qt::SolidPattern);
    if(ui->actionPhotograph->isChecked()){
        view->setBackgroundBrush(background);
    }else{
        view->setBackgroundBrush(whitebrush);
    }
}
void MaskRomTool::on_actionRowsColumns_triggered(){
    setLinesVisible(ui->actionRowsColumns->isChecked());
}
void MaskRomTool::on_actionBits_triggered(){
    setBitsVisible(ui->actionBits->isChecked());
}
void MaskRomTool::on_actionViolations_triggered(){
    setViolationsVisible(ui->actionViolations->isChecked());
}
void MaskRomTool::on_actionViolationsDialog_triggered(){
    this->violationDialog.show();
}
void MaskRomTool::on_actionCrosshair_triggered(){
    scene->setCrosshairVisible(ui->actionCrosshair->isChecked());
}


void MaskRomTool::setLinesVisible(bool b){
    foreach (QGraphicsLineItem* item, rows){
        item->setVisible(b);
    }
    foreach (QGraphicsLineItem* item, cols){
        item->setVisible(b);
    }
}
void MaskRomTool::setBitsVisible(bool b){
    bitsVisible=b; //Default for new bits.
    foreach (QGraphicsItem* item, bits){
        item->setVisible(b);
    }
    foreach (QGraphicsItem* item, bitfixes){
        item->setVisible(b);
    }
}
void MaskRomTool::setViolationsVisible(bool b){
    foreach (QGraphicsItem* item, violations){
        item->setVisible(b);
    }
}

//Highlights one item close to the center.
void MaskRomTool::centerOn(QGraphicsItem* item){
    view->centerOn(item);
}


//We might be a GUI, but keyboards are where it's add!
void MaskRomTool::keyPressEvent(QKeyEvent *event){
    RomLineItem *rlitem;
    int key=event->key();

    switch(key){
    //Zoom/view keys.
    case Qt::Key_H: //Home button.
        if(event->modifiers()&Qt::SHIFT){ //Set home.
            home=scene->scenepos;
        }else{ //Jump to home when no modifiers are held.
            ui->graphicsView->centerOn(home);
        }
        break;
    case Qt::Key_Q: //Reset Zoom
        ui->graphicsView->resetTransform();
        ui->graphicsView->totalScaleFactor=1;
        break;
    case Qt::Key_A:
        if(event->modifiers()&Qt::SHIFT){ //Damage/Ambiguate bit.
            markUndoPoint();
            damageBit(scene->scenepos);
        }else{ //Zoom in.
            ui->graphicsView->scale(1/1.2);
        }
        break;
    case Qt::Key_Z: //Undo, Redo, Zoom out.
        if(event->modifiers()&Qt::CTRL && !(event->modifiers()&Qt::SHIFT))
            undo();
        else if(event->modifiers()&Qt::CTRL && event->modifiers()&Qt::SHIFT)
            redo();
        else //zoom out
            ui->graphicsView->scale(1.2);
        break;
    case Qt::Key_Tab: //Show/Hide BG
        nextMode();
        break;
    case Qt::Key_F:
        if(event->modifiers()&Qt::SHIFT){ //Force a Bit
            markUndoPoint();
            fixBit(scene->scenepos);
            statusBar()->showMessage(tr("Forced a bit."));
        }else{//Focus
            ui->graphicsView->centerOn(scene->focusItem());
            statusBar()->showMessage(tr("Focusing on selected item."));
        }
        break;
    case Qt::Key_V: //DRC
        if((event->modifiers()&Qt::SHIFT))
            clearViolations(); //Clear out the violations for now.
        else
            runDRC(false);  //Just run the normal DRC, no key combo for the long one.

        break;


    //Show errors.
    case Qt::Key_E: // Errors
        nextViolation(); //Select the next violation.
        break;

    //Modify the focus object.
    case Qt::Key_D: //Delete an object.  With Shift, it deletes many.
        if(event->modifiers()&Qt::SHIFT){
            markUndoPoint();
            foreach(QGraphicsItem* item, scene->selection){
                removeItem(item);
            }
            scene->setFocusItem(0);
            statusBar()->showMessage(tr("Deleted all selected items."));
        }else if(scene->focusItem()){
            markUndoPoint();
            removeItem(scene->focusItem());
            scene->setFocusItem(0);
            statusBar()->showMessage(tr("Deleted item."));
        }else{
            statusBar()->showMessage(tr("There's no item to delete.  Maybe SHIFT+D?"));
        }

        break;

    //Save or set Position
    case Qt::Key_S:
        if(event->modifiers()&Qt::CTRL){
            on_saveButton_triggered();
        }else if(scene->focusItem()){
            switch(scene->focusItem()->type()){
            case QGraphicsItem::UserType: //row
            case QGraphicsItem::UserType+1: //column
                RomLineItem *line=(RomLineItem*)scene->focusItem();
                moveLine(line, scene->scenepos);
            }
        }
        break;


    //These insert an object and set its focus.
    case Qt::Key_Space:
    case Qt::Key_R: //Row line.
        markUndoPoint();
        rlitem=new RomLineItem(RomLineItem::LINEROW);
        if(event->key()==Qt::Key_Space
                || event->modifiers()&Qt::SHIFT)
            rlitem->setLine(lastrow);
        else
            rlitem->setLine(0, 0,
                        scene->presspos.x()-scene->scenepos.x(),
                        scene->presspos.y()-scene->scenepos.y()
                    );
        lastrow=rlitem->globalline();
        scene->addItem(rlitem);
        rlitem->setPos(scene->scenepos);
        scene->setFocusItem(rlitem);
        rows.insert(rlitem);
        markLine(rlitem);
        break;

    case Qt::Key_C: //Column line.
        markUndoPoint();
        rlitem=new RomLineItem(RomLineItem::LINECOL);
        if(event->modifiers()&Qt::SHIFT)
            rlitem->setLine(lastcol);
        else
            rlitem->setLine(0, 0,
                        scene->presspos.x()-scene->scenepos.x(),
                        scene->presspos.y()-scene->scenepos.y()
                    );
        lastcol=rlitem->globalline();
        scene->addItem(rlitem);
        rlitem->setPos(scene->scenepos);
        scene->setFocusItem(rlitem);
        cols.insert(rlitem);
        markLine(rlitem);
        break;


    //These operate on the loaded data.
    case Qt::Key_M:  //Mark Bits.
        markBits();
        if(asciiDialog.isVisible())
            on_asciiButton_triggered();
        statusBar()->showMessage(tr("Marked bits."));
        if(event->modifiers()&Qt::SHIFT){
            on_actionHexView_triggered();
            statusBar()->showMessage(tr("Decoded bits."));
        }
        break;
    }

}

//Direction function to run the rules, called by CLI and GUI.
bool MaskRomTool::runDRC(bool all){
    //Clear the field for new violations.
    clearViolations();

    //We'll crash if the bits aren't aligned.
    markBits();

    statusBar()->showMessage(tr("Running the Design Rule Checks. (DRC)"));

    RomRuleCount count;
    if(ui->drcCount->isChecked() || all)
        count.evaluate(this);

    RomRuleDuplicate dupes;
    if(ui->drcDuplicates->isChecked() || all)
        dupes.evaluate(this);

    RomRuleSanity sanity;
    if(ui->drcSanity->isChecked() || all)
        sanity.evaluate(this);

    RomRuleAmbiguous ambiguity;
    if(ui->drcAmbiguous->isChecked() || all)
        ambiguity.evaluate(this);


    statusBar()->showMessage(tr("Found %1 Design Rule Check (DRC) violations.").arg(violations.count()));
    if(violations.count()>0)
        on_actionViolationsDialog_triggered();

    return true;
}
//Menu item to run the rules.
void MaskRomTool::on_actionRunDRC_triggered(){
    runDRC(false);
}

//Runs all the rules, even those not checked.
void MaskRomTool::on_actionRunAllDRC_triggered(){
    runDRC(true);
}

//This just clears out the pending violations.
void MaskRomTool::on_actionClearViolations_triggered(){
    clearViolations();
}



//Marks a DRC violation in the scene.
void MaskRomTool::addViolation(RomRuleViolation* violation){
    QString message("%1: %2");
    qDebug()<<message.arg((violation->error?"ERROR":"WARNING"),
                              violation->title);

    violations.insert(violation);
    scene->addItem(violation);
    violationDialog.addViolation(violation);
}
//Clears all DRC violation.  Called before redrawing them.
void MaskRomTool::clearViolations(){
    violationDialog.clearViolations();
    foreach (RomRuleViolation* v, violations){
        removeItem(v);
    }
}
//Select the next violation.
void MaskRomTool::nextViolation(){
    violationDialog.nextViolation();
}

void MaskRomTool::on_importDiff_triggered(){
    if(verbose)
        qDebug()<<"Importing a diff.";
    RomEncoderDiff differ;
    QString filename = QFileDialog::getOpenFileName(this, tr("Diff Bits as ASCII"),"",tr("Textfiles (*.txt)"));

    if(!filename.isEmpty()){
        //Remove the old violations.
        clearViolations();

        differ.readFile(this, filename);
    }

    statusBar()->showMessage(tr("Found %1 bit differences.").arg(violations.count()));
    if(violations.count()>0)
        on_actionViolationsDialog_triggered();
}


//Exports ASCII for ZORROM/Bitviewer.
void MaskRomTool::on_exportASCII_triggered(){
    RomDecoderAscii exporter;
    QString filename = QFileDialog::getSaveFileName(this,tr("Export Bits as ASCII"),
                                                    "bits.txt",
                                                    tr("Textfiles (*.txt)"));
    if(!filename.isEmpty())
        exporter.writeFile(this,filename);
}

//Exports CSV for Matlab or Excel.
void MaskRomTool::on_exportCSV_triggered(){
    RomDecoderCSV exporter;
    QString filename = QFileDialog::getSaveFileName(this,tr("Export Bits as CSV"),
                                                    "bits.csv",
                                                    tr("CSV (*.csv)"));
    if(!filename.isEmpty())
        exporter.writeFile(this,filename);
}

//Exports Python for custom scripts.
void MaskRomTool::on_exportPython_triggered(){
    RomDecoderPython exporter;
    QString filename = QFileDialog::getSaveFileName(this,tr("Save Python"),
                                                    "bits.py",
                                                    tr("Python (*.py)"));
    if(!filename.isEmpty())
        exporter.writeFile(this,filename);
}

//Exports just the bit values and location as JSON.
void MaskRomTool::on_exportJSONBits_triggered(){
    RomDecoderJson json;
    QString filename = QFileDialog::getSaveFileName(this,tr("Save JSON"), tr("bits.json"));
    if(!filename.isEmpty())
        json.writeFile(this, filename);
}


//New exported, uses GatoROM and decoding dialog.
void MaskRomTool::on_exportROMBytes_triggered(){
    GatoROM gato=gatorom();

    if(gato.decoder==0){
        QMessageBox::warning(this, "Mask ROM Tool",
                             "Use Edit/Decoding to set a decoder.",
                             QMessageBox::Ok,
                             QMessageBox::Ok);
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this,tr("Save ROM"), tr("rom.bin"));
    if(!filename.isEmpty()){
        QFile fout(filename);
        fout.open(QIODevice::WriteOnly);
        fout.write(gato.decode());
    }
}


//Exports a .png image from the current project.
void MaskRomTool::on_exportPhotograph_triggered(){
    RomDecoderPhotograph photo;
    QString filename = QFileDialog::getSaveFileName(this,tr("Save Photograph"), tr("romphoto.png"));
    if(!filename.isEmpty()){
        photo.writeFile(this, filename);
    }
}

//Pop up the about dialog.
void MaskRomTool::on_aboutButton_triggered(){
    aboutDialog about;
    about.exec();
}

//Display the ASCII preview window.
void MaskRomTool::on_asciiButton_triggered(){
    RomDecoderAscii exporter;
    asciiDialog.setText(exporter.preview(this));
    asciiDialog.show();
}

//Display the Hex preview window.
void MaskRomTool::on_actionHexView_triggered(){
    gatorom();
    hexDialog.show();
}

//Pop a dialog to apply the threshold.
void MaskRomTool::on_thresholdButton_triggered(){
    thresholdDialog.setMaskRomTool(this);
    thresholdDialog.show();
    updateThresholdHistogram();
}


void MaskRomTool::on_linecolorButton_triggered(){
    QColor color = QColorDialog::getColor(Qt::black, this );
    if( color.isValid() ){
        markUndoPoint();

        lineColor = color;
        //update&redraw
        for(QSet<RomLineItem*>::iterator i = rows.begin(), end = rows.end(); i != end; ++i){
            ((QGraphicsLineItem*)*i)->setPen(QPen(lineColor, 2));
        }
        for(QSet<RomLineItem*>::iterator i = cols.begin(), end = cols.end(); i != end; ++i){
            ((QGraphicsLineItem*)*i)->setPen(QPen(lineColor, 2));
        }
    }
}

//Shows the decoder dialog.
void MaskRomTool::on_decoderButton_triggered(){
    decodeDialog.show();
}

//Pop a dialog to choose the alignment constraints.
void MaskRomTool::on_alignconstrainButton_triggered(){
    alignDialog.setMaskRomTool(this);
    alignDialog.show();
}

//Updates the histogram, iff the window is visible.
void MaskRomTool::updateThresholdHistogram(){
    if(!thresholdDialog.isVisible())
        return;

    qreal reds[256], greens[256], blues[256];
    qreal skyhigh=0;//Maximum value.

    //Zero the score.
    for(int i=0; i<256; i++)
        reds[i]=greens[i]=blues[i]=0;

    //Count the bits of each color.
    foreach(RomBitItem* bit, bits){
        long pixel=bit->bitvalue_raw(this, background);
        int r=((pixel>>16)&0xFF);
        int g=((pixel>>8)&0xFF);
        int b=((pixel)&0xFF);
        reds[r]++;
        greens[g]++;
        blues[b]++;
    }

    QLineSeries *red = new QLineSeries();
    red->setName("Red");
    QLineSeries *green = new QLineSeries();
    green->setName("Green");
    QLineSeries *blue = new QLineSeries();
    blue->setName("Blue");
    for(int i=0; i<256; i++){
        red->append(i,reds[i]);
        green->append(i,greens[i]);
        blue->append(i,blues[i]);
        if(reds[i]>skyhigh) skyhigh=reds[i];
        if(greens[i]>skyhigh) skyhigh=greens[i];
        if(blues[i]>skyhigh) skyhigh=blues[i];
    }

    QLineSeries *redmark = new QLineSeries();
    redmark->append(thresholdR,0);
    redmark->append(thresholdR,skyhigh);
    QLineSeries *greenmark = new QLineSeries();
    greenmark->append(thresholdG,0);
    greenmark->append(thresholdG,skyhigh);
    QLineSeries *bluemark = new QLineSeries();
    bluemark->append(thresholdB,0);
    bluemark->append(thresholdB,skyhigh);

    histogramchart.legend()->hide();
    histogramchart.removeAllSeries();
    histogramchart.addSeries(red);
    histogramchart.addSeries(redmark);
    histogramchart.addSeries(green);
    histogramchart.addSeries(greenmark);
    histogramchart.addSeries(blue);
    histogramchart.addSeries(bluemark);

    //Colors must be set *after* adding them to the chart.
    red->setColor(Qt::red);
    redmark->setColor(Qt::red);
    green->setColor(Qt::green);
    greenmark->setColor(Qt::green);
    blue->setColor(Qt::blue);
    bluemark->setColor(Qt::blue);

    histogramchart.createDefaultAxes();
    histogramchart.setTitle("Bit Histogram");

    thresholdDialog.setChart(&histogramchart);
}


//Set the threshold.
void MaskRomTool::setBitThreshold(qreal r, qreal g, qreal b){
    thresholdR=r;
    thresholdG=g;
    thresholdB=b;

    //Redraws only if the histogram is visible.
    updateThresholdHistogram();
}

//Get the threshold.
void MaskRomTool::getBitThreshold(qreal &r, qreal &g, qreal &b){
    r=thresholdR;
    g=thresholdG;
    b=thresholdB;

    if(verbose)
        qDebug()<<"Returning RGB threshold"<<r<<g<<b;
}

//Sets the bit display size.
void MaskRomTool::setBitSize(qreal size){
    //qDebug()<<"Changing bit size from"<<bitSize<<"to"<<size;
    bitSize=size;

    //We do not remark the bits here, but we do resize them.
    foreach (RomBitItem* bit, bits)
        bit->setBitSize(bitSize);
    foreach (RomBitFix* fix, bitfixes)
        fix->setBitSize(bitSize);

    //Violations are not dynamically resized.
    RomRuleViolation::bitSize=bitSize;
}
//Gets the bit display size.
qreal MaskRomTool::getBitSize(){
    return bitSize;
}


//Sets the skip count for the aligner.
void MaskRomTool::setAlignSkipCountThreshold(uint32_t count){
    alignSkipThreshold=count;
}
//Gets the count.
void MaskRomTool::getAlignSkipCountThreshold(uint32_t &count){
    count=alignSkipThreshold;
}

//Opens *either* an image or a JSON description of lines.
void MaskRomTool::fileOpen(QString filename){
    //On macOS, each window likes to have a file attached.
    window()->setWindowFilePath(filename);
    QProgressDialog progress("Loading", "Abort Load", 0, 10, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    progress.setValue(5);

    if(!filename.endsWith(".json")){
        imagefilename=filename;


        //This is necessary in QT6, but isn't defined in QT5.
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if(verbose){
            qDebug()<<"Allocation limit was "<<QImageReader::allocationLimit()<<"MB";
            qDebug()<<"Disabling allocation limit.";
        }
        QImageReader::setAllocationLimit(0);
#endif

        //We load the image as a background, so it's not really a part of the scene.
        if(verbose)
            qDebug()<<"Loading background image: "<<imagefilename;
        background=QImage(imagefilename);
        if(background.isNull()){
            qDebug()<<"Error opening "<<imagefilename;
            if(QGuiApplication::platformName()!="offscreen"){
                QMessageBox msgBox;
                msgBox.setText("Error opening "+imagefilename);
                msgBox.exec();
            }
            return;
        }
        //Adjust the crosshair sizes in the scene.
        scene->linesizex=background.width();
        scene->linesizey=background.height();
        setWindowTitle("MaskRomTool "+imagefilename);
        second.setWindowTitle("Second view of "+imagefilename);

        view->setBackgroundBrush(background);
        view->setCacheMode(QGraphicsView::CacheBackground);
        second.view->setBackgroundBrush(background);
        second.view->setCacheMode(QGraphicsView::CacheBackground);

        //With the view the same as the image dimensions, we get fancy scrollbars and stuff.
        view->setSceneRect(0,0,background.width(), background.height());
        second.view->setSceneRect(0,0,background.width(), background.height());

        if(verbose)
            qDebug()<<"Loaded background image of "
               <<background.width()<<","<<background.height();
        progress.close();
    }

    //And maybe there's a .json file to load?
    QFile file;
    if(!filename.endsWith(".json"))
        file.setFileName(filename+".json");
    else
        file.setFileName(filename);

    if(!file.open(QIODevice::ReadOnly)){
        if(verbose)
            qDebug() << "Json file couldn't be opened/found.  Starting from scratch.";
        return;
    }

    //Read the JSON bytes.
    QByteArray byteArray;
    byteArray = file.readAll();
    file.close();

    //Check the file format.
    QJsonParseError parseError;
    QJsonDocument jsonDoc;
    jsonDoc = QJsonDocument::fromJson(byteArray, &parseError);
    if(parseError.error != QJsonParseError::NoError){
        qWarning() << "Parse error at " << parseError.offset << ":" << parseError.errorString();
        //FIXME: Also a dialog box?
        return;
    }

    //Grab the object.
    importJSON(jsonDoc.object());
    //Clear undo history, so we can't undo past the beginning.
    undostack.empty();
    redostack.empty();
}

//Opens an image as the backgound for annotation, and matching .json if it's available.
void MaskRomTool::on_openButton_triggered(){
    QString newfilename = QFileDialog::getOpenFileName(this, tr("Open Image"), QDir::homePath(),
                                                       tr("Image Files (*.png *.jpg *.bmp *.tif)"));


    if(newfilename.isEmpty())
        return;

    fileOpen(newfilename);
}

//A line item has both a start location and a vector.  This normalizes it to only be a line.
QLineF MaskRomTool::lineFromLineItem(QGraphicsLineItem *a){
    QLineF linea=a->line();

    qreal x1, y1, x2, y2;
    x1=linea.x1()+a->x();
    y1=linea.y1()+a->y();
    x2=linea.x2()+a->x();
    y2=linea.y2()+a->y();

    return QLineF(x1, y1, x2, y2);
}

//Returns the collision point of two lines, if one exists.
QPointF MaskRomTool::bitLocation(QGraphicsLineItem *a, QGraphicsLineItem *b){
    QPointF toret;
    //QLineF::IntersectionType type;
    lineFromLineItem(a).intersects(lineFromLineItem(b), &toret);

    return toret;
}

//Is there a bit at a point?
RomBitItem* MaskRomTool::getBit(QPointF point){
    QList<QGraphicsItem*> items=scene->items(point);
    RomBitItem* bit=0;
    foreach (QGraphicsItem* item, items){
        if(item->type()==QGraphicsItem::UserType+2) //Is it a bit?
            bit=(RomBitItem*) item;
    }
    return bit;
}

//Given a bit or its position, is there a matching BitFix?
RomBitFix* MaskRomTool::getBitFix(QPointF point, bool create){
    return getBitFix(getBit(point), create);
}
RomBitFix* MaskRomTool::getBitFix(RomBitItem* bit, bool create){
    if(!bit)
        return 0;

    QList<QGraphicsItem*> items=scene->items(bit->pos());
    RomBitFix* fix=0;
    foreach (QGraphicsItem* item, items){
        if(item->type()==QGraphicsItem::UserType+3)
            fix=(RomBitFix*) item;
    }
    if(create && !fix){
        fix=new RomBitFix(bit);
        scene->addItem(fix);
        bitfixes.insert(fix);
    }
    return fix;
}
//Fixes a bit at a position.
void MaskRomTool::fixBit(QPointF point){
    if(!bitsVisible) return;
    //We fix the bit's position, not the click position.
    RomBitItem* bit=getBit(point);
    fixBit(bit);
}
void MaskRomTool::fixBit(RomBitItem* bit){
    if(!bitsVisible) return;
    RomBitFix* fix=getBitFix(bit, true);
    if(fix){
        fix->setValue(!fix->bitValue());
        bit->setFix(fix);
    }
}
//Clears all bit fixes.
void MaskRomTool::clearBitFixes(){
    //Just removes all the fixes, handy when you're retrying
    //a bad project.
    markUndoPoint();
    foreach (RomBitFix* item, bitfixes.values()){
        bitfixes.remove(item);
        removeItem(item);
    }
}
//Marks a bit at a position as damaged.
void MaskRomTool::damageBit(QPointF point){
    if(!bitsVisible) return;
    //We fix the bit's position, not the click position.
    RomBitItem* bit=getBit(point);
    damageBit(bit);
}
void MaskRomTool::damageBit(RomBitItem* bit){
    if(!bitsVisible) return;
    RomBitFix* fix=getBitFix(bit, true);
    if(fix){
        fix->setAmbiguious(!fix->bitAmbiguous());
        bit->setFix(fix);
    }
}

//Saves the lines for later import.
void MaskRomTool::on_saveButton_triggered(){
    QJsonObject obj=exportJSON();
    QByteArray ba = QJsonDocument(obj).toJson();

    statusBar()->showMessage(tr("Saved to ")+imagefilename+".json");

    QFile fout(imagefilename+".json");
    fout.open(QIODevice::WriteOnly);
    fout.write(ba);
}



//Marks an individual bit.
void MaskRomTool::markBit(QPointF point){
    alignmentdirty=true;

    //This rectangle will be a small box around the pixel.
    RomBitItem *bit=new RomBitItem(point, bitSize);
    scene->addItem(bit);
    bit->setVisible(bitsVisible);
    bits.insert(bit);

    bit->bitvalue_raw(this, background);
    bit->bitvalue_sample(this, background, thresholdR, thresholdG, thresholdB);

    bitcount++;
}


//Marks all of the bits that are on a line.
void MaskRomTool::markLine(RomLineItem* line){
    /* This is called in a loop to identify all of the lines,
     * but as a performance hack, we also use it mark the newly
     * created bits when a new row or column is placed.
     *
     * This is why we need to handle both rows and columns,
     * rather than just one type.
     */

    foreach(QGraphicsItem* item, scene->collidingItems(line)){
        //We are only looking for columns that collide with rows here.  All other collisions are irrelevant.
        if(line->linetype==RomLineItem::LINEROW
                && item->type()==item->UserType+1  //Is the colliding item a column?
            ){
            RomLineItem* col=(RomLineItem*) item;
            QPointF point=bitLocation(line, col);
            markBit(point);
        }else if(line->linetype==RomLineItem::LINECOL
                   && item->type()==item->UserType  //Is the colliding item a row?
                   ){
            RomLineItem* row=(RomLineItem*) item;
            QPointF point=bitLocation(row, line);
            markBit(point);
        }
    }

    //We also adjust the crosshairs to the last lines made.
    switch(line->linetype){
    case RomLineItem::LINEROW:
        scene->setRowAngle(line->line().angle());
        break;
    case RomLineItem::LINECOL:
        scene->setColAngle(line->line().angle());
        break;
    default:
        qDebug()<<"Unknown line type"<<line->linetype;
        break;
    }
}

//Moves a line to a new position.
void MaskRomTool::moveLine(RomLineItem* line, QPointF newpoint){
    bool bitswerevisible=bitsVisible;
    setBitsVisible(true);

    removeLine(line,false);  //Remove the line's bits, but not the line itself.
    line->setPos(newpoint);
    markLine(line); //Remark the line.

    //Restore visibility.
    setBitsVisible(bitswerevisible);
}

//Mark up all of the bits where rows and columns collide.
void MaskRomTool::markBits(){
    static long lastbitcount=50000;
    bool bitswerevisible=bitsVisible;
    bool lineswerevisible=ui->actionRowsColumns->isChecked();


    if(!lineswerevisible)
        setLinesVisible(true);

    //We're blowing away the alignment here, will need to rebuild it later.
    alignmentdirty=true;

    //First we remove all the old bits.  This is very slow.
    foreach (QGraphicsItem* item, bits)
        removeItem(item);

    bitcount=0;
    setBitsVisible(false);

    //Mark every line collision.
    foreach (RomLineItem* line, cols)
        markLine(line);

    //Mark all the fixes.
    markFixes();

    //Show the bits if--and only if--we've set that style.
    setBitsVisible(bitswerevisible);
    //Same for the lines.
    setLinesVisible(lineswerevisible);

    if(verbose)
        qDebug()<<"Marked"<<bitcount<<"bits.";
    lastbitcount=bitcount;
}

//Marks the bit fixes.
void MaskRomTool::markFixes(){
    /* We assume that fixes are far outnumbered by bits,
     * so it's far faster to search for every bit that
     * overlaps a fix than every fix that overlaps a bit.
     */
    bool bitswerevisible=bitsVisible;
    setBitsVisible(true);
    foreach (RomBitFix* fix, bitfixes){
        RomBitItem* bit=getBit(fix->pos());
        if(bit) // Apply the fix.
            bit->setFix(fix);
        else    // Destroy fixes that don't match bits.
            removeItem(fix);
    }
    setBitsVisible(bitswerevisible);
}

//Marks up all of the known bits with new samples.
void MaskRomTool::remarkBits(){
    foreach (RomBitItem* bit, bits){
        bit->bitvalue_raw(this, background);
        bit->bitvalue_sample(this, background, thresholdR, thresholdG, thresholdB);
    }
    //We remark all fixes here, because it's fast.
    markFixes();
}


//Marks the table of bits.  This is needed for export, but not to update the display.
RomBitItem* MaskRomTool::markBitTable(){
    static RomBitItem* firstbit=0;

    //Make sure all the bits are ready.
    if(alignmentdirty){
        markBits();

        alignmentdirty=false;
        firstbit=aligner->markBitTable(this);
    }

    //If the alignment was cancelled, it's dirty.
    if(!firstbit)
        alignmentdirty=true;

    //If we have a bit, update the counts.
    rowcount=colcount=0;
    RomBitItem* bit=firstbit;
    while(bit){
        rowcount++;
        bit=bit->nextrow;
    }
    bit=firstbit;
    while(bit){
        colcount++;
        bit=bit->nexttoright;
    }

    return firstbit;
}

//This exports the state to JSON.
QJsonObject MaskRomTool::exportJSON(){
    QJsonObject root;
    //Could be handy if anyone needs to reverse engineer this.
    root["00about"]="Export from MaskROMTool by Travis Goodspeed";
    root["00github"]="http://github.com/travisgoodspeed/mcuexploits/";
    root["00imagefilename"]=imagefilename;

    /* We try not to break compatibility, but as features are added,
     * we should update this date to indicate the new file format
     * version number.
     *
     * 2023.12.07 -- Gatorom strings now work.
     * 2023.09.15 -- Adds 'linecolor'.
     * 2023.09.04 -- Adds 'gatorom' with string description.
     * 2023.05.14 -- Adds 'inverted' bits.
     * 2023.05.08 -- Adds 'sampler' and 'samplersize'.
     * 2023.05.05 -- Adds the 'alignthreshold' field.  Defaults to 5 if missing.
     * 2022.09.28 -- First public release.
     */
    root["00version"]="2023.12.07";

    //These threshold values will change in a later version.
    QJsonObject settings;
    settings["red"]=thresholdR;
    settings["green"]=thresholdG;
    settings["blue"]=thresholdB;
    settings["bitsize"]=bitSize;
    settings["alignthreshold"]=QJsonValue((int) alignSkipThreshold); //2023.05.05
    settings["sampler"]=sampler->name;           //2023.05.08
    settings["samplersize"]=getSamplerSize();    //2023.05.08
    settings["inverted"]=inverted;               //2023.05.14
    settings["gatorom"]=gr.description();        //2023.09.04
    settings["linecolor"]=lineColor.name();      //2023.09.15
    root["settings"]=settings;


    QJsonArray jrows;
    foreach (RomLineItem* item, rows){
        QJsonObject o;
        item->write(o);
        jrows.push_back(o);
    }
    root["rows"] = jrows;

    QJsonArray jcols;
    foreach (RomLineItem* item, cols){
        QJsonObject o;
        item->write(o);
        jcols.push_back(o);
    }
    root["cols"] = jcols;

    QJsonArray jfixes;
    foreach (RomBitFix* item, bitfixes){
        QJsonObject o;
        item->write(o);
        jfixes.push_back(o);
    }
    root["bitfixes"] = jfixes;

    /* For now, we don't save the bits but instead regenerate
       them from the rows, cols, and thresholds.  This might cause
       some bit errors as code refactors, but hopefully fixed points
       will take care of that.
     */
    return root;
}

//The imports the state from JSON.
void MaskRomTool::importJSON(QJsonObject o){
    //Recursive is bad here!
    importLock++;
    if(importLock>1)
        qDebug()<<"Import lock is"<<importLock;

    QJsonValue github=o.value("00github");
    QJsonValue about=o.value("00about");

    //Thresholds default to zero when undefined.
    QJsonObject settings=o["settings"].toObject();
    QJsonValue red=settings.value("red");
    QJsonValue green=settings.value("green");
    QJsonValue blue=settings.value("blue");
    setBitThreshold(red.toDouble(0), green.toDouble(0), blue.toDouble(0));
    setBitSize(settings.value("bitsize").toDouble(10));
    QJsonValue alignskipthreshold=settings.value("alignthreshold");
    setAlignSkipCountThreshold(alignskipthreshold.toInt(5)); //Default of 5.
    QJsonValue inverted=settings.value("inverted");
    this->inverted=inverted.toBool(false); //Defaults to not inverting bits.

    lineColor = QColor(settings.value("linecolor").toString("#000000"));

    QJsonValue grsetting=settings.value("gatorom");
    this->gr.configFromDescription(grsetting.toString(""));
    decodeDialog.setMaskRomTool(this);


    //New bit sampler algorithms.
    QJsonValue sampler=settings.value("sampler");
    chooseSampler(sampler.toString("Default"));
    QJsonValue samplersize=settings.value("samplersize");
    setSamplerSize(samplersize.toInt(0));


    //Line items.
    QJsonArray jrows=o["rows"].toArray();
    QJsonArray jcols=o["cols"].toArray();
    //Fixes
    QJsonArray jfixes=o["bitfixes"].toArray();

    //Progress bars are cool when things get slow.
    QProgressDialog progress("Loading JSON", "Abort Load", 0, jrows.size()+jcols.size()+jfixes.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    int count=0;

    //Rows.
    for(int i = 0; i < jrows.size(); i++){
        RomLineItem *l=new RomLineItem(RomLineItem::LINEROW);
        l->read(jrows[i]);
        l->setPen(QPen(lineColor, 2));
        scene->addItem(l);
        rows.insert(l);
        lastrow=l->line();

        progress.setValue(count++);
    }

    //Columns.
    for(int i = 0; i < jcols.size(); i++){
        RomLineItem *c=new RomLineItem(RomLineItem::LINECOL);
        c->read(jcols[i]);
        c->setPen(QPen(lineColor, 2));
        scene->addItem(c);
        cols.insert(c);
        lastcol=c->line();

        progress.setValue(count++);
    }

    //Bit Fixes
    for(int i = 0; i < jfixes.size(); i++){
        RomBitFix *fix=new RomBitFix(0);
        fix->read(jfixes[i]);
        scene->addItem(fix);
        bitfixes.insert(fix);

        progress.setValue(count++);
    }

    if(verbose)
        qDebug()<<"Done loading, now marking bits.";

    progress.close();

    //After loading all that, we should be able to decode the bits.
    markBits();
    setBitSize(bitSize);

    //And correct missing stats.
    //thresholdDialog.refreshStats();  //FIXME: This crashes.
    thresholdDialog.setMaskRomTool(this);
    updateThresholdHistogram();

    importLock--;
}


// This displays the second window.  Maybe we ought to have a third?
void MaskRomTool::on_actionSecond_triggered(){
    second.mrt=this;         //Apply a backreference for sharing keystrokes.
    second.show();           //Show the window.
    second.resize(1024,768); //Resizing it makes scrollbars visible.
}

// This hilights all bits in the range selected in the Hex viewer.
void MaskRomTool::on_actionHighlightHexSelection_triggered(){
    highlightAdrRange(
        hexDialog.start,
        hexDialog.end
        );
}

// Marks a warning for bits in range.
void MaskRomTool::highlightAdrRange(uint32_t start, uint32_t end){
    if(verbose)
        qDebug()<<"Marking address range from"<<start<<"to"<<end;

    //Just another type of violation, so we have to clear the real ones.
    clearViolations();

    //Update the decoding and mark the bits.
    gatorom();

    foreach (RomBitItem* b, bits){
        uint32_t a=b->adr;

        if(start<=a && a<=end){
            RomRuleViolation* violation=new RomRuleViolation(b->pos(),
                                                               QString::asprintf("Bit at 0x%x",a),
                                                               QString::asprintf("Bit at 0x%x",a));
            violation->error=false;
            addViolation(violation);
        }else{
            //qDebug()<<"Skipping adr"<<a;
        }
    }

    this->violationDialog.show();
}

//Menu item to remove all bit fixes.
void MaskRomTool::on_actionClearForcedBits_triggered(){
    clearBitFixes();
}

//Jump to the very next violation.  Handy in error correcting.
void MaskRomTool::on_actionSelectNextViolation_triggered(){
    nextViolation();
}


void MaskRomTool::on_actionUndo_triggered(){
    undo();
}

void MaskRomTool::on_actionRedo_triggered(){
    redo();
}

