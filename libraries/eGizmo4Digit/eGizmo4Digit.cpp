/*
    4 Digit Display v 2.0

 This sample program functions
 as a 4 digit 7 segment display
 counter.

 Codes by:
 e-Gizmo Mechatronix Central
 http://www.e-gizmo.com

*/

#include "Arduino.h"
#include "eGizmo4Digit.h"

eGizmo4Digit::eGizmo4Digit(int STB_pin, int CLK_pin, int DIN_pin)
{
	// Data, Strobe, and clock LD1207
	STB = STB_pin;
	CLK = CLK_pin;
	DIN = DIN_pin;
//	Num[10] = {0xBF, 0x06, 0xDB, 0x4F, 0xE6, 0x6D, 0xFD, 0x07, 0xFF, 0x67};
}

void eGizmo4Digit::OVERFLOW()
{
	for (int i = 0; i < 4; i++)
	{
		SEND_RAW_DATA(i, overflow[i]);
	}
}

void eGizmo4Digit::UNDERFLOW()
{
        for (int i = 0; i < 4; i++)
        {
                SEND_RAW_DATA(i, underflow[i]);
        }
}


void eGizmo4Digit::ERROR()
{
        for (int i = 0; i < 4; i++)
        {
                SEND_RAW_DATA(i, err[i]);
        }
}


void eGizmo4Digit::begin() {
	pinMode(STB, OUTPUT);
	pinMode(CLK, OUTPUT);
	pinMode(DIN, OUTPUT);

	digitalWrite(STB, HIGH);
	digitalWrite(CLK, HIGH);
	digitalWrite(DIN, HIGH);

	SET_DISP_MODE();
	SET_DATA_MODE();
	DISP_OFF();
	DISP_ON();
	SET_DISP_CTRL(4 | 0x08);
}

void eGizmo4Digit::SET_DISP_MODE() {
	digitalWrite(STB, LOW);
	//  WRITE_COMMAND(0x00);    //4 Grids, 12 Segments
	//  WRITE_COMMAND(0x01);    //5 Grids, 11 Segments
	//  WRITE_COMMAND(0x02);    //6 Grids, 10 Segments
	WRITE_BYTE(0x03);    //7 Grids, 9 Segments    (default)
	digitalWrite(STB, HIGH);
}

void eGizmo4Digit::SET_DATA_MODE(void) {
	digitalWrite(STB, LOW);
	WRITE_BYTE(0x40);    //Increment Address after Data has been Written    (default)
	//  WRITE_COMMAND(0x44);    //Fixes Address
	digitalWrite(STB, HIGH);
}

void eGizmo4Digit::SEND_DATA(int whichDigit, int numberSent, bool decimalPoint){
	digitalWrite(STB, LOW);
	WRITE_BYTE(0xC0 | whichDigit * 2); // Converts 0, 1, 2, 3 into the needed 0x00, 0x02, 0x04, 0x06
	WRITE_BYTE(Num[decimalPoint][numberSent]);
	digitalWrite(STB, HIGH);
}

void eGizmo4Digit::SEND_RAW_DATA(int whichDigit, byte dataToWrite){
        digitalWrite(STB, LOW);
        WRITE_BYTE(0xC0 | whichDigit * 2); // Converts 0, 1, 2, 3 into the needed 0x00, 0x02, 0x04, 0x06
        WRITE_BYTE(dataToWrite);
        digitalWrite(STB, HIGH);
}


void eGizmo4Digit::SET_DISP_CTRL(byte inten) {
	digitalWrite(STB, LOW);
	WRITE_BYTE(0x80 | inten);    //inten = 0 to 7
	digitalWrite(STB, HIGH);
}

void eGizmo4Digit::DISP_ON() {
	digitalWrite(STB, LOW);
	WRITE_BYTE(0x80 | 0x08);
	digitalWrite(STB, HIGH);
}

void eGizmo4Digit::DISP_OFF(void) {
	digitalWrite(STB, LOW);
	WRITE_BYTE(0x80 & ~0x08);
	digitalWrite(STB, HIGH);
}

void eGizmo4Digit::WRITE_BYTE(byte db_data)
{
      digitalWrite(DIN, db_data & 1);
      digitalWrite(CLK, LOW);
      digitalWrite(CLK, HIGH);
      digitalWrite(DIN, db_data & 2);
      digitalWrite(CLK, LOW);
      digitalWrite(CLK, HIGH);
      digitalWrite(DIN, db_data & 4);
      digitalWrite(CLK, LOW);
      digitalWrite(CLK, HIGH);
      digitalWrite(DIN, db_data & 8);
      digitalWrite(CLK, LOW);
      digitalWrite(CLK, HIGH);
      digitalWrite(DIN, db_data & 16);
      digitalWrite(CLK, LOW);
      digitalWrite(CLK, HIGH);
      digitalWrite(DIN, db_data & 32);
      digitalWrite(CLK, LOW);
      digitalWrite(CLK, HIGH);
      digitalWrite(DIN, db_data & 64);
      digitalWrite(CLK, LOW);
      digitalWrite(CLK, HIGH);
      digitalWrite(DIN, db_data & 128);
      digitalWrite(CLK, LOW);
      digitalWrite(CLK, HIGH);
}

void eGizmo4Digit::WRITE_BIT(boolean db_bit)
{
      digitalWrite(DIN, db_bit);
      digitalWrite(CLK, LOW);
      digitalWrite(CLK, HIGH);
      digitalWrite(CLK, LOW);
      digitalWrite(CLK, HIGH);
      digitalWrite(CLK, LOW);
      digitalWrite(CLK, HIGH);
      digitalWrite(CLK, LOW);
      digitalWrite(CLK, HIGH);
      digitalWrite(CLK, LOW);
      digitalWrite(CLK, HIGH);
      digitalWrite(CLK, LOW);
      digitalWrite(CLK, HIGH);
      digitalWrite(CLK, LOW);
      digitalWrite(CLK, HIGH);
      digitalWrite(CLK, LOW);
      digitalWrite(CLK, HIGH);
}

