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

//#include <stdio.h>
//#include <stdlib.h>
//#include <iostream>
#include <fstream>
#include <string.h>

#include "type_convert.h"
#include "base64.h"

// Colour Console Output...
#if !defined(__WIN32__)
const char *RESET = "[0m";			// DEFAULT
const char *COL_RED = "[31m";		// RED
const char *COL_BLUE = "[34m";		// BLUE
const char *COL_GREEN = "[32m";	// GREEN
#else
const char *RESET = "";
const char *COL_RED = "";
const char *COL_BLUE = "";
const char *COL_GREEN = "";
#endif

// Program banner and help
const string progbanner = "             _                   _        _\n"
                          "      __   _(_) _____      _____| |_ __ _| |_ ___\n"
                          "      \\ \\ / / |/ _ \\ \\ /\\ / / __| __/ _` | __/ _ \\\n"
                          "       \\ V /| |  __/\\ V  V /\\__ \\ || (_| | ||  __/\n"
                          "        \\_/ |_|\\___| \\_/\\_/ |___/\\__\\__,_|\\__\\___|\n\n"
                          "                      Version 2.0.0\n"
                          "              by Ian Ventura-Whiting (Fizz)\n"
                          "                viewstate.sourceforge.net";
const string progver = "Viewstate 2.0.0 by Ian Ventura-Whiting (Fizz)";

#define mode_none 0
#define mode_help 1
#define mode_version 2
#define mode_error 3
#define mode_encode 4
#define mode_decode 5

#define noise_quiet 0
#define noise_normal 1
#define noise_verbose 2


struct programOptions
{
	// Files...
	char *inputFilename;				// Input file
	char *outputFilename;				// Output file
	char *hashFilename;					// File containing a hash

	// Options...
	int viewstateMode;					// Mode for the viewstate program
	int noise;							// Output noise
	bool htmlInput;						// Treat input as HTML
	bool showHash;						// Show the viewstate hash (if one exists)
	bool parseInput;					// Parse the viewstate data
	bool urlSafe;						// URL safe output option for encoded viewstate
};


int main(int argc, char *argv[])
{
	// Variables...
	struct programOptions options;
	ifstream inFile;
	ofstream outFile;
	ifstream hashFile;

	// Init...
	memset(&options, 0, sizeof(options));
	options.viewstateMode = mode_none;	// Mode for the viewstate program
	options.noise = noise_normal;		// Quiet option
	options.htmlInput = false;			// Treat input as HTML
	options.showHash = true;			// Show the viewstate hash (if one exists)
	options.parseInput = false;			// Parse the viewstate data
	options.urlSafe = false;			// URL safe output option for encoded viewstate

	// Process command line options...
	for (int loop = 1; loop < argc; loop++)
	{
		// MODES...

		// If decode mode
		if (strcmp(argv[loop], "--decode") == 0)
		{
			if (options.viewstateMode == mode_none)
				options.viewstateMode = mode_decode;
			else
				options.viewstateMode = mode_error;
		}

		// encode mode
		else if (strcmp(argv[loop], "--encode") == 0)
		{
			if (options.viewstateMode == mode_none)
				options.viewstateMode = mode_encode;
			else
				options.viewstateMode = mode_error;
		}

		// help mode
		else if (strcmp(argv[loop], "--help") == 0)
		{
			if (options.viewstateMode == mode_none)
				options.viewstateMode = mode_help;
			else
				options.viewstateMode = mode_error;
		}

		// version mode
		else if (strcmp(argv[loop], "--version") == 0)
		{
			if (options.viewstateMode == mode_none)
				options.viewstateMode = mode_version;
			else
				options.viewstateMode = mode_error;
		}

		// GENERAL OPTIONS...

		// Input file
		else if ((strncmp(argv[loop], "--input=", 8) == 0) && (strlen(argv[loop]) > 8))
			options.inputFilename = argv[loop] + 8;

		// Output file
		else if ((strncmp(argv[loop], "--output=", 9) == 0) && (strlen(argv[loop]) > 9))
			options.outputFilename = argv[loop] + 9;

		// Quiet option
		else if (strcmp(argv[loop], "--quiet") == 0)
			options.noise = noise_quiet;

		// Verbose option
		else if (strcmp(argv[loop], "--verbose") == 0)
			options.noise = noise_verbose;

		// DECODE OPTIONS...

		// HTML input option
		else if (strcmp(argv[loop], "--html") == 0)
		{
			if (options.viewstateMode == mode_encode)
				options.viewstateMode = mode_error;
			else
				options.htmlInput = true;
		}

		// Show hash option
		else if (strcmp(argv[loop], "--no-hash") == 0)
		{
			if (options.viewstateMode == mode_encode)
				options.viewstateMode = mode_error;
			else
				options.showHash = false;
		}

		// Parse option
		else if (strcmp(argv[loop], "--parse") == 0)
		{
			if (options.viewstateMode == mode_encode)
				options.viewstateMode = mode_error;
			else
				options.parseInput = true;
		}

		// ENCODE OPTIONS...

		// Hash file
		else if ((strncmp(argv[loop], "--hash=", 7) == 0) && (strlen(argv[loop]) > 7))
		{
			if (options.viewstateMode == mode_decode)
				options.viewstateMode = mode_error;
			else
				options.hashFilename = argv[loop] + 7;
		}

		// URL Safe option
		else if (strcmp(argv[loop], "--urlsafe") == 0)
		{
			if (options.viewstateMode == mode_decode)
				options.viewstateMode = mode_error;
			else
				options.urlSafe = true;
		}

		// Catch all other command line parameters
		else
			options.viewstateMode = mode_error;
	}

	switch (options.viewstateMode)
	{
		// Program version...
		case mode_version:
			cout << progver << endl;
			break;

		// An error occured...
		case mode_none:
		case mode_error:
			cout << COL_RED << "ERROR: Invalid commandline arguements!" << endl << endl;

		// Show help...
		case mode_help:
			cout << COL_BLUE << progbanner << RESET << endl << endl;
			// Program description...
			cout << "Viewstate is a small tool that can decode, encode and verify" << endl;
			cout << "ASP .Net viewstate data.  Viewstate data is used to maintain" << endl;
			cout << "state between  web pages and  includes some  simple security" << endl;
			cout << "options." << endl << endl;
			cout << "At present Viewstate  cannot decrypt  encrypted data  and it" << endl;
			cout << "doesn't generate  a hash  when encoding.  If you know of how" << endl;
			cout << "these can be done, get in touch and I'll add it. :-)" << endl << endl;

			// Program command
			cout << COL_BLUE << "Command:" << RESET << endl << endl;
			cout << COL_GREEN << "   " << argv[0] << " <mode> [options]" << RESET << endl << endl;

			// Program modes
			cout << COL_BLUE << "Modes:" << RESET << endl << endl;

			cout << COL_GREEN << "   --decode" << RESET << endl;
			cout << "   If possible, decode the supplied viewstate data." << endl << endl;

			cout << COL_GREEN << "   --encode" << RESET << endl;
			cout << "   Encode the supplied viewstate data." << endl << endl;

			cout << COL_GREEN << "   --version" << RESET << endl;
			cout << "   Display the program version." << endl << endl;

			cout << COL_GREEN << "   --help" << RESET << endl;
			cout << "   Show this on-line help text you are reading now." << endl << endl;

			// General options
			cout << COL_BLUE << "General Options:" << RESET << endl;
			cout << "   The general  options can  be used  with either  encode or" <<endl;
			cout << "   decode modes." << endl << endl;

			cout << COL_GREEN << "   --input=<file>" << RESET << endl;
			cout << "   An input file for processing. If not specified, the input" << endl;
			cout << "   is retrieved from stdin." << endl << endl;

			cout << COL_GREEN << "   --output=<file>" << RESET << endl;
			cout << "   The  output file for the  result.  If  not specified  the" << endl;
			cout << "   output is sent to stdout." << endl << endl;

			cout << COL_GREEN << "   --quiet" << RESET << endl;
			cout << "   Do not show the processing information." << endl << endl;

			cout << COL_GREEN << "   --verbose" << RESET << endl;
			cout << "   Show additional debug output." << endl << endl;

			// Decode options
			cout << COL_BLUE << "Decode Mode Options:" << RESET << endl;
			cout << "   These  options  are  for   viewstate  decoding.   If  the" << endl;
			cout << "   viewstate data is encrypted, the encrypted data is shown." << endl << endl;

			cout << COL_GREEN << "   --html" << RESET << endl;
			cout << "   Treat the  input file as raw HTML.  If this option is not" << endl;
			cout << "   specified  the input  will be  treated as  raw  viewstate" << endl;
			cout << "   data." << endl << endl;

			cout << COL_GREEN << "   --no-hash" << RESET << endl;
			cout << "   Do not show the viewstate hash, if one exists." << endl << endl;

			cout << COL_GREEN << "   --parse" << RESET << endl;
			cout << "   Parse the viewstate  data to make it  more human readable" << endl;
			cout << "   and output to stdout.  At present,  only the old style of" << endl;
			cout << "   viewstate is parsed." << endl << endl;

			// Encode options
			cout << COL_BLUE << "Encode Mode Options:" << RESET << endl;
			cout << "   These are for viewstate encoding." << endl << endl;

			cout << COL_GREEN << "   --hash=<file>" << RESET << endl;
			cout << "   Append a  hash to  the encoded  viewstate data.  The hash" << endl;
			cout << "   must be calculated and valid,  else it is rejected by the" << endl;
			cout << "   server." << endl << endl;

			cout << COL_GREEN << "   --urlsafe" << RESET << endl;
			cout << "   Make the output safe to use in a URL." << endl << endl;
			break;

		case mode_decode:
		case mode_encode:
			// Display program banner and version...
			if (options.noise != noise_quiet)
				cout << COL_BLUE << progbanner << RESET << endl << endl;

			// Display command line options...
			if (options.noise == noise_verbose)
			{
				cout << COL_GREEN << "Command: " << RESET;
				for (int loop = 0; loop < argc; loop++)
					cout << argv[loop] << " ";
				cout << endl;
			}

			if (options.noise != noise_quiet)
			{
				// Show mode...
				if (options.viewstateMode == mode_decode)
					cout << COL_GREEN << "Mode   : " << RESET << "Decode" << endl;
				else
					cout << COL_GREEN << "Mode   : " << RESET << "Encode" << endl;

				// Show Input...
				if (options.inputFilename != 0)
					cout << COL_GREEN << "Input  : " << RESET << options.inputFilename << endl;
				else
					cout << COL_GREEN << "Input  : " << RESET << "stdin" << endl;

				// Show Output...
				if (options.outputFilename != 0)
					cout << COL_GREEN << "Output : " << RESET << options.outputFilename << endl;
				else
					cout << COL_GREEN << "Output : " << RESET << "stdout" << endl;

				// Hash file...
				if (options.hashFilename != 0)
					cout << COL_GREEN << "Hash   : " << RESET << options.hashFilename << endl;
			}

			// Opening input...
			if (options.noise == noise_verbose)
				cout << COL_GREEN << "Opening input:" << RESET;
			if (options.inputFilename != 0)
			{
			}
			break;
	}
}

/*

		// Open input file
		
			
			inFile.open(argv[options.inputFile] + 8);

			// File open ok
			if (inFile.is_open())
			{
				if ((options.noise == noise_verbose) && (options.inputFile != 0))
					cout << " OK" << endl;

				// if it is a HTML file, extract the viewstate...
				if (options.htmlInput == true)
				{
					if (options.noise == noise_verbose)
						cout << COL_GREEN << "Extracting Viewstate:" << RESET;

					// get length of file...
					inFile.seekg (0, ios::end);
					fileLength = inFile.tellg();
					inFile.seekg (0, ios::beg);

					// allocate file contents memory...
					fileBuffer = new char [fileLength];

					// Read file contents...
					inFile.read (fileBuffer, fileLength);

					// Strip end characters
					tempString = fileBuffer;
					tempString = tempString.substr(0, fileLength);
					tempString = StringStrip(tempString);

					// free memory...
					delete[] fileBuffer;

					// Find start position...
					int startPosition = tempString.find("name=\"__VIEWSTATE\"");
					if (startPosition != -1)
					{
						startPosition = tempString.find("value=\"", startPosition);

						// Find end position...
						if (startPosition != -1)
						{
							int endPosition = tempString.find('"', startPosition + 7);

							// Extract viewstate data...
							if (endPosition != -1)
							{
								inputString = tempString.substr(startPosition + 7, endPosition - startPosition - 8);
								if (options.noise == noise_verbose)
									cout << " OK" << endl;
							}
							else
							{
								if (options.noise == noise_verbose)
									cout << COL_RED << " FAIL" << RESET << endl;
								cout << COL_RED << "ERROR: HTML Viewstate data value does not end properly!" << RESET << endl;
							}
						}
						else
						{
							if (options.noise == noise_verbose)
								cout << COL_RED << " FAIL" << RESET << endl;
							cout << COL_RED << "ERROR: No Viewstate value=\"<the data>\" found in HTML!" << RESET << endl;
						}
					}
					else
					{
						if (options.noise == noise_verbose)
							cout << COL_RED << " FAIL" << RESET << endl;
						cout << COL_RED << "ERROR: Viewstate data not found in HTML!" << RESET << endl;
					}
				}

				// Read contents of input file...
				else
				{
					// get length of file...
					inFile.seekg (0, ios::end);
					fileLength = inFile.tellg();
					inFile.seekg (0, ios::beg);

					// allocate file contents memory...
					fileBuffer = new char [fileLength];

					// Read file contents...
					inFile.read (fileBuffer, fileLength);

					// Strip end characters
					tempString = fileBuffer;
					tempString = tempString.substr(0, fileLength);
					inputString = StringStrip(tempString);

					// free memory...
					delete[] fileBuffer;
				}

				// Closing file
				if (options.inputFile != 0)
				{
					if (options.noise == noise_verbose)
						cout << COL_GREEN << "Closing file:" << RESET;
					inFile.close();
					if (options.noise == noise_verbose)
						cout << " DONE" << endl;
				}
			}

			// Do it...
			if (inputString != "")
			{
				switch (options.viewstateMode)
				{


					// Decode / Check Viewstate
					case mode_check:
					case mode_decode:

						// Decode the viewstate...
						if (options.noise == noise_verbose)
							cout << COL_GREEN << "Decoding Viewstate:" << RESET;
						outputString = Base64Decode(inputString);

						// Decoded ok...
						if (outputString != "")
						{
							if (options.noise == noise_verbose)
								cout << " OK" << endl;

							// Output viewstate type...
							if (options.noise != noise_quiet)
							{
								cout << COL_GREEN << "Viewstate Type: " << RESET;
								if (outputString[0] == -1)
									cout << "New Type" << endl;
								else if (outputString[0] == 't' && outputString[1] == '<')
									cout << "Old Type" << endl;
								else
									cout << "Encrypted" << endl;
							}

							// If Decode Mode...
							if (options.viewstateMode == mode_decode)
							{
								if (options.noise != noise_quiet)
									cout << COL_GREEN << "Decoded Viewstate:" << RESET << endl;
								cout << outputString << endl;
							}
						}
						else
						{
							if (options.noise == noise_verbose)
								cout << COL_RED << " FAIL" << RESET << endl;
							cout << COL_RED << "ERROR: Could not decode Viewstate. It could be corrupt!" << RESET << endl;
						}
						break;


					// Encode Viewstate
					case mode_encode:

						tempString.clear();
						// Append a hash?
						if (options.hashFile != 0)
						{
							// Open hash file...
							if (options.noise == noise_verbose)
								cout << COL_GREEN << "Opening Hash File:" << RESET;
							hashFile.open(argv[options.hashFile] + 7, ios::binary);
							if (hashFile.is_open())
							{
								if (options.noise == noise_verbose)
									cout << " OK" << endl << COL_GREEN << "Appending Hash:" << RESET;

								// get length of file...
								hashFile.seekg (0, ios::end);
								fileLength = hashFile.tellg();
								hashFile.seekg (0, ios::beg);

								// allocate memory...
								fileBuffer = new char [fileLength];

								// Read file...
								hashFile.read (fileBuffer, fileLength);
								tempString = fileBuffer;
								tempString = tempString.substr(0, fileLength);
								tempString = StringStrip(tempString);
								hashFile.close();
								inputString.append(tempString);

								// free memory...
								delete[] fileBuffer;

								if (options.noise == noise_verbose)
									cout << " OK" << endl;
							}
							else
							{
								if (options.noise == noise_verbose)
									cout << COL_RED << " FAIL" << RESET << endl;
								cout << COL_RED << "ERROR: Could not open hash file!" << RESET << endl;
							}
						}

						// Encode the viewstate...
						if (options.noise == noise_verbose)
							cout << COL_GREEN << "Encoding Viewstate:" << RESET;
						outputString = Base64Encode(inputString);
						if ((outputString != inputString) && (options.noise == noise_verbose))
							cout << " OK" << endl;
						else if (outputString == inputString)
						{
							if (options.noise == noise_verbose)
								cout << COL_RED << " FAIL" << RESET << endl;
							cout << COL_RED << "ERROR: Could not encode Viewstate!" << RESET << endl;
						}

						// URL Encode?
						if (options.urlSafe == true)
						{
							if (options.noise == noise_verbose)
								cout << COL_GREEN << "URL Encoding:" << RESET;
							outputString = URLEncode(outputString);
							if ((outputString != "") && (options.noise == noise_verbose))
								cout << " OK" << endl;
							else if (outputString == "")
							{
								if (options.noise == noise_verbose)
									cout << COL_RED << " FAIL" << RESET << endl;
								cout << COL_RED << "ERROR: Could not URL encode!" << RESET << endl;
							}
						}
						break;
				}
			}
			else
				cout << COL_RED << "ERROR: Nothing to process!" << RESET << endl;
		}
		else
		{
			if ((options.noise == noise_verbose) && (options.inputFile != 0))
				cout << COL_RED << " FAIL" << RESET << endl;
			cout << COL_RED << "ERROR: Could not open input file!" << RESET << endl;
		}
	}
*/

