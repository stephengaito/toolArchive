#include <string.h>
//#include <stdio.h>
#include <stdbool.h>
#include "lexer.h"


Utf8CharStr::Utf8CharStr(const char* someUtf8Chars) {
  utf8Chars = someUtf8Chars;
  numBytes  = strlen(someUtf8Chars);
  restart();
}

void Utf8CharStr::restart(void) {
  nextByte  = utf8Chars;
}

// We use the Wikipedia
// [UTF-8::Description](http://en.wikipedia.org/wiki/UTF-8#Description)
//
void Utf8CharStr::backup(void) {
  // ensure we have not walked off the end of the string
  if (utf8Chars + numBytes < nextByte) nextByte = utf8Chars + numBytes;
  while(true) {
    // backup one byte
    nextByte--;
    // ensure we have not backed off over the front of the string
    if (nextByte < utf8Chars) {
       restart();
       return;
    }
    // check to see if this is "start" byte
    if ((*nextByte & 0xC0) != 0x80) {
      // this is a start byte... so we are done
      return;
    }
  }
}

// We use the Wikipedia
// [UTF-8::Description](http://en.wikipedia.org/wiki/UTF-8#Description)
//
utf8Char_t Utf8CharStr::nextUtf8Char(void) {
  utf8Char_t nullChar;
  nullChar.u = 0;

  // check to see if we are in the string
  // if not return the null character
  if (utf8Chars+numBytes < nextByte) return nullChar;

  utf8Char_t result;
  result.u = 0;

  // assume that this is a valid character
  // and copy over the first byte
  result.c[0] = *nextByte;

  // now find out how many more bytes need to be copied
  int additionalBytes = 0;
  if ((*nextByte & 0x80) == 0) {
    // this is a one byte ASCII char
  } else if ((*nextByte & 0xE0) == 0xC0) {
    // this is a 2 byte char
    additionalBytes = 1;
  } else if ((*nextByte & 0xF0) == 0xE0) {
    // this is a 3 byte char
    additionalBytes = 2;
  } else if ((*nextByte & 0xF8) == 0xF0) {
    // this is a 4 byte char
    additionalBytes = 3;
  } else if ((*nextByte & 0xFC) == 0xF8) {
    // this is a 5 byte char
    additionalBytes = 4;
  } else if ((*nextByte & 0xFE) == 0xFC) {
    // this is a 6 byte char
    additionalBytes = 5;
  } else {
    // this is a malformed character
    // return the null character
    return nullChar;
  }

  nextByte++;  // move to the next byte
  for(int i = 1; i <= additionalBytes; i++) {
    // check to see if we are still in the string
    // if not return the null character
    if (utf8Chars+numBytes < nextByte) return nullChar;
    // if these additional characters are not of the form 10xxxxxx
    // then this is a malformed utf8 character
    // so return the null character
    if ((*nextByte & 0xC0) != 0x80) return nullChar;
    // copy over this byte and increment the nextByte pointer
    result.c[i] = *nextByte;
    nextByte++;
  }

  return result;
}

Classifier::Classifier(void) {
  // create the className2classSet mapping of characters to HAT-trie value_t
  className2classSet = hattrie_create();

  // create the utf8Char2classSet mapping of characters to HAT-trie value_t
  utf8Char2classSet = hattrie_create();
}

classSet_t Classifier::findClassSet(const char* aClassName) {
  value_t *classSetPtr = hattrie_tryget(className2classSet,
                                        aClassName,
                                        strlen(aClassName));
  if (!classSetPtr) return 0;
  return *classSetPtr;
}

classSet_t Classifier::registerClassSet(const char* aClassName,
                                      uint64_t aClassSet) {
  value_t *classSetPtr = hattrie_get(className2classSet,
                                     aClassName,
                                     strlen(aClassName));
  if (!classSetPtr) return 0;
  classSet_t oldClassSet = *classSetPtr;
  *classSetPtr = aClassSet;
  return oldClassSet;
}

void Classifier::classifyUtf8CharsAs(const char* someUtf8Chars,
                                     const char* aClassName) {

  classSet_t newClassSet = findClassSet(aClassName);

  Utf8CharStr *utf8Chars = new Utf8CharStr(someUtf8Chars);
  utf8Char_t aUtf8Char = utf8Chars->nextUtf8Char();
  while(aUtf8Char.u != 0) {
    classSet_t *classSetPtr = hattrie_get(utf8Char2classSet,
                                       aUtf8Char.c, strlen(aUtf8Char.c));
    if (!classSetPtr) return;
    *classSetPtr = newClassSet;
    aUtf8Char = utf8Chars->nextUtf8Char();
  }
}

classSet_t Classifier::getClassSet(const char* someUtf8Chars) {
  Utf8CharStr *utf8Chars = new Utf8CharStr(someUtf8Chars);
  utf8Char_t aUtf8Char = utf8Chars->nextUtf8Char();
  return getClassSet(aUtf8Char);
}

classSet_t Classifier::getClassSet(utf8Char_t aUtf8Char) {
  classSet_t *classSetPtr = hattrie_tryget(utf8Char2classSet,
                                           aUtf8Char.c, strlen(aUtf8Char.c));

  // if this is an unclassified character return the empty class set
  if (!classSetPtr) return 0;

  return *classSetPtr;
}
