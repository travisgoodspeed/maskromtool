#include <QDebug>
#include <QProcess>
#include <QTemporaryFile>

#include "gatograderyara.h"

GatoGraderYara::GatoGraderYara(QString rulefile){
    //qDebug()<<"Launching GatoGraderYara with file "<<rulefile;
    this->rulefile=rulefile;
}

GatoGraderYara::~GatoGraderYara(){

}


int GatoGraderYara::grade(QByteArray ba){
    //No sense grading an empty result.
    if(ba.length()==0) return 0;

    //This temporary file will be deleted when the function returns.
    QTemporaryFile tmpfile;
    tmpfile.open();
    tmpfile.write(ba);
    tmpfile.flush();
    tmpfile.close();

    //Libyara would be faster, but this seems to be fast enough.
    QProcess process;
    process.start("yara",
                  QStringList()
                      <<"-c"   // Count matches
                      <<rulefile
                      <<tmpfile.fileName()
                  );
    process.waitForFinished();

    //Result is an integer number of rule matches.
    result=process.readAllStandardOutput();
    error=process.readAllStandardError();

    bool okay=false;
    int res=result.toInt(&okay);
    if(okay)
        return 100*res;


    //Something when wrong.
    if(result.length()==0 && error.length()==0){
        error=QString("Is Yara missing from the path?");
        qDebug()<<error;
    }else if(error.length()>0){
        qDebug()<<"Error: "<<error;
    }else{
        qDebug()<<"Unexpected result from Yara."<<result;
    }
    return 0;
}
