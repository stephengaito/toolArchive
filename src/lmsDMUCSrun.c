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
#include <netdb.h>
#include <sys/wait.h>

#define bufferLen 1024
#define noHostAvailable "0.0.0.0"
#define dmucsHostRequestSleep 10
#define dmucsHostRequestTryMax 10

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

  char clientHostName[bufferLen];
  if (gethostname(clientHostName, bufferLen)) {
    fprintf(stderr, "Could not get our host name\n");
    close(dmucsSocketFD);
    exit(-1);
  }

  printf("client host name: [%s]\n", clientHostName);

  struct hostent *clientHostEntity = gethostbyname(clientHostName);
  if (clientHostEntity == NULL) {
    fprintf(stderr, "Could not get our host IP entity information");
    close(dmucsSocketFD);
    exit(-1);
  }
  printf("client host: [%s]\n", clientHostEntity->h_name);
  
  struct in_addr clientHostIP;
  memcpy(&clientHostIP, clientHostEntity->h_addr, sizeof(clientHostIP));
  char clientHostIPv4Addr[bufferLen];
  memset(&clientHostIPv4Addr, 0, bufferLen);
  strncat(clientHostIPv4Addr, inet_ntoa(clientHostIP), 16);
  
  printf("Client Host IPv4 addr: [%s]\n", clientHostIPv4Addr);
  
  //////////////////////////////////////////////
  // Now build and send the DMUCS host request
  
  char dmucsHostRequest[bufferLen];
  char dmucsHostResponse[bufferLen];
  memset(dmucsHostRequest,  0, bufferLen);
  memset(dmucsHostResponse, 0, bufferLen);
  
  strncat(dmucsHostRequest, "host ", 5);
  strncat(dmucsHostRequest, clientHostIPv4Addr, 16);
  strncat(dmucsHostRequest, " ", 1);
  strncat(dmucsHostRequest, machineType, 900);
  
  printf("DMUCS host request: [%s]\n", dmucsHostRequest);

  for(int try = dmucsHostRequestTryMax; 0 < try; try--) {
    if (write(dmucsSocketFD, dmucsHostRequest, strlen(dmucsHostRequest)+1) < 0) {
      fprintf(
        stderr,
        "Could not send dmucsHostRequest to DMUCS server: %s\n",
        strerror(errno)
      );
      close(dmucsSocketFD);
      exit(-1);
    }
  
    memset(dmucsHostResponse, 0, bufferLen);
  
    if (read(dmucsSocketFD, dmucsHostResponse, bufferLen-1) < 0) {
      fprintf(
        stderr,
        "Could not read dmucsHostResponse from DMUCS server: %s\n",
        strerror(errno)
      );
      close(dmucsSocketFD);
      exit(-1);
    }
  
    printf("DMUCS host response: [%s]\n", dmucsHostResponse);
    
    if (strncmp(dmucsHostResponse, noHostAvailable, strlen(noHostAvailable)) == 0){
      printf("DMUCS has no available hosts... trying %d more times\n", try);
    } else {
      break;
    }
    sleep(dmucsHostRequestSleep);
  }
  
  if (strncmp(dmucsHostResponse, noHostAvailable, strlen(noHostAvailable)) == 0) {
    fprintf(stderr, "DMUCS has no available hosts\n");
    close(dmucsSocketFD);
    exit(-1);
  }
  
  printf("DMUCS has assigned us the host: [%s]\n", dmucsHostResponse);
  
  struct in_addr compileServerAddr;
  inet_pton(AF_INET, dmucsHostResponse, &compileServerAddr);
  struct hostent *compileServerEntity =
    gethostbyaddr(&compileServerAddr, sizeof(compileServerAddr), AF_INET);

  printf("Compile server host name: [%s]\n", compileServerEntity->h_name);

  //////////////////////////////////////////////
  // Now fork to a child process...
  //
  // ... in the child...  exec the command (possibly using ssh)
  // ... in the parent... wait for the child (and return the child's status)

  int childStatus = 0;
  int forkedPID = fork();
  if (forkedPID == 0) {
    // child process
    printf("Hello from the child!\n");
    printf(
      "hostname: [%s] dmucsHostResponse: [%s]\n", 
      clientHostIPv4Addr, dmucsHostResponse
    );

    const char *cmdArgs[argc+10];
    int cmdArgNum = 0;
    memset(cmdArgs, 0, sizeof(cmdArgs));
    
    if (strncmp(clientHostIPv4Addr, dmucsHostResponse, bufferLen-1) == 0) {
      // we are calling a process on the same machine
      // so we do not need to use ssh
    } else {
      // we are calling a process on a different machine
      // so we need to use ssh
      cmdArgs[cmdArgNum++] = "ssh";
      cmdArgs[cmdArgNum++] = compileServerEntity->h_name;
    }
    for(int i = optind; i < argc; i++) {
      cmdArgs[cmdArgNum++] = argv[i];
    }

    printf("exec'ing the command: [");
    for(int i = 0; i < cmdArgNum; i++) {
      printf("%s ", cmdArgs[i]);
    }
    printf("]\n");

    if (execvp(cmdArgs[0], (char**)cmdArgs) < 0) {
      fprintf(stderr, "execvp failed: %s\n", strerror(errno));
      exit(-1);
    }

    printf("Goodbye from the child!\n"); // we should never reach here!!
    exit(0);
  } else {
    // parent process
    fprintf(stderr, "Hello from the parent!\n");
    wait(&childStatus);
    fprintf(stderr, "Finished waiting for the child process!\n");
  }
  
  close(dmucsSocketFD);
  
  int parentStatus = -1;
  if (WIFEXITED(childStatus)) parentStatus = WEXITSTATUS(childStatus);
  
  printf("Exit status: %d\n", parentStatus);
  
  exit(parentStatus);
}
 
