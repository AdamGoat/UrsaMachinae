#include "rpiPWM1.h"


int main (void){
        
    rpiPWM1 pwm(1000.0, 256, 80.0, rpiPWM1::MSMODE);
    // initialize PWM1 output to 1KHz 8-bit resolution 80% Duty Cycle & PWM mode is MSMODE
    unsigned int dcyccount = 0; // reset Duty Cycle to Zero
    /*while(dcyccount != 256){
        pwm.setDutyCycleCount(dcyccount); // increase Duty Cycle by 16 counts every two seconds
        dcyccount += 16;// until we hit 256 counts or 100% duty cycle
        printf("Duty Cycle is %3.2lf \n",pwm.getDutyCycle());
        printf("Divisor is %d\n", pwm.getDivisor());
        usleep(2000000);
    }*/
    pwm.setDutyCycleCount(128);
    return 0;
}
