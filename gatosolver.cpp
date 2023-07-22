#include <QDebug>

#include "gatosolver.h"
#include "gatorom.h"

#include "gatodecoderarm6.h"
#include "gatodecodercolsdownl.h"
#include "gatodecodercolsdownr.h"
#include "gatodecodercolsleft.h"
#include "gatodecodercolsright.h"
#include "gatodecodersqueezelr.h"
#include "gatodecodertlcsfont.h"
#include "gatodecodermsp430.h"


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
    decoders[5]=new GatoDecoderARM6();
    decoders[6]=new GatoDecoderTLCSFont();
    decoders[7]=new GatoDecoderMSP430();
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


//This is a little lazy, might cause bugs on big endian machines.
struct statefield {
    unsigned char flipx : 1;
    //unsigned char flipy : 1;
    unsigned char rotation : 2;
    unsigned char invert : 1;
    unsigned char decoder : 3;  //Always the last thing we iterate.
    unsigned char toohigh : 1;  //Zero until the others have overflowed.
};

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

        rom->flipx(state->flipx);
        rom->invert(state->invert);
        rom->decoder=decoders[state->decoder];

        //We'll have invalid states within the table when the decoder is null.
        if(rom->decoder){
            rom->decode();
            return true;
        }

        //If the state is invalid, quietly try the next.
        this->state++;
        state=(statefield*) &(this->state);
    }

    return false;
}

//Get the grade, from -1 to 100.
int GatoSolver::grade(){
    assert(grader);
    assert(rom);

    return grader->grade(rom->decoded);
}
