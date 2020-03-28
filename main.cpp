#include "mbed.h"
#include <ctime>

Serial pc(USBTX, USBRX);
AnalogOut Aout(DAC0_OUT);
AnalogIn Ain(A0);
BusOut display(D6, D7, D9, D10, D11, D5, D4, D8);
DigitalOut redLED(LED_RED);
DigitalOut greenLED(LED_GREEN);
DigitalIn  Switch(SW3);


char table[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

int sample = 128;
int i;
float ADCdata[128];

int main(void)
{
    // set the initial state
    greenLED = 0;
    redLED = 1;
    display = 0x00;

    // At first, sample the data and send them to pc
    for (i = 0; i < sample; i++) {
        ADCdata[i] = Ain;
        wait(1.0/sample);
    }
    for (i = 0; i < sample; i++) {
        pc.printf("%1.3f\r\n", ADCdata[i]);
        wait(0.001);
    }
    // calculate the frequency
    bool positive = true;
    int count = 0;
    clock_t begin = clock();
    int freq;
    int digit[3];

    // sample for 2 second
    while((clock() - begin) / CLOCKS_PER_SEC < 2) {
        // record the times of oscillations
        if (positive && Ain < 0.5) { 
            count++;
            positive = false;
        }
        else if (!positive && Ain > 0.5) {
            count++;
            positive = true;
        }
    }
    freq = count / 2;
    digit[0] = freq / 100;
    digit[1] = freq % 100 / 10;
    digit[2] = freq % 10;

    // show the sine wave for 3 second
    while ((clock() - begin) / CLOCKS_PER_SEC < 5) {
        for(float i = 0; i < 2; i += 0.05) {
            // so there will be 2 / 0.05 = 40 times of loop
            Aout = 0.5 + 0.5*sin(i*3.14159);
            wait(1.0 / freq / 40); // so the entire sine wave will cost 1 / freq second 
        }
    }


    // wait for someone pressing the button
    bool show_result;
    while (1) {   
        if (Switch == 1) {
            greenLED = 0;
            redLED = 1;
            show_result = true;        
        }
        else if (Switch == 0) {
            greenLED = 1;
            redLED = 0;
            show_result = false;
        }
        
        if (show_result) {
            display = table[digit[0]];
            wait(1);
            display = table[digit[1]];
            wait(1);
            display = table[digit[2]] | 0x80;
            wait(1);  
        }
        else 
            display = 0x00;
    }
}
