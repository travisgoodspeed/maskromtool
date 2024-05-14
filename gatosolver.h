#ifndef GATOSOLVER_H
#define GATOSOLVER_H

#include <QByteArray>
class GatoROM;
class GatoDecoder;

/* The concept of a solver is shamelessly stolen from Zorrom,
 * but our implementation details are quite different.
 *
 * Basically, the GatoSolver class enumerates every possible
 * layout of a GatoRom, while a GatoGrader scores the likelihood
 * that a given decoding is correct.
 */

class GatoGrader{
public:
    //Returns a grade of a byte array.
    //Negative grades are a hard rejection, 100 is hard success.
    virtual int grade(QByteArray ba)=0;
};

class GatoGraderAll : public GatoGrader
{
public:
    GatoGraderAll();
    int grade(QByteArray ba);
};

class GatoSolver{
public:
    //Instantiate the solver on a ROM and a Grader.
    GatoSolver(GatoROM *rom, GatoGrader *grader);
    //Initialize the solver to its first state.
    void init();
    //Step to the next state.  Returns false when complete.
    bool next();
    //Are we there yet?
    bool finished();
    //Get the grade, from -1 to 100.
    int grade();

    //State number, the iterator that init() and next() operate on.
    int state=0;
    //Rearranges to the ROM to the current state.
    bool applyState();
private:
    GatoROM *rom;
    GatoGrader *grader;
    GatoDecoder *decoders[16];
};

#endif // GATOSOLVER_H
