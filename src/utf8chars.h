#ifndef UTF8CHARS_H
#define UTF8CHARS_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "assertions.h"
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

    /// \brief An invariant which should ALWAYS be true for any
    /// instance of a Utf8Cchars class.
    ///
    /// Throws an AssertionFailure with a brief description of any
    /// inconsistencies discovered.
    bool invariant(void) const {
      if ((origUtf8Chars == NULL) ||
          (utf8Chars     == NULL) ||
          (nextByte      == NULL) ||
          (lastByte      == NULL)) {
        if (origUtf8Chars != NULL) throw AssertionFailure("origUtf8Chars not NULL");
        if (utf8Chars     != NULL) throw AssertionFailure("utf8Chars not NULL");
        if (nextByte      != NULL) throw AssertionFailure("nextByte not NULL");
        if (lastByte      != NULL) throw AssertionFailure("lastByte not NULL");
      }
      if (!((origUtf8Chars <= lastByte) &&
            (utf8Chars     <= lastByte) &&
            (nextByte      <= lastByte) &&
            (utf8Chars     <= nextByte) &&
            (origUtf8Chars <= utf8Chars)))
        throw AssertionFailure("incorrectly ordered origUtf8Chars, utf8Chars, nextByte and/or lastByte");
      if (!validUtf8Chars(utf8Chars, lastByte))
        throw AssertionFailure("invalid UTF8 characters");
      return true;
    }

    /// \brief The ownership model for the Utf8Chars.
    enum Ownership {
      DoNotOwn, TakeOwnership, Duplicate
    };

    /// \brief A list of UTF8 white space charaters
    static const char whiteSpaceChars[];

    /// \brief Create an instance of the Utf8Chars using the byte array
    /// someUtf8Chars with the provided OwnerShip model.
    Utf8Chars(const char* someUtf8Chars, Ownership ownership = DoNotOwn);

    /// \brief Destroy this object.
    ///
    /// If the underlying C-String is owned by this object, the string
    /// will be freed as well.
    ~Utf8Chars(void);

    /// \brief Create a cloned copy of this Utf8Chars starting at
    /// the current location and *not* owning the underlying C-String.
    ///
    /// If subStream is true then the cloned Utf8Chars "starts" at the
    /// parent's nextbyte (current position).
    Utf8Chars *clone(bool subStream = false) {
      ASSERT(invariant());
      Utf8Chars *result = new Utf8Chars(utf8Chars, DoNotOwn);
      if (subStream) result->utf8Chars = nextByte;
      result->nextByte = nextByte;
      result->origUtf8Chars = origUtf8Chars;
      result->lastByte = lastByte;
      ASSERT(result->invariant());
      return result;
    }

    /// \brief Update the position of one Utf8Chars from an other
    /// Utf8Chars instance.
    void updatePositionFrom(Utf8Chars *otherChars) {
      // return if otherChars is not a clone of this
      if (!otherChars)                                       return;
      ASSERT(otherChars->invariant());
      if (origUtf8Chars        != otherChars->origUtf8Chars) return;
      if (lastByte             != otherChars->lastByte)      return;
      if (otherChars->nextByte <  utf8Chars)                 return;
      nextByte = otherChars->nextByte;
      ASSERT(invariant());
    }

    /// \brief Returns true if the last character was the last one
    /// in the underlying C-String.
    bool atEnd(void) {
      ASSERT(invariant());
      return (lastByte <= nextByte);
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
      ASSERT(invariant());
      if (lastByte < nextByte) return 0;
      return *nextByte++;
    }

    /// \brief Returns the stream start.
    const char *getStart(void) {
      ASSERT(invariant());
      return utf8Chars;
    }

    /// \brief Returns the number of bytes, not neccessarily the number
    /// of UTF8 characters, in the stream from the start to the current
    /// character.
    size_t getNumberOfBytesRead(void) {
      ASSERT(invariant());
      if (lastByte <= nextByte) nextByte = lastByte;
      return nextByte - utf8Chars;
    }

    /// \brief Returns a (strndup'ed) copy of the current stream read.
    char *getCopyOfTextRead(void) {
      ASSERT(invariant());
      return strndup(utf8Chars, getNumberOfBytesRead());
    }

    /// \brief Returns the number of bytes which could still be read
    /// from the stream.
    ///
    /// Note that the number of bytes might not be the number of
    /// Utf8Chars which could be read from the rest of this stream.
    size_t getNumberOfBytesToRead(void) {
      ASSERT(invariant());
      if (lastByte <= nextByte) nextByte = lastByte;
      return lastByte - nextByte;
    }

    /// \brief Returns a (strndup'ed) copy of the stream which has not
    /// yet been read.
    char *getCopyOfTextToRead(size_t numBytesToCopy = 30) {
      ASSERT(invariant());
      if (lastByte <= nextByte) nextByte = lastByte;
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

    /// \brief Returns true if the text provided is a valid collection
    /// of UTF8 characters.
    ///
    /// If the text provided contains non UTF8 bytes this method will
    /// either return false or throw an AssertionFailure exception
    /// depending upon whether or not the C/C++ macro DEBUG is defined.
    /// (see the assertions.h file).
    static bool validUtf8Chars(const char *textStart, size_t textLength);

    /// \brief Returns true if the text provided is a valid collection
    /// of UTF8 characters.
    ///
    /// If the text provided contains non UTF8 bytes this method will
    /// either return false or throw an AssertionFailure exception
    /// depending upon whether or not the C/C++ macro DEBUG is defined.
    /// (see the assertions.h file).
    static bool validUtf8Chars(const char *textStart, const char *textEnd);

  protected:

    /// \brief Whether or not this C-string is owned by this object
    bool ownsString;

    /// \brief The original C-string of UTF8 characters.
    const char* origUtf8Chars;

    /// \brief The current (sub)C-string of UTF8 characters.
    const char* utf8Chars;

    /// \brief The last byte in the C-string representing this string of
    /// UTF8 characters.
    const char* lastByte;

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
