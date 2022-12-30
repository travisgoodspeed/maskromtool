#include<QDebug>
#include<QFile>

#include "romencoderdiff.h"
#include "romrule.h"

//Not much to do here.
RomEncoderDiff::RomEncoderDiff() {}

/* The purpose here is to take a file as input, then flag every bit of
 * the active project that disagrees with that file as a potential error.
 *
 * In practice, the operator will maintain two projects and message the
 * damaged bits of them until they agree as a way to eliminate ambiguous
 * bits and errors.
 */
void RomEncoderDiff::readFile(MaskRomTool *m, QString filename){
    long row=0, col=0;

    //Input file and byte array.
    QFile fin(filename);
    fin.open(QFile::ReadOnly);
    QByteArray ba=fin.readAll();

    //Local bits as linked lists of rows.
    RomBitItem* rowbit = m->markBitTable();  //First bit of current row.
    RomBitItem* bit=rowbit;                  //Current bit of the row.

    //Shotgun parser doesn't check newlines, just bits.
    for (qsizetype i = 0; i < ba.size(); ++i) {
        //File bit to to compare against.
        char c=ba.at(i);  //Value of the next bit in the input file.
        if(c!='1' && c!='0')
            continue;

        //In case we've fallen off the end of a row or column.
        if(!bit){
            rowbit=bit=rowbit->nextrow;
            row+=1;
            col=0;
        }
        if(!bit){
            qDebug()<<"Bit miscount in diff?";
            return;
        }

        //Compare the bit.
        if(bit->bitValue()==(c=='1'?true:false)){
            //qDebug()<<"Bit is right.";
        }else{
            qDebug()<<"Bit at "<<row<<","<<col<<" is wrong.";
            RomRuleViolation* violation=new RomRuleViolation(bit->pos(),
                                                             QString("Ambiguous bit %1,%2").arg(bit->row).arg(bit->col),
                                                             "This bit is close to the threshold.  Try fixing it to the value that you see?");
            violation->error=true;
            m->addViolation(violation);
        }

        //Advance to next local bit before continuing loop.
        bit=bit->nexttoright;
        col+=1;
    }
}
