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

typedef size_t Boolean;
typedef size_t UInteger;
#define false 0
#define true  1

 /* option parser configuration */
static struct option longOpts[] = {
  {"trace", no_argument, 0, 't'},
  {"quite", no_argument, 0, 'q'},
  {"debug", no_argument, 0, 'd'},
  {"help",  no_argument, 0, 'h'},
  {0, 0, 0, 0}
};
 /* getopt_long stores the option index here. */
static int optId = 0;

static void optionHelp(const char* progName) {
  fprintf(stderr, "Usage: %s [options] [file...]\n\n", progName);
  fprintf(stderr, "options:\n");
  fprintf(stderr, "  -d --debug\n");
  fprintf(stderr, "  -t --trace\n");
  fprintf(stderr, "  -q --quiet\n\n");
  fprintf(stderr, "files to load:\n");
  fprintf(stderr, "  Any remaining options are treated as files to be loaded.\n");

  exit(0);
}

int main(int argc, char* argv[]) {
  int opt = 0;
  Boolean debug     = false;
  Boolean tracingOn = false;

  while ((opt = getopt_long(argc, argv,
                            "dtqh",
                            longOpts, &optId)) != -1) {
    switch (opt) {
      case 'd': debug      = true; break;
      case 't': tracingOn  = true; break;
      case 'q':
        tracingOn          = false;
        break;
      case 'h':
      default:
        optionHelp(argv[0]);
    }
  }

  printf("Hello world!\n");
  if (tracingOn) { }
  if (debug)     { }
}
 