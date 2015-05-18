#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>

#include <utf8chars.h>

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "utf8Chars\n");
  printf(  "  uint64_t = %zu bytes (%zu bits)\n", sizeof(uint64_t),   sizeof(uint64_t)*8);
  printf(  "utf8Char_t = %zu bytes (%zu bits)\n", sizeof(utf8Char_t), sizeof(utf8Char_t)*8);
  printf(  "----------------------------------\n");

  describe("Utf8Chars buffer", [](){

    it("create a Utf8Chars buffer", [&](){
      Utf8Chars *someChars = new Utf8Chars("silly");
      AssertThat(someChars, Is().Not().EqualTo((Utf8Chars*)0));
    });

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
    });

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
    });

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
    });

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
    });

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
    });

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
    });

  }); // Utf8Chars buffer

});

