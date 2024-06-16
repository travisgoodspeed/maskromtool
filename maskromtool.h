#ifndef MASKROMTOOL_H
#define MASKROMTOOL_H

#include <QMainWindow>
#include <QGraphicsLineItem>
#include <QLineSeries>
#include <QSettings>

#include "romlineitem.h"
#include "rombititem.h"
#include "asciidialog.h"
#include "romhexdialog.h"
#include "romthresholddialog.h"
#include "romdecodedialog.h"
#include "romaligndialog.h"
#include "romaligner.h"
#include "rombitsampler.h"
#include "romruledialog.h"
#include "romstringsdialog.h"
#include "romsecond.h"
#include "romsolverdialog.h"
#include "romsolutionsdialog.h"
#include "romdisdialog.h"

#include "gatorom.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MaskRomTool; }
QT_END_NAMESPACE

//Vague prototypes of classes to avoid ciruclar referencing.
class RomRuleViolation;
class RomScene;
class RomView;
class RomBitFix;

extern unsigned int verbose;

class MaskRomTool : public QMainWindow{
    Q_OBJECT

public:
    MaskRomTool(QWidget *parent = nullptr, bool opengl=true);
    ~MaskRomTool();
    void closeEvent(QCloseEvent *event);

    void enableVerbose(unsigned int level=1);
    void enableOpenGL(unsigned int antialiasing=0);

    // Undo, Redo, or mark a checkpoint for undoing later.
    QStack<QJsonObject> undostack, redostack;
    //No point in keeping a thousand steps in memory.
    uint32_t maxUndoCount=50;
    void undo();
    void redo();
    void markUndoPoint();
    void clear();

    //Persistant user settings.
    QSettings settings();
    QColor lineColor;

    //Background image of the ROM.
    QImage background;
    QSet<RomLineItem*> rows;     //Rows produce bits where they intersect with columns.
    QSet<RomLineItem*> cols;     //Columns produce bits on rows.
    QSet<RomBitItem*> bits;      //All of the bits from crosses.
    QSet<RomBitFix*> bitfixes;   //Forced bits, used to keep human agency above the thinking machine.
    QSet<RomRuleViolation*> violations;  //DRC Errors and warnings.
    long bitcount=0;             //Total count of bits from the latest marking.
    long rowcount=0, colcount=0;
    void removeItem(QGraphicsItem* item);    //Removed an item of any type from the view and its QSet.


    /* This giant function handles our keypresses, including ones that maybe
     * ought to go through menu handlers or some sort of a handler list.
     * Cleaning it up will be a refactoring project in the near future.
     */
    void keyPressEvent(QKeyEvent *event);

    void nextMode();                         //Called from the Tab key, this hides or shows bits.
    bool linesVisible=true;
    void setLinesVisible(bool b=true);       //Shows or hides the rows and columns.
    bool bitsVisible=true;                   //Are we currently seeing the bits?
    void setBitsVisible(bool b=true);        //Show or hide the bits.
    void setViolationsVisible(bool b=true);  //Show or hide the DRC violations.

    //Marks all of the bit positions, but not their connections.
    void markBits();
    //Re-marks bits at all of the old positions, but new samples.
    void remarkBits();
    //Marks one bit.
    void markBit(QPointF point);
    //Marks bits off of one line.
    void markLine(RomLineItem* line);
    //Updates the crosshair's angle.
    void updateCrosshairAngle(RomLineItem* line);
    //Remove bits and a line.
    void removeLine(RomLineItem* line, bool fromsets=true);
    //Moves a line to a new location.
    void moveLine(RomLineItem* line, QPointF newpoint);
    //Inserts a new line, either row or column.
    bool insertLine(RomLineItem* line);

    //Get a bit at a point.
    RomBitItem* getBit(QPointF point);

    //Fixes a bit at a position.
    void fixBit(QPointF point);
    void fixBit(RomBitItem* bit);
    //Marks a bit at a position as damaged.
    void damageBit(QPointF point);
    void damageBit(RomBitItem* bit);
    //Given a bit or its position, is there a matching BitFix?
    RomBitFix* getBitFix(QPointF point, bool create=false);
    RomBitFix* getBitFix(RomBitItem* bit, bool crate=false);
    //Clears all bit fixes.
    void clearBitFixes();

    //Highlight stuff for easy visibility.
    void highlightAdrRange(uint32_t start, uint32_t end);

    //Updates the histogram dialog.
    void updateThresholdHistogram(bool force=false);

    //Marks the table of bits, returns top-left entry.
    RomBitItem* markBitTable();
    //Marks the table of bits, returning it as a GatoROM object.
    GatoROM gatorom();

    //Opens an image file (and its json) or just the json.
    void fileOpen(QString filename);

    //Sets the threshold.  Expect this to change in later revisions.
    void setBitThreshold(qreal r, qreal g, qreal b);
    //Gets the bit threshold.
    void getBitThreshold(qreal &r, qreal &g, qreal &b);
    //Sets the bit display size.
    void setBitSize(qreal size);
    //Gets the bit display size.
    qreal getBitSize();

    //Sets the skip count for the aligner.
    void setAlignSkipCountThreshold(uint32_t count);
    //Gets the count.
    void getAlignSkipCountThreshold(uint32_t &count);

    //Handy filenames.
    QString imagefilename;

    QLineF lineFromLineItem(QGraphicsLineItem *a);
    QPointF bitLocation(QGraphicsLineItem *a, QGraphicsLineItem *b);

    //These are for load/save of the project state.  See RomDecoderJson for the bit position exporter.
    int importLock=0;
    QJsonObject exportJSON();
    void importJSON(QJsonObject);


    //Dialog windows.
    ASCIIDialog asciiDialog;
    QChart histogramchart;
    RomThresholdDialog thresholdDialog;
    RomAlignDialog alignDialog;
    RomRuleDialog violationDialog;
    RomSecond second;
    RomDecodeDialog decodeDialog;
    RomHexDialog hexDialog;
    RomStringsDialog stringsDialog;
    RomSolverDialog solverDialog;
    RomSolutionsDialog solutionsDialog;
    RomDisDialog disDialog;

    //Selectable Strategies
    RomAligner *aligner=0;
    QSet<RomAligner*> aligners;
    RomBitSampler *sampler=0;
    QSet<RomBitSampler*> samplers;

    //Sampler functions.
    void addSampler(RomBitSampler *sampler); //Adds support for a sampler.
    void chooseSampler(QString name); //Chooses a sampler by name.
    void setSamplerSize(int size);
    int getSamplerSize();
    bool inverted=false; //Bits are often inverted by photography.

    //Alignment strategies.
    void addAligner(RomAligner *aligner);
    void chooseAligner(QString name);


    //Navigation
    void centerOn(QGraphicsItem* item);
    QPointF home; //Selectable home position, controlled by the H key.

    //DRC
    bool runDRC(bool all=false);
    void addViolation(RomRuleViolation* violation);
    RomRuleViolation* getBitViolation(RomBitItem* bit);
    void clearViolations();
    void nextViolation();


    //The scene.
    RomScene* scene;
    //The view
    RomView* view;
    //Last row and column.
    QLineF lastrow, lastcol;
    //Was the last line dropped a row or a column?
    int lastlinetype=0;
    //We regenerate from this when needed.
    GatoROM gr;

    //Histogram bits.
    qreal reds[256], greens[256], blues[256];

private slots:
    void on_openButton_triggered();
    void on_saveButton_triggered();
  
    void on_alignconstrainButton_triggered();
    void on_actionRunDRC_triggered();

    //Edit
    void on_thresholdButton_triggered();
    void on_decoderButton_triggered();
    void on_linecolorButton_triggered();
  
    //View a window.
    void on_aboutButton_triggered();
    void on_asciiButton_triggered();
    void on_actionHexView_triggered();

    //Import from a file.  (Loose definition.
    void on_importDiff_triggered();

    //Export to a file.
    void on_exportASCII_triggered();
    void on_exportCSV_triggered();
    void on_exportJSONBits_triggered();
    void on_exportPython_triggered();
    void on_exportROMBytes_triggered();
    void on_exportPhotograph_triggered();

    //printing
    void on_actionPrint_triggered();

    //View or hide.
    void on_actionPhotograph_triggered();
    void on_actionRowsColumns_triggered();
    void on_actionBits_triggered();
    void on_actionViolations_triggered();
    void on_actionViolationsDialog_triggered();
    void on_actionCrosshair_triggered();
    void on_actionOpenGL_triggered();
    void on_actionSecond_triggered();

    void on_actionClearViolations_triggered();
    void on_actionRunAllDRC_triggered();
    void on_actionHighlightHexSelection_triggered();
    void on_actionClearForcedBits_triggered();
    void on_actionSelectNextViolation_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();

    void on_stringsButton_triggered();

    void on_exportHistogram_triggered();

    void on_actionSolver_triggered();
    void on_actionDisassembly_triggered();

private:
    Ui::MaskRomTool *ui;
    qreal thresholdR, thresholdG, thresholdB;
    qreal bitSize=10;
    bool alignmentdirty=true;
    uint32_t alignSkipThreshold=5;

    //Marks the bit fixes.
    void markFixes();
};
#endif // MASKROMTOOL_H
