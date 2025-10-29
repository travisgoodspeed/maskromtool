#include <QDebug>

#include "gatosolver.h"
#include "gatorom.h"

#include "gatodecodercolsdownl.h"
#include "gatodecodercolsdownr.h"
#include "gatodecodercolsleft.h"
#include "gatodecodercolsright.h"
#include "gatodecodersqueezelr.h"
#include "gatodecodertlcsfont.h"
#include "gatodecodercolsdownlswap.h"  //NEC uCOM4
#include "gatodecodertms320c28x.h"     //TI TMS320C28x
#include "gatodecoderz86x1.h"


// Default solver is a dummy state that covers everything.
GatoGraderAll::GatoGraderAll(){}
// Approves of every state.
int GatoGraderAll::grade(QByteArray ba){
    return 100;
}

GatoGrader::~GatoGrader(){

}


GatoSolver::GatoSolver(GatoROM *rom, GatoGrader *grader){
    assert(grader);
    assert(rom);

    this->rom=rom;
    this->grader=grader;

    //We can grow this table when it becomes too large.
    decoders[0]=new GatoDecoderColsLeft();
    decoders[1]=new GatoDecoderColsRight();
    decoders[2]=new GatoDecoderSqueezeLR();
    decoders[3]=new GatoDecoderColsDownL();
    decoders[4]=new GatoDecoderColsDownR();
    decoders[5]=new GatoDecoderTLCSFont();
    decoders[6]=new GatoDecoderZ86x1();
    decoders[7]=new GatoDecoderColsDownLSwap(); //NEC uCOM4
    decoders[8]=new GatoDecoderTMS320C28x();    //TI TMS320C28x
    //Remainder of table must be null.
    decoders[9]=0;
    decoders[10]=0;
    decoders[11]=0;
    decoders[12]=0;
    decoders[13]=0;
    decoders[14]=0;
    decoders[15]=0;

}


//Initialize the solver to its first state.
void GatoSolver::init(){
    state=0;
    applyState();
}
//Step to the next state.  Returns 0 when complete.
bool GatoSolver::next(){
    state++;
    return applyState();
}


//Packing works differently on each compiler.
#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#elif defined(__GNUC__)
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif


//This is a little lazy, might cause bugs on big endian machines.
PACK(struct statefield {
    uint16_t rotation : 2;
    uint16_t flipx : 1;
    //uint16_t flipy : 1;
    uint16_t invert : 1;
    uint16_t bank : 2;     //No banking, left or right banking.
    uint16_t decoder : 4;  //Always the last thing we iterate.
    uint16_t toohigh : 1;  //Zero until the others have overflowed.
});

//Are we there yet?
bool GatoSolver::finished(){
    statefield *state=(statefield*) &(this->state);
    return state->toohigh;
}

bool GatoSolver::applyState(){
    /* Basically the state is a big-ass integer that contains bitfields
     * for all relevant settings.  Two bits for the rotation, so many bits
     * for the decoder type, etc.  By scanning all values of it, we can
     * attempt every valid decoding of a ROM.
     */

    statefield *state=(statefield*) &(this->state);


    while(!state->toohigh){  //Overflow condition.
        switch(state->rotation){
        case 0:
            rom->rotate(0,true);
            break;
        case 1:
            rom->rotate(90,true);
            break;
        case 2:
            rom->rotate(180,true);
            break;
        case 3:
            rom->rotate(270,true);
            break;
        }

        rom->bank=state->bank;
        rom->flipx(state->flipx);
        rom->invert(state->invert);
        rom->decoder=decoders[state->decoder];

        //We'll have invalid states within the table when the decoder is null.
        if(rom->decoder && rom->bank!=3){
            rom->decode();  //Implies an eval().
            return true;
        }

        //If the state is invalid, quietly try the next.
        this->state++;
        //state=(statefield*) &(this->state); //No need, because the pointer aims here.
    }

    return false;
}


//Get the grade, from -1 to 100.
int GatoSolver::grade(){
    assert(grader);
    assert(rom);

    int grade=grader->grade(rom->decoded);
    return grade;
}

