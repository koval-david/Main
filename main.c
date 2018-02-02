/*   
 * David Koval, Desmond Tomaselli
 * Professor Traver
 * Project 2
 * Feb. 5, 2017
 */

/*********** COMPILER DIRECTIVES *********/

// #include for textbook library header files
#include "pic24_all.h"

// #defines for handy constants 
#define LED LATAbits.LATA0  // LED on microstick is connected to RA0 (PORTA, bit 0)
#define PWM_PERIOD 3125    //20ms PWM period with clock cycle of 6.4us from 1:256 prescale
#define C_MIN 203          //1.3ms (min) pulse width for rotation servo
#define C_MAX 266          //1.7ms (max) pulse width for rotation servo
#define P_MIN 117          //0.75ms (min) pulse width for positional servo
#define P_MAX 351          //2.25ms (max) pulse width for positional servo

/*********** GLOBAL VARIABLE AND FUNCTION DEFINITIONS *******/
uint16_t pulse_width;       //pulse width for servo

void configOC1() {
    T2CONbits.TON = 0; //Turns the timer off.
    CONFIG_OC1_TO_RP(RB0_RP); //Maps the OC1 output to the remappable pin, RB1.
    OC1RS = 0; //Clears the register
    OC1R = 0; //Clears the register
    OC1CONbits.OCTSEL = 0; //Sets the output compare module to use Timer 2 as the clock source.
    OC1CONbits.OCM = 0b110; //Sets it to operate in PWM mode with fault pin disabled. 
}

void config0C2(void){
    T2CONbits.TON = 0; //Turns the timer off.
    CONFIG_OC2_TO_RP(RB4_RP); //Maps the OC2 output to the remappable pin, RB4.
    OC2RS = 0;  // Clears OC2RS register
    OC2R = 0;   // Clears OC2R register
    OC2CONbits.OCTSEL = 0; //Sets the output compare module to use Timer 2 as the clock source.
    OC2CONbits.OCM = 0b110; //Sets it to operate in PWM mode with fault pin disabled. 
}

void configTimer2() {
    T2CONbits.TON = 0; //Turns the timer off.
    T2CONbits.TCKPS = 0b11;
    T2CONbits.TCS = 0; //Sets the timer 2 to the instruction clock. 
    PR2 = PWM_PERIOD;
    TMR2 = 0; //Clears the timer 2 register.
    _T2IF = 0; //Clears the timer 2 flag.
    T2CONbits.TON = 1; //Turns the timer on.
}

void _ISR _T2Interrupt(void) {
    OC1RS = pulse_width;
    _T2IF = 0; //Clears the Timer 2 interrupt flag, the last instruction in ISR
}

void continuousRatationServo(void){
    DELAY_MS(500);
        for (pulse_width=C_MIN; pulse_width < C_MAX; pulse_width++) {
            DELAY_MS(500);
        }
        for (pulse_width=C_MAX; pulse_width > C_MIN; pulse_width--) {
            DELAY_MS(500);
        }
}

void positionalServo(void){
    DELAY_MS(500);
        for (pulse_width=P_MIN; pulse_width < P_MAX; pulse_width++) {
            DELAY_MS(500);
        }
        for (pulse_width=P_MAX; pulse_width > P_MIN; pulse_width--) {
            DELAY_MS(500);
        }
}
/********** MAIN PROGRAM LOOP********************************/
int main ( void )  //main function that....
{ 
/* Define local variables */


/* Call configuration routines */
	configClock();  //Sets the clock to 40MHz using FRC and PLL
  	configHeartbeat(); //Blinks the LED on RA1
    configOC1(); //Configures the register
    config0C2(); //Configures the reg
    configTimer2(); //Configures the timer
    

/* Initialize ports and other one-time code */
    _T2IE = 1; //Enables the timer 2 interrupts
    pulse_width = 234;

/* Main program loop */
    while (1) {	
        continuousRatationServo();
//        positionalServo();
    }
        
}
