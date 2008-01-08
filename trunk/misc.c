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


int getViewstateFromHTML(char *inputString, int maxLength)
{
	// Variables...
	int inPos = 0;
	int outPos = 0;
	int found = false;
	int read = false;

	while (inPos < maxLength)
	{
		if (strncmp("name=\"__VIEWSTATE\"", inputString + inPos, 18) == 0)
		{
			inPos += 17;
			found = true;
		}

		else if ((found == true) && (strncmp("value=\"", inputString + inPos, 7) == 0))
		{
			inPos += 6;
			read = true;
			found = false;
		}

		else if ((read == true) && (inputString[inPos] == 34))
			read = false;

		else if ((read == true) && (inputString[inPos] != 34))
		{
			inputString[outPos] = inputString[inPos];
			outPos++;
		}

		inPos++;
	}

	if (outPos > 0)
	{
		inputString[outPos] = 0;
		return true;
	}
	else
		return false;
}


