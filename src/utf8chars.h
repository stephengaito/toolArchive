#ifndef UTF8CHARS_H
#define UTF8CHARS_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <assert.h>
#define ASSERT assert

#include "varArray.h"

/// \brief The utf8Char_struct (utf8Char_t) is a simple union to allow
/// a single UTF8 character to be viewed as either an array of 8 bytes, or
/// as a single unsigned 64 bit integer.
typedef union utf8Char_struct {
  char     c[8]; ///< a UTF8 character as an array of at most 8 bytes
  uint64_t u;    ///< a UTF8 character as an unsigned 64 bit integer
} utf8Char_t;

/// \brief The Utf8Chars class encapsulates UTF8 strings. In particular
/// the Utf8Chars class understands how to walk forwards and backwards over
/// UTF8 characters.
class Utf8Chars {
  public:

    /// \brief The ownership model for the Utf8Chars.
    enum Ownership {
      DoNotOwn, TakeOwnership, Duplicate
    };

    /// \brief A list of UTF8 white space charaters
    static const char whiteSpaceChars[];

    /// \brief Create an instance of the Utf8Chars...
    ///
    /// using the byte array someUtf8Chars.
    Utf8Chars(
      const char* someUtf8Chars, ///< [in] the byte array of UTF8 chars
      Ownership ownership = DoNotOwn
    ); ///< Create an instance of Utf8Chars

    /// \brief Destroy this object.
    ///
    /// If the underlying C-String is owned by this object, the string
    /// will be freed as well.
    ~Utf8Chars(void);

    /// \brief Create a cloned copy of this Utf8Chars starting at
    /// the current location and *not* owning the underlying C-String.
    Utf8Chars *clone(void) {
      return new Utf8Chars(nextByte, DoNotOwn);
    }

    /// \brief Returns true if the last character was the last one
    /// in the underlying C-String.
    bool atEnd(void) {
      return (utf8Chars+numBytes <= nextByte);
    }

    /// \brief Set next character back to the begining of the
    /// underlying C-string.
    void restart();

    /// \brief Backup ONE UTF8 character
    void backup();

    /// \brief Return the next UTF8 character.
    ///
    /// If there are no more characters, returns the null character.
    ///
    /// If the Utf8Chars::getNextByte and Utf8Chars::nextUtf8Char methods
    /// are intermixed then the nextByte might *not* be the begining of
    /// a UTF8 character. In this case the Utf8Chars::nextUtf8Char will
    /// return a null character (which could be interpreted to represent
    /// the end of the Utf8Chars character stream.
    utf8Char_t nextUtf8Char();

    /// \brief Return the next byte as a character.
    ///
    /// If there are no more characters, returns the null character.
    ///
    /// This *might* leave the collection of characters in the middle
    /// of a UTF8 character.
    ///
    /// If the Utf8Chars::getNextByte and Utf8Chars::nextUtf8Char methods
    /// are intermixed then the nextByte might *not* be the begining of
    /// a UTF8 character. In this case the Utf8Chars::nextUtf8Char will
    /// return a null character (which could be interpreted to represent
    /// the end of the Utf8Chars character stream.
    char getNextByte(void) {
      if (utf8Chars+numBytes < nextByte) return 0;
      return *nextByte++;
    }

    /// \brief Returns the stream start.
    const char *getStart(void) {
      return utf8Chars;
    }

    /// \brief Returns the number of bytes, not neccessarily the number
    /// of UTF8 characters, in the stream from the start to the current
    /// character.
    size_t getNumberOfBytesRead(void) {
      if (utf8Chars+numBytes <= nextByte) nextByte = utf8Chars+numBytes;
      return nextByte - utf8Chars;
    }

    /// \brief Returns a (strndup'ed) copy of the current stream read.
    char *getCopyOfTextRead(void) {
      return strndup(utf8Chars, getNumberOfBytesRead());
    }

    /// \brief Returns a (strndup'ed) copy of the stream which has not
    /// yet been read.
    char *getCopyOfTextToRead(size_t numBytesToCopy = 30) {
      if (utf8Chars+numBytes <= nextByte) nextByte = utf8Chars+numBytes;
      return strndup(nextByte, numBytesToCopy);
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

    /// \brief Whether or not this C-string is owned by this object
    bool ownsString;

    /// \brief The C-string of UTF8 characters.
    const char* utf8Chars;

    /// \brief The total number of bytes contained in this string of
    /// UTF8 characters.
    ///
    /// The number of bytes will generally be larger than the number of
    /// [UTF8 characters](http://en.wikipedia.org/wiki/UTF-8) if there
    /// are any non-[ASCII](http://en.wikipedia.org/wiki/ASCII)
    /// characters in the string.
    size_t      numBytes;

    /// \brief The nextByte to be returned by either of the
    /// Utf8Chars::nextUtf8Char or Utf8Chars::getNextByte methods.
    ///
    /// If the Utf8Chars::getNextByte and Utf8Chars::nextUtf8Char methods
    /// are intermixed then the nextByte might *not* be the begining of
    /// a UTF8 character. In this case the Utf8Chars::nextUtf8Char will
    /// return a null character (which could be interpreted to represent
    /// the end of the Utf8Chars character stream.
    const char* nextByte;
};

#endif
