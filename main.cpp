#include "maskromtool.h"
#include "romdecoderascii.h"
#include "romdecodermarc4.h"
#include "romdecoderjson.h"
#include "romdecoderpython.h"
#include "romdecoderphotograph.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

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



    // Exit after processing other arguments.
    QCommandLineOption exitOption(QStringList() << "e" << "exit",
                                  QCoreApplication::translate("main", "Exit after processing arguments."));
    parser.addOption(exitOption);
    // OpenGL isn't yet stable enough to be standard.
    QCommandLineOption openglOption(QStringList() << "opengl",
                                  QCoreApplication::translate("main", "Enable OpenGL.  (Not yet stable.)"));
    parser.addOption(openglOption);

    // Design Rule Check
    QCommandLineOption drcOption(QStringList() << "d" << "drc",
                                  QCoreApplication::translate("main", "Run default Design Rule Checks."));
    QCommandLineOption DRCOption(QStringList() << "D" << "DRC",
                                  QCoreApplication::translate("main", "Run all Design Rule Checks."));
    parser.addOption(drcOption);
    parser.addOption(DRCOption);


    // Exporting to ASCII art.
    QCommandLineOption asciiExportOption(QStringList() << "a" << "export-ascii",
        QCoreApplication::translate("main", "Export ASCII bits for use in ZorRom."),
        QCoreApplication::translate("main", "file"));
    parser.addOption(asciiExportOption);
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

    if(parser.isSet(openglOption)){
        qDebug()<<"Enbling OpenGL.";
        mrt.enableOpenGL();
    }

    //Export to ASCII.
    if(parser.isSet(asciiExportOption)){
        qDebug()<<"Exporting to ASCII.";
        RomDecoderAscii exporter;
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
