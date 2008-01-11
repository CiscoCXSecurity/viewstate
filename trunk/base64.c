/***************************************************************************
 *   viewstate - a .Net viewstate decoder / encoder                        *
 *   Copyright (C) 2004 - 2008 by Ian Ventura-Whiting (Fizz)               *
 *   fizz@titania.co.uk                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

// See RFC3548 for details of Base64 Encode / Decode standards


// Base64 character string
const char *base64String = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const int bitCalc[8] = {1, 2, 4, 8, 16, 32, 64, 128};


// Base64 character check
int checkBase64Char(char inputChar)
{
	// Variables...
	int loop = 0;

	for (loop = 0; loop < 64; loop++)
	{
		if (base64String[loop] == inputChar)
			return true;
	}
	if (inputChar == '=')
		return true;
	else
		return false;
}


// Base64 character strip (gets rid of odd characters at the end)
void base64StringStrip(char *inputString)
{
	// Variables...
	char endChar;

	// Stripped Characters...
	// 32 - Space
	// 10 - Line Feed
	// 13 - Carridge Return
	if (strlen(inputString) != 0)
	{
		// Loop until characters have gone...
		endChar = inputString[strlen(inputString) - 1];
		while (checkBase64Char(endChar) == false)
		{
			inputString[strlen(inputString) - 1] = 0;
			endChar = inputString[strlen(inputString) - 1];
		}
	}
}


// Base64 Check string
int base64Check(char *inputString)
{
	// Variables...
	int stringLength = 0;
	int loop = 0;

	stringLength = strlen(inputString);
	if (stringLength != 0)
	{
		for (loop = 0; loop < stringLength; loop++)
		{
			if (checkBase64Char(inputString[loop]) == false)
				return false;
		}
		return true;
	}
	else
		return false;
}

// Base 64 encode
// (but first you need to reserve a string big enough)
int base64Encode(char *inputString, long inputSize, char *outputString)
{
	// Variables...
	long loop = 0;
	long outLoop = 0;
	char in1 = 0;
	char in2 = 0;
	char in3 = 0;
	char inter1 = 0;
	char inter2 = 0;
	char inter3 = 0;
	char inter4 = 0;

	// Check string length
	if (inputSize == 0)
		return false;

	// Convert String to Base 64...
	for (loop = 0; loop < inputSize; loop += 3)
	{
		// In Chars...
		in1 = inputString[loop];
		if (loop + 1 < inputSize)
			in2 = inputString[loop + 1];
		else
			in2 = 0;
		if (loop + 2 < inputSize)
			in3 = inputString[loop + 2];
		else
			in3 = 0;

		// 1st char...
		inter1 = in1;
		inter1 = inter1 >> 2;
		inter1 = inter1 & 63;
		outputString[outLoop] = base64String[inter1];

		// 2nd char...
		inter2 = in1;
		inter2 = inter2 << 4;
		inter2 = inter2 & 48;
		inter3 = in2;
		inter3 = inter3 >> 4;
		inter3 = inter3 & 15;
		inter2 = inter2 | inter3;
		outputString[outLoop + 1] = base64String[inter2];

		// 3rd Char
		if (loop + 1 < inputSize)
		{
			inter3 = in2;
			inter3 = inter3 << 2;
			inter3 = inter3 & 60;
			inter4 = in3;
			inter4 = inter4 >> 6;
			inter4 = inter4 & 3;
			inter3 = inter3 | inter4;
			outputString[outLoop + 2] = base64String[inter3];
		}
		else
			outputString[outLoop + 2] = 61;

		// 4th..
		if (loop + 2 < inputSize)
		{
			inter4 = in3;
			inter4 = inter4 & 63;
			outputString[outLoop + 3] = base64String[inter4];
		}
		else
			outputString[outLoop + 3] = 61;
		
		outLoop += 4;
	}

	return true;
}


long base64DecodeChars(char *inputString)
{
	// Variables...
	int loop = 0;
	long stringLoop = 0;
	int outString = 0;
	unsigned char in1, in2, in3, in4 = 0;
	unsigned char inter1, inter2, inter3, inter4 = 0;
	unsigned char out1, out2, out3 = 0;

	base64StringStrip(inputString);

	// Check to see if the string is Base64 and not 0 in length
	if ((base64Check(inputString) == false) || (strlen(inputString) == 0))
		return 0;
	else
	{
		// Convert from base64 converts every 4 characters to 3. This is due to
		// base64 conversion from 6bit to 8bit characters.
		for (stringLoop = 0; stringLoop < strlen(inputString); stringLoop += 4)
		{

			// Init in characters...
			in1 = inputString[stringLoop];
			if (stringLoop + 1 < strlen(inputString))
				in2 = inputString[stringLoop + 1];
			else
				in2 = 'A';
			if (stringLoop + 2 < strlen(inputString))
				in3 = inputString[stringLoop + 2];
			else
				in3 = 'A';
			if (stringLoop + 3 < strlen(inputString))
				in4 = inputString[stringLoop + 3];
			else
				in4 = 'A';

			// Decode characters
			for (loop = 0; loop < 64; loop++)
				if (base64String[loop] == in1) inter1 = loop;
			for (loop = 0; loop < 64; loop++)
				if (base64String[loop] == in2) inter2 = loop;
			for (loop = 0; loop < 64; loop++)
				if (base64String[loop] == in3) inter3 = loop;
			for (loop = 0; loop < 64; loop++)
				if (base64String[loop] == in4) inter4 = loop;

			// Convert 4 6 bit characters into 3 8 bit ones...
			// First char...
			inter1 = inter1 << 2;
			inter1 = inter1 & 252;
			out1 = inter2 >> 4;
			out1 = out1 & 15;
			out1 = out1 | inter1;
			// Second char...
			inter2 = inter2 << 4;
			inter2 = inter2 & 240;
			out2 = inter3;
			out2 = out2 >> 2;
			out2 = out2 & 15;
			out2 = out2 | inter2;
			// Third char...
			inter3 = inter3 << 6;
			inter3 = inter3 & 192;
			out3 = inter3 | inter4;

			// Replace inputString...
			inputString[outString] = out1;
			outString++;
			inputString[outString] = out2;
			outString++;
			inputString[outString] = out3;
			outString++;
		}
	}

	// String end...
	inputString[outString] = 0;

	return outString;
}


