#include <string.h>
#include <stdio.h>

#include <cUtils/specs/specs.h>

#ifndef protected
#define protected public
#endif

#include <dynUtf8Parser/utf8chars.h>

/// \brief Test the Utf8Chars buffered stream.
///
/// The Utf8Chars class provides a buffered stream of utf8Char_t types.
/// These tests verify the required behaviour of this buffered stream of
/// utf8Char_t types.
describe(Utf8Chars) {

  specSize(uint64_t);
  specSize(utf8Char_t);

  /// Simply ensure we can correctly create a new Utf8Chars object
  /// on a standard C-string.
  it("create a Utf8Chars buffer") {
    Utf8Chars *someChars = new Utf8Chars("silly");
    shouldNotBeNULL(someChars);
    shouldBeEqual(someChars->origUtf8Chars, someChars->utf8Chars);
    shouldBeEqual(someChars->nextByte,      someChars->utf8Chars);
    shouldBeFalse(someChars->ownsString);
    delete someChars;
    someChars = new Utf8Chars("silly", Utf8Chars::DoNotOwn);
    shouldNotBeNULL(someChars);
    shouldBeEqual(someChars->origUtf8Chars, someChars->utf8Chars);
    shouldBeEqual(someChars->nextByte,      someChars->utf8Chars);
    shouldBeFalse(someChars->ownsString);
    delete someChars;
  } endIt();

  /// Simply ensure we can correctly create a new Utf8Chars object
  /// on a standard C-string which will be duplicated.
  it("create a Utf8Chars buffer and duplicate string") {
    const char* cString = "silly";
    Utf8Chars *someChars = new Utf8Chars(cString, Utf8Chars::Duplicate);
    shouldNotBeNULL(someChars);
    shouldBeEqual(someChars->nextByte,    someChars->utf8Chars);
    shouldBeTrue(someChars->ownsString);
    shouldBeEqual(someChars->utf8Chars,   cString);
    shouldNotBeEqual((void*)someChars->utf8Chars,   (void*)cString);
    delete someChars;
  } endIt();

  /// Simply ensure we can correctly create a new Utf8Chars object
  /// on a standard C-string which will take over ownership.
  it("create a Utf8Chars buffer and take over ownership") {
    const char* cString = strdup("silly");
    Utf8Chars *someChars = new Utf8Chars(cString, Utf8Chars::TakeOwnership);
    shouldNotBeNULL(someChars);
    shouldBeEqual((void*)someChars->nextByte, (void*)someChars->utf8Chars);
    shouldBeTrue(someChars->ownsString);
    shouldBeEqual((void*)someChars->utf8Chars, (void*)cString);
    delete someChars;
  } endIt();

  it("Show that we can getStart, getNumberBytesRead and copyOfReadText") {
    Utf8Chars *someChars = new Utf8Chars("silly");
    shouldNotBeNULL(someChars);
    shouldBeEqual((void*)someChars->nextByte, (void*)(someChars->utf8Chars));
    utf8Char_t expectedChar;
    expectedChar.u = 0;
    expectedChar.c[0] = 's';
    shouldBeEqual(someChars->nextUtf8Char().u, expectedChar.u);
    expectedChar.c[0] = 'i';
    shouldBeEqual(someChars->nextUtf8Char().u, expectedChar.u);
    expectedChar.c[0] = 'l';
    shouldBeEqual(someChars->nextUtf8Char().u, expectedChar.u);
    expectedChar.c[0] = 'l';
    shouldBeEqual(someChars->nextUtf8Char().u, expectedChar.u);
    const char *someCharsStart = someChars->getStart();
    shouldBeEqual(someCharsStart, someChars->utf8Chars);
    shouldBeEqual(someChars->getNumberOfBytesRead(), 4);
    char *sill = someChars->getCopyOfTextRead();
    shouldNotBeEqual(someChars->utf8Chars,  sill);
    shouldBeEqual(strlen(sill), 4);
    shouldBeEqual(sill[0], 's');
    shouldBeEqual(sill[1], 'i');
    shouldBeEqual(sill[2], 'l');
    shouldBeEqual(sill[3], 'l');
    shouldBeEqual(sill[4], 0);
    free(sill);
    delete someChars;
  } endIt();

  /// Confirm that Utf8Chars::codePoint2utf8Char handles provides the
  /// correct mapping from code points to utf8Chat_t types. In particular,
  /// we check each of the code point boundaries.
  it("convert code points to utf8Char_t") {
    utf8Char_t expectedChar;
    expectedChar.u = 0;
    expectedChar.c[0] = ' ';
    shouldBeEqual(Utf8Chars::codePoint2utf8Char(0x20).u, expectedChar.u);
    expectedChar.u = 0;
    expectedChar.c[0] = 0xDF;
    expectedChar.c[1] = 0xBF;
    shouldBeEqual(Utf8Chars::codePoint2utf8Char(0x7FF).u, expectedChar.u);
    expectedChar.u = 0;
    expectedChar.c[0] = 0xEF;
    expectedChar.c[1] = 0xBF;
    expectedChar.c[2] = 0xBF;
    shouldBeEqual(Utf8Chars::codePoint2utf8Char(0xFFFF).u, expectedChar.u);
    expectedChar.u = 0;
    expectedChar.c[0] = 0xF7;
    expectedChar.c[1] = 0xBF;
    expectedChar.c[2] = 0xBF;
    expectedChar.c[3] = 0xBF;
    shouldBeEqual(Utf8Chars::codePoint2utf8Char(0x1FFFFF).u, expectedChar.u);
    expectedChar.u = 0;
    expectedChar.c[0] = 0xFB;
    expectedChar.c[1] = 0xBF;
    expectedChar.c[2] = 0xBF;
    expectedChar.c[3] = 0xBF;
    expectedChar.c[4] = 0xBF;
    shouldBeEqual(Utf8Chars::codePoint2utf8Char(0x3FFFFFF).u, expectedChar.u);
    expectedChar.u = 0;
    expectedChar.c[0] = 0xFD;
    expectedChar.c[1] = 0xBF;
    expectedChar.c[2] = 0xBF;
    expectedChar.c[3] = 0xBF;
    expectedChar.c[4] = 0xBF;
    expectedChar.c[5] = 0xBF;
    shouldBeEqual(Utf8Chars::codePoint2utf8Char(0x7FFFFFFF).u, expectedChar.u);
    expectedChar.u = 0;
    shouldBeEqual(Utf8Chars::codePoint2utf8Char(0x7FFFFFFFF).u, expectedChar.u);
  } endIt();

  /// Ensure that we can iterate over a *simple* ASCII string of one
  /// charater.
  it("iterate over a simple ascii string") {
    Utf8Chars *someChars = new Utf8Chars("s");
    utf8Char_t expectedChar;
    expectedChar.u    = 0;
    expectedChar.c[0] = 's';
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    shouldBeEqual((someChars->nextUtf8Char()).u, 0);
    someChars->restart();
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    shouldBeEqual((someChars->nextUtf8Char()).u, 0);
    someChars->backup();
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    shouldBeEqual((someChars->nextUtf8Char()).u, 0);
    delete someChars;
  } endIt();

  /// Ensure that we can iterate over a *simple* ASCII string of multiple
  /// charaters.
  it("iterate over a long ascii string") {
    Utf8Chars *someChars = new Utf8Chars("silly");
    utf8Char_t expectedChar;
    expectedChar.u    = 0;
    expectedChar.c[0] = 's';
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    expectedChar.c[0] = 'i';
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    someChars->backup();
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    expectedChar.c[0] = 'l';
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    expectedChar.c[0] = 'l';
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    expectedChar.c[0] = 'y';
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    shouldBeEqual((someChars->nextUtf8Char()).u, 0);
    someChars->restart();
    expectedChar.c[0] = 's';
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    delete someChars;
  } endIt();

  /// Ensure that we can iterate over a string which contains one
  /// UTF8 charater.
  it("iterate over a simple utf8 string") {
    Utf8Chars *someChars = new Utf8Chars("€");
    utf8Char_t expectedChar;
    expectedChar.u    = 0;
    expectedChar.c[0] = 0xE2;
    expectedChar.c[1] = 0x82;
    expectedChar.c[2] = 0xAC;
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    shouldBeEqual((someChars->nextUtf8Char()).u, 0);
    someChars->restart();
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    shouldBeEqual((someChars->nextUtf8Char()).u, 0);
    someChars->backup();
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    shouldBeEqual((someChars->nextUtf8Char()).u, 0);
    delete someChars;
  } endIt();

  /// Ensure that we can iterate over a string which contains multiple
  /// UTF8 charaters.
  it("iterate over a long utf8 string") {
    Utf8Chars *someChars = new Utf8Chars("$¢€[");
    utf8Char_t expectedChar;
    expectedChar.u    = 0;
    expectedChar.c[0] = '$';
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    expectedChar.u    = 0;
    expectedChar.c[0] = 0xC2;
    expectedChar.c[1] = 0xA2;
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    expectedChar.u    = 0;
    expectedChar.c[0] = 0xE2;
    expectedChar.c[1] = 0x82;
    expectedChar.c[2] = 0xAC;
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    someChars->backup();
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    someChars->backup();
    someChars->backup();
    expectedChar.u    = 0;
    expectedChar.c[0] = 0xC2;
    expectedChar.c[1] = 0xA2;
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    expectedChar.u    = 0;
    expectedChar.c[0] = 0xE2;
    expectedChar.c[1] = 0x82;
    expectedChar.c[2] = 0xAC;
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    expectedChar.u    = 0;
    expectedChar.c[0] = '[';
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    shouldBeEqual((someChars->nextUtf8Char()).u, 0);
    someChars->restart();
    expectedChar.u    = 0;
    expectedChar.c[0] = '$';
    shouldBeEqual((someChars->nextUtf8Char()).u, expectedChar.u);
    delete someChars;
  } endIt();

#ifdef NOT_DEFINED

  /// Check that Utf8Chars::nextUtf8Char can handle mallformed
  /// "utf8" byte sequences.
  it("iterate over malformed UTF8 strings") {
    utf8Char_t nullChar;
    nullChar.u = 0;

    utf8Char_t aChar;
    aChar.u = 0;
    aChar.c[0] = 0xFF;
    Utf8Chars *someChars = new Utf8Chars(aChar.c);
    shouldBeEqual(someChars->nextUtf8Char().u, nullChar.u);
    aChar.u = 0;
    aChar.c[0] = 0xC0;
    aChar.c[1] = 0xFF;
    someChars = new Utf8Chars(aChar.c);
    shouldBeEqual(someChars->nextUtf8Char().u, nullChar.u);
    aChar.u = 0;
    aChar.c[0] = 0xC0;
    aChar.c[1] = 0x00;
    someChars = new Utf8Chars(aChar.c);
    shouldBeEqual(someChars->nextUtf8Char().u, nullChar.u);
    aChar.u = 0;
    aChar.c[0] = 0xC0;
    aChar.c[1] = 0x80;
    someChars = new Utf8Chars(aChar.c);
    shouldBeEqual(someChars->nextUtf8Char().u, aChar.u);
    delete someChars;
  } endIt();

#endif

  /// Ensure that the Utf8Chars::whiteSpaceChars constant contains
  /// the white space UTF8 characters we are expecting.
  ///
  /// This test also contains code (normally commeted out) to produce
  /// the C code required to provide the Utf8Chars::whiteSpaceChars
  /// structure.
  it("whiteSpaceChars contains UTF8 white space characters") {
/*
    //
    // This list of whitespace has been taken from
    // [In Python, how to listall characters matched by POSIX extended regex
    // `[:space:]`?](http://stackoverflow.com/a/8922773)
    //
    typedef struct codePoint_struct {
      uint64_t    cp;
      const char* comment;
    } codePoint_t;
    codePoint_t whiteSpaceCodePoints[] = {
      {'\t', "tab"},
      {'\n', "new line"},
      {0x0B, "vertical tab"},
      {0x0C, "new page"},
      {'\r', "carrige return"},
      {0x1C, "file separator"},
      {0x1D, "group separator"},
      {0x1E, "record separator"},
      {0x1F, "unit separator"},
      {' ',  "SPACE"},
      {0x85, "?"},
      {0xA0, "NO-BREAK SPACE"},
      {0x1680, "OGHAM SPACE MARK"},
      {0x180e, "MONGOLIAN VOWEL SEPARATOR"},
      {0x2000, "EN QUAD"},
      {0x2001, "EM QUAD"},
      {0x2002, "EN SPACE"},
      {0x2003, "EM SPACE"},
      {0x2004, "THREE-PER-EM SPACE"},
      {0x2005, "FOUR-PER-EM SPACE"},
      {0x2006, "SIX-PER-EM SPACE"},
      {0x2007, "FIGURE SPACE"},
      {0x2008, "PUNCTUATION SPACE"},
      {0x2009, "THIN SPACE"},
      {0x200a, "HAIR SPACE"},
      {0x2028, "LINE SEPARATOR"},
      {0x2029, "PARAGRAPH SEPARATOR"},
      {0x202f, "NARROW NO-BREAK SPACE"},
      {0x205f, "MEDIUM MATHEMATICAL SPACE"},
      {0x3000,  "IDEOGRAPHIC SPACE"},
      {0, ""}
    };
    fprintf(stdout, "\nWhite space codePoint -> UTF8 character table:\n");
    utf8Char_t utf8Char;
    for(int i = 0; 0 < whiteSpaceCodePoints[i].cp; i++) {
      utf8Char = Utf8Chars::codePoint2utf8Char(whiteSpaceCodePoints[i].cp);
      fprintf(stdout, "  0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, // 0x%04lX %s\n",
        utf8Char.c[0] & 0xFF, utf8Char.c[1] & 0xFF, utf8Char.c[2] & 0xFF,
        utf8Char.c[3] & 0xFF, utf8Char.c[4] & 0xFF, utf8Char.c[5] & 0xFF,
        whiteSpaceCodePoints[i].cp, whiteSpaceCodePoints[i].comment
      );
    }
*/
    Utf8Chars *whiteSpace = new Utf8Chars(Utf8Chars::whiteSpaceChars);
    utf8Char_t expectedChar;
    expectedChar.u = 0;
    expectedChar.c[0] = ' ';
    shouldBeTrue(whiteSpace->containsUtf8Char(expectedChar));
    expectedChar.u = 0;
    expectedChar.c[0] = '\x1f';
    shouldBeTrue(whiteSpace->containsUtf8Char(expectedChar));
    expectedChar = Utf8Chars::codePoint2utf8Char(0x2004);
    shouldBeTrue(whiteSpace->containsUtf8Char(expectedChar));
    expectedChar = Utf8Chars::codePoint2utf8Char(0x2005);
    shouldBeEqual(whiteSpace->nextUtf8Char().u, expectedChar.u);
    expectedChar = Utf8Chars::codePoint2utf8Char(0x2025);
    shouldBeFalse(whiteSpace->containsUtf8Char(expectedChar));
    delete whiteSpace;
  } endIt();

  /// We occasionally need to iterate over a Utf8Chars stream byte by byte
  /// instead of utf8Char_t by utf8Char_t. This test ensures that we can
  /// (reasonably) mix Utf8Chars::nextUtf8Char and Utf8Chars::getNextByte.
  it("can mix the use of getNextBye and nextUtf8Char") {
    Utf8Chars *someChars = new Utf8Chars("some ch€racters");
    size_t i = 0;
    for (; i < 3; i++) someChars->getNextByte();
    utf8Char_t expectedChar;
    expectedChar.u = 0;
    expectedChar.c[0] = 'e';
    shouldBeEqual(someChars->nextUtf8Char().u, expectedChar.u);
    for ( i++; i < 7; i++) someChars->getNextByte();
    expectedChar = Utf8Chars::codePoint2utf8Char(0x20AC);
    shouldBeEqual(someChars->nextUtf8Char().u, expectedChar.u);
    delete someChars;
  } endIt();

  it("Should keep the origUtf8Chars when cloned") {
    const char *cString = "some characters";
    Utf8Chars *someChars = new Utf8Chars(cString);
    shouldBeEqual((void*)someChars->origUtf8Chars, (void*)cString);
    someChars->nextUtf8Char();
    shouldBeEqual((void*)someChars->origUtf8Chars, (void*)cString);
    someChars->nextUtf8Char();
    shouldBeEqual((void*)someChars->origUtf8Chars, (void*)cString);
    Utf8Chars *clonedChars = someChars->clone(false);
    shouldBeEqual((void*)(clonedChars->origUtf8Chars),
      (void*)(someChars->origUtf8Chars));
    shouldBeEqual((void*)(clonedChars->utf8Chars), (void*)(cString));
    shouldBeEqual(clonedChars->nextByte, someChars->nextByte);
    delete clonedChars;
    clonedChars = someChars->clone(true);
    shouldBeEqual((void*)(clonedChars->origUtf8Chars),
      (void*)(someChars->origUtf8Chars));
    shouldBeEqual((void*)(clonedChars->utf8Chars), (void*)(cString+2));
    shouldBeEqual(clonedChars->utf8Chars, clonedChars->nextByte);
    shouldBeEqual(clonedChars->nextByte, someChars->nextByte);
    //
    // update position from the wrong clone should do nothing
    //
    clonedChars->updatePositionFrom(someChars);
    shouldBeEqual(clonedChars->nextByte, clonedChars->utf8Chars);
    //
    // update position the correct way around should change the nextByte
    //
    someChars->updatePositionFrom(clonedChars);
    shouldBeEqual(someChars->nextByte, clonedChars->nextByte);
    delete clonedChars;
    delete someChars;
  } endIt();

} endDescribe(Utf8Chars);

