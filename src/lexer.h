#ifndef LEXER_H
#define LEXER_H

#include <hat-trie.h>
typedef value_t classSet_t;

#include <stdint.h>

typedef union utf8Char_struct {
  char     c[8];
  uint64_t u;
} utf8Char_t;

class Utf8CharStr {
  public:
    Utf8CharStr(const char* someUtf8Chars);
    void restart();
    void backup();
    utf8Char_t nextUtf8Char();

  private:
    const char* utf8Chars;
    size_t      numBytes;
    const char* nextByte;
};

class Classifier {

  public:
    Classifier();
    classSet_t findClassSet(const char* aClassName);
    classSet_t registerClassSet(const char* aClassName,
                                classSet_t aClassSet);
    void classifyUtf8CharsAs(const char*  someUtf8Chars,
                             const char* aClassName);
    classSet_t getClassSet(utf8Char_t aUtf8Char);
    classSet_t getClassSet(const char* aUtf8Char);

  private:
    hattrie_t *className2classSet;
    hattrie_t *utf8Char2classSet;

};


#endif
