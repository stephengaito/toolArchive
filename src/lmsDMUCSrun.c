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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define bufferLen 1024
#define noHostAvailable "0.0.0.0"

#define DEBUG(frmt, ...)    \
if (debug) printf(frmt, ##__VA_ARGS__)

#define VERBOSE(frmt, ...)    \
if (verbose) printf(frmt, ##__VA_ARGS__)

#define ERROREXIT(frmt, ...)                    \
{                                               \
  fprintf(stderr, frmt, ##__VA_ARGS__);         \
  if (-1 < dmucsSocketFD) close(dmucsSocketFD); \
  exit(-1);                                     \
}

typedef size_t Boolean;
typedef size_t UInteger;
#define false 0
#define true  1

//////////////////////////////////////////////
// Start by handling the options

const char* shortOpts = "s:p:t:u:l:m:r:vdh";

 /* option parser configuration */
static struct option longOpts[] = {
  {"server",     required_argument, 0, 's'},
  {"port",       required_argument, 0, 'p'},
  {"type",       required_argument, 0, 't'},
  {"user",       required_argument, 0, 'u'},
  {"logFile",    required_argument, 0, 'l'},
  {"maxRetries", required_argument, 0, 'm'},
  {"retrySleep", required_argument, 0, 'r'},
  {"verbose",    no_argument,       0, 'v'},
  {"debug",      no_argument,       0, 'd'},
  {"help",       no_argument,       0, 'h'},
  {0, 0, 0, 0}
};
 /* getopt_long stores the option index here. */
static int optId = 0;

static void optionHelp(const char* progName) {
  fprintf(stderr, "Usage: lmsDMUCSrun [options] [compile command to be run ...]\n\n");
  fprintf(stderr, "options:\n");
  fprintf(stderr, "  -s <server>              : the machine name of the DMUCS server\n");
  fprintf(stderr, "  --server <server>\n\n");
  fprintf(stderr, "  -p <port>                : the port used by the DMUCS server\n");
  fprintf(stderr, "  --port <port>\n\n");
  fprintf(stderr, "  -t <type>                : the type of compile machine to request\n");
  fprintf(stderr, "  --type <type>              from the DMUCS server\n\n");
  fprintf(stderr, "  -u <remoteUser>          : the remote user used to run the compile command\n");
  fprintf(stderr, "  -remoteUser <remoteUser>\n\n");
  fprintf(stderr, "  -l <path>                : a path to a log file into which all output\n");
  fprintf(stderr, "  --logFile <path>           (both stdout and stderr) will be collected\n\n");
  fprintf(stderr, "  -m <maxRetries>          : the maximum number of DMUCS host request retries\n");
  fprintf(stderr, "  -maxRetries <maxRetries>\n\n");
  fprintf(stderr, "  -r <retrySleep>          : the number of seconds to sleep between DMUCS\n");
  fprintf(stderr, "  -retrySleep <retrySleep>   host request retries\n\n");
  fprintf(stderr, "  -v                       : provide a running commentary of what\n");
  fprintf(stderr, "  --verbose                    we are doing\n\n");
  fprintf(stderr, "  -d                       : provide a low-level running commentary of what\n");
  fprintf(stderr, "  --debug                    we are doing\n\n");
  fprintf(stderr, "  -h                       : this help description\n");
  fprintf(stderr, "  --help\n\n");
  fprintf(stderr, "compile command to be run:\n");
  fprintf(stderr, "  Any remaining options are treated as the compile command to be run.\n\n");
  fprintf(stderr, "  NOTE: that ALL lmsDMUCSrun specific options\n");
  fprintf(stderr, "        MUST come BEFORE the command to be run.\n");

  exit(0);
}

int main(int argc, char* argv[]) {
  int         opt                        = 0;
  const char* dmucsHostName              = "localhost";
  uint32_t    dmucsPort                  = 9714;
  const char* logFile                    = NULL;
  const char* machineType                = "";
  const char* remoteUser                 = "";
  Boolean     verbose                    = false;
  Boolean     debug                      = false;
  int         dmucsHostRequestRetrySleep = 2;
  int         dmucsHostRequestRetryMax   = 1000;
  int         dmucsSocketFD              = -1;

  const char*           dmucsHostNameEnv = getenv("DMUCS_SERVER");
  if (dmucsHostNameEnv) dmucsHostName    = dmucsHostNameEnv;
  const char*           dmucsPortENV     = getenv("DMUCS_PORT");
  if (dmucsPortENV)     dmucsPort        = atol(dmucsPortENV);
  const char*           remoteUserEnv    = getenv("USER");
  if (remoteUserEnv)    remoteUser       = remoteUserEnv;
  const char*           logFileEnv       = getenv("LMS_LOG_FILE");
  if (logFileEnv)       logFile          = logFileEnv;
  
  while ((opt = getopt_long(argc, argv, shortOpts, longOpts, &optId)) != -1) {
    switch (opt) {
      case 's': dmucsHostName              = optarg;       break;
      case 'p': dmucsPort                  = atol(optarg); break;
      case 't': machineType                = optarg;       break;
      case 'u': remoteUser                 = optarg;       break;
      case 'l': logFile                    = optarg;       break;
      case 'm': dmucsHostRequestRetryMax   = atol(optarg); break;
      case 'r': dmucsHostRequestRetrySleep = atol(optarg); break;
      case 'v': verbose                    = true;         break;
      case 'd': debug                      = true;
                verbose                    = true;         break;
      case 'h':
      case '?':
      default:
        optionHelp(argv[0]);
    }
  }

  //////////////////////////////////////////////
  // Now we want to redirect all output (stdout/stderr)
  // IF the user has specified a logFile
  
  if (logFile) {
    VERBOSE("lmsDMUCSrun: redirecting all output to [%s]\n", logFile);
    int logFileFD = open(logFile, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    if (logFileFD < 0) {
      ERROREXIT(
        "lmsDMUCSrun: Could not redirect output to [%s] error: %s\n",
        logFile, strerror(errno)
      );
    }
    dup2(logFileFD, 1); // redirect stdout
    dup2(logFileFD, 2); // redirect stderr
    VERBOSE("lmsDMUCSrun: redirected all output to [%s]\n", logFile);
  }

  
  if (verbose) {
    printf("lmsDMUCSrun: lms DMUCS command runner (v0.0)\n\n");
    printf("lmsDMUCSrun:  DMUCS server: [%s]\n", dmucsHostName);
    printf("lmsDMUCSrun:    DMUCS port: [%d]\n", dmucsPort);
    printf("lmsDMUCSrun:  machine type: [%s]\n", machineType);
    printf("lmsDMUCSrun:   remote user: [%s]\n", remoteUser);
    printf("lmsDMUCSrun: log file path: [%s]\n", logFile);
    printf("lmsDMUCSrun:   max retries: [%d]\n", dmucsHostRequestRetryMax);
    printf("lmsDMUCSrun:   retry sleep: [%d]\n", dmucsHostRequestRetrySleep);
    printf("\nlmsDMUCSrun: command: [");
    for(int i = optind; i < argc; i++) {
      if (optind < i) printf(" ");
      printf("%s", argv[i]);
    }
    printf("]\n");
    printf("\n");
  }

  if (argc <= optind) {
    ERROREXIT("lmsDMUCSrun: no command found... doing nothing!\n");
  }  

  //////////////////////////////////////////////
  // Now connect to the DMUCS server
  
  if((dmucsSocketFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    ERROREXIT("lmsDMUCSrun: Could not open a tcp socket to the DMUCS server\n");
  }
  
  DEBUG("lmsDMUCSrun: dmucsSocketFD: [%d]\n", dmucsSocketFD);
  
  struct sockaddr_in dmucsAddress;
  memset(&dmucsAddress, 0, sizeof(dmucsAddress));
  dmucsAddress.sin_family = AF_INET;
  dmucsAddress.sin_port   = htons(dmucsPort);

  struct hostent *dmucsHostEntity = gethostbyname(dmucsHostName);
  if (dmucsHostEntity == NULL) {
    close(dmucsSocketFD);
    ERROREXIT("lmsDMUCSrun: Could not get the DMUCS host IP entity information");
  }
  
  DEBUG("lmsDMUCSrun: dmucsHost: [%s]\n", dmucsHostEntity->h_name);
  dmucsAddress.sin_addr = * ((struct in_addr *) dmucsHostEntity->h_addr);
  
  if (connect(
        dmucsSocketFD,
        (struct sockaddr *)&dmucsAddress,
        sizeof(dmucsAddress)
      ) < 0) {
    ERROREXIT(
      "lmsDMUCSrun: Could not connect to the DMUCS server: %s\n",
      strerror(errno)
    );
  }
  
  //////////////////////////////////////////////
  // Now get our hostname

  char clientHostName[bufferLen];
  if (gethostname(clientHostName, bufferLen)) {
    ERROREXIT("lmsDMUCSrun: Could not get our host name\n");
  }

  DEBUG("lmsDMUCSrun: client host name: [%s]\n", clientHostName);

  struct hostent *clientHostEntity = gethostbyname(clientHostName);
  if (clientHostEntity == NULL) {
    ERROREXIT("lmsDMUCSrun: Could not get our host IP entity information");
  }
  DEBUG("lmsDMUCSrun: client host: [%s]\n", clientHostEntity->h_name);
  
  struct in_addr clientHostIP;
  memcpy(&clientHostIP, clientHostEntity->h_addr, sizeof(clientHostIP));
  char clientHostIPv4Addr[bufferLen];
  memset(&clientHostIPv4Addr, 0, bufferLen);
  strncat(clientHostIPv4Addr, inet_ntoa(clientHostIP), 16);
  
  DEBUG("lmsDMUCSrun: Client Host IPv4 addr: [%s]\n", clientHostIPv4Addr);
  
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
  
  DEBUG("lmsDMUCSrun: DMUCS host request: [%s]\n", dmucsHostRequest);

  for(int try = dmucsHostRequestRetryMax; 0 < try; try--) {
    if (write(dmucsSocketFD, dmucsHostRequest, strlen(dmucsHostRequest)+1) < 0) {
      ERROREXIT(
        "lmsDMUCSrun: Could not send dmucsHostRequest to DMUCS server: %s\n",
        strerror(errno)
      );
    }
  
    memset(dmucsHostResponse, 0, bufferLen);
  
    if (read(dmucsSocketFD, dmucsHostResponse, bufferLen-1) < 0) {
      ERROREXIT(
        "lmsDMUCSrun: Could not read dmucsHostResponse from DMUCS server: %s\n",
        strerror(errno)
      );
    }
  
    DEBUG("lmsDMUCSrun: DMUCS host response: [%s]\n", dmucsHostResponse);
    
    if (strncmp(dmucsHostResponse, noHostAvailable, strlen(noHostAvailable)) == 0){
      DEBUG("lmsDMUCSrun: DMUCS has no available hosts... trying %d more times\n", try);
    } else {
      break;
    }
    sleep(dmucsHostRequestRetrySleep);
  }
  
  if (strncmp(dmucsHostResponse, noHostAvailable, strlen(noHostAvailable)) == 0) {
    ERROREXIT("lmsDMUCSrun: DMUCS has no available hosts\n");
  }
  
  VERBOSE("lmsDMUCSrun: DMUCS has assigned us the host: [%s]\n", dmucsHostResponse);
  
  struct in_addr compileServerAddr;
  inet_pton(AF_INET, dmucsHostResponse, &compileServerAddr);
  struct hostent *compileServerEntity =
    gethostbyaddr(&compileServerAddr, sizeof(compileServerAddr), AF_INET);

  VERBOSE("lmsDMUCSrun: Compile server host name: [%s]\n", compileServerEntity->h_name);

  //////////////////////////////////////////////
  // Now fork to a child process...
  //
  // ... in the child...  exec the command (possibly using ssh)
  // ... in the parent... wait for the child (and return the child's status)

  int childStatus = 0;
  int forkedPID = fork();
  if (forkedPID == 0) {
    // child process
    DEBUG("lmsDMUCSrun: Hello from the child!\n");
    DEBUG(
      "lmsDMUCSrun: hostname: [%s] dmucsHostResponse: [%s]\n", 
      clientHostIPv4Addr, dmucsHostResponse
    );

    const char *cmdArgs[argc+10];
    int cmdArgNum = 0;
    memset(cmdArgs, 0, sizeof(cmdArgs));
    
    if ((strncmp(clientHostIPv4Addr, dmucsHostResponse, bufferLen-1) == 0) &&
        (strncmp(remoteUser, getenv("USER"), strlen(remoteUser)) == 0)){
      // we are calling a process on the same machine
      // with the same "remoteUser" as the system "USER"
      // so we do not need to use ssh
    } else {
      // we are calling a process on a different machine
      // so we need to use ssh
      cmdArgs[cmdArgNum++] = "ssh";
      char sshUserHost[bufferLen];
      memset(sshUserHost, 0, bufferLen);
      strncat(sshUserHost, remoteUser, strlen(remoteUser));
      strncat(sshUserHost, "@", 1);
      strncat(
        sshUserHost,
        compileServerEntity->h_name,
        strlen(compileServerEntity->h_name)
      );
      cmdArgs[cmdArgNum++] = sshUserHost;
    }
    for(int i = optind; i < argc; i++) {
      cmdArgs[cmdArgNum++] = argv[i];
    }

    if (verbose) {
      printf("lmsDMUCSrun: exec'ing the command: [");
      for(int i = 0; i < cmdArgNum; i++) {
        printf("%s ", cmdArgs[i]);
      }
      printf("]\n");
      printf("----------------------------------------------------\n");
    }

    if (execvp(cmdArgs[0], (char**)cmdArgs) < 0) {
      dmucsSocketFD = -1; // the child should NOT close the dmucs socket!
      ERROREXIT("lmsDMUCSrun: execvp failed: %s\n", strerror(errno));
    }

    DEBUG("lmsDMUCSrun: Goodbye from the child!\n"); // we should never reach here!!
    exit(0);
  } else {
    // parent process
    DEBUG("lmsDMUCSrun: Hello from the parent!\n");
    wait(&childStatus);
    VERBOSE("----------------------------------------------------\n");
    DEBUG("lmsDMUCSrun: Finished waiting for the child process!\n");
  }
  
  //////////////////////////////////////////////
  // Now close the connection to the DMUCS
  // (this allows the DMUCS server to reassign
  //  this host to another compile task) 

  if (-1 < dmucsSocketFD) close(dmucsSocketFD);
  
  //////////////////////////////////////////////
  // Now return the child process' status
  
  int parentStatus = -1;
  if (WIFEXITED(childStatus)) parentStatus = WEXITSTATUS(childStatus);
  
  VERBOSE("lmsDMUCSrun: Exit status: %d\n", parentStatus);
  
  exit(parentStatus);
}
 
