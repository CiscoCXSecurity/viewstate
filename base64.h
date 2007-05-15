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

#ifndef BASE64_H
#define BASE64_H

#include <iostream>

using namespace std;

// Private internal routines...
// char EncodeBase64Char(unsigned char);		// Encode a single character
// unsigned char DecodeBase64Char(char);		// Decode a single character
// bool CheckBase64Char(char);					// Check a single character is Base64

// Public routines...
string Base64Encode(string);					// Base64 Encode
string Base64Decode(string);					// Base64 Decode
bool Base64Check(string);						// Base64 Check
string StringStrip(string);						// Strips End-of-file characters from string

#endif
