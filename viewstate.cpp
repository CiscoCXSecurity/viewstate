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

// Colour Console Output
const char *RESET = "[0m";			// DEFAULT
const char *COL_RED = "[31m";		// RED
const char *COL_BLUE = "[34m";		// BLUE
const char *COL_GREEN = "[32m";	// GREEN

// Program banner and help
const string bannerlogo = "            _                   _        _\n     __   _(_) _____      _____| |_ __ _| |_ ___\n     \\ \\ / / |/ _ \\ \\ /\\ / / __| __/ _` | __/ _ \\\n      \\ V /| |  __/\\ V  V /\\__ \\ || (_| | ||  __/\n       \\_/ |_|\\___| \\_/\\_/ |___/\\__\\__,_|\\__\\___|";
const string version = "                     Version 2.0.0\n             by Ian Ventura-Whiting (Fizz)";

#define mode_none 0
#define mode_error 1
#define mode_encode 2
#define mode_decode 3
#define mode_check 4

#define noise_quiet 0
#define noise_normal 1
#define noise_verbose 2


struct programOptions
{
	// Options...
	int inputFile;						// Points to commandline arguement containing input file
	int outputFile;						// Points to commandline arguement containing output file
	int viewstateMode;					// Mode for the viewstate program
	int noise;							// Output noise
	bool htmlInput;						// Treat input as HTML
	bool showHash;						// Show the viewstate hash (if one exists)
	bool parseInput;					// Parse the viewstate data
	int hashFile;						// Points to commandline arguement containing a hash file
	bool urlSafe;						// URL safe output option for encoded viewstate
};


int main(int argc, char *argv[])
{
	// Variables...
	struct programOptions options;
	int fileLength;
	char *fileBuffer;
	string tempString;
	ifstream hashFile;
	ifstream inFile;
	ofstream outFile;
	string inputString;
	string outputString;
	bool showHelp = false;				// Show on-line help

	// Defaults...
	options.inputFile = 0;				// Points to commandline arguement containing input file
	options.outputFile = 0;				// Points to commandline arguement containing output file
	options.viewstateMode = mode_none;	// Mode for the viewstate program
	options.noise = noise_normal;		// Quiet option
	options.htmlInput = false;			// Treat input as HTML
	options.showHash = false;			// Show the viewstate hash (if one exists)
	options.parseInput = false;			// Parse the viewstate data
	options.hashFile = 0;				// Points to commandline arguement containing a hash file
	options.urlSafe = false;			// URL safe output option for encoded viewstate

	// Process command line options...
	for (int loop = 1; loop < argc; loop++)
	{
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

		// check mode
		else if (strcmp(argv[loop], "--check") == 0)
		{
			if (options.viewstateMode == mode_none)
				options.viewstateMode = mode_check;
			else
				options.viewstateMode = mode_error;
		}

		// Input file
		else if (strncmp(argv[loop], "--input=", 8) == 0)
			options.inputFile = loop;

		// Output file
		else if (strncmp(argv[loop], "--output=", 9) == 0)
		{
			if (options.viewstateMode == mode_check)
				showHelp = true;
			else
				options.outputFile = loop;
		}

		// Quiet option
		else if (strcmp(argv[loop], "--quiet") == 0)
			options.noise = noise_quiet;

		// Verbose option
		else if (strcmp(argv[loop], "--verbose") == 0)
			options.noise = noise_verbose;

		// HTML input option
		else if (strcmp(argv[loop], "--html") == 0)
		{
			if (options.viewstateMode == mode_encode)
				showHelp = true;
			else
				options.htmlInput = true;
		}

		// Show hash option
		else if (strcmp(argv[loop], "--showhash") == 0)
		{
			if (options.viewstateMode == mode_encode)
				showHelp = true;
			else
				options.showHash = true;
		}

		// Parse option
		else if (strcmp(argv[loop], "--parse") == 0)
		{
			if (options.viewstateMode == mode_encode)
				showHelp = true;
			else
				options.parseInput = true;
		}

		// Hash file
		else if (strncmp(argv[loop], "--hash=", 7) == 0)
		{
			if (options.viewstateMode != mode_encode)
				showHelp = true;
			else
				options.hashFile = loop;
		}

		// URL Safe option
		else if (strcmp(argv[loop], "--urlSafe") == 0)
		{
			if (options.viewstateMode != mode_encode)
				showHelp = true;
			else
				options.urlSafe = true;
		}

		// Catch all other command line parameters
		else
			showHelp = true;
	}

	// Show help...
	if ((showHelp == true) || (options.viewstateMode == mode_none) || (options.viewstateMode == mode_error))
	{
		// Program banner and version...
		cout << COL_BLUE << bannerlogo << RESET << endl << endl;
		cout << version << endl << endl;

		// Program description...
		cout << "   Web pages provided by ASP.net will typically contain" << endl;
		cout << "   viewstate data. Viewstate is used by ASP.net to" << endl;
		cout << "   maintain state between web pages. This program is" << endl;
		cout << "   designed to decode, encode or check the ASP.net" << endl;
		cout << "   viewstate data provided by ASP.net." <<endl << endl;

		// Program command
		cout << COL_BLUE << "Command:" << RESET << endl << endl;
		cout << COL_GREEN << "   " << argv[0] << " <mode> [options]" << RESET << endl << endl;

		// Program modes
		cout << COL_BLUE << "Modes:" << RESET << endl << endl;

		cout << COL_GREEN << "   --decode" << RESET << endl;
		cout << "   Decode the encoded viewstate data, if possible." << endl << endl;

		cout << COL_GREEN << "   --encode" << RESET << endl;
		cout << "   Encode the viewstate data." << endl << endl;

		cout << COL_GREEN << "   --check" << RESET << endl;
		cout << "   Decode the encoded viewstate data, but not output." << endl << endl;

		// Program options
		cout << COL_BLUE << "General Options:" << RESET << endl << endl;

		cout << COL_GREEN << "   --help" << RESET << endl;
		cout << "   Display this command line help. Overrides all other" << endl;
		cout << "   options." << endl << endl;

		cout << COL_GREEN << "   --input=<file>" << RESET << endl;
		cout << "   The input file for processing. If not specified, the" << endl;
		cout << "   input is retrieved from stdin." << endl << endl;

		cout << COL_GREEN << "   --output=<file>" << RESET << endl;
		cout << "   The output file for the result of an encoding or" <<endl;
		cout << "   decoding operation. If not specified, the output" << endl;
		cout << "   is sent to stdout." << endl << endl;

		cout << COL_GREEN << "   --quiet" << RESET << endl;
		cout << "   Do not show the processing information." << endl << endl;

		cout << COL_GREEN << "   --verbose" << RESET << endl;
		cout << "   Show additional debug output." << endl << endl;

		cout << COL_BLUE << "Decode/Check Mode Options:" << RESET << endl << endl;

		cout << COL_GREEN << "   --html" << RESET << endl;
		cout << "   The input file for processing should be treated as" << endl;
		cout << "   raw HTML. The default is to treat the input as raw" << endl;
		cout << "   viewstate data." << endl << endl;

		cout << COL_GREEN << "   --showhash" << RESET << endl;
		cout << "   Show the viewstate hash, if one exists. Viewstate" << endl;
		cout << "   has a hash appended by default, if a hash doesn't" << endl;
		cout << "   exist then it is possible to modify the data and" << endl;
		cout << "   submit a modified viewstate back to the server." << endl << endl;

		cout << COL_GREEN << "   --parse" << RESET << endl;
		cout << "   Parse the viewstate data to make it more human" << endl;
		cout << "   readable and output to stdout." << endl << endl;

		cout << COL_BLUE << "Encode Mode Options:" << RESET << endl << endl;

		cout << COL_GREEN << "   --hash=<file>" << RESET << endl;
		cout << "   Append a hash to the encoded viewstate data." << endl << endl;

		cout << COL_GREEN << "   --urlsafe" << RESET << endl;
		cout << "   Make the output safe to use in a URL." << endl << endl;
	}

	// Lets do it!
	else
	{
		// Display program banner and version...
		if (options.noise != noise_quiet)
			cout << COL_BLUE << bannerlogo << RESET << endl << endl << version << endl << endl;

		// Display command line options
		if (options.noise == noise_verbose)
		{
			cout << COL_GREEN << "Command: " << RESET;
			for (int loop = 0; loop < argc; loop++)
			{
				cout << argv[loop] << " ";
			}
			cout << endl;
		}
		if (options.noise != noise_quiet)
		{
			cout << COL_GREEN << "Mode   : " << RESET;
			switch (options.viewstateMode)
			{
				case mode_decode:
					cout << "Decode" << endl;
					break;
				case mode_encode:
					cout << "Encode" << endl;
					break;
				case mode_check:
					cout << "Check" << endl;
					break;
			}
			cout << COL_GREEN << "Input  : " << RESET;
			if (options.inputFile != 0)
				cout << argv[options.inputFile] + 8 << endl;
			else
				cout << "stdin" << endl;
			if (options.viewstateMode != mode_check)
			{
				cout << COL_GREEN << "Output : " << RESET;
				if (options.outputFile != 0)
					cout << argv[options.outputFile] + 9 << endl;
				else
					cout << "stdout" << endl;
			}
			cout << endl;
		}

		// Open input file
		if (options.inputFile != 0)
		{
			if (options.noise == noise_verbose)
				cout << COL_GREEN << "Opening file:" << RESET;
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

							// Check mode, output viewstate type...
							if ((options.viewstateMode == mode_check) || (options.noise != noise_quiet))
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

						// Append a hash?
						if (options.hashFile != 0)
						{
							tempString.clear();

							// Open hash file...
							if (options.noise == noise_verbose)
								cout << COL_GREEN << "Opening Hash File:" << RESET << endl;
							hashFile.open(argv[options.hashFile] + 7, ios::binary);
							if (hashFile.is_open())
							{
								if (options.noise == noise_verbose)
									cout << " OK" << endl;

								// get length of file...
								hashFile.seekg (0, ios::end);
								fileLength = hashFile.tellg();
								hashFile.seekg (0, ios::beg);

								// allocate memory...
								fileBuffer = new char [fileLength];

								// Read file...
								hashFile.read (fileBuffer, fileLength);
								str_Hash = fileBuffer;
								str_Hash = str_Hash.substr(0, fileLength);
								str_Hash = StringStrip(str_Hash);
								hashFile.close();
								str_FileData.append(str_Hash);

								// free memory...
  								delete[] chr_HashBuffer;

								if (bl_Verbose == true)
									cout << "  [OK]" << endl;
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

	return EXIT_SUCCESS;
}
