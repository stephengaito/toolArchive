#include <string.h>

#include "classifier.h"

Classifier::Classifier(void) {
  // create the className2classSet mapping of characters to HAT-trie value_t
  className2classSet = hattrie_create();

  // create the utf8Char2classSet mapping of characters to HAT-trie value_t
  utf8Char2classSet = hattrie_create();

  unClassifiedSet = ~0L;
}

Classifier::classSet_t Classifier::findClassSet(const char* aClassName) {
  value_t *classSetPtr = hattrie_tryget(className2classSet,
                                        aClassName,
                                        strlen(aClassName));
  if (!classSetPtr) return 0;
  return *classSetPtr;
}

Classifier::classSet_t Classifier::registerClassSet(const char* aClassName,
                                                    classSet_t aClassSet) {
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
  unClassifiedSet &= ~newClassSet;
  Utf8Chars *utf8Chars = new Utf8Chars(someUtf8Chars);
  utf8Char_t aUtf8Char = utf8Chars->nextUtf8Char();
  while(aUtf8Char.u != 0) {
    classSet_t *classSetPtr = hattrie_get(utf8Char2classSet,
                                       aUtf8Char.c, strlen(aUtf8Char.c));
    if (!classSetPtr) return;
    *classSetPtr = newClassSet;
    aUtf8Char = utf8Chars->nextUtf8Char();
  }
}

void Classifier::classifyWhiteSpace(classSet_t aClassSet) {
  registerClassSet("whiteSpace", aClassSet);
  classifyUtf8CharsAs(Utf8Chars::whiteSpaceChars, "whiteSpace");
}

Classifier:: classSet_t Classifier::getClassSet(const char* someUtf8Chars) {
  Utf8Chars *utf8Chars = new Utf8Chars(someUtf8Chars);
  utf8Char_t aUtf8Char = utf8Chars->nextUtf8Char();
  return getClassSet(aUtf8Char);
}

Classifier::classSet_t Classifier::getClassSet(utf8Char_t aUtf8Char) {
  classSet_t *classSetPtr = hattrie_tryget(utf8Char2classSet,
                                           aUtf8Char.c, strlen(aUtf8Char.c));

  // if this is an unclassified character return the empty class set
  if (!classSetPtr) return unClassifiedSet;

  return *classSetPtr;
}
