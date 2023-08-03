#include "maskromtool.h"
#include "romdecoderascii.h"
#include "romdecodercsv.h"
#include "romdecodermarc4.h"
#include "romdecoderarm6.h"
#include "romdecoderjson.h"
#include "romdecoderpython.h"
#include "romdecoderphotograph.h"
#include "romencoderdiff.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>
#include <QMessageBox>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);

    //Stupid portability bug.  It's a lot faster to name a font that exists on macOS with no GUI.
    if(a.platformName()=="offscreen")
        a.setFont(QFont("Helvetica Neue"));

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "maskromtool_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }



    QCommandLineParser parser;
    parser.setApplicationDescription("Mask ROM Tool");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("image", QCoreApplication::translate("main", "ROM photograph to open."));
    parser.addPositionalArgument("json", QCoreApplication::translate("main", "JSON lines to open."));


    //Print verbose debugging messages.
    QCommandLineOption verboseOption(QStringList() << "V" << "verbose",
                                     QCoreApplication::translate("main", "Print verbose debugging messages."));
    parser.addOption(verboseOption);
    //Stress test.
    QCommandLineOption stressOption(QStringList() << "stress",
                                     QCoreApplication::translate("main", "Stress test bit marking."));
    parser.addOption(stressOption);
    // Exit after processing other arguments.
    QCommandLineOption exitOption(QStringList() << "e" << "exit",
                                  QCoreApplication::translate("main", "Exit after processing arguments."));
    parser.addOption(exitOption);


    /* We used to have an option for OpenGL.  Deprecated now that it's default.
    QCommandLineOption openglOption(QStringList() << "opengl",
                                  QCoreApplication::translate("main", "Enable OpenGL.  (Not yet stable.)"));
    parser.addOption(openglOption);
     */

    // Design Rule Check
    QCommandLineOption drcOption(QStringList() << "d" << "drc",
                                  QCoreApplication::translate("main", "Run default Design Rule Checks."));
    QCommandLineOption DRCOption(QStringList() << "D" << "DRC",
                                  QCoreApplication::translate("main", "Run all Design Rule Checks."));
    parser.addOption(drcOption);
    parser.addOption(DRCOption);

    //Bit Samplers
    QCommandLineOption samplerOption(QStringList() << "sampler",
                                     QCoreApplication::translate("main", "Bit Sampling Algorithm."),
                                     "Default"
                                     );
    parser.addOption(samplerOption);


    // Diffing against ASCII Art
    QCommandLineOption asciiDiffOption(QStringList() << "diff-ascii",
        QCoreApplication::translate("main", "Compares against ASCII art, for finding errors."),
        QCoreApplication::translate("main", "file"));
    parser.addOption(asciiDiffOption);

    // Exporting to ASCII art.
    QCommandLineOption asciiExportOption(QStringList() << "a" << "export-ascii",
        QCoreApplication::translate("main", "Export ASCII bits for use in ZorRom."),
        QCoreApplication::translate("main", "file"));
    parser.addOption(asciiExportOption);
    // Exporting to CSV table.
    QCommandLineOption csvExportOption(QStringList() << "export-csv",
        QCoreApplication::translate("main", "Export CSV bits for use in Matlab or Excel."),
        QCoreApplication::translate("main", "file"));
    parser.addOption(csvExportOption);
    // Exporting to JSON bit positions.
    QCommandLineOption jsonExportOption(QStringList() << "export-json",
        QCoreApplication::translate("main", "Export JSON bit positions."),
        QCoreApplication::translate("main", "file"));
    parser.addOption(jsonExportOption);
    // Exporting to Python arrays.
    QCommandLineOption pythonExportOption(QStringList() << "export-python",
        QCoreApplication::translate("main", "Export Python arrays."),
        QCoreApplication::translate("main", "file"));
    parser.addOption(pythonExportOption);
    // Exporting a MARC4 ROM.
    QCommandLineOption marc4ExportOption(QStringList() << "export-marc4",
        QCoreApplication::translate("main", "Export MARC4 ROM banks, left to right."),
        QCoreApplication::translate("main", "file"));
    parser.addOption(marc4ExportOption);
    // Exporting an ARM6 ROM.
    QCommandLineOption arm6ExportOption(QStringList() << "export-arm6",
        QCoreApplication::translate("main", "Export ARM6L (MYK82) ROM."),
        QCoreApplication::translate("main", "file"));
    parser.addOption(arm6ExportOption);
    // Exporting a photo.
    QCommandLineOption photoExportOption(QStringList() << "export-photo",
        QCoreApplication::translate("main", "Export a photograph."),
        QCoreApplication::translate("main", "file"));
    parser.addOption(photoExportOption);



    // Process the actual command line arguments given by the user
    parser.process(a);

    const QStringList args = parser.positionalArguments();

    MaskRomTool mrt;
    mrt.show();
    for(int i=0; i<args.count(); i++)
        mrt.fileOpen(args[i]);

    if(parser.isSet(verboseOption)){
        //Don't print anything because the function takes care of it for us.
        mrt.enableVerbose();
    }

    //Chooses the sampling algorithm.
    if(parser.isSet(samplerOption)){
        mrt.chooseSampler(parser.value(samplerOption));
    }

    if(parser.isSet(asciiDiffOption)){
        qDebug()<<"Diffing in ASCII against"<<parser.value(asciiDiffOption);
        RomEncoderDiff differ;
        differ.readFile(&mrt, parser.value(asciiDiffOption));
    }

    //Export to ASCII.
    if(parser.isSet(asciiExportOption)){
        qDebug()<<"Exporting to ASCII.";
        RomDecoderAscii exporter;
        exporter.writeFile(&mrt, parser.value(asciiExportOption));
    }
    //Export to CSV.
    if(parser.isSet(csvExportOption)){
        qDebug()<<"Exporting to CSV.";
        RomDecoderCSV exporter;
        exporter.writeFile(&mrt, parser.value(asciiExportOption));
    }
    //Export to JSON bit positions.
    if(parser.isSet(jsonExportOption)){
        qDebug()<<"Exporting to JSON bit positions.";
        RomDecoderJson exporter;
        exporter.writeFile(&mrt, parser.value(jsonExportOption));
    }
    //Export to Python.
    if(parser.isSet(pythonExportOption)){
        qDebug()<<"Exporting to Python.";
        RomDecoderPython exporter;
        exporter.writeFile(&mrt, parser.value(pythonExportOption));
    }
    //Export a photo.
    if(parser.isSet(photoExportOption)){
        qDebug()<<"Exporting a photo.";
        RomDecoderPhotograph exporter;
        exporter.writeFile(&mrt, parser.value(photoExportOption));
    }
    //Export to MARC4.
    if(parser.isSet(marc4ExportOption)){
        qDebug()<<"Exporting MARC4 ROM.  Banks may be in the wrong order.";
        RomDecoderMarc4 exporter;
        exporter.writeFile(&mrt, parser.value(marc4ExportOption));
    }
    //Export to ARM6.
    if(parser.isSet(arm6ExportOption)){
        qDebug()<<"Exporting ARM6 ROM.";
        RomDecoderARM6 exporter;
        exporter.writeFile(&mrt, parser.value(arm6ExportOption));
    }

    //Stress test.
    if(parser.isSet(stressOption)){
        qDebug()<<"Stressing the loaded project.";
        for(int i=0; i<10; i++)
            mrt.markBits();
    }

    //We don't return a failure code yet, but will if it comes to that.
    if(parser.isSet(DRCOption))
        mrt.runDRC(true);
    else if(parser.isSet(drcOption))
        mrt.runDRC(false);

    //We let the GUI take hold unless asked to do otherwise.
    if(!parser.isSet(exitOption)){
        if(a.platformName().contains("wayland")){
            QMessageBox msgBox;
            msgBox.setWindowTitle("Wayland is Unstable");
            msgBox.setText("The wayland driver is unstable.  Please pass '-platform xcb' to maskromtool to use Xorg instead.");
            msgBox.exec();
        }
        return a.exec();
    }

    //We're exiting here, so the return code should indicate the numer of DRC violations.
    long violations=mrt.violations.count();
    if(violations>200) //Too large for the shell.
        violations=200;
    return violations;
}
