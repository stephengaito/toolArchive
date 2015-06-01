#ifndef UTF8CHARS_H
#define UTF8CHARS_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <assert.h>
#define ASSERT assert

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

    /// Set next character back to the begining of the underlying
    /// C-string.
    void restart();

    /// Backup ONE UTF8 character
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

    /// \brief Remember the current location in the stream.
    void mark(void) {
      currentMark = nextByte;
    }

    /// \brief Get the stream at the marked location.
    const char *getMark(void) {
      return currentMark;
    }

    /// \brief Returns the number of bytes, not neccessarily the number
    /// of UTF8 characters, in the marked text.
    size_t getNumberOfBytesInMarkedText(void) {
      return nextByte - currentMark;
    }

    /// \brief Returns a (strndup'ed) copy of the currently marked text.
    ///
    /// The currently marked text are the bytes from the currentMark
    /// until just before the nextByte.
    char *getCopyOfMarkedText(void) {
      return strndup(currentMark, getNumberOfBytesInMarkedText());
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

    /// \brief The currently marked character in the stream.
    ///
    /// This currentMark can be used to remember a previously marked
    /// location in the stream.
    const char* currentMark;
};

#endif
