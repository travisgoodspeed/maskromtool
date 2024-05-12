#include "gatodecoderarm6.h"

#include <QDebug>

/* This decoder is used for a 32-bit ROM on the Mykotronix MYK82 chip,
 * and we call it an ARM6 ROM because it might have been used on other ARM6
 * devices.  It's pretty much the same as cols-left except that it has
 * 32 bits per word instead of 8.
 *
 * While this used to be its own implementation, it now just adjusts the
 * wordsize from cols-left.  When wordsize is configurable in both the GUI
 * and the CLI, I'll probably cut this out.
 */

GatoDecoderARM6::GatoDecoderARM6(){
    name="arm6";
}

