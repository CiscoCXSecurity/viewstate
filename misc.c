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


void stdinTimeout()
{
	fprintf(stderr, "%sERROR: Timeout reading from stdin.%s\n", COL_RED, RESET);
	exit(0);
}


#if !defined(__WIN32__)
int fileDownload(char *serverString, char *requestString, char *saveFile)
{
	// Variables...
	char tempString[128];
	char *sendString = 0;
	int socketDescriptor;
	int status;
	FILE *resultFile;
	struct hostent *patchHost;
	struct sockaddr_in serverAddress;
	struct sockaddr_in localAddress;

	// Open Patch File...
	resultFile = fopen(saveFile, "w");
	if (resultFile == NULL)
	{
		fprintf(stderr, "%sERROR: Could not create save file.%s\n", COL_RED, RESET);
		return false;
	}

	// Resolve Server Name...
	patchHost = gethostbyname(serverString);
	if (patchHost == NULL)
	{
		fprintf(stderr, "%sERROR: Could not resolve hostname %s.%s\n", COL_RED, serverString, RESET);
		fclose(resultFile);
		return false;
	}

	// Configure Server Address and Port
	serverAddress.sin_family = patchHost->h_addrtype;
	memcpy((char *) &serverAddress.sin_addr.s_addr, patchHost->h_addr_list[0], patchHost->h_length);
	serverAddress.sin_port = htons(80);

	// Create Socket
	socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if(socketDescriptor < 0)
	{
		fprintf(stderr, "%sERROR: Could not open a socket.%s\n", COL_RED, RESET);
		fclose(resultFile);
		return false;
	}

	// Configure Local Port
	localAddress.sin_family = AF_INET;
	localAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddress.sin_port = htons(0);
	status = bind(socketDescriptor, (struct sockaddr *) &localAddress, sizeof(localAddress));
	if(status < 0)
	{
		fprintf(stderr, "%sERROR: Could not bind to port.%s\n", COL_RED, RESET);
		fclose(resultFile);
		return false;
	}

	// Connect
	alarm(8);
	status = connect(socketDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
	alarm(0);
	if(status < 0)
	{
		fprintf(stderr, "%sERROR: Could not open a connection to %s.%s\n", COL_RED, serverString, RESET);
		fclose(resultFile);
		return false;
	}

	// Reserve and create send string...
	sendString = malloc(strlen(requestString) + 78);
	memset(sendString, 0, strlen(requestString) + 78);
	sprintf(sendString, "GET %s HTTP/1.0\r\nUser-Agent: Viewstate\r\nAccept: */*\r\nConnection: Keep-Alive\r\n\r\n", requestString);

	// Send request
	status = send(socketDescriptor, sendString, strlen(sendString) + 1, 0);
	if (status < 0)
	{
		free (sendString);
		close(socketDescriptor);
		fclose(resultFile);
		fprintf(stderr, "%sERROR: Could not open send request.%s\n", COL_RED, RESET);
		return false;
	}
	free (sendString);

	// Recieve Data
	memset(tempString ,0 , sizeof(tempString));
	while (recv(socketDescriptor, tempString, sizeof(tempString) -1, 0) > 0)
	{
		fprintf(resultFile, "%s", tempString);
		memset(tempString ,0 , sizeof(tempString));
	}

	// Cleanup...
	fclose(resultFile);
	close(socketDescriptor);
	return true;
}
#endif

