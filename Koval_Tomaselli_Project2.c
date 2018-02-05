/*   
 * David Koval, Desmond Tomaselli
 * Professor Traver
 * Project 2
 * Feb. 5, 2017
 */

/*********** COMPILER DIRECTIVES *********/

// #include for textbook library header files
#include "adc.h"                // header for simple adc library
#include "pic24_all.h"

// #defines for handy constants 
#define PWM_PERIOD 3125     //20ms PWM period with clock cycle of 6.4us from 1:256 prescale

#define C_MIN 203           //1.3ms (min) pulse width for rotation servo
#define C_MAX 266           //1.7ms (max) pulse width for rotation servo
#define C_MID 234           //1.0ms pulse for rotation servo

#define P_MIN 117           //0.75ms (min) pulse width for positional servo
#define P_MAX 351           //2.25ms (max) pulse width for positional servo
#define P_MID 156           //1.5ms pulse for positional servo

#define SW PORTBbits.RB15           //Set switch to pin 26 (port RB15)
#define SW_CONTINUOUS() (SW == 1)   //Switch is reading a high value
#define SW_POSITIONAL() (SW != 1)   //Switch is reading a low value

/*********** GLOBAL VARIABLE AND FUNCTION DEFINITIONS *******/
uint16_t C_pulse_width;         //pulse width for Continuous motor
uint16_t P_pulse_width;         //pulse width for Positional motor
uint16_t adcvalue;              //potentiometer value

// Configures Output compare module 1 for continuous Ratation Servo
void configOC1() {
    T2CONbits.TON = 0;        //Turns timer 2 off.
    CONFIG_OC1_TO_RP(RB0_RP); //Maps the OC1 output to the remappable pin, RB0.
    OC1RS = 0;                //Clears the RS register
    OC1R = 0;                 //Clears the R register
    OC1CONbits.OCTSEL = 0;    //Sets the output compare module to use Timer 2 as the clock source.
    OC1CONbits.OCM = 0b110;   //Sets it to operate in PWM mode with fault pin disabled.
}

// Configures Output compare module 2 for positional Servo
void config0C2(void){
    T2CONbits.TON = 0;          //Turns timer 2 off.
    CONFIG_OC2_TO_RP(RB4_RP);   //Maps the OC2 output to the remappable pin, RB4.
    OC2RS = 0;                  //Clears the OC2RS register
    OC2R = 0;                   //Clears the OC2R register
    OC2CONbits.OCTSEL = 0;      //Sets the output compare module to use Timer 2 as the clock source.
    OC2CONbits.OCM = 0b110;     //Sets it to operate in PWM mode with fault pin disabled.
}


//Configures Timer 2
void configTimer2() {
    T2CON = 0x0030;             //Configs timer 2, with presacle 1:256
    PR2 = PWM_PERIOD;           //Sets period
    TMR2 = 0;                   //Clears the timer 2 register.
    _T2IF = 0;                  //Clears the flag, T2IF
}

//Scales adcvalue to proportional value in the range of p_min and p_max
uint16_t scale(uint16_t u16_x, uint16_t p_min, uint16_t p_max){
    return (uint16_t)(((u16_x/4095.0)*(p_max-p_min)) + p_min);
}

//Configures the timer 2 interupt and loads its registers
void _ISR _T2Interrupt(void) {
    OC1RS = C_pulse_width;      //Load OC1RS register with Continuous pulse width
    OC2RS = P_pulse_width;      //Load OC2RS register with Positional pulse width
    _T2IF = 0;                  //Clears the Timer 2 interrupt flag, the last instruction in ISR
}


/********** MAIN PROGRAM LOOP********************************/
int main ( void )  //main function that....
{ 
/* Define local variables */


/* Call configuration routines */
	configClock();  //Sets the clock to 40MHz using FRC and PLL
  	configHeartbeat(); //Blinks the LED on RA1
    configOC1(); //Configures output compare module 1 for Continuous motor
    config0C2(); //Configures output compare module 2 for Positional motor
    configTimer2(); //Configures timer 2
    
    

/* Initialize ports and other one-time code */
    TRISBbits.TRISB15 = 1;  //Map pin 26 (RB15) to switch
    _TRISB15 = 1;           //Set pin 26 (RB15) to be an input pin
    _CN11PUE = 1;           //Set pull up resistor for pin 26 (RB15)
    
    initADC(AN1);           //Initialze pin AN1 for analog reading of Potentiometer
    
    _T2IE = 1;              //Enables the timer 2 interrupts
    T2CONbits.TON = 1;      //Turns timer 2 on
    
/* Main program loop */
    while (1) {
        adcvalue = readADC(AN1);        //reads in value from port AN1 (voltage from resistance of Potentiometer)
        if(SW_POSITIONAL()){            //If switch is set to ground then control Positional motor
            C_pulse_width = 0;          //Set continuous servo motor PW to 0
            P_pulse_width = scale(adcvalue, P_MIN, P_MAX);  //Scale potentiometer voltage to a PW that corresponds to a position between 0 and 180 degrees
            DELAY_MS(50);                                   //50 ms delay
        }
        if(SW_CONTINUOUS()){            //If switch is set to high then control Continuous
            C_pulse_width = scale(adcvalue, C_MIN, C_MAX);  //Scale potentiometer voltage to PW that changes speed of continous motor
            DELAY_MS(50);               //50 ms delay
        }
        
    }
        
}
