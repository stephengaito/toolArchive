#ifndef LEXER_H
#define LEXER_H

#include <hat-trie.h>
typedef value_t classSet_t;

#include <stdint.h>

/**

\brief The utf8Char_struct (utf8Char_t) is a simple union to allow a
single UTF8 character to be viewed as either an array of 8 bytes, or as
a single unsigned 64 bit integer.

*/
typedef union utf8Char_struct {
  char     c[8]; ///< a UTF8 character as an array of at most 8 bytes
  uint64_t u;    ///< a UTF8 character as an unsigned 64 bit integer
} utf8Char_t;

/**

\brief The Utf8CharStr class encapsulates UTF8 strings. In particular
the Utf8CharStr class understands how to walk forwards and backwards
over UTF8 characters.

*/
class Utf8CharStr {
  public:

    /// \brief A list of UTF8 white space charaters
    static const char whiteSpaceChars[];

    /// \brief Create an instance of the Utf8CharStr...
    ///
    /// using the byte array someUtf8Chars.
    Utf8CharStr(
      const char* someUtf8Chars ///< [in] the byte array of UTF8 chars
    ); ///< Create an instance of Utf8CharStr

    /// Set next character back to the begining
    void restart();

    /// Backup ONE UTF8 character
    void backup();

    /// \brief Return the next UTF8 character.
    ///
    /// If there are no more characters, returns the null character.
    utf8Char_t nextUtf8Char();

    /// \brief Returns true if the Utf8CharStr contians the given UTF8 char
    ///
    /// **NOTE** containsUtf8Char restarts the nextUtf8Char pointer
    /// *and* if the expectedUtf8Char is found, leaves the nextUtf8Char
    /// pointer pointing at the *next* character in the string.
    bool containsUtf8Char(utf8Char_t expectedUtf8Char);


    /// \brief Convert an integer code point into a UTF8 character
    static utf8Char_t codePoint2utf8Char(uint64_t codePoint);

  private:
    const char* utf8Chars;
    size_t      numBytes;
    const char* nextByte;
};

/**

\brief The Classifier class is used to classify UTF8 characters.

Users can register new class names and associate with each class name a
fixed class set.

A classification class set is a bit set of base classification
indicators (one for each bit) which can be combined using any C/C++
bitwise operation.

*/
class Classifier {

  public:

    /// \brief Create a UTF8 character classifier.
    ///
    Classifier();

    /// \brief Find the class set associated with a given class.
    ///
    /// Returns the empty set if the given class name has not been
    /// registered.
    classSet_t findClassSet(
      const char* aClassName ///< [in] the UTF8 string name of the class.
    );

    /// \brief Register a class set to a given class.
    ///
    /// Returns the previously registered class set
    ///
    /// Returns the empty set if this class has never been registered.
    classSet_t registerClassSet(
      const char* aClassName, ///< [in] the UTF8 string name of the class.
      classSet_t aClassSet    ///< [in] the (bit) class set
    );

    /// \brief Declare the classification of a collection of UTF8 characters.
    ///
    /// **NOTE** that when classifing a given character twice with two
    /// different classes the *last* classification is used.
    void classifyUtf8CharsAs(
      const char*  someUtf8Chars, ///< [in] the collection of UTF8 characters to classify.
      const char* aClassName      ///< [in] the name of the *previously* registered class to use to classify these characters
    );

    /// \brief Get the class set classification for a given character.
    ///
    /// Returns the empty set if this character has never been classified.
    classSet_t getClassSet(
      utf8Char_t aUtf8Char ///< [in] the UTF8 character to be classified
    );

    /// \brief Get the class set classification for a given character.
    ///
    /// Returns the empty set if this character has never been classified.
    classSet_t getClassSet(
      const char* aUtf8Char ///< [in] a UTF8 character to be classified. If there are multiple characters **ONLY** the first character is classified.
    );

  private:
    hattrie_t *className2classSet;
    hattrie_t *utf8Char2classSet;

};


#endif
