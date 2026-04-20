// This is a yara-x rule for recognizing MSP430 boot ROMs.  These are
// usually 64 bits wide and 128 bits tall, using a 16-bit word size.

rule msp430bsl  {
meta:
  description = "MSP430 Bootloader ROM"
  noisy=false
  wordsize=16
  
strings:


/* This rule matches against the BSL ROM of the MSP430F149,
   MSP430F449, and hopefully other families.  It relies on a few
   consistent aspects of these ROMS that are shared by the MSP430 and
   MSP430X families.  It is not expected to work on custom ROMs that
   lack the BSL, but you can still solve the generic part to know the
   decoding settings of the custom part.

   1. Every BSL begins with at least one entry point near 0x0c00.
   2. Every BSL disables interrupts.
   3. Every BSL adjusts the P1OUT (0x0021) and P1DIR (0x0022) bits.

   If this rule fails to match a BSL version, please report
   that as a bug in the issue tracker.

   --Travis

 */

$msp430bsl = {
  0? 0c   //Early entry point, some versions lack late entry.
  [6-12]  //skip
  32 c2   //Diable Interrupts
  [8-64]  //skip
  ?2 d3 21 00 //bis.b ?, 0x0021
  ?2 d3 22 00 //bis.b ?, 0x0022
}

condition:
  $msp430bsl
}

