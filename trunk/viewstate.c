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
#define mode_encode 3

#define false 0
#define true 1


// Includes...
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#if !defined(__WIN32__)
#include <netdb.h>
#include <sys/socket.h>
#endif


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


// Temp Dir...
#if !defined(__WIN32__)
const char *tmpDir = "/tmp/";
#else
const char *tmpDir = "%TEMPDIR%";
#endif


// Viewstate includes...
#include "base64.c"
#include "misc.c"
#include "parse.c"
#include "writedata.c"





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
	int sizeRead = 0;
	long viewstateLength = 0;
	FILE *inputFile = stdin;
	FILE *outputFile = stdout;
	FILE *tempFileHandle = 0;
	char *http = 0;
	char *inputFileName = 0;
	char *outputFileName = 0;
	char *inputString = 0;
	char *outputString = 0;
	char tempChar = 0;
	char temporaryFileName[64] = "";
	char serverName[64] = "";
	char buffer[1024];
	struct stat *fileStats = 0;
	int parseStatus = 0;
	int verbose = 1;
	long argLoop = 0;
	int tempInt = 0;

	signal(14, stdinTimeout);

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

		// Encode
		else if (strcmp("--encode", argv[argLoop]) == 0)
			mode = mode_encode;

		// Input
		else if (strncmp("--input=", argv[argLoop], 8) == 0)
			inputFileName = argv[argLoop] + 8;

		// Output
		else if (strncmp("--output=", argv[argLoop], 9) == 0)
			outputFileName = argv[argLoop] + 9;

		// HTTP
		else if (strncmp("--url=", argv[argLoop], 6) == 0)
			http = argv[argLoop] + 6;

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

	// If input is from stdin...
	if ((inputFileName == 0) && ((mode == mode_decode) || (mode == mode_encode)) && (http == 0))
	{
		// Read stdin...
		while (feof(inputFile) == 0)
		{
			// Read...
			memset(buffer, 0, 1024);
#if !defined(__WIN32__)
			alarm(2);
#endif
			sizeRead = fread(buffer, 1, 1024, inputFile);
#if !defined(__WIN32__)
			alarm(0);
#endif

			// Write to file...
			if (tempFileHandle == 0)
			{
				sprintf(temporaryFileName, "%sdelete-me-%d", tmpDir, rand());
				tempFileHandle = fopen(temporaryFileName, "w");
				fwrite(buffer, 1, sizeRead, tempFileHandle);
			}
			else
				fwrite(buffer, 1, sizeRead, tempFileHandle);
		}
		fclose(tempFileHandle);
		inputFileName = temporaryFileName;

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
			printf("Viewstate data  can be encrypted  or have  a hash appended\n");
			printf("to ensure that the data  has not  been modified.  However,\n");
			printf("viewstate can also be sent without any form of protection,\n");
			printf("this enables the data to be modified.\n");
			printf("When running  viewstate,  if the decode or  encode options\n");
			printf("are not specified,  it is assumed that help is required.\n\n");
			printf("%sCommand:%s\n", COL_BLUE, RESET);
			printf("  %s%s [Mode] [Options]%s\n\n", COL_GREEN, argv[0], RESET);
			printf("%sModes:%s\n", COL_BLUE, RESET);
			printf("  %s--decode%s             Decode viewstate data.\n", COL_GREEN, RESET);
			printf("  %s--encode%s             Encode viewstate data.\n", COL_GREEN, RESET);
			printf("  %s--version%s            Display the program version.\n", COL_GREEN, RESET);
			printf("  %s--help%s               Display the help text you are now\n", COL_GREEN, RESET);
			printf("                       reading.\n\n");
			printf("%sGeneral Options:%s\n", COL_BLUE, RESET);
			printf("  %s--input=<file>%s       Input file to process. By default\n", COL_GREEN, RESET);
			printf("                       input is from stdin.\n");
			printf("  %s--output=<file>%s      File  to  output to.  By  default\n", COL_GREEN, RESET);
			printf("                       output is from stdout.\n");
			printf("  %s--verbose%s            Increase verbosity.\n", COL_GREEN, RESET);
			printf("  %s--quiet%s              Decrease verbosity.\n\n", COL_GREEN, RESET);
			printf("%sDecode Options:%s\n", COL_BLUE, RESET);
			printf("  %s--url=<url>%s          Get the viewstate from a URL,  no\n", COL_GREEN, RESET);
			printf("                       SSL support in this version.  Specify\n");
			printf("                       as http://www.somewhere.com.\n");
			printf("  %s--parse%s              If possible, parse the viewstate.\n", COL_GREEN, RESET);
			printf("  %s--hash%s               Output hash, if one exists.\n", COL_GREEN, RESET);
			printf("  %s--raw%s                The input is raw viewstate data.\n", COL_GREEN, RESET);
			printf("  %s--no-hex%s             Disable hex output.\n\n", COL_GREEN, RESET);
			break;


		// Decode Viewstate
		case mode_decode:
			// If input is from a URL
			if (http != 0)
			{
				if (strncasecmp(http, "https://", 8) == 0)
				{
					error = true;
					fprintf(outputFile, "%sERROR:%s HTTPS is not supported in this version.\n", COL_RED, RESET);
				}

				else if (strncasecmp(http, "http://", 7) == 0)
				{
					sprintf(temporaryFileName, "%sdelete-me-%d", tmpDir, rand());

					// Is there a path...
					tempInt = 7;
					while ((tempInt < strlen(http)) && (http[tempInt] != '/'))
						tempInt++;

					// No path...
					if (tempInt == strlen(http))
					{
						if (fileDownload(http + 7, "/", temporaryFileName) == true)
							inputFileName = temporaryFileName;
					}

					// Path...
					else
					{
						// Get name...
						tempInt = 7;
						while ((tempInt < strlen(http)) && (http[tempInt] != '/'))
						{
							serverName[tempInt - 7] = http[tempInt];
							tempInt++;
						}
						serverName[tempInt - 7] = 0;

						if (fileDownload(serverName, http + tempInt, temporaryFileName) == true)
							inputFileName = temporaryFileName;
					}
					
				}

				else
				{
					error = true;
					fprintf(outputFile, "%sERROR:%s URL is not valid. It should be http://...\n", COL_RED, RESET);
				}
			}

			// Open output file...
			if (outputFileName != 0)
				outputFile = fopen(outputFileName, "w");

			if ((outputFile != 0) && (error == false))
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


		// Encode Viewstate
		case mode_encode:
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

					// Reserve memory for output string
					outputString = malloc((int)(fileStats->st_size / 3 * 4) + 2);
					memset (outputString, 0, (int)(fileStats->st_size / 3 * 4) + 2);

					// Encode string...
					if (base64Encode(inputString, fileStats->st_size, outputString) == true)
					{
						if (verbose > 0)
							fprintf(outputFile, "%sViewstate :%s\n", COL_BLUE, RESET);
						fprintf(outputFile, "%s", outputString);
						if (verbose > 0)
							fprintf(outputFile, "\n");
					}
					else
						fprintf(outputFile, "%sERROR:%s Failed to encode string.\n", COL_RED, RESET);

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

	// Stdin temporary file...
	if (temporaryFileName[0] != 0)
	{
		unlink(temporaryFileName);
	}

	return 0;
}

