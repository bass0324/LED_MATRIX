// led_display.c
//
// Controlling LED display prototype with Raspberry Pi.
// gcc -o tempCheck tempCheck.c -I/usr/local/include -L/usr/local/lib -lwiringPi

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Define GPIO Pins
#define P1_RED 0
#define P1_GREEN 2
#define P1_BLUE 3
#define P2_RED 8
#define P2_GREEN 9
#define P2_BLUE 7

#define PWM_MAX 32

#define LED_NUM 2
#define CLRS 3

// Declare Functions Prior to Main

// Matrix Data
enum {CLR_RED, CLR_GREEN, CLR_BLUE};
unsigned char led[LED_NUM][CLRS];

unsigned char pwm = 0;
			  
// PI_THREAD (timeToDraw)
// {
	// while(1)
	// {
		// drawMap();
		// delayMicroseconds(10);
	// }
// }

int main(void)
{
	if (wiringPiSetup () == -1)
		exit (1);

	// Set RPI data pins to be output
	pinMode(P_RED, OUTPUT);
	pinMode(P_GREEN, OUTPUT);
	pinMode(P_BLUE, OUTPUT);
	pinMode(P_PWR, OUTPUT);
	pinMode(P_LATCH, OUTPUT);
	pinMode(P_CLK, OUTPUT);
	
	//x = piThreadCreate(timeToDraw);
	//if (x != 0)
    //   printf ("it didn't start\n")
	
	while(1)
	{
		digitalWrite(P1_RED, HIGH);
		delayMicroseconds(1000);
		digitalWrite(P1_RED, LOW);
		digitalWrite(P1_GREEN, HIGH);
		delayMicroseconds(1000);
		digitalWrite(P1_GREEN, LOW);
		digitalWrite(P1_BLUE, HIGH);
		delayMicroseconds(1000);
		digitalWrite(P1_BLUE, LOW);
		digitalWrite(P2_RED, HIGH);
		delayMicroseconds(1000);
		digitalWrite(P2_RED, LOW);
		digitalWrite(P2_GREEN, HIGH);
		delayMicroseconds(1000);
		digitalWrite(P2_GREEN, LOW);
		digitalWrite(P2_BLUE, HIGH);
		delayMicroseconds(1000);
		digitalWrite(P2_BLUE, LOW);
	}
	
	return 0;
}