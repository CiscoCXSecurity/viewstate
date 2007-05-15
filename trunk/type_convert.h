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

#ifndef TYPE_CONVERT_H
#define TYPE_CONVERT_H

#include <iostream>

using namespace std;
 
// Public Routines
int BinaryToInt(string);			// Converts a Binary String into an Integer
string IntToBinary(int);			// Converts a Integer into a Binary String
string URLEncode(string);			// Coverts a String into a URL friendly string

// Private Routines
// bool CheckURLChar(char);			// Tests to see if a character is URL friendly

#endif
