#ifndef GATOROM_H
#define GATOROM_H

/* So GatoROM is the main class of the GatoROM library, responsible for
 * holding your physically-ordered input bits, transforming them, and
 * then spitting out some logically-ordered output bytes.
 *
 * The algorithm is loosely the same as ZorROM:
 * 1. Rotation comes first: 0, 90, 180, or 270 degrees.
 * 2. Flips come next, in X, Y, or in both.
 * 3. Decoder comes last.  If this is missing, we write a new one.
 *
 * A GatoBit is a little more complicated than a bool.  This adds
 * some performance overhead, but it should be manageably small
 * and allow for things like reverse transforms through pointers.
 * In the CLI mode, we only use the value and not the pointer.
 *
 */


#include <QString>
#include <QByteArray>

/* We try to avoid GUI dependencies, but printing is a
 * necessary exception.
 */
class QPrinter;

class GatoDecoder;


//Represents just a bit.  CLI uses the value, GUIs might use ptr.
class GatoBit{
public:
    GatoBit(bool v);         //Init with just a value.
    bool val=false;          //Raw value of the bit.
    bool inverted=false;     //True if bits are inverted.
    void* ptr=0;             //Pointer to the bit's object.
    uint32_t adr=0xFFFFFFFF, //Address and mask of the bit.
        mask=0;
    bool getVal();           //Returns not the raw value, but the value after inversion.
};


//Represents an entire ROM, both input and output.
class GatoROM{
public:
    //Initiates an empty ROM, to be populated and resized later.
    GatoROM();
    //Initiates around a standard ASCII art of the bits.
    GatoROM(QString input);
    //Initiates around a raw binary in Sean Riddle's style.
    GatoROM(QByteArray input, uint32_t width);


    //Before any processing at all.
    GatoBit*** inputbits=0;
    uint32_t inputrows=0, inputcols=0;

    //First we rotate, then we flip.
    void reset();                                   //Resets to input state.
    void rotate(uint32_t degrees, bool zerofirst);  //Rotation comes before flips.
    void flipx(bool flipped);                       //Flip horizontally.
    void flipy(bool flipped);                       //Flip vertically.
    void invert(bool inverted);                     //Invert every bit.
    void eval();                                    //Re-applies all transformations.

    //After rotation and mirroring, before decoding.
    GatoBit*** outputbits=0;
    uint32_t outputrows=0, outputcols=0;
    GatoDecoder *decoder=0;
    void setDecoderByName(QString name);

    //Exports the output as ASCII art.
    QString exportString(bool pretty=true);
    //Prints the bits.  Can be handy for manual decoding.
    void print(QPrinter &printer);
    //Decodes the ROM using the configured decoder.
    QByteArray decode();
    QByteArray decoded; //Output bytes, produced by decode().
    //Performs a sanity check.  Call this after decode(), error if false.
    bool checkSanity();

    //Returns an English description of the current ROM state.
    QString description();
    //Loads from the same description.
    void configFromDescription(QString description);

    //Returns the first eight bytes as a preview.
    QString preview();

    //Loads ASCII art text into the structure.
    void loadFromString(QString str);

    int zorrommode=0; //Compatibility with Zorrom's bugs.

    //Don't set these directly.
    int flippedx=0;     //Is X flipped?
    int flippedy=0;     //Is Y flipped?
    int inverted=0;     //Are bits inverted?
    int angle=0;        //Angle of rotation.
private:
    //Allocates the input size, plus a little extra for rotations.
    void setInputSize(const uint32_t rows, const uint32_t cols);
    GatoBit **input=0;  //2D Array of input bits, rows then cols.

};

//Represents a decoder, which turns a transofmred GatoROM into a Byte array.
class GatoDecoder{
public:
    QString name;
    virtual QByteArray decode(GatoROM *gr)=0;
};

#endif // GATOROM_H
