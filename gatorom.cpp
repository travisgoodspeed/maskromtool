#include <QDebug>

#include "gatorom.h"

GatoBit::GatoBit(bool v){
    this->val=v;
}
bool GatoBit::getVal(){
    return this->val ^ this->inverted;
}

//Returns an English description of the current ROM state.
QString GatoROM::description(){
    QString d="";

    if(decoder)
        d.append("--decode-"+decoder->name+" ");
    if(inverted)
        d.append("-i ");
    d.append("-r "+QString::number(angle)+" ");
    if(flippedx)
        d.append("--flipx ");

    return d;
}


//Initiates around a standard ASCII art of the bits.
GatoROM::GatoROM(QString input){
    loadFromString(input);
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
            }else{
                if(thisrowlen>0)
                    qDebug()<<"Expected"<<cols<<"but got"<<thisrowlen<<"on row"<<rows;
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
        //qDebug()<<"Setting row "<<row<<", col"<<col<<"to value"<<input.at(i).toLatin1();

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

//Decodes the ROM using the configured decoder.
QByteArray GatoROM::decode(){
    //Update any unapplied settings.
    eval();

    //If there's a decoder, return the results.
    if(decoder)
        decoded=decoder->decode(this);
    else
        decoded=QByteArray();  //Zero bytes on failure.

    return decoded;
}
//Performs a sanity check.  Call this after decode(), error if false.
bool GatoROM::checkSanity(){
    //Sanity check for accuracy of the markup.
    //This should probably move into the GatoROM class.
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

//Allocates the input buffer, given known dimensions.
void GatoROM::setInputSize(const uint32_t rows, const uint32_t cols){
    if(inputbits)
        qDebug()<<"WARNING: GatoRom input buffer is not empty.  Maybe leaking memory?";

    uint32_t outsize=(rows>cols?rows:cols);
    Q_ASSERT(outsize>0);

    qDebug()<<"Allocating "<<outsize<<"rows and columns";

    /* The output matrix is large enough to fit the rotated or unrotated
     */
    inputbits=(GatoBit***) malloc(16*outsize);
    outputbits=(GatoBit***) malloc(16*outsize);
    for(unsigned int i=0; i<outsize; i++){
        inputbits[i]=(GatoBit**) malloc(16*outsize);
        outputbits[i]=(GatoBit**) malloc(16*outsize);
        //We don't populate the individual elements.
    }
    outputrows=inputrows=rows;
    outputcols=inputcols=cols;
}
