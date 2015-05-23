#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>

#include <classifier.h>

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "classifiers\n");
  printf(  "   value_t = %zu bytes (%zu bits)\n", sizeof(value_t),                 sizeof(value_t)*8);
  printf(  "classSet_t = %zu bytes (%zu bits)\n", sizeof(Classifier::classSet_t), sizeof(Classifier::classSet_t)*8);
  printf(  "----------------------------------\n");

  /// \brief Test the UTF8 Character Classifier class.
  describe("Classifier", [](){

    /// Ensure that we can create a Classifier object.
    it("create classifier", [&](){
      Classifier *classifier = new Classifier();
      AssertThat(classifier, Is().Not().EqualTo((Classifier*)0));
    }); // create classifier

    /// Ensure that we can register new UTF8 character classes.
    ///
    /// A UTF8 character class is a mapping from a class name (as a
    /// Utf8 string) to a bit-set representing the class.
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

    /// Ensure that we can use non ASCII UTF8 characters in a class name.
    it("register a class with a utf8 name", [&](){
      Classifier *classifier = new Classifier();
      // show that naive classifier does not understand whitespace class
      AssertThat(classifier->getClassSet(" "), Is().EqualTo(~0L));
      // now classify a string of utf8 charaters
      classifier->registerClassSet("white€space", 1);
      classifier->classifyUtf8CharsAs(" ", "white€space");
      AssertThat(classifier->getClassSet(" "), Is().EqualTo(1));
    }); // create classifier

    /// Ensure that we can determine the correct classifications of
    /// various characters.
    it("classify characters", [&](){
      Classifier *classifier = new Classifier();
      // show that naive classifier does not understand whitespace class
      AssertThat(classifier->getClassSet(" "), Is().EqualTo(~0L));
      // now classify a string of utf8 charaters
      classifier->registerClassSet("whitespace", 1);
      classifier->classifyUtf8CharsAs(Utf8Chars::whiteSpaceChars, "whitespace");
      AssertThat(classifier->getClassSet(" "), Is().EqualTo(1));
      AssertThat(classifier->getClassSet("a"), Is().EqualTo(~0L));
    }); // create classifier

  }); // Character classification

});

