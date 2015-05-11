#include <bandit/bandit.h>
#include "bandit/localHelpers.h"
using namespace bandit;

#include <lexer.h>

go_bandit([](){

  describe("Utf8CharStr buffer", [](){

    it("create a utf8CharStr buffer", [&](){
      Utf8CharStr *someChars = new Utf8CharStr("silly");
      AssertThat(someChars, Is().Not().EqualTo((Utf8CharStr*)0));
    });

    it("iterate over a simple ascii string", [&](){
      Utf8CharStr *someChars = new Utf8CharStr("s");
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
      Utf8CharStr *someChars = new Utf8CharStr("silly");
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
      Utf8CharStr *someChars = new Utf8CharStr("€");
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
      Utf8CharStr *someChars = new Utf8CharStr("$¢€[");
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

  }); // Utf8CharStr buffer

  describe("Character classification", [](){

    it("create classifier", [&](){
      Classifier *classifier = new Classifier();
      AssertThat(classifier, Is().Not().EqualTo((Classifier*)0));
    }); // create classifier

    it("register a class", [&](){
      Classifier *classifier = new Classifier();

      // show that unknown classes have the empty set
      uint64_t classSet = classifier->findClassSet("silly");
      AssertThat(classSet, Is().EqualTo(0));

      // now register a new class
      uint64_t newClassSet = classifier->registerClassSet("silly", 1);
      AssertThat(newClassSet, Is().EqualTo(0));

      // show that we can find the recently registered class
      classSet = classifier->findClassSet("silly");
      AssertThat(classSet,    Is().EqualTo(1));

      // now re-register existing class with new value
      newClassSet = classifier->registerClassSet("silly", 2);
      AssertThat(newClassSet, Is().EqualTo(1));
      classSet = classifier->findClassSet("silly");
      AssertThat(classSet, Is().EqualTo(2));
    });

    it("register a class with a utf8 name", [&](){
      Classifier *classifier = new Classifier();
      // show that naive classifier does not understand whitespace class
      AssertThat(classifier->getClassSet(" "), Is().EqualTo(0));
      // now classify a string of utf8 charaters
      classifier->registerClassSet("white€space", 1);
      classifier->classifyUtf8CharsAs(" ", "white€space");
      AssertThat(classifier->getClassSet(" "), Is().EqualTo(1));
    }); // create classifier

    it("classify characters", [&](){
      Classifier *classifier = new Classifier();
      // show that naive classifier does not understand whitespace class
      AssertThat(classifier->getClassSet(" "), Is().EqualTo(0));
      // now classify a string of utf8 charaters
      classifier->registerClassSet("whitespace", 1);
      classifier->classifyUtf8CharsAs(" ", "whitespace");
      AssertThat(classifier->getClassSet(" "), Is().EqualTo(1));
    }); // create classifier

  }); // Character classification

});

