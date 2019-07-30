/*
 * lmsDMUCSrun.c
 *
 * Runs a command on a host obtained from a DMUCS server using ssh. 
 *
 * MIT License
 *
 * Copyright (c) 2019 PerceptiSys Ltd (Stephen Gaito)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions: 
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE. 
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
//#include <sys/types.h>
#include <netdb.h>

#define bufferLen 1024

typedef size_t Boolean;
typedef size_t UInteger;
#define false 0
#define true  1

//////////////////////////////////////////////
// Start by handling the options

 /* option parser configuration */
static struct option longOpts[] = {
  {"server",  required_argument, 0, 's'},
  {"port",    required_argument, 0, 'p'},
  {"type",    required_argument, 0, 't'},
  {"logFile", required_argument, 0, 'l'},
  {"debug",   no_argument,       0, 'd'},
  {"help",    no_argument,       0, 'h'},
  {0, 0, 0, 0}
};
 /* getopt_long stores the option index here. */
static int optId = 0;

static void optionHelp(const char* progName) {
  fprintf(stderr, "Usage: %s [options] [file...]\n\n", progName);
  fprintf(stderr, "options:\n");
  fprintf(stderr, "  -s <server>       : the machine name of the DMUCS server\n");
  fprintf(stderr, "  --server <server>\n\n");
  fprintf(stderr, "  -p <port>         : the port used by the DMUCS server\n");
  fprintf(stderr, "  --port <port>\n\n");
  fprintf(stderr, "  -t <type>         : the type of compile machine to request\n");
  fprintf(stderr, "  --type <type>       from the DMUCS server\n\n");
  fprintf(stderr, "  -l <path>         : a path to a log file into which all output\n");
  fprintf(stderr, "  --logFile <path>    (both stdout and stderr) will be collected\n\n");
  fprintf(stderr, "  -d                : provide a running commentary of what\n");
  fprintf(stderr, "  --debug             we are doing\n\n");
  fprintf(stderr, "files to load:\n");
  fprintf(stderr, "  Any remaining options are treated as files to be loaded.\n");

  exit(0);
}

int main(int argc, char* argv[]) {
  int         opt           = 0;
  const char* dmucsHostName = "localhost";
  uint32_t    dmucsPort     = 9714;
  const char* logFile       = NULL;
  const char* machineType   = "";
  const char* remoteUser    = "";
  Boolean     debug         = false;

  const char*           dmucsHostNameEnv = getenv("DMUCS_SERVER");
  if (dmucsHostNameEnv) dmucsHostName    = dmucsHostNameEnv;
  const char*           dmucsPortENV     = getenv("DMUCS_PORT");
  if (dmucsPortENV)     dmucsPort        = atol(dmucsPortENV);
  const char*           remoteUserEnv    = getenv("USER");
  if (remoteUserEnv)    remoteUser       = remoteUserEnv;
  const char*           logFileEnv       = getenv("LMS_LOG_FILE");
  if (logFileEnv)       logFile          = logFileEnv;
  
  while ((opt = getopt_long(argc, argv,
                            "s:p:t:l:dh",
                            longOpts, &optId)) != -1) {
    switch (opt) {
      case 's': dmucsHostName = optarg;       break;
      case 'p': dmucsPort     = atol(optarg); break;
      case 't': machineType   = optarg;       break;
      case 'u': remoteUser    = optarg;       break;
      case 'l': logFile       = optarg;       break;
      case 'd': debug         = true;         break;
      case 'h':
      case '?':
      default:
        optionHelp(argv[0]);
    }
  }

  if (debug) {
    printf("lms DMUCS command runner (v0.0)\n\n");
    printf(" DMUCS server: [%s]\n", dmucsHostName);
    printf("   DMUCS port: [%d]\n", dmucsPort);
    printf(" machine type: [%s]\n", machineType);
    printf("  remote user: [%s]\n", remoteUser);
    printf("log file path: [%s]\n", logFile);
    printf("\ncommand: [");
    for(int i = optind; i < argc; i++) {
      if (optind < i) printf(" ");
      printf("%s", argv[i]);
    }
    printf("]\n");
    printf("\n");
  }

  if (argc <= optind) {
    fprintf(stderr, "no command found... doing nothing!\n");
    exit(-1);
  }  

  //////////////////////////////////////////////
  // Now connect to the DMUCS server
  
  int dmucsSocketFD = 0;
  if((dmucsSocketFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "Could not open a tcp socket to the DMUCS server\n");
    exit(-1);
  }
  
  printf("dmucsSocketFD: [%d]\n", dmucsSocketFD);
  
  struct sockaddr_in dmucsAddress;
  memset(&dmucsAddress, 0, sizeof(dmucsAddress));
  dmucsAddress.sin_family = AF_INET;
  dmucsAddress.sin_port   = htons(dmucsPort);

  struct hostent *dmucsHostEntity = gethostbyname(dmucsHostName);
  if (dmucsHostEntity == NULL) {
    fprintf(stderr, "Could not get the DMUCS host IP entity information");
    close(dmucsSocketFD);
    exit(-1);
  }
  
  printf("dmucsHost: [%s]\n", dmucsHostEntity->h_name);
  dmucsAddress.sin_addr = * ((struct in_addr *) dmucsHostEntity->h_addr);
  
  if (connect(
        dmucsSocketFD,
        (struct sockaddr *)&dmucsAddress,
        sizeof(dmucsAddress)
      ) < 0) {
    fprintf(
      stderr,
      "Could not connect to the DMUCS server: %s\n",
      strerror(errno)
    );
    close(dmucsSocketFD);
    exit(-1);
  }
  
  //////////////////////////////////////////////
  // Now get our hostname

  char hostName[bufferLen];
  if (gethostname(hostName, bufferLen)) {
    fprintf(stderr, "Could not get our host name\n");
    close(dmucsSocketFD);
    exit(-1);
  }

  printf("client hostName: [%s]\n", hostName);

  struct hostent *clientHostEntity = gethostbyname(hostName);
  if (clientHostEntity == NULL) {
    fprintf(stderr, "Could not get our host IP entity information");
    close(dmucsSocketFD);
    exit(-1);
  }
  printf("client host: [%s]\n", clientHostEntity->h_name);
  
  struct in_addr clientHostIP;
  memcpy(&clientHostIP, clientHostEntity->h_addr, sizeof(clientHostIP));
  
  //////////////////////////////////////////////
  // Now build and send the DMUCS host request
  
  char hostRequest[bufferLen];
  memset(hostRequest, 0, bufferLen);
  
  strncat(hostRequest, "host ", 5);
  strncat(hostRequest, inet_ntoa(clientHostIP), 16);
  strncat(hostRequest, " ", 1);
  strncat(hostRequest, machineType, 900);
  
  printf("DMUCS host request: [%s]\n", hostRequest);

  if (write(dmucsSocketFD, hostRequest, strlen(hostRequest)+1) < 0) {
    fprintf(
      stderr,
      "Could not send hostRequest to DMUCS server: %s\n",
      strerror(errno)
    );
    close(dmucsSocketFD);
    exit(-1);
  }
  
  char hostResponse[bufferLen];
  memset(hostResponse, 0, bufferLen);
  
  if (read(dmucsSocketFD, hostResponse, bufferLen-1) < 0) {
    fprintf(
      stderr,
      "Could not read hostResponse from DMUCS server: %s\n",
      strerror(errno)
    );
    close(dmucsSocketFD);
    exit(-1);
  }
  
  printf("DMUCS host response: [%s]\n", hostResponse);
  
  close(dmucsSocketFD);
}
 
