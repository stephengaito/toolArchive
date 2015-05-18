#ifndef UTF8CHARS_H
#define UTF8CHARS_H

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

\brief The Utf8Chars class encapsulates UTF8 strings. In particular
the Utf8Chars class understands how to walk forwards and backwards
over UTF8 characters.

*/
class Utf8Chars {
  public:

    /// \brief A list of UTF8 white space charaters
    static const char whiteSpaceChars[];

    /// \brief Create an instance of the Utf8Chars...
    ///
    /// using the byte array someUtf8Chars.
    Utf8Chars(
      const char* someUtf8Chars ///< [in] the byte array of UTF8 chars
    ); ///< Create an instance of Utf8Chars

    /// Set next character back to the begining
    void restart();

    /// Backup ONE UTF8 character
    void backup();

    /// \brief Return the next UTF8 character.
    ///
    /// If there are no more characters, returns the null character.
    utf8Char_t nextUtf8Char();

    /// \brief Return the next byte as a character.
    ///
    /// This *might* leave the collection of characters in the middle
    /// of a UTF8 character.
    char getNextByte(void) {
      if (utf8Chars+numBytes < nextByte) return 0;
      return *nextByte++;
    }

    /// \brief Returns true if the Utf8Chars contians the given UTF8 char
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

#endif
