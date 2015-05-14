#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>

#include <classifier.h>

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "classifiers\n");
  printf(  "   value_t = %lu bytes (%lu bits)\n", sizeof(value_t),    sizeof(value_t)*8);
  printf(  "classSet_t = %lu bytes (%lu bits)\n", sizeof(classSet_t), sizeof(classSet_t)*8);
  printf(  "----------------------------------\n");


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

