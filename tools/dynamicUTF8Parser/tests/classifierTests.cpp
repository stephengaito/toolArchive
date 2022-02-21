#include <string.h>
#include <stdio.h>

#include <cUtils/specs/specs.h>

#ifndef protected
#define protected public
#endif

#include <dynUtf8Parser/classifier.h>

/// \brief Test the UTF8 Character Classifier class.
describe(Classifier) {

  specSize(value_t);
  specSize(Classifier::classSet_t);

  /// Ensure that we can create a Classifier object.
  it("create classifier") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    delete classifier;
  } endIt();

  /// Ensure that we can register new UTF8 character classes.
  ///
  /// A UTF8 character class is a mapping from a class name (as a
  /// Utf8 string) to a bit-set representing the class.
  it("register a class") {
    Classifier *classifier = new Classifier();

    // show that unknown classes have the empty set
    uint64_t classSet = classifier->findClassSet("silly");
    shouldBeZero(classSet);

    // now register a new class
    uint64_t newClassSet = classifier->registerClassSet("silly", 1);
    shouldBeZero(newClassSet);

    // show that we can find the recently registered class
    classSet = classifier->findClassSet("silly");
    shouldBeEqual(classSet, 1);

    // now re-register existing class with new value
    newClassSet = classifier->registerClassSet("silly", 2);
    shouldBeEqual(newClassSet, 1);
    classSet = classifier->findClassSet("silly");
    shouldBeEqual(classSet, 2);
    delete classifier;
  } endIt();

  /// Ensure that we can use non ASCII UTF8 characters in a class name.
  it("register a class with a utf8 name") {
    Classifier *classifier = new Classifier();
    // show that naive classifier does not understand whitespace class
    shouldBeEqual(classifier->getClassSet(" "), ~0L);
    // now classify a string of utf8 charaters
    classifier->registerClassSet("white€space", 1);
    classifier->classifyUtf8CharsAs(" ", "white€space");
    shouldBeEqual(classifier->getClassSet(" "), 1);
    delete classifier;
  } endIt();

  /// Ensure that we can determine the correct classifications of
  /// various characters.
  it("classify characters") {
    Classifier *classifier = new Classifier();
    // show that naive classifier does not understand whitespace class
    shouldBeEqual(classifier->getClassSet(" "), ~0L);
    // now classify a string of utf8 charaters
    classifier->registerClassSet("whitespace", 1);
    classifier->classifyUtf8CharsAs(Utf8Chars::whiteSpaceChars, "whitespace");
    shouldBeEqual(classifier->getClassSet(" "), 1);
    shouldBeEqual(classifier->getClassSet("a"), ~1L);
    delete classifier;
  } endIt();

} endDescribe(Classifier);

