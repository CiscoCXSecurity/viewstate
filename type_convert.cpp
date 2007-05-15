/***************************************************************************
 *   Copyright (C) 2004-2007 by Ian Ventura-Whiting (Fizz)                 *
 *   fizz@titania.co.uk                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <iostream>

using namespace std;


// List of characters that are dangerous in URL's
static string excludeFromUrl = "=+/";


// Check a character to see if it needs encoding
// returns false if the character needs encoding
bool CheckURLChar(char inputChar)
{
	int notInUrlLength = excludeFromUrl.length();
	bool checkResult = true;
	
	for (int loop = 0; loop < notInUrlLength; loop++)
	{
		if (inputChar == excludeFromUrl[loop])
			checkResult = false;
	}
	
	return checkResult;
}


// Checks a whole string and replaces characters that
// need changing to make them URL friendly
string URLEncode(string inputString)
{
	string outputString = "";
	char tempChar;
	char hexChar[3];
	int stringLength = inputString.length();
	
	for (int loop = 0; loop < stringLength; loop++)
	{
		if (CheckURLChar(inputString[loop]) == false)
		{
			// Add %<hex> value of char to the output string
			outputString.append("%");
			tempChar = inputString[loop];
			snprintf(hexChar, 3, "%x", tempChar);
			outputString.append(hexChar);
		}
		else
		{
			// Add character to the string
			outputString.append(inputString.substr(loop,1));
		}
	}
	
	return outputString;
}


// Convert a binary string into a number
int BinaryToInt(string binaryString)
{
	int returnValue;
	if (binaryString[7] == '1')
		returnValue = 1;
	else
		returnValue = 0;
	if (binaryString[6] == '1')
		returnValue += 2;
	if (binaryString[5] == '1')
		returnValue += 4;
	if (binaryString[4] == '1')
		returnValue += 8;
	if (binaryString[3] == '1')
		returnValue += 16;
	if (binaryString[2] == '1')
		returnValue += 32;
	if (binaryString[1] == '1')
		returnValue += 64;
	if (binaryString[0] == '1')
		returnValue += 128; 
	return returnValue;
}


// Converts an Integer into its binary number
string IntToBinary(int inputByte)
{
	string returnString;
	int charMask = 1<<7;
	for (int int_Loop = 0; int_Loop < 8; int_Loop++)
	{
		if ((inputByte & charMask))
			returnString += "1";
		else
			returnString += "0";
		inputByte <<= 1;
	}
	return returnString;
}
