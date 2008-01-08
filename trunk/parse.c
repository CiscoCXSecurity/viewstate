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

// Check what the data is
// 0 - Data
// 1 - Triplet
// 2 - Pair
// 3 - Array List
// 4 - Item
int typeCheck(char char_1, char char_2)
{
	if (char_2 == '<')
	{
		switch (char_1)
		{
			case 't':
				return 1;
				break;
			case 'p':
				return 2;
				break;
			case 'l':
				return 3;
				break;
			case '@':
				return 3;
				break;
			case 'i':
				return 4;
				break;
			case '1':
				return 4;
				break;
			case 'o':
				return 4;
				break;
			default:
				return 0;
		}
	}
	else
		return 0;
}


// Output indent
void outputIndent(int indent, FILE *outputFile)
{
	// Variables...
	int loop = 0;

	fprintf(outputFile, "\n");
	for (loop = 0; loop < indent; loop++)
		fprintf(outputFile, " ");
}


// Parse the old viewstate type
int parseOldViewstate(FILE *outputFile, char *inputString, int offset, int indent, int verbose, int parse)
{
	// Variables...
	int count = 0;

	if (strlen(inputString) - offset > 2)
	{
		indent += 3;

		switch (typeCheck(inputString[offset], inputString[offset + 1]))
		{
			// Triplet
			case 1:
				offset += 2;

				// First Triplet...
				if ((verbose > 0) && (parse == true))
				{
					fprintf(outputFile, "%s[Triplet]%s", COL_CYAN, RESET);
					outputIndent(indent, outputFile);
					fprintf(outputFile, "%s(1)%s ", COL_GREEN, RESET);
				}
				if (inputString[offset] == ';')
				{
					if (parse == true)
						fprintf(outputFile, "NULL");
				}
				else if (typeCheck(inputString[offset], inputString[offset + 1]) == 0)
				{
					while (inputString[offset] != ';')
					{
						if (parse == true)
							fprintf(outputFile, "%c", inputString[offset]);
						offset++;
					}
				}
				else
					offset = parseOldViewstate(outputFile, inputString, offset, indent, verbose, parse);
				offset++;

				// Second Triplet...
				if ((verbose > 0) && (parse == true))
				{
					outputIndent(indent, outputFile);
					fprintf(outputFile, "%s(2)%s ", COL_GREEN, RESET);
				}
				if (inputString[offset] == ';')
				{
					if (parse == true)
						fprintf(outputFile, "NULL");
				}
				else if (typeCheck(inputString[offset], inputString[offset + 1]) == 0)
				{
					while (inputString[offset] != ';')
					{
						if (parse == true)
							fprintf(outputFile, "%c", inputString[offset]);
						offset++;
					}
				}
				else
					offset = parseOldViewstate(outputFile, inputString, offset, indent, verbose, parse);
				offset++;

				// Third Triplet...
				if ((verbose > 0) && (parse == true))
				{
					outputIndent(indent, outputFile);
					fprintf(outputFile, "%s(3)%s ", COL_GREEN, RESET);
				}
				if (inputString[offset] == '>')
				{
					if (parse == true)
						fprintf(outputFile, "NULL");
				}
				else if (typeCheck(inputString[offset], inputString[offset + 1]) == 0)
				{
					while (inputString[offset] != '>')
					{
						if (parse == true)
							fprintf(outputFile, "%c", inputString[offset]);
						offset++;
					}
				}
				else
					offset = parseOldViewstate(outputFile, inputString, offset, indent, verbose, parse);
				offset++;
				break;

			// Pair
			case 2:
				offset += 2;

				// First Pair...
				if ((verbose > 0) && (parse == true))
				{
					fprintf(outputFile, "%s[Pair]%s", COL_CYAN, RESET);
					outputIndent(indent, outputFile);
					fprintf(outputFile, "%s(1)%s ", COL_GREEN, RESET);
				}
				if (inputString[offset] == ';')
				{
					if (parse == true)
						fprintf(outputFile, "NULL");
				}
				else if (typeCheck(inputString[offset], inputString[offset + 1]) == 0)
				{
					while (inputString[offset] != ';')
					{
						if (parse == true)
							fprintf(outputFile, "%c", inputString[offset]);
						offset++;
					}
				}
				else
					offset = parseOldViewstate(outputFile, inputString, offset, indent, verbose, parse);
				offset++;

				// Second Pair...
				if ((verbose > 0) && (parse == true))
				{
					outputIndent(indent, outputFile);
					fprintf(outputFile, "%s(2)%s ", COL_GREEN, RESET);
				}
				if (inputString[offset] == '>')
				{
					if (parse == true)
						fprintf(outputFile, "NULL");
				}
				else if (typeCheck(inputString[offset], inputString[offset + 1]) == 0)
				{
					while (inputString[offset] != '>')
					{
						if (parse == true)
							fprintf(outputFile, "%c", inputString[offset]);
						offset++;
					}
				}
				else
					offset = parseOldViewstate(outputFile, inputString, offset, indent, verbose, parse);
				offset++;
				break;

			// Array List
			case 3:
				offset += 2;
				count = 1;

				if ((verbose > 0) && (parse == true))
					fprintf(outputFile, "%s[Array]%s", COL_CYAN, RESET);
				while (inputString[offset] != '>')
				{
					if ((verbose > 0) && (parse == true))
					{
						outputIndent(indent, outputFile);
						fprintf(outputFile, "%s(%d)%s ", COL_GREEN, count, RESET);
					}
					if (inputString[offset] == ';')
					{
						if (parse == true)
							fprintf(outputFile, "NULL");
					}
					else if (typeCheck(inputString[offset], inputString[offset + 1]) == 0)
					{
						while (!((inputString[offset] == ';') || (inputString[offset] == '>')))
						{
							if (parse == true)
								fprintf(outputFile, "%c", inputString[offset]);
							offset++;
						}
					}
					else
						offset = parseOldViewstate(outputFile, inputString, offset, indent, verbose, parse);
					count++;
					offset++;
				}
				offset++;
				break;

			// Item
			case 4:
				offset += 2;

				if (inputString[offset] == '>')
				{
					if (parse == true)
						fprintf(outputFile, "NULL");
				}
				else if (typeCheck(inputString[offset], inputString[offset + 1]) == 0)
				{
					while (inputString[offset] != '>')
					{
						if (parse == true)
							fprintf(outputFile, "%c", inputString[offset]);
						offset++;
					}
				}
				else
					offset = parseOldViewstate(outputFile, inputString, offset, indent, verbose, parse);
				offset++;
				break;
		}
	}
	else
		return 0;

	return offset;
}

