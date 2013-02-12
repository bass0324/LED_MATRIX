// led_display.c
//
// Controlling LED display prototype with Raspberry Pi.
// gcc -o tempCheck tempCheck.c -I/usr/local/include -L/usr/local/lib -lwiringPi

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Define GPIO Pins
#define P_RED 12
#define P_GREEN 13
#define P_BLUE 14
#define P_PWR 8
#define P_LATCH 9
#define P_CLK 7

// Matrix Size
#define COLS 16
#define ROWS 8
#define CLRS 3

#define PWM_MAX COLS

// Declare Functions Prior to Main
void horizGrad(const unsigned char clr, const unsigned char dir);
void vertGrad(const unsigned char clr, const unsigned char dir);
void fullOn();
void fullOff();
void pulseOnPwr();
void pulseOffPwr();
void pulseLatch();
void setColors(const unsigned char col, const unsigned char row);
void drawMap();
void updatePixel(unsigned char col, unsigned char row,
				 unsigned char clr, unsigned char val,
				 unsigned char mode);
void drawChar(const unsigned char val, const unsigned char x, const unsigned char y,
			  const unsigned char red, const unsigned char green, const unsigned char blue,
			  const unsigned char mode);
void drawRect(const unsigned char x, const unsigned char y, const unsigned char dx, const unsigned char dy,
			  const unsigned char red, const unsigned char green, const unsigned char blue,
			  const unsigned char mode);
			  
char getTemp(int wChar);
			  
PI_THREAD (timeToDraw)
{
	while(1)
	{
		drawMap();
		delayMicroseconds(10);
	}
}

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
	
	// Matrix Data
	enum {CLR_RED, CLR_GREEN, CLR_BLUE};
	unsigned char led[COLS][ROWS][CLRS];

	enum {MODE_SET, MODE_ADD, MODE_SUB, MODE_MULT, MODE_DIV};
	unsigned char pwm = 0;
	
	// Get Temperature Data from Raspberry Pi (will be implemented later)
	unsigned char temp1 = getTemp(5);
	unsigned char temp2 = getTemp(6);
	unsigned char temp3 = getTemp(8);
	
	piThreadCreate(timeToDraw);
	
	while(1)
	{
		temp1 = getTemp(5);
		temp2 = getTemp(6);
		temp3 = getTemp(8);
		drawChar(temp1,0,2,PWM_MAX,PWM_MAX,PWM_MAX,MODE_SET);
		drawChar(temp2,4,2,PWM_MAX,PWM_MAX,PWM_MAX,MODE_SET);
		drawChar('.',8,2,PWM_MAX,PWM_MAX,PWM_MAX,MODE_SET);
		drawChar(temp2,9,2,PWM_MAX,PWM_MAX,PWM_MAX,MODE_SET);
		drawChar('C',13,2,PWM_MAX,PWM_MAX,PWM_MAX,MODE_SET);
		delay(.2);
	}
	
	return 0;
}

// Functions
void horizGrad(const unsigned char clr, const unsigned char dir)
{
	for(unsigned int col = 0; col < COLS; col++)
	{
		for(unsigned int row = 0; row < ROWS; row++)
		{
			led[col][row][clr] = (dir ? col : COLS - col) *
				((float)PWM_MAX / COLS);
		}
	} 
}

void vertGrad(const unsigned char clr, const unsigned char dir)
{
	for(unsigned char col = 0; col < COLS; col++)
	{
		for(unsigned char row = 0; row < ROWS; row++)
		{
			led[col][row][clr] = (dir ? row : ROWS - row) * 
				((float)PWM_MAX / ROWS);
		}
	} 
}

void fullOn() { memset(led, 0xff, ROWS * COLS * CLRS); }
void fullOff() { memset(led, 0x00, ROWS * COLS * CLRS); }

void pulseOnPwr()
{
	digitalWrite(P_PWR, LOW);
	digitalWrite(P_CLK, HIGH);
	digitalWrite(P_CLK, LOW);
	//_CTR_PORT = _CLK_MASK | _LATCH_MASK;
	//_CTR_PORT = _LATCH_MASK;
}

void pulseOffPwr()
{
	digitalWrite(P_PWR, HIGH);
	digitalWrite(P_CLK, HIGH);
	digitalWrite(P_CLK, LOW);
	//_CTR_PORT = _PWR_MASK | _LATCH_MASK | _CLK_MASK;
	//_CTR_PORT = _PWR_MASK | _LATCH_MASK;
}

void pulseLatch()
{
	digitalWrite(P_LATCH, LOW);
	digitalWrite(P_LATCH, HIGH);
}

void setColors(const unsigned char col, const unsigned char row)
{
	// Or the correct CLR values in
	// Will clock with following pulse...Pwr()
	//_CLR_PORT = (led[col][row][CLR_RED] > pwm ? _RED_MASK : 0) |
	//			(led[col][row][CLR_GREEN] > pwm ? _GREEN_MASK : 0) |
	//			(led[col][row][CLR_BLUE] > pwm ? _BLUE_MASK : 0) ;
	if(led[col][row][CLR_RED] > pwm){digitalWrite(P_RED, HIGH);}
	else{digitalWrite(P_RED, LOW);}
	
	if(led[col][row][CLR_GREEN] > pwm){digitalWrite(P_GREEN, HIGH);}
	else{digitalWrite(P_GREEN, LOW);}
	
	if(led[col][row][CLR_BLUE] > pwm){digitalWrite(P_BLUE, HIGH);}
	else{digitalWrite(P_BLUE, LOW);}
}

void drawMap()
{
	static unsigned char col = 0;
	//for(unsigned char col=0; col < COLS; col++)
	{
		char c = COLS - 1;

		for(; c >= ROWS; c--)
		{
			if (c == col) {
				pulseOnPwr();
			}
			else {
				pulseOffPwr();
			}
		}

		for(; c >= 0; c--)
		{
			setColors(col, c);

			if (c == col) {
				pulseOnPwr();
			}
			else {
				pulseOffPwr();
			}
		}

		pulseLatch();
	}

	if (col++==COLS) {
	   col=0;
	  //check this, if not move back below pwm
	if (pwm++==PWM_MAX)
	{
		pwm = 0;
	}
	}
}

void updatePixel(unsigned char col, unsigned char row,
				 unsigned char clr, unsigned char val,
				 unsigned char mode)
{
	switch (mode) {
		case MODE_ADD:
			led[col][row][clr] += val;
			break;

		case MODE_SUB:
			led[col][row][clr] -= val;
			break;

		case MODE_MULT:
			led[col][row][clr] *= val;
			break;

		case MODE_DIV:
			led[col][row][clr] /= val;
			break;

		case MODE_SET:
		default:
			led[col][row][clr] = val;
			break;
	}
}

void drawChar(const unsigned char val, const unsigned char x, const unsigned char y,
			  const unsigned char red, const unsigned char green, const unsigned char blue,
			  const unsigned char mode)
{
	unsigned char digitMask[3];

	switch (val) {
		case 0:
		{
			unsigned char digitMaskT[] = { 0b11111, 0b10001, 0b11111};  //0
			memcpy(digitMask, digitMaskT, 3);
			break;
		}

		case 1:
		{
			unsigned char digitMaskT[] = { 0b00000, 0b11111, 0b00000};  //1
			memcpy(digitMask, digitMaskT, 3);
			break;
		}

		case 2:
		{
			unsigned char digitMaskT[] = { 0b11101, 0b10101, 0b10111};  //2
			memcpy(digitMask, digitMaskT, 3);
			break;
		}

		case 3:
		{
			unsigned char digitMaskT[] = { 0b10101, 0b10101, 0b11111};   //3
			memcpy(digitMask, digitMaskT, 3);
			break;
		}

		case 4:
		{
			unsigned char digitMaskT[] = { 0b00111, 0b00100, 0b11111};   //4
			memcpy(digitMask, digitMaskT, 3);
			break;
		}

		case 5:
		{
			unsigned char digitMaskT[] = { 0b10111, 0b10101, 0b11101};     //5
			memcpy(digitMask, digitMaskT, 3);
			break;
		}

		case 6:
		{
			unsigned char digitMaskT[] = { 0b11111, 0b10101, 0b11101};   //6
			memcpy(digitMask, digitMaskT, 3);
			break;
		}

		case 7:
		{
			unsigned char digitMaskT[] = { 0b00001, 0b00001, 0b11111};   //7
			memcpy(digitMask, digitMaskT, 3);
			break;
		}

		case 8:
		{
			unsigned char digitMaskT[] = { 0b11111, 0b10101, 0b11111}; //8
			memcpy(digitMask, digitMaskT, 3);
			break;
		}

		case 9:
		{
			unsigned char digitMaskT[] = { 0b10111, 0b10101, 0b11111}; //9
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'A':
		{
			unsigned char digitMaskT[] = { 0b11110, 0b00101, 0b11110};  //A
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'B':
		{
			unsigned char digitMaskT[] = { 0b11111, 0b10101, 0b01010};  //B
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'C':
		{
			unsigned char digitMaskT[] = { 0b11111, 0b10001, 0b10001};  //C
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'D':
		{
			unsigned char digitMaskT[] = { 0b11111, 0b10001, 0b01110};  //D
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'E':
		{
			unsigned char digitMaskT[] = { 0b11111, 0b10101, 0b10101};  //E
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'F':
		{
			unsigned char digitMaskT[] = { 0b11111, 0b00101, 0b00001};  //F
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'G':
		{
			unsigned char digitMaskT[] = { 0b01110, 0b10001, 0b11101};  //G
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'H':
		{
			unsigned char digitMaskT[] = { 0b11111, 0b00100, 0b11111};  //H
			memcpy(digitMask, digitMaskT, 3);
			break;
		}

		case 'I':
		{
			unsigned char digitMaskT[] = { 0b10001, 0b11111, 0b10001};  //I
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'J':
		{
			unsigned char digitMaskT[] = { 0b01000, 0b10000, 0b01111};  //J
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'K':
		{
			unsigned char digitMaskT[] = { 0b11111, 0b00100, 0b11011};  //K
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'L':
		{
			unsigned char digitMaskT[] = { 0b11111, 0b10000, 0b10000};  //L
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'M':
		{
			unsigned char digitMaskT[] = { 0b11111, 0b00010, 0b11111};  //M
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'N':
		{
			unsigned char digitMaskT[] = { 0b11111, 0b01110, 0b11111};  //N
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'O':
		{
			unsigned char digitMaskT[] = { 0b01110, 0b10001, 0b01110};  //O
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'P':
		{
			unsigned char digitMaskT[] = { 0b11111, 0b00101, 0b00010};  //P
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'Q':
		{
			unsigned char digitMaskT[] = { 0b01110, 0b10001, 0b11110};  //Q
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'R':
		{
			unsigned char digitMaskT[] = { 0b11111, 0b01101, 0b10010};  //R
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'S':
		{
			unsigned char digitMaskT[] = { 0b10010, 0b10101, 0b01001};  //S
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'T':
		{
			unsigned char digitMaskT[] = { 0b00001, 0b11111, 0b00001};  //T
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'U':
		{
			unsigned char digitMaskT[] = { 0b11111, 0b10000, 0b11111};  //U
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'V':
		{
			unsigned char digitMaskT[] = { 0b00111, 0b11000, 0b00111};  //V
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'W':
		{
			unsigned char digitMaskT[] = { 0b11111, 0b01000, 0b11111};  //W
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'X':
		{
			unsigned char digitMaskT[] = { 0b11011, 0b00100, 0b11011};  //X
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'Y':
		{
			unsigned char digitMaskT[] = { 0b00011, 0b11100, 0b00011};  //Y
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case 'Z':
		{
			unsigned char digitMaskT[] = { 0b11001, 0b10101, 0b10011};  //Z
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
		
		case '.':
		{
			unsigned char digitMaskT[] = { 0b10000, 0b00000, 0b00000};  //.
			memcpy(digitMask, digitMaskT, 3);
			break;
		}
	}


	//for(char col = max(0, -x); col < 3 && col + x < COLS; col++)
	for(unsigned char col = 0; col < 3 && col + x < COLS; col++)
	{
		//for(char row = max(0, -y); row < 5 && row + y < ROWS; row++)
		for(unsigned char row = 0; row < 5 && row + y < ROWS; row++)
		{
			if (digitMask[col] & 1 << row) {
				updatePixel(col + x, row + y, CLR_RED, red, mode);
				updatePixel(col + x, row + y, CLR_GREEN, green, mode);
				updatePixel(col + x, row + y, CLR_BLUE, blue, mode);
			}
		}
	}
}

void drawRect(const unsigned char x, const unsigned char y, const unsigned char dx, const unsigned char dy,
			  const unsigned char red, const unsigned char green, const unsigned char blue,
			  const unsigned char mode)
{
		for(unsigned char col = max(x, 0) ; col < x + dx && col < COLS; col++)
		{
			for(unsigned char row = max(y, 0) ; row < y + dy && row < ROWS; row++)
			{
				updatePixel(col, row, CLR_RED, red, mode);
				updatePixel(col, row, CLR_GREEN, green, mode);
				updatePixel(col, row, CLR_BLUE, blue, mode);
			}
		}
}

char getTemp(int wChar)
{
  FILE *fp;
  int status;
  char path[20];

  /* Open the command for reading. */
  fp = popen("/opt/vc/bin/vcgencmd measure_temp", "r");
  if (fp == NULL) {
    printf("Failed to run command\n" );
    exit;
  }

  /* Read the output a line at a time - output it. */
  //while (fgets(path, sizeof(path)-1, fp) != NULL) {
  //  printf("%s", path);
  //}

  fscanf(fp,"%s",path);
  //printf("%s",path);
  /* close */
  pclose(fp);
  return path[wChar];
}


// Just Including Arduino Loop For Use Later
// void loop()
// {
    //drawMap();
    // handleSerial();
    // if (modeSet == 1)
    // {
      // for(unsigned char c = 0; c < COLS; c++)
      // {
        // for(unsigned char r = 0; r < ROWS; r++)
        // {
          // if (BandArray[c]>r) {led[c][ROWS-r-1][CLR_BLUE] = PWM_MAX;}
          // else {led[c][ROWS-r-1][CLR_BLUE] = 0;}
          // led[c][r][CLR_RED] = 0;
          // led[c][r][CLR_GREEN] = 0;
        // }
      // }
    // }
    // else if (modeSet == 0)
    // {
      // horizGrad(CLR_BLUE,0);
      // horizGrad(CLR_RED,1);
      // vertGrad(CLR_GREEN,1);
    // }
    // else if (modeSet == 2)
    // {
      // for(unsigned char c = 0; c < COLS; c++)
      // {
        // for(unsigned char r = 0; r < ROWS; r++)
        // {
          // led[c][r][CLR_RED] = Mid;
          // led[c][r][CLR_GREEN] = Treble;
          // led[c][r][CLR_BLUE] = Bass;
        // }
      // }
    // }
    // else if (modeSet == 3)
    // {
      // for(unsigned char c = 0; c < COLS; c++)
      // {
        // for(unsigned char r = 0; r < ROWS; r++)
        // {
          // led[c][r][CLR_RED] = Mid;
          // led[c][r][CLR_GREEN] = Treble;
          // led[c][r][CLR_BLUE] = Bass;
        // }
      // }
    // }
    // else if (modeSet == 4)
    // {
      // if((prevModeSet != 4) || (prevM2 != minute2))
      // {
      // fullOff();
      
      // if(hour1>0)
      // {
        // drawChar(hour1,2,2,PWM_MAX,PWM_MAX,PWM_MAX,MODE_SET);
      // }
      // drawChar(hour2,4,2,PWM_MAX,PWM_MAX,PWM_MAX,MODE_SET);
      // drawChar(minute1,8,2,PWM_MAX,PWM_MAX,PWM_MAX,MODE_SET);
      // drawChar(minute2,12,2,PWM_MAX,PWM_MAX,PWM_MAX,MODE_SET);
      // prevM2 = minute2;
      // }
    // }
    // else if (modeSet == 5)
    // {
      // if (prevModeSet != 5)
      // {
      // bcol = PWM_MAX;
      // rcol = 0;
      // gcol = 0;
      // }
      //if (bcol < PWM_MAX && gcol == 0 && rcol == 0){bcol++;}
      // if (bcol == PWM_MAX && gcol == 0 && rcol < PWM_MAX){rcol++;}
      // else if (bcol > 0 && gcol == 0 && rcol == PWM_MAX){bcol--;}
      // else if (bcol == 0 && gcol < PWM_MAX && rcol == PWM_MAX){gcol++;}
      // else if (bcol == 0 && gcol == PWM_MAX && rcol > 0){rcol--;}
      // else if (bcol < PWM_MAX && gcol == PWM_MAX && rcol == 0){bcol++;}
      // else if (bcol == PWM_MAX && gcol > 0 && rcol == 0){gcol--;}
      // for(unsigned char c = 0; c < COLS; c++)
      // {
        // for(unsigned char r = 0; r < ROWS; r++)
        // {
          // led[c][r][CLR_BLUE] = bcol;
          // led[c][r][CLR_RED] = rcol;
          // led[c][r][CLR_GREEN] = gcol;
        // }
      // }
      // delayMicroseconds(10000);
    // }
    // else if (modeSet == 6)
    // {
      // if (prevModeSet != 6)
      // {
      // for(unsigned char c = 0; c < COLS; c++)
      // {
        // for(unsigned char r = 0; r < ROWS; r++)
        // {
          // led[c][r][CLR_BLUE] = random(PWM_MAX);
          // led[c][r][CLR_RED] = random(PWM_MAX);
          // led[c][r][CLR_GREEN] = random(PWM_MAX);
        // }
      // }
      // }
      // unsigned int ranCol = random(COLS);
      // unsigned int ranRow = random(ROWS);
      // led[ranCol][ranRow][CLR_BLUE] = random(PWM_MAX);
      // led[ranCol][ranRow][CLR_RED] = random(PWM_MAX);
      // led[ranCol][ranRow][CLR_GREEN] = random(PWM_MAX);
      // delay(.5);
    // }
    // else if (modeSet == 7)
    // {
      // fullOff();
      // unsigned int ranCol = random(COLS);
      // unsigned int ranRow = random(ROWS);
      // led[ranCol][ranRow][CLR_BLUE] = PWM_MAX;
      // led[ranCol][ranRow][CLR_RED] = PWM_MAX;
      // led[ranCol][ranRow][CLR_GREEN] = PWM_MAX;
      // delay(.05);
    // }
    // else if (modeSet == 8)
    // {
      // fullOff();
    // }
    // else if (modeSet == 9)
    // {
      // fullOff();
    // }
    // else if (modeSet == 10)
    // {
      // fullOff();
    // }
    // else if (modeSet == 11)
    // {
      // fullOff();
    // }
    // prevModeSet = modeSet;
// }
