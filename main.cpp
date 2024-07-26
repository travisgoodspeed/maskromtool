#include "maskromtool.h"
#include "romdecoderascii.h"
#include "romdecodergato.h"
#include "romdecodercsv.h"
#include "romdecoderjson.h"
#include "romdecoderpython.h"
#include "romdecoderphotograph.h"
#include "romdecoderhistogram.h"
#include "romencoderdiff.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>
#include <QMessageBox>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);

    //Stupid portability bug.
    // It's faster to name a font that exists on macOS with no GUI.
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


    /* We try to guess at the right OpenGL settings, but this are for forcing it
     * if that isn't the right decision.
     */
    QCommandLineOption disableOpenglOption(QStringList() << "disable-opengl",
                                           QCoreApplication::translate("main", "Disable OpenGL."));
    parser.addOption(disableOpenglOption);
    QCommandLineOption enableOpenglOption(QStringList() << "enable-opengl",
                                           QCoreApplication::translate("main", "Enable OpenGL."));
    parser.addOption(enableOpenglOption);


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
                                         QCoreApplication::translate("main", "Export ASCII bits."),
                                         QCoreApplication::translate("main", "file"));
    parser.addOption(asciiExportOption);
    // Exporting to ROM bytes.
    QCommandLineOption rombytesExportOption(QStringList() << "o" << "export",
                                         QCoreApplication::translate("main", "Export ROM bytes."),
                                         QCoreApplication::translate("main", "file"));
    parser.addOption(rombytesExportOption);
    // Export histogram.
    QCommandLineOption histogramExportOption(QStringList() << "export-histogram",
                                            QCoreApplication::translate("main", "Export histogram."),
                                            QCoreApplication::translate("main", "file"));
    parser.addOption(histogramExportOption);

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
    // Exporting a photo.
    QCommandLineOption photoExportOption(QStringList() << "export-photo",
        QCoreApplication::translate("main", "Export a photograph."),
        QCoreApplication::translate("main", "file"));
    parser.addOption(photoExportOption);




    // Process the actual command line arguments given by the user
    parser.process(a);

    const QStringList args = parser.positionalArguments();

    //OpenGL is now stable, but off by default.
    bool opengl=false;
    if(a.platformName().contains("offscreen")) opengl=false; //Doesn't work.
    if(a.platformName().contains("wayland")) opengl=false;   //Unstable.
    if(parser.isSet(disableOpenglOption)) opengl=false;
    if(parser.isSet(enableOpenglOption)) opengl=true;

    MaskRomTool mrt(0, opengl);
    if(parser.isSet(verboseOption)){
        //Don't print anything because the function takes care of it for us.
        mrt.enableVerbose();
    }

    mrt.show();
    for(int i=0; i<args.count(); i++)
        mrt.fileOpen(args[i]);

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
    //Export ROM bytes.
    if(parser.isSet(rombytesExportOption)){
        qDebug()<<"Exporting ROM";
        RomDecoderGato exporter;
        exporter.writeFile(&mrt, parser.value(rombytesExportOption));

    }
    //Export histogram bytes.
    if(parser.isSet(histogramExportOption)){
        qDebug()<<"Exporting Histogram";
        RomDecoderHistogram exporter;
        exporter.writeFile(&mrt, parser.value(histogramExportOption));

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

    //Stress test.
    if(parser.isSet(stressOption)){
        qDebug()<<"Stressing the loaded project.";
        for(int i=0; i<10; i++){
            qDebug()<<"Round"<<i;
            mrt.markBits();
        }
    }

    //We don't return a failure code yet, but will if it comes to that.
    if(parser.isSet(DRCOption))
        mrt.runDRC(true);
    else if(parser.isSet(drcOption))
        mrt.runDRC(false);

    //We let the GUI take hold unless asked to do otherwise.
    if(!parser.isSet(exitOption)){
        if(a.platformName().contains("wayland")){
            qDebug()<<"If Wayland is unstable, pass '-platform xcb' to use Xorg instead.";
        }

        //We never launch the GUI when offscreen, because it should always exit.
        if(!a.platformName().contains("offscreen"))
            return a.exec();
    }

    //We're exiting here, so the return code should indicate the numer of DRC violations.
    long violations=mrt.violations.count();
    if(violations>200) //Too large for the shell.
        violations=200;
    return violations;
}
