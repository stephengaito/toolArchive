#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>

#ifndef private
#define private public
#endif

#include <utf8chars.h>

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "utf8Chars\n");
  printf(  "  uint64_t = %zu bytes (%zu bits)\n", sizeof(uint64_t),   sizeof(uint64_t)*8);
  printf(  "utf8Char_t = %zu bytes (%zu bits)\n", sizeof(utf8Char_t), sizeof(utf8Char_t)*8);
  printf(  "----------------------------------\n");

  /// \brief Test the Utf8Chars buffered stream.
  ///
  /// The Utf8Chars class provides a buffered stream of utf8Char_t types.
  /// These tests verify the required behaviour of this buffered stream of
  /// utf8Char_t types.
  describe("Utf8Chars", [](){

    /// Simply ensure we can correctly create a new Utf8Chars object
    /// on a standard C-string.
    it("create a Utf8Chars buffer", [&](){
      Utf8Chars *someChars = new Utf8Chars("silly");
      AssertThat(someChars,                Is().Not().EqualTo((Utf8Chars*)0));
      AssertThat(someChars->origUtf8Chars, Equals(someChars->utf8Chars));
      AssertThat(someChars->nextByte,      Equals(someChars->utf8Chars));
      AssertThat(someChars->ownsString,    Is().False());
      delete someChars;
      someChars = new Utf8Chars("silly", Utf8Chars::DoNotOwn);
      AssertThat(someChars,                Is().Not().EqualTo((Utf8Chars*)0));
      AssertThat(someChars->origUtf8Chars, Is().EqualTo(someChars->utf8Chars));
      AssertThat(someChars->nextByte,      Is().EqualTo(someChars->utf8Chars));
      AssertThat(someChars->ownsString,    Is().False());
      delete someChars;
    });

    /// Simply ensure we can correctly create a new Utf8Chars object
    /// on a standard C-string which will be duplicated.
    it("create a Utf8Chars buffer and duplicate string", [&](){
      const char* cString = "silly";
      Utf8Chars *someChars = new Utf8Chars(cString, Utf8Chars::Duplicate);
      AssertThat(someChars,              Is().Not().EqualTo((Utf8Chars*)0));
      AssertThat(someChars->nextByte,    Is().EqualTo(someChars->utf8Chars));
      AssertThat(someChars->ownsString,  Is().True());
      AssertThat(someChars->utf8Chars,   Equals(cString));
      AssertThat(someChars->utf8Chars,   Is().Not().EqualTo((char*)cString));
      delete someChars;
    });

    /// Simply ensure we can correctly create a new Utf8Chars object
    /// on a standard C-string which will take over ownership.
    it("create a Utf8Chars buffer and take over ownership", [&](){
      const char* cString = strdup("silly");
      Utf8Chars *someChars = new Utf8Chars(cString, Utf8Chars::TakeOwnership);
      AssertThat(someChars,              Is().Not().EqualTo((Utf8Chars*)0));
      AssertThat(someChars->nextByte,    Equals(someChars->utf8Chars));
      AssertThat(someChars->ownsString,  Is().True());
      AssertThat(someChars->utf8Chars,   Equals(cString));
      delete someChars;
    });

    it("Show that we can getStart, getNumberBytesRead and copyOfReadText", [&](){
      Utf8Chars *someChars = new Utf8Chars("silly");
      AssertThat(someChars,              Is().Not().EqualTo((Utf8Chars*)0));
      AssertThat(someChars->nextByte,    Is().EqualTo(someChars->utf8Chars));
      utf8Char_t expectedChar;
      expectedChar.u = 0;
      expectedChar.c[0] = 's';
      AssertThat(someChars->nextUtf8Char().u, Is().EqualTo(expectedChar.u));
      expectedChar.c[0] = 'i';
      AssertThat(someChars->nextUtf8Char().u, Is().EqualTo(expectedChar.u));
      expectedChar.c[0] = 'l';
      AssertThat(someChars->nextUtf8Char().u, Is().EqualTo(expectedChar.u));
      expectedChar.c[0] = 'l';
      AssertThat(someChars->nextUtf8Char().u, Is().EqualTo(expectedChar.u));
      const char *someCharsStart = someChars->getStart();
      AssertThat(someCharsStart,Equals(someChars->utf8Chars));
      AssertThat(someChars->getNumberOfBytesRead(), Is().EqualTo(4));
      char *sill = someChars->getCopyOfTextRead();
      AssertThat(someChars->utf8Chars,  Is().Not().EqualTo(sill));
      AssertThat(strlen(sill), Is().EqualTo(4));
      AssertThat(sill[0], Is().EqualTo('s'));
      AssertThat(sill[1], Is().EqualTo('i'));
      AssertThat(sill[2], Is().EqualTo('l'));
      AssertThat(sill[3], Is().EqualTo('l'));
      AssertThat(sill[4], Is().EqualTo(0));
      free(sill);
      delete someChars;
    });

    /// Confirm that Utf8Chars::codePoint2utf8Char handles provides the
    /// correct mapping from code points to utf8Chat_t types. In particular,
    /// we check each of the code point boundaries.
    it("convert code points to utf8Char_t", [&](){
      utf8Char_t expectedChar;
      expectedChar.u = 0;
      expectedChar.c[0] = ' ';
      AssertThat(Utf8Chars::codePoint2utf8Char(0x20).u, Is().EqualTo(expectedChar.u));
      expectedChar.u = 0;
      expectedChar.c[0] = 0xDF;
      expectedChar.c[1] = 0xBF;
      AssertThat(Utf8Chars::codePoint2utf8Char(0x7FF).u, Is().EqualTo(expectedChar.u));
      expectedChar.u = 0;
      expectedChar.c[0] = 0xEF;
      expectedChar.c[1] = 0xBF;
      expectedChar.c[2] = 0xBF;
      AssertThat(Utf8Chars::codePoint2utf8Char(0xFFFF).u, Is().EqualTo(expectedChar.u));
      expectedChar.u = 0;
      expectedChar.c[0] = 0xF7;
      expectedChar.c[1] = 0xBF;
      expectedChar.c[2] = 0xBF;
      expectedChar.c[3] = 0xBF;
      AssertThat(Utf8Chars::codePoint2utf8Char(0x1FFFFF).u, Is().EqualTo(expectedChar.u));
      expectedChar.u = 0;
      expectedChar.c[0] = 0xFB;
      expectedChar.c[1] = 0xBF;
      expectedChar.c[2] = 0xBF;
      expectedChar.c[3] = 0xBF;
      expectedChar.c[4] = 0xBF;
      AssertThat(Utf8Chars::codePoint2utf8Char(0x3FFFFFF).u, Is().EqualTo(expectedChar.u));
      expectedChar.u = 0;
      expectedChar.c[0] = 0xFD;
      expectedChar.c[1] = 0xBF;
      expectedChar.c[2] = 0xBF;
      expectedChar.c[3] = 0xBF;
      expectedChar.c[4] = 0xBF;
      expectedChar.c[5] = 0xBF;
      AssertThat(Utf8Chars::codePoint2utf8Char(0x7FFFFFFF).u, Is().EqualTo(expectedChar.u));
      expectedChar.u = 0;
      AssertThat(Utf8Chars::codePoint2utf8Char(0x7FFFFFFFF).u, Is().EqualTo(expectedChar.u));
    });

    /// Ensure that we can iterate over a *simple* ASCII string of one
    /// charater.
    it("iterate over a simple ascii string", [&](){
      Utf8Chars *someChars = new Utf8Chars("s");
      utf8Char_t expectedChar;
      expectedChar.u    = 0;
      expectedChar.c[0] = 's';
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(0));
      someChars->restart();
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(0));
      someChars->backup();
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(0));
      delete someChars;
    });

    /// Ensure that we can iterate over a *simple* ASCII string of multiple
    /// charaters.
    it("iterate over a long ascii string", [&](){
      Utf8Chars *someChars = new Utf8Chars("silly");
      utf8Char_t expectedChar;
      expectedChar.u    = 0;
      expectedChar.c[0] = 's';
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      expectedChar.c[0] = 'i';
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      someChars->backup();
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      expectedChar.c[0] = 'l';
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      expectedChar.c[0] = 'l';
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      expectedChar.c[0] = 'y';
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(0));
      someChars->restart();
      expectedChar.c[0] = 's';
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      delete someChars;
    });

    /// Ensure that we can iterate over a string which contains one
    /// UTF8 charater.
    it("iterate over a simple utf8 string", [&](){
      Utf8Chars *someChars = new Utf8Chars("€");
      utf8Char_t expectedChar;
      expectedChar.u    = 0;
      expectedChar.c[0] = 0xE2;
      expectedChar.c[1] = 0x82;
      expectedChar.c[2] = 0xAC;
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(0));
      someChars->restart();
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(0));
      someChars->backup();
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(0));
      delete someChars;
    });

    /// Ensure that we can iterate over a string which contains multiple
    /// UTF8 charaters.
    it("iterate over a long utf8 string", [&](){
      Utf8Chars *someChars = new Utf8Chars("$¢€[");
      utf8Char_t expectedChar;
      expectedChar.u    = 0;
      expectedChar.c[0] = '$';
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      expectedChar.u    = 0;
      expectedChar.c[0] = 0xC2;
      expectedChar.c[1] = 0xA2;
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      expectedChar.u    = 0;
      expectedChar.c[0] = 0xE2;
      expectedChar.c[1] = 0x82;
      expectedChar.c[2] = 0xAC;
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      someChars->backup();
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      someChars->backup();
      someChars->backup();
      expectedChar.u    = 0;
      expectedChar.c[0] = 0xC2;
      expectedChar.c[1] = 0xA2;
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      expectedChar.u    = 0;
      expectedChar.c[0] = 0xE2;
      expectedChar.c[1] = 0x82;
      expectedChar.c[2] = 0xAC;
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      expectedChar.u    = 0;
      expectedChar.c[0] = '[';
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(0));
      someChars->restart();
      expectedChar.u    = 0;
      expectedChar.c[0] = '$';
      AssertThat((someChars->nextUtf8Char()).u, Is().EqualTo(expectedChar.u));
      delete someChars;
    });

    /// Check that Utf8Chars::nextUtf8Char can handle mallformed
    /// "utf8" byte sequences.
    it("iterate over malformed UTF8 strings",[&](){
      utf8Char_t nullChar;
      nullChar.u = 0;

      utf8Char_t aChar;
      aChar.u = 0;
      aChar.c[0] = 0xFF;
      Utf8Chars *someChars = new Utf8Chars(aChar.c);
      AssertThat(someChars->nextUtf8Char().u, Is().EqualTo(nullChar.u));
      aChar.u = 0;
      aChar.c[0] = 0xC0;
      aChar.c[1] = 0xFF;
      someChars = new Utf8Chars(aChar.c);
      AssertThat(someChars->nextUtf8Char().u, Is().EqualTo(nullChar.u));
      aChar.u = 0;
      aChar.c[0] = 0xC0;
      aChar.c[1] = 0x00;
      someChars = new Utf8Chars(aChar.c);
      AssertThat(someChars->nextUtf8Char().u, Is().EqualTo(nullChar.u));
      aChar.u = 0;
      aChar.c[0] = 0xC0;
      aChar.c[1] = 0x80;
      someChars = new Utf8Chars(aChar.c);
      AssertThat(someChars->nextUtf8Char().u, Is().EqualTo(aChar.u));
      delete someChars;
    });

    /// Ensure that the Utf8Chars::whiteSpaceChars constant contains
    /// the white space UTF8 characters we are expecting.
    ///
    /// This test also contains code (normally commeted out) to produce
    /// the C code required to provide the Utf8Chars::whiteSpaceChars
    /// structure.
    it("whiteSpaceChars contains UTF8 white space characters", [&](){
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
      AssertThat(whiteSpace->containsUtf8Char(expectedChar), Is().True());
      expectedChar.u = 0;
      expectedChar.c[0] = '\x1f';
      AssertThat(whiteSpace->containsUtf8Char(expectedChar), Is().True());
      expectedChar = Utf8Chars::codePoint2utf8Char(0x2004);
      AssertThat(whiteSpace->containsUtf8Char(expectedChar), Is().True());
      expectedChar = Utf8Chars::codePoint2utf8Char(0x2005);
      AssertThat(whiteSpace->nextUtf8Char().u, Is().EqualTo(expectedChar.u));
      expectedChar = Utf8Chars::codePoint2utf8Char(0x2025);
      AssertThat(whiteSpace->containsUtf8Char(expectedChar), Is().False());
    });

    /// We occasionally need to iterate over a Utf8Chars stream byte by byte
    /// instead of utf8Char_t by utf8Char_t. This test ensures that we can
    /// (reasonably) mix Utf8Chars::nextUtf8Char and Utf8Chars::getNextByte.
    it("can mix the use of getNextBye and nextUtf8Char", [&](){
      Utf8Chars *someChars = new Utf8Chars("some ch€racters");
      size_t i = 0;
      for (; i < 3; i++) someChars->getNextByte();
      utf8Char_t expectedChar;
      expectedChar.u = 0;
      expectedChar.c[0] = 'e';
      AssertThat(someChars->nextUtf8Char().u, Is().EqualTo(expectedChar.u));
      for ( i++; i < 7; i++) someChars->getNextByte();
      expectedChar = Utf8Chars::codePoint2utf8Char(0x20AC);
      AssertThat(someChars->nextUtf8Char().u, Is().EqualTo(expectedChar.u));
      delete someChars;
    });

    it("Should keep the origUtf8Chars when cloned", [](){
      const char *cString = "some characters";
      Utf8Chars *someChars = new Utf8Chars(cString);
      AssertThat(someChars->origUtf8Chars, Equals((void*)cString));
      someChars->nextUtf8Char();
      AssertThat(someChars->origUtf8Chars, Equals((void*)cString));
      someChars->nextUtf8Char();
      AssertThat(someChars->origUtf8Chars, Equals((void*)cString));
      Utf8Chars *clonedChars = someChars->clone(false);
      AssertThat((void*)(clonedChars->origUtf8Chars),
        Equals((void*)(someChars->origUtf8Chars)));
      AssertThat((void*)(clonedChars->utf8Chars), Equals((void*)(cString)));
      AssertThat(clonedChars->nextByte, Equals(someChars->nextByte));
      delete clonedChars;
      clonedChars = someChars->clone(true);
      AssertThat((void*)(clonedChars->origUtf8Chars),
        Equals((void*)(someChars->origUtf8Chars)));
      AssertThat((void*)(clonedChars->utf8Chars), Equals((void*)(cString+2)));
      AssertThat(clonedChars->utf8Chars, Equals(clonedChars->nextByte));
      AssertThat(clonedChars->nextByte, Equals(someChars->nextByte));
      //
      // update position from the wrong clone should do nothing
      //
      clonedChars->updatePositionFrom(someChars);
      AssertThat(clonedChars->nextByte, Equals(clonedChars->utf8Chars));
      //
      // update position the correct way around should change the nextByte
      //
      someChars->updatePositionFrom(clonedChars);
      AssertThat(someChars->nextByte, Equals(clonedChars->nextByte));
    });

  }); // Utf8Chars buffer

});

