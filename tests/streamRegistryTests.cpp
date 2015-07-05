#include <string.h>
#include <stdio.h>
#include <exception>

#include <cUtils/specs/specs.h>

#ifndef protected
#define protected public
#endif

#include <dynUtf8Parser/streamRegistry.h>

/// \brief We test the correctness of the StreamRegistry class.
describe(StreamRegistry) {

  specSize(StreamRegistry);

  it("Should be able to create a stream registry") {
    StreamRegistry *someStreams = new StreamRegistry();
    shouldNotBeNULL(someStreams);
    shouldBeZero(someStreams->streams.getNumItems());
    delete someStreams;
  } endIt();

  it("Should be able to add streams to the registry") {
    StreamRegistry *someStreams = new StreamRegistry();
    shouldNotBeNULL(someStreams);
    const char* cString0 = "silly";
    someStreams->addStream(cString0);
    shouldBeEqual(someStreams->streams.getNumItems(), 1);
    shouldBeEqual(someStreams->streams.getItem(0, NULL)->utf8Chars, cString0);
    shouldBeFalse(someStreams->streams.getItem(0, NULL)->ownsString);
    someStreams->addStream(cString0, Utf8Chars::Duplicate);
    shouldBeEqual(someStreams->streams.getNumItems(), 2);
    shouldNotBeEqual((void*)(someStreams->streams.getItem(1, NULL)->utf8Chars),
      (void*)cString0);
    shouldBeTrue(someStreams->streams.getItem(1, NULL)->ownsString);
    char *cString1 = strdup("sillier");
    someStreams->addStream(cString1, Utf8Chars::TakeOwnership);
    shouldBeEqual(someStreams->streams.getNumItems(), 3);
    shouldBeEqual(someStreams->streams.getItem(2, NULL)->utf8Chars, cString1);
    shouldBeTrue(someStreams->streams.getItem(2, NULL)->ownsString);
    const char* cString2 = "silliest";
    Utf8Chars *aUtf8Char = new Utf8Chars(cString2);
    someStreams->addStream(aUtf8Char);
    shouldBeEqual(someStreams->streams.getNumItems(), 4);
    shouldBeEqual(someStreams->streams.getItem(3, NULL), aUtf8Char);
    shouldBeEqual(someStreams->streams.getItem(3, NULL)->utf8Chars, cString2);
    shouldBeFalse(someStreams->streams.getItem(3, NULL)->ownsString);
    delete someStreams;
  } endIt();

  it("should be able to add lots of streams") {
    Utf8Chars *prevChars[30];
    StreamRegistry *someStreams = new StreamRegistry();
    shouldNotBeNULL(someStreams);
    shouldBeZero(someStreams->streams.getNumItems());
    for (size_t i = 0; i < 25; i++) {
      someStreams->addStream("silly");
      shouldBeEqual(someStreams->streams.getNumItems(), i+1);
      prevChars[i] = someStreams->streams.getItem(i, NULL);
      for (size_t j = 0; j < i; j++) {
        shouldBeEqual(someStreams->streams.getItem(j, NULL), prevChars[j]);
      }
    }
    delete someStreams;
  } endIt();

} endDescribe(StreamRegistry);


