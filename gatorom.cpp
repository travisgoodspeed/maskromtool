#include <QDebug>
#include <QPrinter>
#include <QRegularExpression>
#include <QProcess>

#include "gatorom.h"
#include "gatoprinter.h"

//Custom decoders.
#include "gatodecodertlcsfont.h"
#include "gatodecoderz86x1.h"
#include "gatodecodercolsdownlswap.h" // NEC uCOM4
//Zorrom compatibility.
#include "gatodecodercolsdownr.h"
#include "gatodecodercolsdownl.h"
#include "gatodecodercolsleft.h"
#include "gatodecodercolsright.h"
#include "gatodecodersqueezelr.h"

//GoodASM library.
#include "extern/goodasm/goodasm.h"

GatoBit::GatoBit(bool v){
    this->val=v;
}
bool GatoBit::getVal(){
    return this->val ^ this->inverted;
}


/* Decoders are inherently sloppy things, much as we try to
 * keep them clean.  These two functions interpose all requests
 * for pointers to bits in the table, zeroing them out
 * and printing a warning if they are out of range.
 *
 * I'm intentionally not complicating this with exceptions
 * for now, but that might change later.
 */

GatoBit* GatoROM::inputbit(int row, int col){
    if(row<inputrows && col<inputcols)
        return inputbits[row][col];

    qDebug()<<"Illegal fetch of input bit"<<row<<","<<col<<"with args: "<<description();
    if(strictmode) exit(1);
    return 0;
}
GatoBit* GatoROM::outputbit(int row, int col){
    if(row<outputrows && col<outputcols)
        return outputbits[row][col];

    qDebug()<<"Illegal fetch of output bit"<<row<<","<<col<<"with args: "<<description();
    if(strictmode) exit(1);
    return 0;
}

//Returns an English description of the current ROM state.
QString GatoROM::description(){
    QString d="";

    if(wordsize!=8)
        d.append("-w "+QString::number(wordsize)+" ");

    if(zorrommode)
        d.append("-z ");
    if(decoder)
        d.append("--decode-"+decoder->name+" ");
    if(inverted)
        d.append("-i ");
    d.append("-r "+QString::number(angle)+" ");
    if(flippedx)
        d.append("--flipx ");
    if(flippedy)
        d.append("--flipy ");

    //Cuts the image in half.  Needed for MC6801U4.
    if(bank==1)
        d.append("--leftbank ");
    if(bank==2)
        d.append("--rightbank ");

    return d;
}

//Returns an English description of the current ROM state as a filename.
QString GatoROM::descriptiveFilename(){
    QString d="";

    if(wordsize!=8)
        d.append("-word"+QString::number(wordsize));

    if(zorrommode)
        d.append("-z");
    if(decoder)
        d.append("-"+decoder->name);
    if(inverted)
        d.append("-i");
    d.append("-r"+QString::number(angle));
    if(flippedx)
        d.append("-flipx");
    if(flippedy)
        d.append("-flipy");

    //Cuts the image in half.  Needed for MC6801U4.
    if(bank==1)
        d.append("-leftbank");
    if(bank==2)
        d.append("-rightbank");

    return d;
}

//Loads from the same description.
void GatoROM::configFromDescription(QString d){
    //Forgive me for using a shotgun parser here.
    //qDebug()<<"Loading GatoROM setting"<<d;

    zorrommode=d.contains("-z ");
    inverted=d.contains("-i ");
    flippedx=d.contains("--flipx ");
    flippedy=d.contains("--flipy ");

    if(d.contains("--leftbank "))
        bank=1;
    else if(d.contains("--rightbank "))
        bank=2;
    else
        bank=0;


    //Forgive me again for bringing regular expressions into this mess.
    static QRegularExpression decoder("--decode-([a-z0-9\\-]+) ");
    QRegularExpressionMatch match=decoder.match(d);
    if(match.hasMatch()){
        //qDebug()<<"Matched decoder to"<<match.captured(1);
        setDecoderByName(match.captured(1));
    }else{
        //qDebug()<<"Failed to match decoder: "<<d;
    }

    static QRegularExpression angle("-r (\\d+) ");
    match=angle.match(d);
    if(match.hasMatch()){
        bool okay=true;
        int rotation=match.captured(1).toInt(&okay,10);

        if(okay){ //Only apply proper angles.
            this->angle=rotation;
            //rotate(rotation, true);
        }
    }

    static QRegularExpression wordsize("-w (\\d+) ");
    match=wordsize.match(d);
    if(match.hasMatch()){
        bool okay=true;
        int wordsize=match.captured(1).toInt(&okay,10);

        if(okay){ //Only apply proper angles.
            this->wordsize=wordsize;
        }
    }
}

//Sets the secoder by an ASCII name.
void GatoROM::setDecoderByName(QString name){
    if(name=="arm6"){
        //ARM6 is no longer special, but we can provide support anyways to be polite.
        decoder=new GatoDecoderColsLeft();
        wordsize=32;
    }else if(name=="tlcs47font")
        decoder=new GatoDecoderTLCSFont();
    else if(name=="z86x1")
        decoder=new GatoDecoderZ86x1();
    else if(name=="cols-downl-swap")
        decoder=new GatoDecoderColsDownLSwap();
    else if(name=="cols-downr")
        decoder=new GatoDecoderColsDownR();
    else if(name=="cols-downl")
        decoder=new GatoDecoderColsDownL();
    else if(name=="cols-left")
        decoder=new GatoDecoderColsLeft();
    else if(name=="cols-right")
        decoder=new GatoDecoderColsRight();
    else if(name=="squeeze-lr")
        decoder=new GatoDecoderSqueezeLR();
    else
        qDebug()<<"Unknown decoder"<<name;
}

//Prints the bits.  Can be handy for manual decoding.
void GatoROM::print(QPrinter &printer){
    GatoPrinter p(this);
    p.print(printer);
}

//Returns the first eight bytes as a preview.
QString GatoROM::preview(){
    QString p="";
    QByteArray b=decode();

    for(int i=0; i<8 && i<b.length(); i++){
        p+=QString::asprintf("%02x ", (uint8_t) b[i]);
    }

    return p;
}

//Initiates an empty ROM.
GatoROM::GatoROM(){
    loadFromString("1");
}
//Initiates around a standard ASCII art of the bits.
GatoROM::GatoROM(QString input){
    loadFromString(input);
}
//Initiates around a raw binary in Sean Riddle's style.
GatoROM::GatoROM(QByteArray input, uint32_t width){
    /* Sean Riddle shares his dumps in binary, with the most
     * significant bit first.  They don't describe their own width,
     * so it's necessary to supply that as an extra parameter.
     */

    QString result="";

    int bitcount=0;
    for(int i=0; i<input.length(); i++){
        for(int bitmask=0x80; bitmask>0; bitmask>>=1){
            //Append the bit.
            result.append(input[i]&bitmask?"1":"0");
            //Is it the end of a row?
            if(++bitcount%width==0)
                result.append("\n");
        }
    }

    loadFromString(result);
}


// Frees the buffers.
GatoROM::~GatoROM(){
    /* We don't free buffers because they might be shared
     * between multiple copies of the class.
     */
    //freeBuffers();
}


void GatoROM::loadFromString(QString input){
    uint32_t rows=0, cols=0;
    uint32_t thisrowlen=0;

    //Gotta count the rows and columns.
    for(int i=0; i<input.length(); i++){
        switch(input.at(i).toLatin1()){
        case '0':
        case '1':
            thisrowlen++;
            break;
        case '\n':
            //We strongly prefer that all rows have the same length.
            if(cols==0 || thisrowlen==cols){
                cols=thisrowlen;
                thisrowlen=0;
                rows++;
            }else if(thisrowlen>0){
                qDebug()<<"GatoROM::loadFromString expected"<<cols<<"but got"<<thisrowlen<<"on row"<<rows;
                if(strictmode)
                    exit(1);
                return loadFromString("1");
            }
            break;
        }
    }

    //This allocates the input buffer, and overallocates the output buffer.
    setInputSize(rows, cols);

    //Now we populate them.
    uint32_t row=0, col=0;
    Q_ASSERT(inputbits);
    for(int i=0; i<input.length(); i++){
        switch(input.at(i).toLatin1()){
        case '1':
            Q_ASSERT(inputbits[row]);
            inputbits[row][col++]=new GatoBit(true);
            break;
        case '0':
            Q_ASSERT(inputbits[row]);
            inputbits[row][col++]=new GatoBit(false);
            break;
            //In the default case, we don't increment the count.
        }

        if(col>=cols){
            row++;
            col=0;
        }
    }

    //Begin with a basic output buffer.
    reset();
}


//Decodes the ROM using the configured decoder.
QByteArray GatoROM::decode(){
    //Update any unapplied settings.
    eval();

    //Zero bytes on failure.  decoder->decode() should overwrite these.
    decoded=QByteArray();
    decodedDamage=QByteArray();

    //If there's a decoder, return the results.
    if(decoder){
        if(verbose)
            qDebug()<<"Decoding with settings:"<<description();
        /* decoded=*/
        decoder->decode(this);
    }

    //Hard error for the damage and data not to be the same length.
    assert(decoded.length()==decodedDamage.length());

    return decoded;
}

GoodASM *GatoROM::goodasm(){
    GoodASM *ga=new GoodASM();
    QString lang=arch;
    if(lang.startsWith("goodasm/"))
        lang=lang.right(lang.length()-QString("goodasm/").length());
    ga->setLanguage(lang);

    //Set both data and damage mask.
    ga->loadDamage(decodedDamage);
    ga->load(decoded);

    return ga;
}

// Disassembly, produced by Unidasm.
QString GatoROM::dis(bool autocomment, bool asbits,
                     bool asbytes, bool asdamage){
    QByteArray bytes=decode();
    if(bytes.length()==0)
        return QString("ERROR: No bytes to disassemble.");


    /* Architectures are optionally prefixed by the disassembler.  If none
     * is specified, we default to Unidasm.
     */
    QString a=arch, assembler="unidasm";

    //Janky parser.
    QStringList tmp=arch.split("/");
    if(tmp.count()==2){
        assembler=tmp[0];
        a=tmp[1];
    }
    if(a=="")
        return QString("ERROR: Architecture not set in Edit/Decoding.");

    if(assembler=="goodasm"){
        decode();

        GoodASM ga;
        ga.setLanguage(a);

        //Show the data listing in the right way.
        ga.listbits=asbits;
        ga.listdbits=asdamage;
        ga.listbytes=asbytes || asbits || asdamage;
        ga.autocomment=autocomment;

        //Set both data and damage mask.
        ga.loadDamage(decodedDamage);
        ga.load(decoded);


        QString s=ga.source();
        return s;
    }

    QProcess process;
    if(assembler=="r2" || assembler=="rasm2")
        process.start("rasm2", QStringList() << "-BD" <<"-a" << a << "-f" << "-");
    else // unidasm default.
        process.start("unidasm", QStringList() << "-arch" << a << "-");


    process.write(decode());

    process.closeWriteChannel();
    process.waitForFinished(3000);
    QString res=process.readAllStandardOutput();
    if(process.exitStatus()==QProcess::NormalExit && res.length()>0)
        return res;
    return QString("ERROR: Is "+assembler+" in the path?\n\n"+process.readAllStandardError());
}

//Performs a sanity check.  Call this after decode(), error if false.
bool GatoROM::checkSanity(){
    //Sanity check for accuracy of the markup.
    for(unsigned int row=0; row<outputrows; row++){
        for(unsigned int col=0; col<outputcols; col++){
            GatoBit *gb=outputbits[row][col];
            if(!gb){
                qDebug()<<"Bit"<<row<<","<<col<<"is not defined after transformations.";
                return false;
            }
            if(decoded.length()>0 && (gb->adr==0xFFFFFFFF || gb->mask==0)){
                qDebug()<<"Bit"<<row<<","<<col<<" has undefined location of"<<gb->adr<<", bit"<<gb->mask;
                return false;
            }
        }
    }

    return true; //Result is sane.
}

void GatoROM::reset(){
    //Simplest to just rotate back to zero.
    //Later we might need to store a state or something.
    rotate(0, true);
}

//Call this to reapply past transformations.
void GatoROM::eval(){
    //Rotation first.
    rotate(0, false);  //0 additional degrees.

    //Banking after rotation, before flips.
    if(bank==1){        //Left Bank
        outputcols>>=1; //Halve the column count.
    }else if(bank==2){  //Right bank.
        outputcols>>=1; //Halve the column count.
        for(unsigned int row=0; row<outputrows; row++){
            for(int col=0; col<outputcols; col++){
                //Translate the bits left; no mirroring.
                outputbits[row][col]=outputbits[row][col+outputcols];
            }
        }
    }

    //Then flips
    flipx(flippedx);
    flipy(flippedy);

    //Invert?
    invert(inverted);
}

//Applies a rotation to the bits.
void GatoROM::rotate(uint32_t degrees, bool zerofirst){
    if(zerofirst)
        angle=0;

    //Continually update.
    angle+=degrees;
    angle%=360;

    //Fake rotation when buffers don't yet exist.
    if(!inputbits[0]) return;

    //If these fail, we've freed the buffers but not yet rebuilt them.
    assert(inputbits[0]);
    assert(outputbits[0]);

    /* Zorrom flips before rotation, while GatoROM
     * flips after rotation.  When compatibility
     * mode is engaged, we fake compatibility by
     * an extra 180 degree rotation when flipping X.
     */
    switch((angle+((zorrommode&&flippedx)?180:0))%360){
    default:
        qDebug()<<"Unsupported rotation angle of"<<angle<<"degrees.  Defaulting to zero.";
    case 0:
        outputrows=inputrows;
        outputcols=inputcols;
        for(unsigned int row=0; row<inputrows; row++)
            for(unsigned int col=0; col<inputcols; col++)
                outputbits[row][col]=inputbits[row][col];
        break;
    case 90:
        outputrows=inputcols;  //Swapping col and row counts.
        outputcols=inputrows;
        for(unsigned int row=0; row<outputrows; row++)
            for(unsigned int col=0; col<outputcols; col++)
                outputbits[row][outputcols-col-1]=inputbits[col][row];
        break;
    case 180:
        outputrows=inputrows;
        outputcols=inputcols;
        for(unsigned int row=0; row<inputrows; row++)
            for(unsigned int col=0; col<inputcols; col++)
                outputbits[outputrows-row-1][outputcols-col-1]=inputbits[row][col];
        break;
    case 270:
        outputrows=inputcols;  //Swapping col and row counts.
        outputcols=inputrows;
        for(unsigned int row=0; row<outputrows; row++)
            for(unsigned int col=0; col<outputcols; col++)
                outputbits[outputrows-row-1][col]=inputbits[col][row];
        break;
    }
}

//This swaps all of the bit pointers in every row.
void GatoROM::flipx(bool flipped){
    flippedx=flipped;

    if(flipped) for(unsigned int row=0; row<outputrows; row++){
        for(unsigned int col=0; col<(outputcols>>1); col++){
            GatoBit *tmp=outputbits[row][col];
            outputbits[row][col]=outputbits[row][outputcols-col-1];
            outputbits[row][outputcols-col-1]=tmp;
        }
    }
}
//This swaps the row bit pointers, but on columns within a row.
void GatoROM::flipy(bool flipped){
    flippedy=flipped;

    if(flipped) for(unsigned int row=0; row<(outputrows>>1); row++){
        GatoBit** tmp=outputbits[row];
        outputbits[row]=outputbits[outputrows-row-1];
        outputbits[outputrows-row-1]=tmp;
    }
}
//This inverts the bits values.
void GatoROM::invert(bool inverted){
    this->inverted=inverted;
    for(unsigned int row=0; row<outputrows; row++){
        for(unsigned int col=0; col<outputcols; col++){
            outputbits[row][col]->inverted=inverted;
        }
    }
}

//Chooses the bank.
void GatoROM::setBank(int b){
    switch(bank){
    case 0:
    case 1:
    case 2:
        this->bank=b;
        break;
    default:
        qDebug()<<"Ignoring illegal bank of"<<b;
    }
}


//Exports the project as bits.
QString GatoROM::exportString(bool pretty){
    //Note that this works only on vals, not on pointers.
    QString ascii="";
    int i=0, lines=0;
    for(unsigned int row=0; row<outputrows; row++){
        for(unsigned int col=0; col<outputcols; col++){
            ascii.append(outputbits[row][col]->getVal()?"1":"0");
            if(pretty && ++i%8==0)
                ascii.append(" ");
        }
        ascii.append("\n");
        if(++lines%8==0)
            ascii.append("\n");
    }
    return ascii;
}

//Exports the project as bits.
QString GatoROM::exportStringDamage(bool pretty){
    //Note that this works only on vals, not on pointers.
    QString ascii="";
    int i=0, lines=0;
    for(unsigned int row=0; row<outputrows; row++){
        for(unsigned int col=0; col<outputcols; col++){
            ascii.append(outputbits[row][col]->ambiguous?"1":"0");
            if(pretty && ++i%8==0)
                ascii.append(" ");
        }
        ascii.append("\n");
        if(++lines%8==0)
            ascii.append("\n");
    }
    return ascii;
}

//Allocates the input buffer, given known dimensions.
void GatoROM::setInputSize(const uint32_t rows, const uint32_t cols){
    if(inputbits)
        freeBuffers();

    //Output is sized for the worst case in rotation and both axes.
    uint32_t outsize=(rows>cols?rows:cols);

    if(!outsize){
        //qDebug()<<"gatorom.cpp: Output array would be zero, growing to 1x1 for sanity.";
        outsize=1;
    }

    /* The output matrix is large enough to fit the rotated or unrotated
     */
    inputbits=(GatoBit***) malloc(16*outsize);
    outputbits=(GatoBit***) malloc(16*outsize);
    for(unsigned int i=0; i<outsize; i++){
        inputbits[i]=(GatoBit**) malloc(16*outsize);
        outputbits[i]=(GatoBit**) malloc(16*outsize);
        memset(inputbits[i],0,16*outsize);
        memset(outputbits[i],0,16*outsize);
        //We don't populate the individual elements.
    }
    outputrows=inputrows=rows;
    outputcols=inputcols=cols;
}

//Frees the old buffers to avoid leaking memory.
void GatoROM::freeBuffers(){
    //Output is sized for the worst case in rotation and both axes.
    uint32_t outsize=(inputrows>inputcols?inputrows:inputcols);

    //Empty any pointers on the input.
    for(unsigned int i=0; i<outsize; i++){
        //Free the bits.
        for(int j=0; j<outsize; j++){
            if(inputbits[i][j]){
                free(inputbits[i][j]);
                inputbits[i][j]=0;
            }
        }
        //Free the rows.  Separate for input and output.
        free(inputbits[i]);
        inputbits[i]=0;
        free(outputbits[i]);
        outputbits[i]=0;
    }
    //Free the first-bit column.
    free(inputbits);
    inputbits=0;
    free(outputbits);
    outputbits=0;
}



