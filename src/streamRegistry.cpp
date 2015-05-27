#include <stdlib.h>

#include "streamRegistry.h"

#ifndef STREAM_REGISTRY_INCREMENT
#define STREAM_REGISTRY_INCREMENT 10
#endif

StreamRegistry::StreamRegistry(void) {
  streams            = NULL;
  nextStreamToAdd    = 0;
  numPossibleStreams = 0;
}

StreamRegistry::~StreamRegistry(void) {
  if (streams) {
    for (size_t i = 0; i < nextStreamToAdd; i++) {
      if (streams[i]) delete streams[i];
      streams[i] = NULL;
    }
    free(streams);
    streams = NULL;
  }
  nextStreamToAdd    = 0;
  numPossibleStreams = 0;
}

void StreamRegistry::addStream(const char *someUtf8Chars,
                               Utf8Chars::Ownership ownership) {
  addStream(new Utf8Chars(someUtf8Chars, ownership));
}

void StreamRegistry::addStream(Utf8Chars *someUtf8Chars) {

  if (numPossibleStreams <= nextStreamToAdd) {
    // we need to add more stream pointers to our array of streams
    Utf8Chars **oldStreams = streams;
    streams = (Utf8Chars**)calloc(numPossibleStreams +
                                    STREAM_REGISTRY_INCREMENT,
                                  sizeof(Utf8Chars*));
    if (oldStreams) {
      memcpy(streams, oldStreams, numPossibleStreams*sizeof(Utf8Chars*));
      free(oldStreams);
    }
    numPossibleStreams += STREAM_REGISTRY_INCREMENT;
  }
  streams[nextStreamToAdd] = someUtf8Chars;
  nextStreamToAdd++;
}
