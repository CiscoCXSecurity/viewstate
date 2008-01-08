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

#define mode_help 0
#define mode_version 1
#define mode_decode 2

#define false 0
#define true 1


// Includes...
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


// Colour Console Output...
#if !defined(__WIN32__)
const char *RESET = "[0m";			// DEFAULT
const char *COL_RED = "[31m";		// RED
const char *COL_BLUE = "[34m";		// BLUE
const char *COL_GREEN = "[32m";	// GREEN
const char *COL_CYAN = "[36m";		// CYAN
#else
const char *RESET = "";
const char *COL_RED = "";
const char *COL_BLUE = "";
const char *COL_GREEN = "";
const char *COL_CYAN = "";
#endif


// Viewstate includes...
#include "base64.c"
#include "misc.c"
#include "parse.c"
#include "writedata.c"


#if !defined(__WIN32__)
const char *tmpDir = "/tmp/";
#else
const char *tmpDir = "%TEMPDIR%/";
#endif


// Program banner and help
const char *program_banner = "             _                   _        _\n"
                             "      __   _(_) _____      _____| |_ __ _| |_ ___\n"
                             "      \\ \\ / / |/ _ \\ \\ /\\ / / __| __/ _` | __/ _ \\\n"
                             "       \\ V /| |  __/\\ V  V /\\__ \\ || (_| | ||  __/\n"
                             "        \\_/ |_|\\___| \\_/\\_/ |___/\\__\\__,_|\\__\\___|\n\n"
                             "                      Version 2.0\n"
                             "            http://viewstate.sourceforge.net\n"
                             "      Copyright (C) 2004-2008 Ian Ventura-Whiting\n\n";
const char *program_version = "Viewstate 2.0\nCopyright (C) 2004-2008 Ian Ventura-Whiting (Fizz)\n";


int main(int argc, char *argv[])
{
	// Variabless...
	int mode = 0;
	int parse = false;
	int hash = false;
	int hex = true;
	int raw = false;
	int error = false;
	long viewstateLength = 0;
	FILE *inputFile = stdin;
	FILE *outputFile = stdout;
	char *inputFileName = 0;
	char *outputFileName = 0;
	char *inputString = 0;
	char tempChar = 0;
	struct stat *fileStats = 0;
	int parseStatus = 0;
	int verbose = 1;
	long argLoop = 0;

	// Get program parameters
	for (argLoop = 1; argLoop < argc; argLoop++)
	{
		// Help
		if (strcmp("--help", argv[argLoop]) == 0)
			mode = mode_help;

		// Version
		else if (strcmp("--version", argv[argLoop]) == 0)
			mode = mode_version;

		// Decode
		else if (strcmp("--decode", argv[argLoop]) == 0)
			mode = mode_decode;

		// Input
		else if (strncmp("--input=", argv[argLoop], 8) == 0)
			inputFileName = argv[argLoop] + 8;

		// Output
		else if (strncmp("--output=", argv[argLoop], 9) == 0)
			outputFileName = argv[argLoop] + 9;

		// Verbose
		else if (strcmp("--verbose", argv[argLoop]) == 0)
			verbose = 2;

		// Quiet
		else if (strcmp("--quiet", argv[argLoop]) == 0)
			verbose = 0;

		// Parse
		else if (strcmp("--parse", argv[argLoop]) == 0)
			parse = true;

		// Hash
		else if (strcmp("--hash", argv[argLoop]) == 0)
			hash = true;

		// No HEX
		else if (strcmp("--no-hex", argv[argLoop]) == 0)
			hex = false;
		
		// Raw Input
		else if (strcmp("--raw", argv[argLoop]) == 0)
			raw = true;
	}
	
	switch (mode)
	{
		case mode_version:
			printf("%s%s%s", COL_BLUE, program_version, RESET);
			break;

		case mode_help:
			// Program version banner...
			printf("%s%s%s\n", COL_BLUE, program_banner, RESET);
			printf("Viewstate is  a ASP  .Net viewstate  decoder and  encoder.\n");
			printf("Viewstate  data  is   used   to   maintain  state  between\n");
			printf("different pages of a web application.\n");
			printf("Viewstate data  can be encrypted  or have  a hash appended\n");
			printf("to ensure that the data  has not  been modified.  However,\n");
			printf("viewstate can also be sent without any form of protection,\n");
			printf("this enables the data to be modified.\n");
			printf("If decode or encode are not specified,  it is assumed that\n");
			printf("help is required.\n");
			printf("%sCommand:%s\n", COL_BLUE, RESET);
			printf("  %s%s [Options]%s\n\n", COL_GREEN, argv[0], RESET);
			printf("%sOptions:%s\n", COL_BLUE, RESET);
			printf("  %s--decode%s             Decode viewstate data.\n", COL_GREEN, RESET);
			printf("  %s--input=<file>%s       Input file to process. By default\n", COL_GREEN, RESET);
			printf("                       input is from stdin.\n");
			printf("  %s--output=<file>%s      File  to  output to.  By  default\n", COL_GREEN, RESET);
			printf("                       output is from stdout.\n");
			printf("  %s--parse%s              If possible, parse the viewstate.\n", COL_GREEN, RESET);
			printf("  %s--hash%s               Output hash, if one exists.\n", COL_GREEN, RESET);
			printf("  %s--raw%s                The input is raw viewstate data.\n", COL_GREEN, RESET);
			printf("  %s--no-hex%s             Disable hex output.\n", COL_GREEN, RESET);
			printf("  %s--verbose%s            Increase verbosity.\n", COL_GREEN, RESET);
			printf("  %s--quiet%s              Decrease verbosity.\n", COL_GREEN, RESET);
			printf("  %s--version%s            Display the program version.\n", COL_GREEN, RESET);
			printf("  %s--help%s               Display the help text you are now\n", COL_GREEN, RESET);
			printf("                       reading.\n");
			break;

		case mode_decode:
			// Open output file...
			if (outputFileName != 0)
				outputFile = fopen(outputFileName, "w");
			if (outputFile != 0)
			{

				if (verbose > 1)
					fprintf(outputFile, "%s%s%s\n", COL_BLUE, program_banner, RESET);

				// Determine File Size...
				fileStats = malloc(sizeof(struct stat));
				memset(fileStats, 0, sizeof(struct stat));
				stat(inputFileName, fileStats);
				if (fileStats->st_size == 0)
				{
					free(fileStats);
					return 1;
				}

				// Reserve memory for input...
				inputString = malloc(fileStats->st_size + 1);
				memset (inputString, 0, fileStats->st_size + 1);

				// Read in input...
				if (inputFileName != 0)
					inputFile = fopen(inputFileName, "r");
				if (inputFile != NULL)
				{
					for (argLoop = 0; argLoop < fileStats->st_size; argLoop++)
						inputString[argLoop] = fgetc(inputFile);

					// If viewstate needs to be extracted from HTML...
					if (raw == false)
					{
						if (getViewstateFromHTML(inputString, fileStats->st_size) != true)
						{
							error = true;
							fprintf(outputFile, "%sERROR:%s No viewstate data in HTML.\n", COL_RED, RESET);
						}
					}

					// Decode viewstate...
					if (error == false)
					{
						viewstateLength = base64DecodeChars(inputString);
						if (viewstateLength > 0)
						{
	
							// New Viewstate...
							if (inputString[0] == -1)
							{
								if (verbose > 0)
								{
									fprintf(outputFile, "%sType      :%s New\n", COL_BLUE, RESET);
									fprintf(outputFile, "%sProtection:%s Unknown\n", COL_BLUE, RESET);
									fprintf(outputFile, "%sValidation:%s N/A\n", COL_BLUE, RESET);
									fprintf(outputFile, "%sViewstate :%s\n", COL_BLUE, RESET);
								}
								printData(outputFile, inputString, viewstateLength, hex);
								if (verbose > 0)
									fprintf(outputFile, "\n");
							}
	
							// Old Viewstate...
							else if ((inputString[0] == 't') && (inputString[1] == '<'))
							{
								parseStatus = parseOldViewstate(outputFile, inputString, 0, 0, verbose, false);
								if (parseStatus == -1)
								{
									if (verbose > 0)
									{
										fprintf(outputFile, "%sType      :%s Old\n", COL_BLUE, RESET);
										fprintf(outputFile, "%sProtection:%s Unknown%s\n", COL_BLUE, COL_RED, RESET);
										fprintf(outputFile, "%sValidation:%s Failed%s\n", COL_BLUE, COL_RED, RESET);
										fprintf(outputFile, "%sViewstate :%s\n", COL_BLUE, RESET);
									}
									printData(outputFile, inputString, viewstateLength, hex);
									if (verbose > 0)
										fprintf(outputFile, "\n");
								}
								else if (parseStatus == 0)
								{
									if (verbose > 0)
									{
										fprintf(outputFile, "%sType      :%s Old\n", COL_BLUE, RESET);
										fprintf(outputFile, "%sProtection:%s None\n", COL_BLUE, RESET);
										fprintf(outputFile, "%sValidation:%s Passed\n", COL_BLUE, RESET);
										fprintf(outputFile, "%sViewstate :%s\n", COL_BLUE, RESET);
									}
									if (parse == true)
										parseOldViewstate(outputFile, inputString, 0, 0, verbose, parse);
									else
										printData(outputFile, inputString, viewstateLength, hex);
									if (verbose > 0)
										fprintf(outputFile, "\n");
								}
								else
								{
									if (verbose > 0)
									{
										fprintf(outputFile, "%sType      :%s Old\n", COL_BLUE, RESET);
										fprintf(outputFile, "%sProtection:%s Hash\n", COL_BLUE, RESET);
										fprintf(outputFile, "%sValidation:%s Passed\n", COL_BLUE, RESET);
										fprintf(outputFile, "%sViewstate :%s\n", COL_BLUE, RESET);
									}
									if (parse == true)
										parseOldViewstate(outputFile, inputString, 0, 0, verbose, parse);
									else
									{
										tempChar = inputString[parseStatus];
										inputString[parseStatus] = 0;
										printData(outputFile, inputString, parseStatus, hex);
										inputString[parseStatus] = tempChar;
									}
									if (verbose > 0)
										fprintf(outputFile, "\n");
									if (hash == true)
									{
										if (verbose > 0)
											fprintf(outputFile, "%sHash      :%s\n", COL_BLUE, RESET);
										printData(outputFile, inputString + parseStatus, viewstateLength - parseStatus, hex);
										if (verbose > 0)
											fprintf(outputFile, "\n");
									}
								}
							}
	
							// Encrypted Viewstate...
							else
							{
								if (verbose > 0)
								{
									fprintf(outputFile, "%sType      :%s Unknown%s\n", COL_BLUE, COL_RED, RESET);
									fprintf(outputFile, "%sProtection: Encrypted%s\n", COL_BLUE, RESET);
									fprintf(outputFile, "%sValidation:%s N/A\n", COL_BLUE, RESET);
									fprintf(outputFile, "%sViewstate :%s\n", COL_BLUE, RESET);
								}
								printData(outputFile, inputString, viewstateLength, hex);
								if (verbose > 0)
									fprintf(outputFile, "\n");
							}
	
						}
						else
						{
							if (verbose > 0)
								fprintf(outputFile, "%sERROR:%s Decode failed.\n", COL_RED, RESET);
						}
					}

					// Close file...
					if (inputFileName != 0)
						fclose(inputFile);
				}
				else if (verbose > 0)
					fprintf(outputFile, "%sERROR:%s Could not open input.\n", COL_RED, RESET);

				// Free input String
				free(inputString);
	
				// Free Stats...
				free(fileStats);
				
				// Close output File...
				if (outputFileName != 0)
					fclose(outputFile);
			}
			else if (verbose > 0)
				fprintf(outputFile, "%sERROR:%s Could not open output.\n", COL_RED, RESET);
			break;
	}
	return 0;
}

