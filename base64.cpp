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

// See RFC3548 for details of Base64 Encode / Decode standards

#include <iostream>
#include <string>
#include "type_convert.h"
 
using namespace std;


// Base64 character string
static string base64String = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


// Base64 encode a character
char EncodeBase64Char(unsigned char inputChar)
{
	return base64String[inputChar];
}


// Base64 decode a character
unsigned char DecodeBase64Char(char inputChar)
{
	for (int loop = 0; loop < 64; loop++)
	{
		if (base64String[loop] == inputChar)
			return loop;
	}
	return 0;
}


// Base64 character check
bool CheckBase64Char(char inputChar)
{
	for (int loop = 0; loop < 64; loop++)
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
string StringStrip(string inputString)
{
	// Stripped Characters...
	// 32 - Space
	// 10 - Line Feed
	// 13 - Carridge Return
	if (inputString.length() != 0)
	{
		string returnString = inputString;
		char endChar;
		
		// Loop until characters have gone...
		endChar = returnString[returnString.length() - 1];
		while (endChar == 32 || endChar == 10 || endChar == 13)
		{
			returnString = returnString.substr(0, returnString.length() - 1);
			endChar = returnString[returnString.length() - 1];
		}
		return returnString;
	}
	else
		return inputString;
}


// Base64 Check string
bool Base64Check(string inputString)
{
	if (inputString.length() != 0)
	{
		int stringLength = inputString.length();
		for (int loop = 0; loop < stringLength; loop++)
		{
			if (CheckBase64Char(inputString[loop]) == false)
				return false;
		}
		return true;
	}
	else
		return false;
}


// Base64 Encode string
string Base64Encode(string inputString)
{
	// Check to make sure that the string is not empty
	if (inputString.size() == 0)
		return inputString;
		
	// When converting to base64, every 3 characters converts to 4.
	// This is due to the base64 coversion from 8bit to 6bit values.
	string returnValue;
	for (int loop = 0; loop < inputString.size(); loop+=3)
	{
		// declare & set the input strings
		string inputString1 = IntToBinary(inputString[loop]);
		string inputString2 = "00000000";
		if (loop+1 < inputString.size())
			inputString2 = IntToBinary(inputString[loop+1]);
		string inputString3 = "00000000";
		if (loop+2 < inputString.size())
			inputString3 = IntToBinary(inputString[loop+2]);
		
		// Convert 3 8bit characters to 4 6bit characters
		string interString1 = inputString1.substr(0,6);	// 1 is first six bits of first
		string interString2 = inputString1.substr(6);	// 2 is last 2 bits of first
		interString2.append(inputString2.substr(0,4));	//		+ first 4 bits of second
		string interString3 = inputString2.substr(4);	// 3 is last 4 bits of second
		interString3.append(inputString3.substr(0,2));	//		+ first 2 bits of third
		string interString4 = inputString3.substr(2);	// 4 is last 6 bits of third
		
		// Add padding bits
		interString1 = "00" + interString1;
		interString2 = "00" + interString2;
		interString3 = "00" + interString3;
		interString4 = "00" + interString4;
		
		// Encode and output
		returnValue += EncodeBase64Char(BinaryToInt(interString1));
		returnValue += EncodeBase64Char(BinaryToInt(interString2));
		if (loop+1 < inputString.size())
			returnValue += EncodeBase64Char(BinaryToInt(interString3));
		else
			returnValue += "=";
		if (loop+2 < inputString.size())
			returnValue += EncodeBase64Char(BinaryToInt(interString4));
		else
			returnValue += "=";
	} 
	return returnValue;
}


// Base64 Decode string
string Base64Decode(string inputString)
{
	// Check to see if the string is Base64 and not 0 in length
	if ((Base64Check(inputString) == false) || (inputString.length() == 0))
		return "";
	else
	{
		// Convert from base64 converts every 4 characters to 3. This is due to
		// base64 conversion from 6bit to 8bit characters.
		string returnValue;
		
		for (int loop = 0; loop < inputString.length(); loop += 4)
		{
			// Declare and set the input characters
			char inChar1='A';
			char inChar2='A';
			char inChar3='A';
			char inChar4='A';
			inChar1 = inputString[loop];
			if (loop+1 < inputString.length())
				inChar2 = inputString[loop+1];
			if (loop+2 < inputString.length())
				inChar3 = inputString[loop+2];
			if (loop+3 < inputString.length())
				inChar4 = inputString[loop+3];
			
			// Declare and set the decoded output characters
			string interString1 = IntToBinary(DecodeBase64Char(inChar1));
			string interString2 = IntToBinary(DecodeBase64Char(inChar2));
			string interString3 = IntToBinary(DecodeBase64Char(inChar3));
			string interString4 = IntToBinary(DecodeBase64Char(inChar4));
			
			// Trim off the wasted higher bits
			interString1 = interString1.substr(2);
			interString2 = interString2.substr(2);
			interString3 = interString3.substr(2);
			interString4 = interString4.substr(2);
			
			// Change the output chars from 6bit to 8bit and return them
			string outString1 = interString1;					// out1 = all of first
			outString1.append(interString2.substr(0,2));		// 			+ 2 of the second
			string outString2 = interString2.substr(2);		// out2 = last 4 of second
			outString2.append(interString3.substr(0,4));		//			+ first 4 of third
			string outString3 = interString3.substr(4);		// out3 = last 2 of third
			outString3.append(interString4);					//			+ all of forth
			
			// Convert back from binary and add to the decoded string
			char tempChar = BinaryToInt(outString1);
			returnValue += tempChar;
			tempChar = BinaryToInt(outString2);
			returnValue += tempChar;
			tempChar = BinaryToInt(outString3);
			returnValue += tempChar;
		}
		return returnValue;
	}
}
