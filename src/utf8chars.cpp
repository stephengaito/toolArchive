#include <string.h>
#include <stdbool.h>

#include "utf8chars.h"

//
// This list of whitespace has been taken from
// [In Python, how to listall characters matched by POSIX extended regex
// `[:space:]`?](http://stackoverflow.com/a/8922773)
//
const char Utf8Chars::whiteSpaceChars[] = {
  0x09, // 0x0009 tab
  0x0A, // 0x000A new line
  0x0B, // 0x000B vertical tab
  0x0C, // 0x000C new page
  0x0D, // 0x000D carrige return
  0x1C, // 0x001C file separator
  0x1D, // 0x001D group separator
  0x1E, // 0x001E record separator
  0x1F, // 0x001F unit separator
  0x20, // 0x0020 SPACE
  0xC2, 0x85, // 0x0085 ?
  0xC2, 0xA0, // 0x00A0 NO-BREAK SPACE
  0xE1, 0x9A, 0x80, // 0x1680 OGHAM SPACE MARK
  0xE1, 0xA0, 0x8E, // 0x180E MONGOLIAN VOWEL SEPARATOR
  0xE2, 0x80, 0x80, // 0x2000 EN QUAD
  0xE2, 0x80, 0x81, // 0x2001 EM QUAD
  0xE2, 0x80, 0x82, // 0x2002 EN SPACE
  0xE2, 0x80, 0x83, // 0x2003 EM SPACE
  0xE2, 0x80, 0x84, // 0x2004 THREE-PER-EM SPACE
  0xE2, 0x80, 0x85, // 0x2005 FOUR-PER-EM SPACE
  0xE2, 0x80, 0x86, // 0x2006 SIX-PER-EM SPACE
  0xE2, 0x80, 0x87, // 0x2007 FIGURE SPACE
  0xE2, 0x80, 0x88, // 0x2008 PUNCTUATION SPACE
  0xE2, 0x80, 0x89, // 0x2009 THIN SPACE
  0xE2, 0x80, 0x8A, // 0x200A HAIR SPACE
  0xE2, 0x80, 0xA8, // 0x2028 LINE SEPARATOR
  0xE2, 0x80, 0xA9, // 0x2029 PARAGRAPH SEPARATOR
  0xE2, 0x80, 0xAF, // 0x202F NARROW NO-BREAK SPACE
  0xE2, 0x81, 0x9F, // 0x205F MEDIUM MATHEMATICAL SPACE
  0xE3, 0x80, 0x80, // 0x3000 IDEOGRAPHIC SPACE
  0x00              // null C-string terminator
};

Utf8Chars::Utf8Chars(const char* someUtf8Chars,
                     Utf8Chars::Ownership ownership) {
  switch(ownership) {
    case DoNotOwn:
      ownsString = false;
      utf8Chars = someUtf8Chars;
      break;
    case TakeOwnership:
      ownsString = true;
      utf8Chars = someUtf8Chars;
      break;
    default:
      ownsString = true;
      utf8Chars = strdup(someUtf8Chars);
      break;
  }
  lastByte  = utf8Chars+strlen(utf8Chars);
  restart();
}

Utf8Chars::~Utf8Chars(void) {
  if (utf8Chars && ownsString) free((void*)utf8Chars);
  utf8Chars   = NULL;
  ownsString  = false;
  lastByte    = NULL;
  nextByte    = NULL;
}

void Utf8Chars::restart(void) {
  nextByte    = utf8Chars;
}

// We use the Wikipedia
// [UTF-8::Description](http://en.wikipedia.org/wiki/UTF-8#Description)
//
void Utf8Chars::backup(void) {
  // ensure we have not walked off the end of the string
  if (lastByte < nextByte) nextByte = lastByte;
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
utf8Char_t Utf8Chars::nextUtf8Char(void) {
  utf8Char_t nullChar;
  nullChar.u = 0;

  // check to see if we are in the string
  // if not return the null character
  if (lastByte < nextByte) return nullChar;

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
    if (lastByte < nextByte) return nullChar;
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

bool Utf8Chars::containsUtf8Char(utf8Char_t expectedUtf8Char) {
  restart();
  while( nextByte < lastByte) {
    utf8Char_t actualUtf8Char = nextUtf8Char();
    if (actualUtf8Char.u == 0) return false;
    if (actualUtf8Char.u == expectedUtf8Char.u) return true;
  }
  return false;
}

// We use the Wikipedia
// [UTF-8::Description](http://en.wikipedia.org/wiki/UTF-8#Description)
//
utf8Char_t Utf8Chars::codePoint2utf8Char(uint64_t codePoint) {
  utf8Char_t result;
  result.u = 0;
  if (codePoint < 0x80) {                    // 1 byte char
    result.c[0] = codePoint;                 // just copy across

  } else if (codePoint < 0x800) {            // 2 byte char

    result.c[1] = (codePoint & 0x3F) | 0x80; // extract lowest 6 bits and
                                             // format as per additional byte

    codePoint = codePoint>>6;                // shit off lowest 6 bits

    result.c[0] = (codePoint & 0x1F) | 0xC0; // extract lowest 5 bits and
                                             // format as 2 byte start byte

  } else if (codePoint < 0x10000) {          // 3 byte char
    result.c[2] = (codePoint & 0x3F) | 0x80;
    codePoint = codePoint>>6;
    result.c[1] = (codePoint & 0x3F) | 0x80;
    codePoint = codePoint>>6;
    result.c[0] = (codePoint & 0x0F) | 0xE0; // extract lowest 4 bits and
                                             // format as 3 byte start byte

  } else if (codePoint < 0x200000) {         // 4 byte char
    result.c[3] = (codePoint & 0x3F) | 0x80;
    codePoint = codePoint>>6;
    result.c[2] = (codePoint & 0x3F) | 0x80;
    codePoint = codePoint>>6;
    result.c[1] = (codePoint & 0x3F) | 0x80;
    codePoint = codePoint>>6;
    result.c[0] = (codePoint & 0x07) | 0xF0; // extract lowest 3 bits and
                                             // format as 4 byte start byte

  } else if (codePoint < 0x4000000) {        // 5 byte char
    result.c[4] = (codePoint & 0x3F) | 0x80;
    codePoint = codePoint>>6;
    result.c[3] = (codePoint & 0x3F) | 0x80;
    codePoint = codePoint>>6;
    result.c[2] = (codePoint & 0x3F) | 0x80;
    codePoint = codePoint>>6;
    result.c[1] = (codePoint & 0x3F) | 0x80;
    codePoint = codePoint>>6;
    result.c[0] = (codePoint & 0x03) | 0xF8; // extract lowest 2 bits and
                                             // format as 5 byte start byte

  } else if (codePoint < 0x80000000) {       // 6 byte char
    result.c[5] = (codePoint & 0x3F) | 0x80;
    codePoint = codePoint>>6;
    result.c[4] = (codePoint & 0x3F) | 0x80;
    codePoint = codePoint>>6;
    result.c[3] = (codePoint & 0x3F) | 0x80;
    codePoint = codePoint>>6;
    result.c[2] = (codePoint & 0x3F) | 0x80;
    codePoint = codePoint>>6;
    result.c[1] = (codePoint & 0x3F) | 0x80;
    codePoint = codePoint>>6;
    result.c[0] = (codePoint & 0x01) | 0xFC; // extract lowest bit and
                                             // format as 2 byte start byte
  }
  return result;
}

