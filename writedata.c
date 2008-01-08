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


// Print data
void printData(FILE *outputFile, unsigned char *buffer, long dataSize, int hexOutput)
{
	// Variables...
	int charPointer;
	int containsNonPrintable;
	long counter;
	char asciiOutput[17];

	// init...
	containsNonPrintable = false;

	// If HEX output is disabled...
	if (hexOutput == false)
		fprintf(outputFile, "%s", buffer);
	else
	{

		// Find any non-printable characters...
		charPointer = 0;
		while (charPointer < dataSize)
		{
			if ((buffer[charPointer] < 32) && (buffer[charPointer] > 13))
				containsNonPrintable = true;
			else if (buffer[charPointer] < 9)
				containsNonPrintable = true;
			else if ((buffer[charPointer] > 126) && (buffer[charPointer] < 160))
				containsNonPrintable = true;
			charPointer++;
		}

		// If all the characters are printable...
		if (containsNonPrintable == false)
			fprintf(outputFile, "%s", buffer);

		// If they are not all printabltrlene...
		else
		{

			// Init Variables...
			memset(asciiOutput, 0, 17);
			counter = 0;
			charPointer = 0;

			// Print to screen...
			while (charPointer < dataSize)
			{
				fprintf(outputFile, "%02x ", buffer[charPointer]);
				if (buffer[charPointer] < 32)
					asciiOutput[counter] = '.';
				else if ((buffer[charPointer] > 126) && (buffer[charPointer] < 160))
					asciiOutput[counter] = '.';
				else
					asciiOutput[counter] = buffer[charPointer];

				// Increment or loop...
				if (counter < 15)
					counter++;
				else
				{
					fprintf(outputFile, "   %s\n", asciiOutput);
					memset(asciiOutput, 0, 17);
					counter = 0;
				}
				charPointer++;
			}

			// If more to go...
			if (counter != 0)
			{
				while (counter < 17)
				{
					fprintf(outputFile, "   ");
					counter++;
				}
				fprintf(outputFile, "%s\n", asciiOutput);
			}
		}
	}
}

