/*
Conversion of the code given here:
https://github.com/e-Gizmo/4-Digit-Display-v2
to be used with our project.
*/

#ifndef EGIZMO4DIGIT_H
#define EGIZMO4DIGIT_H

#include "Arduino.h"

class eGizmo4Digit
{
	public:
		eGizmo4Digit(int STB_pin, int CLK_pin, int DIN_pin);
		void SEND_DATA(int whichDigit, int numberSent, bool decimalPoint);
		void DISP_OFF();
		void DISP_ON();
		void begin();
		void OVERFLOW();
		void UNDERFLOW();
		void ERROR();
	private:
		void SET_DISP_MODE();
		void SEND_RAW_DATA(int whichDigit, byte dataToWrite);
		void SET_DATA_MODE();
		void SET_DISP_CTRL(byte inten);
		int STB, CLK, DIN;
		byte digitAddress, dataSent;
		// The binary representation of which parts of the seven-segment display to turn on
		// with the representation being A-G, DP, but with A as the least significant bit
		// (i.e., "1" with no decimal point is 00000110 (hence 0x06)
		char Num[2][10] =
		{
			{0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67},
			{0xBF, 0x86, 0xDB, 0xCF, 0xE6, 0xED, 0xFD, 0x87, 0xFF, 0xE7}
		};
		char err[4] = {0x00, 0x79, 0x50, 0x50};
		char overflow[4] = {0x00, 0x00, 0x3F, 0x38};
		char underflow[4] = {0x00, 0x00, 0x38, 0x5C};
		byte digit0, digit1, digit2, digit3;
		void WRITE_BYTE(byte db_data);
		void WRITE_BIT(boolean db_bit);
};

#endif
