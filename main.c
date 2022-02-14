#include <msp430.h> 
#include "UcsControl.h"

/**
 * main.c
 */
void neoPixelPWMSetUp();

#define dcoFreq 20                      //MHz.
#define sclkDiv 2                          //SPI sclk divide. SCLK MAX to the DAC is 20MHz.
int alternate=0;
int bitCount=0;
int resetCount=0;
int main(void)
{
 	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    unsigned char oscFail = 1;
    /*********Set clock frequency*********************************************/
    //unsigned char testPass = 1;
    ucsSelSource(1,1,1,1);
    oscFail = ucsDcoFreqSet (dcoFreq, 2, 1);            //set sclk to dcoFreq
    if (oscFail)
        return 1;
    /***End***Set clock frequency*********************************************/

	neoPixelPWMSetUp();
	_bis_SR_register(GIE);
	TA0CTL|= TAIE; //enabling timer A;

	while(1){
	    }
	return 0;
}

void neoPixelPWMSetUp(){
    P1DIR |= BIT2;
    TA0CTL = TASSEL_2|ID_0|MC_1|TAIFG;
    TA0CCTL1=OUTMOD_7|CCIFG|CCIE;
    TA0EX0= TAIDEX_0;
    TA0CCR0=23;
    TA0IV=0xFF;
    TA0CCR1=8;// equals to a 0
    //TA0CCR1=12; //equals to a 1

    P1SEL |= BIT2;
}


#pragma vector = TIMER0_A1_VECTOR //CCR0   /* 0xFFE8
__interrupt void dataTransferNEOPIXEL (void){
    switch(__even_in_range(TA0IV,0xE))
    {
    case 0:break;
    case 2:
        TA0CCTL1 &= ~CCIFG; //signals 1 pwm cycles has been completed. Means that a bit has gone through.
       if(alternate==1){
            TA0CCR1=8;// equals to a 0
            alternate=0;
        }
            else if(alternate == 0){
            TA0CCR1=12; //equals to a 1
            alternate=1;
        }
        bitCount++;
        break;
    case 4:break;
    case 6:break;
    case 8:break;
    case 0xA:break;
    case 0xC:break;
    case 0xE:
        TA0CTL &= ~TAIFG; //signals 1 pwm cycles has been completed. Means that a bit has gone through.
        if(bitCount >= 384){
            TA0CCR1=0;
            alternate=2;
            resetCount++;
        }
        if(resetCount > 68){
            alternate=0;
            resetCount=0;
            bitCount=0;
        }
        break;
    default: break;
    }


}



